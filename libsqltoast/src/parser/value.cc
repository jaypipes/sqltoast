/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <term> ::=
//     <factor>
//     | <term> <asterisk> <factor>
//     | <term> <solidus> <factor>
bool parse_numeric_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_term_t>& out) {
    lexer& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<numeric_factor_t> factor;
    std::unique_ptr<numeric_factor_t> operand;
    ST_DBG("before parse_numeric_factor(factor). cur_tok is ", cur_tok);
    if (! parse_numeric_factor(ctx, cur_tok, factor)) {
        ST_DBG("parse_numeric_factor(factor) -> false. "
               "returning false from parse_numeric_term()");
        return false;
    }
    ST_DBG("parse_numeric_term(factor) -> true. cur_tok is ", cur_tok);
    goto ensure_term;
optional_operator:
    // Check to see if we've currently got a * or a / arithmetic operator as
    // our current symbol. If so, that indicates we should expect to parse
    // another numeric factor as an operand to the arithmetic equation.
    ST_DBG("check cur_tok ", cur_tok, " is optional operator");
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ASTERISK) {
        cur_tok = lex.next();
        ST_DBG("found ASTERISK operator. expect parse_numeric_factor(operand). "
               "cur_tok is ", cur_tok);
        if (! parse_numeric_factor(ctx, cur_tok, operand)) {
            ST_DBG("parse_numeric_factor(operand) -> false. SYNTAX ERROR.");
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_numeric_factor;
        }
        ST_DBG("parse_numeric_factor(operand) -> true. "
               "returning true from parse_numeric_term()");
        if (out)
            out->multiply(operand);
        return true;
    } else if (cur_sym == SYMBOL_SOLIDUS) {
        cur_tok = lex.next();
        ST_DBG("found SOLIDUS operator. expect parse_numeric_factor(operand). "
               "cur_tok is ", cur_tok);
        if (! parse_numeric_factor(ctx, cur_tok, operand)) {
            ST_DBG("parse_numeric_factor(operand) -> false. SYNTAX ERROR.");
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_numeric_factor;
        }
        ST_DBG("parse_numeric_factor(operand) -> true. "
               "returning true from parse_numeric_term()");
        if (out)
            out->divide(operand);
        return true;
    }
    return true;
err_expect_numeric_factor:
    {
        std::stringstream estr;
        estr << "Expected <numeric factor> after finding numeric operator "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
ensure_term:
    if (ctx.opts.disable_statement_construction)
        goto optional_operator;
    out = std::make_unique<numeric_term_t>(factor);
    goto optional_operator;
}

// <factor> ::= [ <sign> ] <numeric primary>
//
// <numeric primary> ::=
//     <value expression primary>
//     | <numeric value function>
bool parse_numeric_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_factor_t>& out) {
    int8_t sign = 0;
    lexer& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<numeric_primary_t> primary;
    std::unique_ptr<value_expression_primary_t> value;

    // Parse the optional sign...
    if (cur_sym == SYMBOL_PLUS || cur_sym == SYMBOL_MINUS) {
        if (cur_sym == SYMBOL_PLUS)
            sign = 1;
        else
            sign = -1;
        cur_tok = lex.next();
    }
    // try parsing a value expression primary. If not, try parsing a numeric
    // function
    if (parse_value_expression_primary(ctx, cur_tok, value))
        goto push_vep;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (! parse_numeric_function(ctx, cur_tok, primary))
        return false;
    goto push_func;
push_vep:
    if (ctx.opts.disable_statement_construction)
        return true;
    primary = std::make_unique<numeric_value_t>(value);
    goto push_func;
push_func:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<numeric_factor_t>(primary, sign);
    return true;
}

// <value expression primary> ::=
//     <unsigned value specification>
//     | <column reference>
//     | <set function specification>
//     | <scalar subquery>
//     | <case expression>
//     | <left paren> <value expression> <right paren>
//     | <cast specification>
bool parse_value_expression_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t vep_lexeme;
    vep_type_t vep_type;
    symbol_t cur_sym = cur_tok.symbol;
    if (parse_unsigned_value_specification(ctx, cur_tok, out))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (cur_tok.is_identifier()) {
        vep_type = VEP_TYPE_COLUMN_REFERENCE;
        vep_lexeme = cur_tok.lexeme;
        cur_tok = lex.next();
        goto push_ve;
    }
    if (cur_tok.is_punctuator() || cur_tok.is_keyword())
        goto check_punc_keywords;
    return false;
check_punc_keywords:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_LPAREN:
            cur_tok = lex.next();
            goto subquery_or_parenthesized_value_expression;
        case SYMBOL_COUNT:
        case SYMBOL_AVG:
        case SYMBOL_MAX:
        case SYMBOL_MIN:
        case SYMBOL_SUM:
            return parse_set_function(ctx, cur_tok, out);
        case SYMBOL_COALESCE:
        case SYMBOL_NULLIF:
        case SYMBOL_CASE:
            return parse_case_expression(ctx, cur_tok, out);
        default:
            return false;
    }
subquery_or_parenthesized_value_expression:
    // There are two possible value expressions that follow a LPAREN: scalar
    // subqueries and precedent value expressions (parens-enclosed value
    // expressions where the parens indicates that the value expression should
    // be evaluated before outer value expressions)
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SELECT)
        goto process_subquery;
    goto process_parenthesized_value_expression;
