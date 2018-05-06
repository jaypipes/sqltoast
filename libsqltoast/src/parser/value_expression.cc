/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <row value constructor> ::=
//     <row value constructor element>
//     | <left paren> <row value constructor list> <right paren>
//     | <row subquery>
bool parse_row_value_constructor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    token_t start_tok = lex.current_token;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<row_value_constructor_t> element;
    std::vector<std::unique_ptr<row_value_constructor_t>> elements;
    if (parse_row_value_constructor_element(ctx, cur_tok, out)) {
        return true;
    }
    // Reset cursor to before parsing of element attempt. Remember that a row
    // value constructor element can also start with a LPAREN, which is why we
    // do this.
    lex.cursor = start;
    lex.current_token = cur_tok = start_tok;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        return false;
    cur_tok = lex.next();
    if (cur_sym == SYMBOL_SELECT) {
        // TODO(jaypipes): handle subquery row value constructors
        return false;
    }
    goto process_rvc_list_element;
process_rvc_list_element:
    if (! parse_row_value_constructor_element(ctx, cur_tok, element))
        goto err_expect_row_value_constructor_element;
    elements.emplace_back(std::move(element));
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_rvc_list_element;
    }
    goto expect_rparen;
err_expect_row_value_constructor_element:
    {
        std::stringstream estr;
        estr << "Expected <row value constructor element> "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_rvc;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_rvc:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<row_value_constructor_list_t>(elements);
    return true;
}

// <row value constructor element> ::=
//     <value expression>
//     | <null specification>
//     | <default specification>
bool parse_row_value_constructor_element(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor_t>& out) {
    std::unique_ptr<value_expression_t> value_exp;
    if (parse_value_expression(ctx, cur_tok, value_exp)) {
        out = std::make_unique<row_value_expression_t>(value_exp);
        return true;
    }
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        lexer_t& lex = ctx.lexer;
        symbol_t cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_NULL) {
            out = std::make_unique<row_value_constructor_element_t>(RVC_ELEMENT_TYPE_NULL);
            cur_tok = lex.next();
            return true;
        } else if (cur_sym == SYMBOL_DEFAULT) {
            out = std::make_unique<row_value_constructor_element_t>(RVC_ELEMENT_TYPE_DEFAULT);
            cur_tok = lex.next();
            return true;
        }
        return false;
    }
}

// <value expression> ::=
//     <numeric value expression>
//     | <string value expression>
//     | <datetime value expression>
//     | <interval value expression>
bool parse_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    token_t start_tok = cur_tok;
    if (parse_numeric_value_expression(ctx, cur_tok, out))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    // Reset our cursor
    lex.cursor = start;
    cur_tok = start_tok;
    if (parse_string_value_expression(ctx, cur_tok, out))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    // Reset our cursor
    lex.cursor = start;
    cur_tok = start_tok;
    if (parse_datetime_value_expression(ctx, cur_tok, out))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    // Reset our cursor
    lex.cursor = start;
    cur_tok = start_tok;
    return parse_interval_value_expression(ctx, cur_tok, out);
}

