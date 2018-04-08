/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast/sqltoast.h"

#include "parser/parse.h"
#include "parser/error.h"

namespace sqltoast {

// <table constraint definition> ::=
//     [ <constraint name definition> ] <table constraint> [ <constraint check time> ]
//
// <table constraint> ::=
//     <unique constraint definition>
//     | <referential constraint definition>
//     | <check constraint definition>
bool parse_constraint(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t constraint_name;

    if (cur_sym == SYMBOL_CONSTRAINT) {
        cur_tok = lex.next();
        goto expect_constraint_name;
    }
    goto expect_constraint_type;
expect_constraint_name:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    constraint_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto expect_constraint_type;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_constraint_type:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_UNIQUE:
            cur_tok = ctx.lexer.next();
            out = std::make_unique<unique_constraint_t>(false);
            goto expect_col_list;
        case SYMBOL_PRIMARY:
            cur_tok = ctx.lexer.next();
            goto expect_key;
        case SYMBOL_FOREIGN:
            cur_tok = ctx.lexer.next();
            if (! parse_foreign_key_constraint(ctx, cur_tok, out))
                return false;
            goto push_constraint;
        case SYMBOL_CHECK:
            // TODO
        default:
            if (constraint_name)
                goto err_expect_constraint_type;
            // Just return false, since callers could be looking for a column
            // definition (on the table)
            return false;
    }
err_expect_constraint_type:
    expect_any_error(ctx, {SYMBOL_FOREIGN, SYMBOL_UNIQUE, SYMBOL_PRIMARY, SYMBOL_CHECK});
    return false;
expect_key:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_KEY)
        goto err_expect_key;
    cur_tok = lex.next();
    out = std::make_unique<unique_constraint_t>(true);
    goto expect_col_list;
err_expect_key:
    expect_error(ctx, SYMBOL_KEY);
    return false;
expect_col_list:
    // We get here if we've successfully parsed the constraint type (UNIQUE,
    // PRIMARY KEY or FOREIGN KEY <referencing column>) and now we expect a
    // parens-enclosed, comma-delimited list of column names
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_column;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_column:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    out->columns.emplace_back(cur_tok.lexeme);
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_column;
    }
    goto expect_rparen;
expect_rparen:
    // We get here after successfully parsing a column list and now we're
    // looking for that closing parentheses
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_constraint;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_constraint:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (constraint_name)
        out->name = constraint_name;
    return true;
}

// <referential constraint definition> ::=
//     FOREIGN KEY <left paren> <referencing columns> <right paren>
//     <references specification>
//
// <referencing columns> ::= <reference column list>
bool parse_foreign_key_constraint(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::vector<lexeme_t> referencing_columns;

    // We get here after successfully processing a FOREIGN symbol, which must
    // now be followed by the KEY symbol, a list of referencing columns and a
    // <references specification> clause
    if (cur_sym == SYMBOL_KEY) {
        cur_tok = lex.next();
        goto process_referencing_columns;
    }
    goto err_expect_key;
err_expect_key:
    expect_error(ctx, SYMBOL_KEY);
    return false;
process_referencing_columns:
    // We get here if we've successfully parsed the FOREIGN KEY symbols and now
    // expect the parens-enclosed <referencing columns> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        if (! parse_identifier_list(ctx, cur_tok, referencing_columns))
            return false;
        goto expect_references_specification;
    }
    goto err_expect_lparen;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
expect_references_specification:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_REFERENCES) {
        cur_tok = lex.next();
        if (! parse_references_specification(ctx, cur_tok, out))
            return false;
        goto push_constraint;
    }
    goto err_expect_references;
err_expect_references:
    expect_error(ctx, SYMBOL_REFERENCES);
    return false;
push_constraint:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        out->columns = std::move(referencing_columns);
        return true;
    }
}

} // namespace sqltoast