process_subquery:
{
    parse_position_t subq_start = cur_tok.lexeme.start;
    std::unique_ptr<query_expression_t> subq;
    if (! parse_query_expression(ctx, cur_tok, subq))
        return false;
    parse_position_t subq_end = cur_tok.lexeme.start - 1;
    vep_lexeme.start = subq_start;
    vep_lexeme.end = subq_end;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<scalar_subquery_t>(subq, vep_lexeme);
    return true;
}
process_parenthesized_value_expression:
{
    parse_position_t inner_val_start = cur_tok.lexeme.start;
    std::unique_ptr<value_expression_t> inner_value;
    if (! parse_value_expression(ctx, cur_tok, inner_value))
        return false;
    parse_position_t inner_val_end = cur_tok.lexeme.start - 1;
    vep_lexeme.start = inner_val_start;
    vep_lexeme.end = inner_val_end;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        return false; // Could be a row value constructor list
    cur_tok = lex.next();
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<parenthesized_value_expression_t>(inner_value, vep_lexeme);
    return true;
}
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_ve:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<value_expression_primary_t>(vep_type, vep_lexeme);
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
        std::unique_ptr<value_expression_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t uvs_lexeme;
    uvs_type_t uvs_type;
    symbol_t cur_sym = cur_tok.symbol;
    if (cur_tok.is_literal()) {
        uvs_type = UVS_TYPE_UNSIGNED_NUMERIC;
        uvs_lexeme = cur_tok.lexeme;
        cur_tok = lex.next();
        goto push_spec;
    }
    switch (cur_sym) {
        case SYMBOL_DATE:
            uvs_type = UVS_TYPE_DATETIME;
            cur_tok = lex.next();
            goto expect_char_string;
        case SYMBOL_TIME:
            uvs_type = UVS_TYPE_DATETIME;
            cur_tok = lex.next();
            goto expect_char_string;
        case SYMBOL_TIMESTAMP:
            uvs_type = UVS_TYPE_DATETIME;
            cur_tok = lex.next();
            goto expect_char_string;
        case SYMBOL_INTERVAL:
            uvs_type = UVS_TYPE_INTERVAL;
            cur_tok = lex.next();
            goto expect_char_string;
        case SYMBOL_USER:
            uvs_type = UVS_TYPE_USER;
            uvs_lexeme = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_spec;
        case SYMBOL_CURRENT_USER:
            uvs_type = UVS_TYPE_CURRENT_USER;
            uvs_lexeme = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_spec;
        case SYMBOL_SESSION_USER:
            uvs_type = UVS_TYPE_SESSION_USER;
            uvs_lexeme = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_spec;
        case SYMBOL_SYSTEM_USER:
            uvs_type = UVS_TYPE_SYSTEM_USER;
            uvs_lexeme = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_spec;
        case SYMBOL_VALUE:
            uvs_type = UVS_TYPE_VALUE;
            uvs_lexeme = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_spec;
        case SYMBOL_COLON:
            cur_tok = lex.next();
            goto expect_parameter;
        case SYMBOL_QUESTION_MARK:
            uvs_type = UVS_TYPE_PARAMETER;
            uvs_lexeme = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_spec;
        default:
            return false;
    }
expect_char_string:
    // We get here if we found the DATE symbol and now we expect a <date
    // string>. A <date string> is a single-quote-enclosed character string
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LITERAL_CHARACTER_STRING)
        goto err_expect_char_string;
    uvs_lexeme = cur_tok.lexeme;
    cur_tok = lex.next();
    goto push_spec;
err_expect_char_string:
    expect_error(ctx, SYMBOL_LITERAL_CHARACTER_STRING);
    return false;
expect_parameter:
    // We get here after hitting a COLON. A parameter name is now expected,
    // followed by an optional <indicator parameter> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    uvs_type = UVS_TYPE_PARAMETER;
    uvs_lexeme = cur_tok.lexeme;
    cur_tok = lex.next();
    // TODO(jaypipes): Maybe support the INDICATOR clause?
    goto push_spec;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_spec:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<unsigned_value_specification_t>(uvs_type, uvs_lexeme);
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
        std::unique_ptr<value_expression_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t sf_start = lex.cursor;
    parse_position_t sf_end;
    set_function_type_t func_type;
    bool star = false;
    bool distinct = false;
    std::unique_ptr<value_expression_t> operand;
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
    star = true;
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
        distinct = true;
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
    sf_end = lex.cursor;
    cur_tok = lex.next();
    goto push_set_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_set_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<set_function_t>(
            func_type, lexeme_t(sf_start, sf_end), star, distinct, operand);
    return true;
}

// <case expression> ::= <case abbreviation> | <case specification>
//
// <case abbreviation> ::=
//     NULLIF <left paren> <value expression>
//     <comma> <value expression> <right paren>
//     | COALESCE <left paren> <value expression>
//     { <comma> <value expression> }... <right paren>
//
// <case specification> ::= <simple case> | <searched case>
bool parse_case_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t case_start = lex.cursor;
    parse_position_t case_end;
    std::unique_ptr<value_expression_t> value;
    std::unique_ptr<value_expression_t> left;
    std::unique_ptr<value_expression_t> right;
    std::vector<std::unique_ptr<value_expression_t>> values;
    symbol_t cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_COALESCE:
            cur_tok = lex.next();
            goto process_coalesce;
        case SYMBOL_NULLIF:
            cur_tok = lex.next();
            goto process_nullif;
        case SYMBOL_CASE:
            cur_tok = lex.next();
            goto process_case;
        default:
            return false;
    }
