/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "parser/error.h"
#include "parser/parse.h"

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
bool parse_select(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<query_specification_t> query;

    if (! parse_query_specification(ctx, cur_tok, query))
        return false;
    goto statement_ending;
statement_ending:
    // We get here if we have already successfully processed the SELECT
    // statement and are expecting EOS or SEMICOLON as the next non-comment
    // token. Alternately, we can find an RPAREN, which would indicate we may
    // be at the end of a valid subquery, which is also fine. We rely on the
    // caller to check for the validity of an RPAREN in the current lexical
    // context. Finally, a SELECT, not enclosed by parens, is expected for the
    // CREATE VIEW statement, and the SELECT may be followed by a WITH <level>
    // CHECK OPTION clause. So, we need to terminate with a WITH symbol as
    // well...
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON ||
            cur_sym == SYMBOL_EOS ||
            cur_sym == SYMBOL_RPAREN ||
            cur_sym == SYMBOL_WITH)
        goto push_statement;
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON, SYMBOL_RPAREN});
    return false;
push_statement:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<select_statement_t>(query);
    return true;
}

} // namespace sqltoast
