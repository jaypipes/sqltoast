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
    if (cur_tok.is_literal())
        goto push_literal;
    if (cur_tok.is_identifier())
        goto push_identifier;
    return false;
push_literal:
    {
        ve_type = VALUE_EXPRESSION_TYPE_LITERAL;
        goto push_ve;
    }
push_identifier:
    {
        ve_type = VALUE_EXPRESSION_TYPE_COLUMN;
        goto push_ve;
    }
push_ve:
    out = std::make_unique<value_expression_t>(ve_type, cur_tok.lexeme);
    cur_tok = lex.next();
    return true;
}

} // namespace sqltoast