process_coalesce:
    // We get here if we found the COALESCE symbol, which must be followed by
    // one or more value expressions enclosed by parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_coalesce_argument;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_coalesce_argument:
    if (! parse_value_expression(ctx, cur_tok, value))
        goto err_expect_value_expression;
    values.emplace_back(std::move(value));
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_coalesce_argument;
    }
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    case_end = lex.cursor;
    cur_tok = lex.next();
    goto push_coalesce;
err_expect_value_expression:
    {
        std::stringstream estr;
        estr << "expected <value expression> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
process_nullif:
    // We get here if we found the NULLIF symbol, which must be followed by two
    // comma-separated value expressions enclosed by parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    if (! parse_value_expression(ctx, cur_tok, left))
        goto err_expect_value_expression;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_COMMA)
        goto err_expect_comma;
    cur_tok = lex.next();
    if (! parse_value_expression(ctx, cur_tok, right))
        goto err_expect_value_expression;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    case_end = lex.cursor;
    cur_tok = lex.next();
    goto push_nullif;
err_expect_comma:
    expect_error(ctx, SYMBOL_COMMA);
    return false;
process_case:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_WHEN) {
        cur_tok = lex.next();
        return parse_searched_case_expression(ctx, cur_tok, out);
    }
    return parse_simple_case_expression(ctx, cur_tok, out);
push_coalesce:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<coalesce_function_t>(
            lexeme_t(case_start, case_end), values);
    return true;
push_nullif:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<nullif_function_t>(
            lexeme_t(case_start, case_end), left, right);
    return true;
}

// <simple case> ::=
//     CASE <case operand>
//     <simple when clause> ...
//     [ <else clause> ]
//     END
//
// <case operand> ::= <value expression>
//
// <simple when clause> ::= WHEN <when operand> THEN <result>
//
// <when operand> ::= <value expression>
//
// <result> ::= <result expression> | NULL
//
// <result expression> ::= <value expression>
//
// <else clause> ::= ELSE <result>
bool parse_simple_case_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t case_start = lex.cursor;
    parse_position_t case_end;
    std::unique_ptr<value_expression_t> operand;
    std::vector<simple_case_expression_when_clause_t> when_clauses;
    std::unique_ptr<value_expression_t> else_value;
    std::unique_ptr<value_expression_t> when_operand;
    std::unique_ptr<value_expression_t> when_result;
    symbol_t cur_sym = cur_tok.symbol;
    if (! parse_value_expression(ctx, cur_tok, operand))
        return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_WHEN)
        goto err_expect_when;
    cur_tok = lex.next();
    goto process_simple_when;
process_simple_when:
    // A WHEN clause for a simple case expression is the following:
    //  WHEN <value expression> THEN <value_expression>
    // We have already processed the WHEN symbol at this point.
    if (! parse_value_expression(ctx, cur_tok, when_operand))
        return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_THEN)
        goto err_expect_then;
    cur_tok = lex.next();
    if (! parse_value_expression(ctx, cur_tok, when_result))
        return false;
    when_clauses.emplace_back(
            simple_case_expression_when_clause_t(
                    when_operand, when_result));
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_WHEN) {
        cur_tok = lex.next();
        goto process_simple_when;
    }
    goto process_optional_else;
err_expect_when:
    expect_error(ctx, SYMBOL_WHEN);
    return false;
err_expect_then:
    expect_error(ctx, SYMBOL_THEN);
    return false;
process_optional_else:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ELSE) {
        cur_tok = lex.next();
        if (! parse_value_expression(ctx, cur_tok, else_value))
            return false;
    }
    goto process_end;
process_end:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_END)
        goto err_expect_end;
    cur_tok = lex.next();
    case_end = lex.cursor;
    goto push_simple_case;
err_expect_end:
    expect_error(ctx, SYMBOL_END);
    return false;
push_simple_case:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (else_value)
        out = std::make_unique<simple_case_expression_t>(
                lexeme_t(case_start, case_end), operand, when_clauses, else_value);
    else
        out = std::make_unique<simple_case_expression_t>(
                lexeme_t(case_start, case_end), operand, when_clauses);
    return true;
}

// <searched case> ::=
//     CASE
//     <searched when clause> ...
//     [ <else clause> ]
//     END
//
// <searched when clause> ::=
//     WHEN <search condition>
//     THEN <result>
bool parse_searched_case_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t case_start = lex.cursor;
    parse_position_t case_end;
    std::vector<searched_case_expression_when_clause_t> when_clauses;
    std::unique_ptr<value_expression_t> else_value;
    std::unique_ptr<search_condition_t> when_cond;
    std::unique_ptr<value_expression_t> when_result;
    symbol_t cur_sym = cur_tok.symbol;
    goto process_searched_when;