// <numeric value expression> ::=
//     <term>
//     | <numeric value expression> <plus sign> <term>
//     | <numeric value expression> <minus sign> <term>
bool parse_numeric_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out) {
    lexer& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<numeric_term_t> term;
    std::unique_ptr<numeric_term_t> operand;
    if (! parse_numeric_term(ctx, cur_tok, term))
        return false;
    goto ensure_expression;
optional_operator:
    // Check to see if we've currently got a + or - arithmetic operator as
    // our current symbol. If so, that indicates we should expect to parse
    // another numeric term as an operand to the arithmetic equation.
    cur_sym = cur_tok.symbol;
    if (is_value_expression_terminator(cur_sym))
        return true;
    if (cur_sym == SYMBOL_PLUS) {
        cur_tok = lex.next();
        if (! parse_numeric_term(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_numeric_term;
        }
        if (out) {
            numeric_expression_t* ne = static_cast<numeric_expression_t *>(out.get());
            ne->add(operand);
        }
        goto optional_operator;
    } else if (cur_sym == SYMBOL_MINUS) {
        cur_tok = lex.next();
        if (! parse_numeric_term(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_numeric_term;
        }
        if (out) {
            numeric_expression_t* ne = static_cast<numeric_expression_t *>(out.get());
            ne->subtract(operand);
        }
        goto optional_operator;
    }
    return false;
err_expect_numeric_term:
    {
        std::stringstream estr;
        estr << "Expected <numeric term> after finding numeric operator "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
ensure_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<numeric_expression_t>(term);
    goto optional_operator;
}

// <string value expression> :=
//     <character value expression>
//     | <bit value expression>
bool parse_string_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out) {
    if (parse_character_value_expression(ctx, cur_tok, out))
        return true;
    //if (parse_bit_value_expression(ctx, cur_tok, out))
    //    return true;
    return false;
}

// <character value expression> ::=
//     <concatenation> | <character factor>
//
// <concatenation> ::=
//     <character value expression> <concatenation operator> <character factor>
bool parse_character_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::vector<std::unique_ptr<character_factor>> values;
    std::unique_ptr<character_factor_t> factor;
    if (! parse_character_factor(ctx, cur_tok, factor))
        return false;
    values.emplace_back(std::move(factor));
    goto optional_concat;
optional_concat:
    // Look for terminating symbols or the concatenation operator which
    // indicates to add further character factors to the list of factors in
    // this character value expression
    cur_sym = cur_tok.symbol;
    if (is_value_expression_terminator(cur_sym))
        goto push_ve;
    if (cur_sym == SYMBOL_CONCATENATION) {
        cur_tok = lex.next();
        if (! parse_character_factor(ctx, cur_tok, factor)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_char_factor;
        }
        if (ctx.opts.disable_statement_construction)
            goto optional_concat;
        values.emplace_back(std::move(factor));
        goto optional_concat;
    }
    return false;
err_expect_char_factor:
    {
        std::stringstream estr;
        estr << "Expected <character factor> after concatenation "
                "operator but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_ve:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<character_value_expression_t>(values);
    return true;
}

// <datetime value expression> ::=
//     <datetime term>
//     | <interval value expression> <plus sign> <datetime term>
//     | <datetime value expression> <plus sign> <interval term>
//     | <datetime value expression> <minus sign> <interval term>
bool parse_datetime_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym;
    std::unique_ptr<datetime_term_t> left;
    std::unique_ptr<interval_term_t> operand;
    if (! parse_datetime_term(ctx, cur_tok, left))
        return false;
    goto ensure_expression;
optional_operator:
    // Check to see if we've currently got a + or - arithmetic operator as
    // our current symbol. If so, that indicates we should expect to parse
    // an interval term as an operand to the datetime equation represented by
    // the entire datetime value expression.
    cur_sym = cur_tok.symbol;
    if (is_value_expression_terminator(cur_sym))
        return true;
    if (cur_sym == SYMBOL_PLUS) {
        cur_tok = lex.next();
        if (! parse_interval_term(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_interval_term;
        }
        if (out) {
            datetime_value_expression_t* de =
                static_cast<datetime_value_expression_t *>(out.get());
            de->add(operand);
        }
        goto optional_operator;
    } else if (cur_sym == SYMBOL_MINUS) {
        cur_tok = lex.next();
        if (! parse_interval_term(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_interval_term;
        }
        if (out) {
            datetime_value_expression_t* de =
                static_cast<datetime_value_expression_t *>(out.get());
            de->subtract(operand);
        }
        goto optional_operator;
    }
    return false;
err_expect_interval_term:
    {
        std::stringstream estr;
        estr << "Expected <interval term> after finding numeric operator "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
ensure_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<datetime_value_expression_t>(left);
    goto optional_operator;
}

// <interval value expression> ::=
//     <interval term>
//     | <interval value expression 1> <plus sign> <interval term 1>
//     | <interval value expression 1> <minus sign> <interval term 1>
//     | <left paren> <datetime value expression> <minus sign>
//       <datetime term> <right paren> <interval qualifier>
bool parse_interval_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym;
    std::unique_ptr<interval_term_t> left;
    std::unique_ptr<interval_term_t> operand;
    if (! parse_interval_term(ctx, cur_tok, left))
        return false;
    goto ensure_expression;
optional_operator:
    // Check to see if we've currently got a + or - arithmetic operator as our
    // current symbol. If so, that indicates we should expect to parse an
    // interval term as an operand to the interval equation represented by the
    // entire interval value expression.
    cur_sym = cur_tok.symbol;
    if (is_value_expression_terminator(cur_sym))
        return true;
    if (cur_sym == SYMBOL_PLUS) {
        cur_tok = lex.next();
        if (! parse_interval_term(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_interval_term;
        }
        if (out) {
            interval_value_expression_t* ie =
                static_cast<interval_value_expression_t *>(out.get());
            ie->add(operand);
        }
        goto optional_operator;
    } else if (cur_sym == SYMBOL_MINUS) {
        cur_tok = lex.next();
        if (! parse_interval_term(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_interval_term;
        }
        if (out) {
            interval_value_expression_t* ie =
                static_cast<interval_value_expression_t *>(out.get());
            ie->subtract(operand);
        }
        goto optional_operator;
    }
    return false;
err_expect_interval_term:
    {
        std::stringstream estr;
        estr << "Expected <interval term> after finding numeric operator "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
ensure_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<interval_value_expression_t>(left);
    goto optional_operator;
}

} // namespace sqltoast
