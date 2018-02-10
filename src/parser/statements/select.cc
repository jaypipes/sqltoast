/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

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
        ctx.result.statements.emplace_back(std::move(stmt_p));
        return true;
    }
}

} // namespace sqltoast