process_searched_when:
    // A WHEN clause for a searched case expression is the following:
    //  WHEN <search_condition>  THEN <value_expression>
    // We have already processed the WHEN symbol at this point.
    if (! parse_search_condition(ctx, cur_tok, when_cond))
        return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_THEN)
        goto err_expect_then;
    cur_tok = lex.next();
    if (! parse_value_expression(ctx, cur_tok, when_result))
        return false;
    when_clauses.emplace_back(
            searched_case_expression_when_clause_t(
                    when_cond, when_result));
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_WHEN) {
        cur_tok = lex.next();
        goto process_searched_when;
    }
    goto process_optional_else;
err_expect_then:
    expect_error(ctx, SYMBOL_THEN);
    return false;
process_optional_else:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ELSE) {
        cur_tok = lex.next();
        if (! parse_value_expression(ctx, cur_tok, else_value))
            return false;
    }
    goto process_end;
process_end:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_END)
        goto err_expect_end;
    cur_tok = lex.next();
    case_end = lex.cursor;
    goto push_searched_case;
err_expect_end:
    expect_error(ctx, SYMBOL_END);
    return false;
push_searched_case:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (else_value)
        out = std::make_unique<searched_case_expression_t>(
                lexeme_t(case_start, case_end), when_clauses, else_value);
    else
        out = std::make_unique<searched_case_expression_t>(
                lexeme_t(case_start, case_end), when_clauses);
    return true;
}

// <numeric value function> ::=
//     <position expression>
//     | <extract expression>
//     | <length expression>
//
// <position expression> ::=
//     POSITION <left paren> <character value expression>
//     IN <character value expression> <right paren>
//
// <extract expression> ::=
//     EXTRACT <left paren> <extract field>
//     FROM <extract source> <right paren>
//
// <extract field> ::=
//     <datetime field>
//     | <time zone field>
//
// <datetime field> ::=
//     <non-second datetime field>
//     | SECOND
//
// <time zone field> ::=
//     TIMEZONE_HOUR
//     | TIMEZONE_MINUTE
//
// <extract source> ::=
//     <datetime value expression>
//     | <interval value expression>
//
// <length expression> ::=
//     <char length expression>
//     | <octet length expression>
//     | <bit length expression>
//
// <char length expression> ::=
//     { CHAR_LENGTH | CHARACTER_LENGTH }
//     <left paren> <string value expression> <right paren>
//
// <octet length expression> ::=
//     OCTET_LENGTH <left paren> <string value expression> <right paren>
//
// <bit length expression> ::=
//     BIT_LENGTH <left paren> <string value expression> <right paren>
bool parse_numeric_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_primary_t>& out) {
    lexer& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    numeric_function_type_t func_type;
    interval_unit_t extract_field;
    std::unique_ptr<value_expression_t> to_find; // only used for POSITION
    std::unique_ptr<value_expression_t> value;
    switch (cur_sym) {
        case SYMBOL_POSITION:
            cur_tok = lex.next();
            goto process_position_expression;
        case SYMBOL_EXTRACT:
            cur_tok = lex.next();
            goto process_extract_expression;
        case SYMBOL_CHAR_LENGTH:
        case SYMBOL_CHARACTER_LENGTH:
            cur_tok = lex.next();
            func_type = NUMERIC_FUNCTION_TYPE_CHAR_LENGTH;
            goto process_length_expression;
        case SYMBOL_BIT_LENGTH:
            cur_tok = lex.next();
            func_type = NUMERIC_FUNCTION_TYPE_BIT_LENGTH;
            goto process_length_expression;
        case SYMBOL_OCTET_LENGTH:
            cur_tok = lex.next();
            func_type = NUMERIC_FUNCTION_TYPE_OCTET_LENGTH;
            goto process_length_expression;
        default:
            return false;
    }
process_position_expression:
    // We get here after getting the POSITION symbol. We now expect a
    // parens-enclosed <to find> IN <subject>
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    if (! parse_character_value_expression(ctx, cur_tok, to_find)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        goto err_expect_character_value_expression;
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IN)
        goto err_expect_in;
    cur_tok = lex.next();
    if (! parse_character_value_expression(ctx, cur_tok, value)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        goto err_expect_character_value_expression;
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_position_expression;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
err_expect_character_value_expression:
{
    std::stringstream estr;
    estr << "Expected <character value expression> but found " << cur_tok
         << std::endl;
    create_syntax_error_marker(ctx, estr);
    return false;
}
err_expect_in:
    expect_error(ctx, SYMBOL_IN);
    return false;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
process_extract_expression:
    // We get here after getting the EXTRACT symbol. We now expect a
    // parens-enclosed <extract field> FROM <extract source>
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_YEAR:
            extract_field = INTERVAL_UNIT_YEAR;
            break;
        case SYMBOL_MONTH:
            extract_field = INTERVAL_UNIT_MONTH;
            break;
        case SYMBOL_DAY:
            extract_field = INTERVAL_UNIT_DAY;
            break;
        case SYMBOL_HOUR:
            extract_field = INTERVAL_UNIT_DAY;
            break;
        case SYMBOL_MINUTE:
            extract_field = INTERVAL_UNIT_MINUTE;
            break;
        case SYMBOL_SECOND:
            extract_field = INTERVAL_UNIT_SECOND;
            break;
        default:
            goto err_expect_extract_field;
    }
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_FROM)
        goto err_expect_from;
    cur_tok = lex.next();
    // Try to parse the <extract source> element, which can be either a
    // datetime value expression or an interval value expression
    if (! parse_datetime_value_expression(ctx, cur_tok, value)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        if (! parse_interval_value_expression(ctx, cur_tok, value))
            goto err_expect_extract_source;
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_extract_expression;
err_expect_from:
    expect_error(ctx, SYMBOL_FROM);
    return false;
err_expect_extract_field:
{
    std::stringstream estr;
    estr << "Expected <extract field> but found " << cur_tok
         << std::endl;
    create_syntax_error_marker(ctx, estr);
    return false;
}
err_expect_extract_source:
{
    std::stringstream estr;
    estr << "Expected <extract source> which can be a datetime or "
            "interval value expression but found " << cur_tok
         << std::endl;
    create_syntax_error_marker(ctx, estr);
    return false;
}
process_length_expression:
    // We get here after getting a one of the CHAR_LENGTH, BIT_LENGTH or
    // OCTET_LENGTH symbols. We now need to process the required string
    // expression enclosed by parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    if (! parse_string_value_expression(ctx, cur_tok, value))
        goto err_expect_string_value_expression;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_length_expression;
err_expect_string_value_expression:
{
    std::stringstream estr;
    estr << "Expected <string value expression> but found " << cur_tok
         << std::endl;
    create_syntax_error_marker(ctx, estr);
    return false;
}
push_position_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<position_expression_t>(to_find, value);
    return true;
push_extract_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<extract_expression_t>(
            extract_field, value);
    return true;
push_length_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<length_expression_t>(func_type, value);
    return true;
}

