/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"
#include "value.h"

namespace sqltoast {

// <row value constructor> ::=
//     <row value constructor element>
//     | <left paren> <row value constructor list> <right paren>
//     | <row subquery>
//
// <row value constructor element> ::=
//     <value expression>
//     | <null specification>
//     | <default specification>
//
// <value expression> ::=
//     <numeric value expression>
//     | <string value expression>
//     | <datetime value expression>
//     | <interval value expression>
//
// <numeric value expression> ::=
//     <term>
//     | <numeric value expression> <plus sign> <term>
//     | <numeric value expression> <minus sign> <term>
//
// <term> ::=
//     <factor>
//     | <term> <asterisk> <factor>
//     | <term> <solidus> <factor>
//
// <factor> ::= [ <sign> ] <numeric primary>
//
// <numeric primary> ::= <value expression primary> | <numeric value function>
//
bool parse_row_value_constructor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    if (parse_value_expression(ctx, cur_tok, out))
        return true;
    return false;
}

// <value expression primary> ::=
//     <unsigned value specification>
//     | <column reference>
//     | <set function specification>
//     | <scalar subquery>
//     | <case expression>
//     | <left paren> <value expression> <right paren>
//     | <cast specification>
//
// <unsigned value specification> ::=
//     <unsigned literal>
//     | <general value specification>
//
// <unsigned literal> ::=
//     <unsigned numeric literal>
//     | <general literal>
//
// <general literal>    ::=
//     <character string literal>
//     | <national character string literal>
//     | <bit string literal>
//     | <hex string literal>
//     | <datetime literal>
//     | <interval literal>
//
// <general value specification> ::=
//     <parameter specification>
//     | <dynamic parameter specification>
//     | <variable specification>
//     | USER
//     | CURRENT_USER
//     | SESSION_USER
//     | SYSTEM_USER
//     | VALUE
bool parse_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    lexer_t& lex = ctx.lexer;
    value_expression_type_t ve_type;
    symbol_t cur_sym = cur_tok.symbol;
    if (cur_tok.is_literal()) {
        ve_type = VALUE_EXPRESSION_TYPE_LITERAL;
        goto push_ve;
    }
    if (cur_tok.is_punctuator() || cur_tok.is_keyword())
        goto check_punc_keywords;
    if (cur_tok.is_identifier()) {
        ve_type = VALUE_EXPRESSION_TYPE_COLUMN;
        goto push_ve;
    }
    return false;
push_literal:
    ve_type = VALUE_EXPRESSION_TYPE_LITERAL;
    goto push_ve;
check_punc_keywords:
    switch (cur_sym) {
        case SYMBOL_LPAREN:
            cur_tok = lex.next();
            goto subquery_or_subexpression;
        case SYMBOL_USER:
        case SYMBOL_CURRENT_USER:
        case SYMBOL_SESSION_USER:
        case SYMBOL_SYSTEM_USER:
        case SYMBOL_VALUE:
            ve_type = VALUE_EXPRESSION_TYPE_GENERAL;
            goto push_ve;
        case SYMBOL_COLON:
            cur_tok = lex.next();
            goto expect_parameter;
        default:
            return false;
    }
subquery_or_subexpression:
    // There are two possible value expressions that follow a LPAREN: scalar
    // subqueries and precedent value expressions (parens-enclosed value
    // expressions where the parens indicates that the value expression should
    // be evaluated before outer value expressions)
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SELECT) {
        cur_tok = lex.next();
        ve_type = VALUE_EXPRESSION_TYPE_SCALAR_SUBQUERY;
        goto expect_rparen_then_push;
    }
    if (! parse_value_expression(ctx, cur_tok, out))
        return false;
    cur_tok = lex.current_token;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    return true; // out is already set to the subexpression...
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
expect_rparen_then_push:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_ve;
expect_parameter:
    // We get here after hitting a COLON. A parameter name is now expected,
    // followed by an optional <indicator parameter> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    ve_type = VALUE_EXPRESSION_TYPE_PARAMETER;
    goto push_ve;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_ve:
    out = std::make_unique<value_expression_t>(ve_type, cur_tok.lexeme);
    cur_tok = lex.next();
    return true;
}

} // namespace sqltoast
