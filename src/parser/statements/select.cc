/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "derived_column.h"
#include "parser/error.h"
#include "parser/parse.h"
#include "statements/select.h"

namespace sqltoast {

// The SELECT statement follows this EBNF form for the ANSI-92 SQL dialect:
//
// <query specification> ::=
//     SELECT [ <set quantifier> ] <select list> <table expression>
//
// <set quantifier> ::= DISTINCT | ALL
//
// <select list> ::=
//     <asterisk>
//     | <select sublist> [ { <comma> <select sublist> }... ]
//
// <select sublist> ::= <derived column> | <qualifier> <period> <asterisk>
//
// <derived column> ::= <value expression> [ <as clause> ]
//
// <as clause> ::= [ AS ] <column name>
//
// <table expression> ::=
//     <from clause>
//     [ <where clause> ]
//     [ <group by clause> ]
//     [ <having clause> ]

bool parse_select(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    token_t& cur_tok = lex.current_token;
    symbol_t cur_sym = cur_tok.symbol;
    std::vector<derived_column_t> selected_columns;
    bool distinct = false;

    // BEGIN STATE MACHINE

    // We get here after successfully finding the SELECT symbol (the current
    // symbol in the lexer is the SELECT symbol), which can optionally be
    // followed by the DISTINCT or ALL symbol and then the <select list> clause

    cur_tok = lex.next(); // Consume the SELECT symbol...
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_DISTINCT) {
        cur_tok = lex.next();
        distinct = true;
    } else if (cur_sym == SYMBOL_ALL) {
        cur_tok = lex.next();
        distinct = false;
    }
expect_derived_column:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_IDENTIFIER:
            selected_columns.emplace_back(derived_column_t(cur_tok.lexeme));
            cur_tok = lex.next();
            goto optional_column_alias;
        case SYMBOL_ASTERISK:
            selected_columns.emplace_back(derived_column_t(cur_tok.lexeme));
            cur_tok = lex.next();
            goto comma_or_from;
        default:
            goto err_expect_derived_column;
    }
err_expect_derived_column:
    expect_any_error(ctx, {SYMBOL_ASTERISK, SYMBOL_IDENTIFIER});
    return false;
optional_column_alias:
    // We get here after consuming a column identifier or value expression. An
    // alias can be provided for this column either by specifying an identifier
    // immediately after the value expression or the keyword AS followed by the
    // alias.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AS) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_IDENTIFIER)
            goto err_expect_identifier;
    }
    if (cur_sym == SYMBOL_IDENTIFIER) {
        derived_column_t& dc = selected_columns.back();
        dc.alias = cur_tok.lexeme;
        cur_tok = lex.next();
    }
    goto comma_or_from;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
comma_or_from:
    // We get here after consuming a derived column and now we expect to find
    // either a comma, indicating another derived column should be next, or the
    // FROM symbol, indicating the end of the <select list> clause.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto expect_derived_column;
    }
    if (cur_sym != SYMBOL_FROM)
        goto err_expect_comma_or_from;
    cur_tok = lex.next();
    goto from_clause;
err_expect_comma_or_from:
    expect_any_error(ctx, {SYMBOL_COMMA, SYMBOL_FROM});
    return false;
from_clause:
    // TODO
    goto statement_ending;
statement_ending:
    // We get here if we have already successfully processed the SELECT
    // statement and are expecting EOS or SEMICOLON as the next non-comment
    // token
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS)
        goto push_statement;
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON});
    return false;
push_statement:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        auto stmt_p = std::make_unique<statements::select_t>();
        stmt_p->distinct = distinct;
        stmt_p->selected_columns = std::move(selected_columns);
        ctx.result.statements.emplace_back(std::move(stmt_p));
        return true;
    }
}

} // namespace sqltoast