// <character factor> ::=
//     <character primary> [ <collate clause> ]
bool parse_character_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<character_factor_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t collation;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<character_primary_t> primary;
    if (! parse_character_primary(ctx, cur_tok, primary))
        return false;
    goto optional_collation;
optional_collation:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COLLATE) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_IDENTIFIER)
           goto err_expect_identifier;
        collation = cur_tok.lexeme;
        cur_tok = lex.next();
    }
    goto push_factor;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_factor:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<character_factor_t>(primary, collation);
    return true;
}

// <character primary> ::=
//     <value expression primary>
//     | <string value function>
bool parse_character_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<character_primary_t>& out) {
    std::unique_ptr<value_expression_primary_t> value_primary;
    std::unique_ptr<string_function_t> string_function;
    if (parse_value_expression_primary(ctx, cur_tok, value_primary))
        goto push_primary;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (! parse_string_function(ctx, cur_tok, string_function))
        return false;
    goto push_primary;
push_primary:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (value_primary)
        out = std::make_unique<character_primary_t>(value_primary);
    else
        out = std::make_unique<character_primary_t>(string_function);
    return true;
}

// <string value function> ::= <character value function> | <bit value function>
//
// <character value function> ::=
//     <character substring function>
//     | <fold>
//     | <form-of-use conversion>
//     | <character translation>
//     | <trim function>
//
// <fold> ::=
//     { UPPER | LOWER }
//     <left paren> <character value expression> <right paren>
bool parse_string_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out) {
    lexer_t& lex = ctx.lexer;
    string_function_type_t func_type;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> operand;
    switch (cur_sym) {
        case SYMBOL_UPPER:
            func_type = STRING_FUNCTION_TYPE_UPPER;
            cur_tok = lex.next();
            goto expect_lparen;
        case SYMBOL_LOWER:
            func_type = STRING_FUNCTION_TYPE_LOWER;
            cur_tok = lex.next();
            goto expect_lparen;
        case SYMBOL_SUBSTRING:
            cur_tok = lex.next();
            return parse_substring_function(ctx, cur_tok, out);
        case SYMBOL_CONVERT:
            cur_tok = lex.next();
            return parse_convert_function(ctx, cur_tok, out);
        case SYMBOL_TRANSLATE:
            cur_tok = lex.next();
            return parse_translate_function(ctx, cur_tok, out);
        case SYMBOL_TRIM:
            cur_tok = lex.next();
            return parse_trim_function(ctx, cur_tok, out);
        default:
            return false;
    }
expect_lparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_operand;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_operand:
    if (! parse_character_value_expression(ctx, cur_tok, operand)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        goto err_expect_operand;
    }
    goto expect_rparen;
err_expect_operand:
    {
        std::stringstream estr;
        estr << "Expected <character value expression> as operand but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<string_function_t>(func_type, operand);
    return true;
}

// <character substring function> ::=
//     SUBSTRING <left paren> <character value expression>
//     FROM <start position>
//     [ FOR <string length> ] <right paren>
//
// <start position> ::= <numeric value expression>
//
// <string length> ::= <numeric value expression>
bool parse_substring_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> operand;
    std::unique_ptr<value_expression_t> start_position_val;
    std::unique_ptr<value_expression_t> for_length_val;

    // The SUBSTRING symbol has already been consumed so we now need the left
    // parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_operand;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_operand:
    if (! parse_character_value_expression(ctx, cur_tok, operand))
       return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_FROM)
       goto err_expect_from;
    cur_tok = lex.next();
    if (! parse_numeric_value_expression(ctx, cur_tok, start_position_val)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        goto err_expect_start_position;
    }
    goto optional_for_length;
err_expect_from:
    expect_error(ctx, SYMBOL_FROM);
    return false;
