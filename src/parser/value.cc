/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

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
bool parse_row_value_constructor(
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
            out = std::make_unique<row_value_constructor_t>(RVC_TYPE_NULL);
            cur_tok = lex.next();
            return true;
        } else if (cur_sym == SYMBOL_DEFAULT) {
            out = std::make_unique<row_value_constructor_t>(RVC_TYPE_DEFAULT);
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
    if (parse_numeric_value_expression(ctx, cur_tok, out)) {
        // Check the current symbol. If it's not a terminator like a semicolon,
        // rparen or comma, then reset the cursor and attempt to parse a string
        // value expression...
        symbol_t cur_sym = cur_tok.symbol;
        switch (cur_sym) {
            case SYMBOL_SEMICOLON:
            case SYMBOL_COMMA:
            case SYMBOL_RPAREN:
            case SYMBOL_LPAREN:
            case SYMBOL_EOS:
            case SYMBOL_EQUAL:
            case SYMBOL_EXCLAMATION:
            case SYMBOL_LESS_THAN:
            case SYMBOL_GREATER_THAN:
            case SYMBOL_AND:
            case SYMBOL_OR:
            case SYMBOL_FROM:
            case SYMBOL_WHERE:
            case SYMBOL_HAVING:
            case SYMBOL_GROUP:
                return true;
            default:
                lex.cursor = start;
                cur_tok = start_tok;
        }
    }
    if (parse_string_value_expression(ctx, cur_tok, out))
        return true;
    return false;
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
    if (cur_sym == SYMBOL_PLUS) {
        cur_tok = lex.next();
        if (! parse_numeric_term(ctx, cur_tok, operand))
            return false;
        if (out) {
            numeric_expression_t* ne = static_cast<numeric_expression_t *>(out.get());
            ne->add(operand);
        }
    } else if (cur_sym == SYMBOL_MINUS) {
        cur_tok = lex.next();
        if (! parse_numeric_term(ctx, cur_tok, operand))
            return false;
        if (out) {
            numeric_expression_t* ne = static_cast<numeric_expression_t *>(out.get());
            ne->subtract(operand);
        }
    }
    return true;
ensure_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<numeric_expression_t>(term);
    goto optional_operator;
}

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
    if (! parse_numeric_factor(ctx, cur_tok, factor))
        return false;
    goto ensure_term;
optional_operator:
    // Check to see if we've currently got a * or a / arithmetic operator as
    // our current symbol. If so, that indicates we should expect to parse
    // another numeric factor as an operand to the arithmetic equation.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ASTERISK) {
        cur_tok = lex.next();
        if (! parse_numeric_factor(ctx, cur_tok, operand))
            return false;
        if (out)
            out->multiply(operand);
    } else if (cur_sym == SYMBOL_SOLIDUS) {
        cur_tok = lex.next();
        if (! parse_numeric_factor(ctx, cur_tok, operand))
            return false;
        if (out)
            out->divide(operand);
    }
    return true;
ensure_term:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<numeric_term_t>(factor);
    goto optional_operator;
}

// <factor> ::= [ <sign> ] <numeric primary>
//
// <numeric primary> ::= <value expression primary> | <numeric value function>
bool parse_numeric_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_factor_t>& out) {
    int sign;
    lexer& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_primary_t> value_primary;
    goto optional_sign;
optional_sign:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_PLUS || cur_sym == SYMBOL_MINUS) {
        if (cur_sym == SYMBOL_PLUS)
            sign = 1;
        else
            sign = -1;
        cur_tok = lex.next();
    }
    goto process_value_primary;
process_value_primary:
    if (! parse_value_expression_primary(ctx, cur_tok, value_primary))
        return false;
    goto push_ve;
push_ve:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<numeric_factor_t>(value_primary, sign);
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
    if (cur_tok.is_punctuator() || cur_tok.is_keyword())
        goto check_punc_keywords;
    if (cur_tok.is_identifier()) {
        vep_type = VEP_TYPE_COLUMN_REFERENCE;
        vep_lexeme = cur_tok.lexeme;
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
    if (cur_sym == SYMBOL_SELECT)
        goto process_subquery;
    goto process_subexpression;
process_subquery:
{
    parse_position_t subq_start = cur_tok.lexeme.start;
    std::unique_ptr<statement_t> subq;
    if (! parse_select(ctx, cur_tok, subq))
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
process_subexpression:
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
        goto err_expect_rparen;
    cur_tok = lex.next();
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<value_subexpression_t>(inner_value, vep_lexeme);
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
    sf_end = lex.cursor;
    cur_tok = lex.next();
    goto push_set_function;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_set_function:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (operand)
        out = std::make_unique<set_function_t>(func_type, lexeme_t(sf_start, sf_end), operand);
    else
        out = std::make_unique<set_function_t>(func_type, lexeme_t(sf_start, sf_end));
    return true;
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
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_CONCATENATION) {
        cur_tok = lex.next();
        if (! parse_character_factor(ctx, cur_tok, factor))
            return false;
        if (ctx.opts.disable_statement_construction)
            goto optional_concat;
        values.emplace_back(std::move(factor));
        goto optional_concat;
    }
    goto push_ve;
push_ve:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<character_value_expression_t>(values);
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
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
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
//
// <character translation> ::=
//     TRANSLATE <left paren> <character value expression>
//     USING <translation name> <right paren>
//
// <translation name> ::= <qualified name>
//
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
bool parse_string_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out) {
    lexer_t& lex = ctx.lexer;
    string_function_type_t func_type;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> operand;
    switch (cur_sym) {
        case SYMBOL_SUBSTRING:
            cur_tok = lex.next();
            return parse_substring_function(ctx, cur_tok, out);
        case SYMBOL_UPPER:
            func_type = STRING_FUNCTION_TYPE_UPPER;
            cur_tok = lex.next();
            goto expect_lparen;
        case SYMBOL_LOWER:
            func_type = STRING_FUNCTION_TYPE_LOWER;
            cur_tok = lex.next();
            goto expect_lparen;
        case SYMBOL_CONVERT:
            cur_tok = lex.next();
            return parse_convert_function(ctx, cur_tok, out);
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

} // namespace sqltoast
