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
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        lexer_t& lex = ctx.lexer;
        symbol_t cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_NULL) {
            out = std::make_unique<null_value_t>(cur_tok.lexeme);
            cur_tok = lex.next();
            return true;
        } else if (cur_sym == SYMBOL_DEFAULT) {
            out = std::make_unique<default_value_t>(cur_tok.lexeme);
            cur_tok = lex.next();
            return true;
        }
        return false;
    }
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
bool parse_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t ve_lexeme;
    value_expression_type_t ve_type;
    symbol_t cur_sym = cur_tok.symbol;
    if (parse_unsigned_value_specification(ctx, cur_tok, out))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (cur_tok.is_punctuator() || cur_tok.is_keyword())
        goto check_punc_keywords;
    if (cur_tok.is_identifier()) {
        ve_type = VALUE_EXPRESSION_TYPE_COLUMN;
        ve_lexeme = cur_tok.lexeme;
        cur_tok = lex.next();
        goto push_ve;
    }
    return false;
check_punc_keywords:
    switch (cur_sym) {
        case SYMBOL_LPAREN:
            cur_tok = lex.next();
            goto subquery_or_subexpression;
        case SYMBOL_COUNT:
        case SYMBOL_AVG:
        case SYMBOL_MAX:
        case SYMBOL_MIN:
        case SYMBOL_SUM:
            return parse_set_function(ctx, cur_tok, out);
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
    ve_lexeme = out->lexeme;
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
push_ve:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<value_expression_t>(ve_type, ve_lexeme);
    return true;
}

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
bool parse_unsigned_value_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t ve_lexeme;
    value_expression_type_t ve_type;
    symbol_t cur_sym = cur_tok.symbol;
    if (cur_tok.is_literal()) {
        ve_type = VALUE_EXPRESSION_TYPE_LITERAL;
        ve_lexeme = cur_tok.lexeme;
        cur_tok = lex.next();
        goto push_spec;
    }
    switch (cur_sym) {
        case SYMBOL_USER:
        case SYMBOL_CURRENT_USER:
        case SYMBOL_SESSION_USER:
        case SYMBOL_SYSTEM_USER:
        case SYMBOL_VALUE:
            ve_type = VALUE_EXPRESSION_TYPE_GENERAL;
            ve_lexeme = cur_tok.lexeme;
            goto push_spec;
        case SYMBOL_COLON:
            cur_tok = lex.next();
            goto expect_parameter;
        case SYMBOL_QUESTION_MARK:
            ve_type = VALUE_EXPRESSION_TYPE_PARAMETER;
            ve_lexeme = cur_tok.lexeme;
            goto push_spec;
        default:
            return false;
    }
expect_parameter:
    // We get here after hitting a COLON. A parameter name is now expected,
    // followed by an optional <indicator parameter> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    ve_type = VALUE_EXPRESSION_TYPE_PARAMETER;
    ve_lexeme = cur_tok.lexeme;
    cur_tok = lex.next();
    // TODO(jaypipes): Maybe support the INDICATOR clause?
    goto push_spec;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_spec:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<value_expression_t>(ve_type, ve_lexeme);
    return true;
}

// <set function specification> ::=
//     COUNT <left paren> <asterisk> <right paren>
//     | <general set function>
//
// <general set function> ::=
//     <set function type> <left paren> [ <set quantifier> ] <value expression> <right paren>
//
// <set function type> ::= AVG | MAX | MIN | SUM | COUNT
//
// <set quantifier> ::= DISTINCT | ALL
bool parse_set_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    lexer_t& lex = ctx.lexer;
    set_function_type_t func_type;
    std::unique_ptr<row_value_constructor_t> operand;
    symbol_t cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_COUNT:
            func_type = SET_FUNCTION_TYPE_COUNT;
            cur_tok = lex.next();
            goto process_count_function;
        case SYMBOL_AVG:
            func_type = SET_FUNCTION_TYPE_AVG;
            cur_tok = lex.next();
            goto process_set_function;
        case SYMBOL_MAX:
            func_type = SET_FUNCTION_TYPE_MAX;
            cur_tok = lex.next();
            goto process_set_function;
        case SYMBOL_MIN:
            func_type = SET_FUNCTION_TYPE_MIN;
            cur_tok = lex.next();
            goto process_set_function;
        case SYMBOL_SUM:
            func_type = SET_FUNCTION_TYPE_SUM;
            cur_tok = lex.next();
            goto process_set_function;
        default:
            return false;
    }
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_count_function:
    // We get here if we found the COUNT symbol. The COUNT() set function is
    // the only set function that can take the form COUNT -> LPAREN -> ASTERISK
    // -> RPAREN, so we handle this specially.
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ASTERISK) {
        cur_tok = lex.next();
        goto process_count_asterisk;
    }
    goto optional_set_quantifier;
process_count_asterisk:
    // We get here after finding COUNT followed by LPAREN followed by
    // ASTERISK.  Ensure the next symbol is an RPAREN and then create a
    // value_expression_t for the operand of '*';
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    func_type = SET_FUNCTION_TYPE_COUNT_STAR;
    goto push_set_function;
process_set_function:
    // We get here if we found one of the non-COUNT set functions (AVG, SUM,
    // MIN, and MAX). These can be followed by a LPAREN, an optional set
    // quantifier, a value expression and then a RPAREN
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto optional_set_quantifier;
optional_set_quantifier:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_DISTINCT || cur_sym == SYMBOL_ALL) {
        if (cur_sym == SYMBOL_ALL) {
            cur_tok = lex.next();
            goto process_operand;
        }
        switch (func_type) {
            case SET_FUNCTION_TYPE_COUNT:
                func_type = SET_FUNCTION_TYPE_COUNT_DISTINCT;
                break;
            case SET_FUNCTION_TYPE_MAX:
                func_type = SET_FUNCTION_TYPE_MAX_DISTINCT;
                break;
            case SET_FUNCTION_TYPE_MIN:
                func_type = SET_FUNCTION_TYPE_MIN_DISTINCT;
                break;
            case SET_FUNCTION_TYPE_AVG:
                func_type = SET_FUNCTION_TYPE_AVG_DISTINCT;
                break;
            case SET_FUNCTION_TYPE_SUM:
                func_type = SET_FUNCTION_TYPE_SUM_DISTINCT;
                break;
            default:
                return false; // should be unreachable
        }
        cur_tok = lex.next();
    }
    goto process_operand;
process_operand:
    if (! parse_value_expression(ctx, cur_tok, operand))
        return false;
    cur_tok = lex.current_token;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_set_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_set_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (operand)
        out = std::make_unique<set_function_t>(func_type, operand);
    else
        out = std::make_unique<set_function_t>(func_type);
    return true;
}

} // namespace sqltoast