err_expect_start_position:
    {
        std::stringstream estr;
        estr << "Expected <numeric value expression> after FROM but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
optional_for_length:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_FOR) {
        cur_tok = lex.next();
        if (! parse_numeric_value_expression(ctx, cur_tok, for_length_val)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_numeric_for_length;
        }
    }
    goto expect_rparen;
err_expect_numeric_for_length:
    {
        std::stringstream estr;
        estr << "Expected <numeric value expression> after FOR but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (for_length_val)
        out = std::make_unique<substring_function_t>(
                operand, start_position_val, for_length_val);
    else
        out = std::make_unique<substring_function_t>(
                operand, start_position_val);
    return true;
}

// <form-of-use conversion> ::=
//     CONVERT <left paren> <character value expression>
//     USING <form-of-use conversion name> <right paren>
//
// <form-of-use conversion name> ::= <qualified name>
bool parse_convert_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> operand;
    lexeme_t conversion_name;

    // The CONVERT symbol has already been consumed so we now need the left
    // parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_operand;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_operand:
    if (! parse_character_value_expression(ctx, cur_tok, operand))
       return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_USING)
       goto err_expect_using;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    conversion_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto expect_rparen;
err_expect_using:
    expect_error(ctx, SYMBOL_USING);
    return false;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<convert_function_t>(operand, conversion_name);
    return true;
}

// <character translation> ::=
//     TRANSLATE <left paren> <character value expression>
//     USING <translation name> <right paren>
//
// <translation name> ::= <qualified name>
bool parse_translate_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> operand;
    lexeme_t translation_name;

    // The TRANSLATE symbol has already been consumed so we now need the left
    // parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_operand;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_operand:
    if (! parse_character_value_expression(ctx, cur_tok, operand))
       return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_USING)
       goto err_expect_using;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    translation_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto expect_rparen;
err_expect_using:
    expect_error(ctx, SYMBOL_USING);
    return false;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<translate_function_t>(operand, translation_name);
    return true;
}

// <trim function> ::=
//     TRIM <left paren> <trim operands> <right paren>
//
// <trim operands> ::=
//     [ [ <trim specification> ] [ <trim character> ]
//     FROM ] <trim source>
//
// <trim specification> ::=
//     LEADING
//     | TRAILING
//     | BOTH
//
// <trim character> ::= <character value expression>
//
// <trim source> ::= <character value expression>
bool parse_trim_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> operand;
    std::unique_ptr<value_expression_t> trim_char;
    trim_specification_t trim_spec = TRIM_SPECIFICATION_BOTH;

    // The TRIM symbol has already been consumed so we now need the left
    // parens
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto optional_specification;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
optional_specification:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_TRAILING:
            trim_spec = TRIM_SPECIFICATION_TRAILING;
            cur_tok = lex.next();
            goto expect_trim_character;
        case SYMBOL_LEADING:
            trim_spec = TRIM_SPECIFICATION_LEADING;
            cur_tok = lex.next();
            goto expect_trim_character;
        case SYMBOL_BOTH:
            trim_spec = TRIM_SPECIFICATION_BOTH;
            cur_tok = lex.next();
            goto expect_trim_character;
        default:
            goto process_operand;
    }
expect_trim_character:
    if (! parse_character_value_expression(ctx, cur_tok, trim_char)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        goto err_expect_trim_character;
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_FROM)
        goto err_expect_from;
    cur_tok = lex.next();
    goto process_operand;
err_expect_trim_character:
    {
        std::stringstream estr;
        estr << "Expected <character value expression> after "
                "<trim specification> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
err_expect_from:
    expect_error(ctx, SYMBOL_FROM);
    return false;
process_operand:
    if (! parse_character_value_expression(ctx, cur_tok, operand)) {
        if (ctx.result.code == PARSE_SYNTAX_ERROR)
            return false;
        goto err_expect_operand;
    }
    goto expect_rparen;
err_expect_operand:
    {
        std::stringstream estr;
        estr << "Expected <character value expression> as operand for TRIM "
                "function but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<trim_function_t>(operand, trim_spec, trim_char);
    return true;
}

// <datetime term> ::= <datetime factor>
//
// <datetime primary> ::=
//     <value expression primary>
//     | <datetime value function>
bool parse_datetime_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_term_t>& out) {
    std::unique_ptr<datetime_factor_t> factor;
    if (! parse_datetime_factor(ctx, cur_tok, factor))
        return false;
    goto push_term;
push_term:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<datetime_term_t>(factor);
    return true;
}

// <datetime factor> ::= <datetime primary> [ <time zone> ]
bool parse_datetime_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_factor_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t tz;
    std::unique_ptr<datetime_primary_t> primary;
    if (! parse_datetime_primary(ctx, cur_tok, primary))
        return false;
    goto optional_timezone;
optional_timezone:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AT) {
        cur_tok = lex.next();
        goto expect_timezone_specifier;
    }
    goto push_factor;
expect_timezone_specifier:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_LOCAL:
            cur_tok = lex.next();
            goto push_factor;
        case SYMBOL_TIME:
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
            if (cur_sym != SYMBOL_ZONE)
                goto err_expect_zone;
            cur_tok = lex.next();
            goto expect_tz_name;
        default:
            goto err_expect_local_or_time;
    }
