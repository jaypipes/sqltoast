/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "parser/parse.h"
#include "parser/error.h"
#include "parser/token.h"

namespace sqltoast {

//
// Parses a table constraint, which follows this EBNF grammar for ANSI-92:
//
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
        std::vector<std::unique_ptr<constraint_t>>& constraints) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<identifier_t> constraint_name_p;
    std::unique_ptr<constraint_t> constraint_p;

    // BEGIN STATE MACHINE

    if (cur_sym == SYMBOL_CONSTRAINT) {
        cur_tok = ctx.lexer.next();
        goto expect_constraint_name;
    }
    goto expect_constraint_type;
expect_constraint_name:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    constraint_name_p = std::make_unique<identifier_t>(cur_tok.lexeme);
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
            constraint_p = std::move(std::make_unique<unique_constraint_t>(false));
            goto expect_col_list;
        case SYMBOL_PRIMARY:
            cur_tok = ctx.lexer.next();
            goto expect_key;
        case SYMBOL_FOREIGN:
        case SYMBOL_CHECK:
            // TODO
        default:
            if (! constraint_name_p.get())
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
    constraint_p = std::move(std::make_unique<unique_constraint_t>(true));
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
    constraint_p->columns.emplace_back(identifier_t(cur_tok.lexeme));
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
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        if (constraint_name_p.get())
            constraint_p->name = std::move(constraint_name_p);
        constraints.emplace_back(std::move(constraint_p));
        return true;
    }
}

} // namespace sqltoast