err_expect_zone:
    expect_error(ctx, SYMBOL_ZONE);
    return false;
err_expect_local_or_time:
    expect_any_error(ctx, {SYMBOL_LOCAL, SYMBOL_TIME});
    return false;
expect_tz_name:
    // The ANSI standard says that <time zone specifier> is TIME ZONE
    // <interval value expression> however I've never seen anyone
    // specify anything other than a string timezone literal like
    // 'UTC', so for now we're just going to store the current lexeme
    // as the tz for the datetime factor
    if (! cur_tok.is_literal())
        goto err_expect_tz_name;
    tz = cur_tok.lexeme;
    cur_tok = lex.next();
    goto push_factor;
err_expect_tz_name:
    {
        std::stringstream estr;
        estr << "Expected <time zone name> after AT TIME ZONE but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_factor:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<datetime_factor_t>(primary, tz);
    return true;
}

// <datetime primary> ::=
//     <value expression primary>
//     | <datetime value function>
bool parse_datetime_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_primary_t>& out) {
    std::unique_ptr<value_expression_primary_t> primary;
    if (parse_value_expression_primary(ctx, cur_tok, primary))
        goto push_primary;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    return parse_datetime_function(ctx, cur_tok, out);
push_primary:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<datetime_value_t>(primary);
    return true;
}

// <datetime value function> ::=
//     <current date value function>
//     |     <current time value function>
//     |     <current timestamp value function>
//
// <current date value function> ::= CURRENT_DATE
//
// <current time value function> ::=
//     CURRENT_TIME [ <left paren> <time precision> <right paren> ]
//
// <current timestamp value function> ::=
//     CURRENT_TIMESTAMP [ <left paren> <timestamp precision> <right paren> ]
bool parse_datetime_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    datetime_function_type_t func_type;
    size_t time_precision = 0;

    switch (cur_sym) {
        case SYMBOL_CURRENT_DATE:
            func_type = DATETIME_FUNCTION_TYPE_CURRENT_DATE;
            cur_tok = lex.next();
            goto push_func;
        case SYMBOL_CURRENT_TIME:
            func_type = DATETIME_FUNCTION_TYPE_CURRENT_TIME;
            cur_tok = lex.next();
            goto optional_time_precision;
        case SYMBOL_CURRENT_TIMESTAMP:
            func_type = DATETIME_FUNCTION_TYPE_CURRENT_TIMESTAMP;
            cur_tok = lex.next();
            goto optional_time_precision;
        default:
            return false;
    }
optional_time_precision:
    // We get here after parsing a CURRENT_TIME or CURRENT_TIMESTAMP symbol and
    // we can now get a parens-enclosed time precision specifier
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto push_func;
    cur_tok = lex.next();
    if (! parse_length_specifier(ctx, cur_tok, &time_precision))
        return false;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_func;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_func:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<current_datetime_function_t>(
            func_type, time_precision);
    return true;
}

// <interval term> ::=
//     <interval factor>
//     | <interval term 2> <asterisk> <factor>
//     | <interval term 2> <solidus> <factor>
//     | <term> <asterisk> <interval factor>
bool parse_interval_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_term_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<interval_factor_t> factor;
    std::unique_ptr<numeric_factor_t> operand;
    if (! parse_interval_factor(ctx, cur_tok, factor))
        return false;
    goto ensure_term;
optional_operator:
    // We have already successfully parsed an interval factor and ensured that
    // the out parameter is already an instance of an interval_term_t. At this
    // point, before we return true, we need to check to see if either the
    // asterisk or solidus symbols are the next symbol. If they are, then we
    // must parse a *numeric factor* and apply that parsed numeric factor to
    // the interval term sitting in the out param
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ASTERISK || cur_sym == SYMBOL_SOLIDUS) {
        cur_tok = lex.next();
        if (! parse_numeric_factor(ctx, cur_tok, operand)) {
            if (ctx.result.code == PARSE_SYNTAX_ERROR)
                return false;
            goto err_expect_numeric_factor;
        }
        if (out) {
            if (cur_sym == SYMBOL_ASTERISK)
                out->multiply(operand);
            else
                out->divide(operand);
        }
    }
    return true;
err_expect_numeric_factor:
    {
        std::stringstream estr;
        estr << "Expected <numeric factor> after finding numeric operator "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
ensure_term:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<interval_term_t>(factor);
    goto optional_operator;
}

// <interval factor> ::= [ <sign> ] <interval primary>
bool parse_interval_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_factor_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    int8_t sign = 0;
    std::unique_ptr<interval_primary_t> primary;

    if (cur_sym == SYMBOL_PLUS) {
        cur_tok = lex.next();
        sign = 1;
    } else if (cur_sym == SYMBOL_MINUS) {
        cur_tok = lex.next();
        sign = -1;
    }
    if (! parse_interval_primary(ctx, cur_tok, primary))
        return false;
    goto push_factor;
push_factor:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<interval_factor_t>(sign, primary);
    return true;
}

// <interval primary> ::=
//     <value expression primary> [ <interval qualifier> ]
bool parse_interval_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_primary_t>& out) {
    std::unique_ptr<value_expression_primary_t> primary;
    std::unique_ptr<interval_qualifier_t> qualifier;
    if (! parse_value_expression_primary(ctx, cur_tok, primary))
        return false;
    parse_interval_qualifier(ctx, cur_tok, qualifier);
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    goto push_primary;
push_primary:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<interval_primary_t>(primary, qualifier);
    return true;
}

// <interval qualifier> ::=
//     <start field> TO <end field>
//     | <single datetime field>
//
// <start field> ::=
//     <non-second datetime field>
//     [ <left paren> <interval leading field precision> <right paren> ]
//
// <non-second datetime field> ::= YEAR | MONTH | DAY | HOUR | MINUTE
//
// <interval leading field precision> ::= <unsigned integer>
//
// <end field>    ::=
//     <non-second datetime field>
//     | SECOND
//     [ <left paren> <interval fractional seconds precision> <right paren> ]
//
// <interval fractional seconds precision>    ::=   <unsigned integer>
//
// <single datetime field>    ::=
//     <non-second datetime field>
//     [ <left paren> <interval leading field precision> <right paren> ]
//     | SECOND [ <left paren> <interval leading field precision>
//     [ <comma> <left paren> <interval fractional seconds precision> ]
//     <right paren> ]
bool parse_interval_qualifier(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_qualifier_t>& out) {
    lexer_t& lex = ctx.lexer;
    interval_unit_t start_interval;
    size_t start_precision = 0;
    size_t start_fractional_precision = 0;
    interval_unit_t end_interval;
    size_t end_fractional_precision = 0;
    symbol_t cur_sym = cur_tok.symbol;

    switch (cur_sym) {
        case SYMBOL_YEAR:
            start_interval = INTERVAL_UNIT_YEAR;
            cur_tok = lex.next();
            goto optional_start_leading_precision;
        case SYMBOL_MONTH:
            start_interval = INTERVAL_UNIT_MONTH;
            cur_tok = lex.next();
            goto optional_start_leading_precision;
        case SYMBOL_DAY:
            start_interval = INTERVAL_UNIT_DAY;
            cur_tok = lex.next();
            goto optional_start_leading_precision;
        case SYMBOL_HOUR:
            start_interval = INTERVAL_UNIT_HOUR;
            cur_tok = lex.next();
            goto optional_start_leading_precision;
        case SYMBOL_MINUTE:
            start_interval = INTERVAL_UNIT_MINUTE;
            cur_tok = lex.next();
            goto optional_start_leading_precision;
        case SYMBOL_SECOND:
            start_interval = INTERVAL_UNIT_SECOND;
            cur_tok = lex.next();
            goto optional_start_second_precision;
        default:
            return false;
    }
optional_start_leading_precision:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        if (! parse_length_specifier(ctx, cur_tok, &start_precision))
            return false;
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_RPAREN)
            goto err_expect_rparen;
        cur_tok = lex.next();
    }
    goto optional_to;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
optional_start_second_precision:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        if (! parse_length_specifier(ctx, cur_tok, &start_precision))
            return false;
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_RPAREN) {
            cur_tok = lex.next();
            goto optional_to;
        } else if (cur_sym == SYMBOL_COMMA) {
            cur_tok = lex.next();
            if (! parse_length_specifier(
                    ctx, cur_tok, &start_fractional_precision))
                return false;
            cur_sym = cur_tok.symbol;
            if (cur_sym != SYMBOL_RPAREN)
                goto err_expect_rparen;
            cur_tok = lex.next();
        }
    }
    goto optional_to;
optional_to:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_TO) {
        cur_tok = lex.next();
        goto process_end_field;
    }
    goto push_qualifier;
process_end_field:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_YEAR:
            end_interval = INTERVAL_UNIT_YEAR;
            cur_tok = lex.next();
            goto push_qualifier_with_end;
        case SYMBOL_MONTH:
            end_interval = INTERVAL_UNIT_MONTH;
            cur_tok = lex.next();
            goto push_qualifier_with_end;
        case SYMBOL_DAY:
            end_interval = INTERVAL_UNIT_DAY;
            cur_tok = lex.next();
            goto push_qualifier_with_end;
        case SYMBOL_HOUR:
            end_interval = INTERVAL_UNIT_HOUR;
            cur_tok = lex.next();
            goto push_qualifier_with_end;
        case SYMBOL_MINUTE:
            end_interval = INTERVAL_UNIT_MINUTE;
            cur_tok = lex.next();
            goto push_qualifier_with_end;
        case SYMBOL_SECOND:
            end_interval = INTERVAL_UNIT_SECOND;
            cur_tok = lex.next();
            goto optional_end_second_precision;
        default:
            return false;
    }
optional_end_second_precision:
    // The <end_field> element, if there is a SECOND interval unit, can have a
    // single precision qualifier that indicates the **fractional precision**.
    // The leading precision of a SECONDs <end field> is always 0.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        if (! parse_length_specifier(ctx, cur_tok, &end_fractional_precision))
            return false;
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_RPAREN)
            goto err_expect_rparen;
        cur_tok = lex.next();
    }
    goto push_qualifier_with_end;
push_qualifier:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<interval_qualifier_t>(
                start_interval, start_precision, start_fractional_precision);
    return true;
push_qualifier_with_end:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<interval_qualifier_t>(
                start_interval, start_precision, start_fractional_precision,
                end_interval, end_fractional_precision);
    return true;
}

} // namespace sqltoast
