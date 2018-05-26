/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <search condition> ::=
//     <boolean term>
//     | <search condition> OR <boolean term>
bool parse_search_condition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<search_condition_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<boolean_term_t> term;

    // We get here after getting one of the symbols that precede a search
    // condition's definition, which include the WHERE and HAVING symbols, as
    // well as the AND and OR symbols when constructing compound predicates

    if (! parse_boolean_term(ctx, cur_tok, term))
        return false;
    goto ensure_search_condition;
optional_or:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_OR) {
        cur_tok = lex.next();
        if (! parse_boolean_term(ctx, cur_tok, term))
            return false;
        if (ctx.opts.disable_statement_construction)
            goto optional_or;
        out->terms.emplace_back(std::move(term));
        goto optional_or;
    }
    return true;
ensure_search_condition:
    if (ctx.opts.disable_statement_construction)
        goto optional_or;
    out = std::make_unique<search_condition_t>();
    out->terms.emplace_back(std::move(term));
    goto optional_or;
}

// <boolean term> ::=
//     <boolean factor>
//     | <boolean term> AND <boolean factor>
bool parse_boolean_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<boolean_factor_t> factor;

    if (! parse_boolean_factor(ctx, cur_tok, factor))
        return false;
    goto ensure_term;
optional_and:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AND) {
        cur_tok = lex.next();
        if (! parse_boolean_factor(ctx, cur_tok, factor))
            return false;
        if (! ctx.opts.disable_statement_construction)
            out->and_(factor);
        goto optional_and;
    }
    return true;
ensure_term:
    if (ctx.opts.disable_statement_construction)
        goto optional_and;
    out = std::make_unique<boolean_term_t>(factor);
    goto optional_and;
}

// <boolean factor> ::= [ NOT ] <boolean test>
//
// <boolean test> ::= <boolean primary> [ IS [ NOT ] <truth value> ]
//
// <boolean primary> ::= <predicate> | <left paren> <search condition> <right paren>
bool parse_boolean_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_factor_t>& out) {
    lexer_t& lex = ctx.lexer;
    token_t start_tok = cur_tok;
    parse_position_t start = lex.cursor;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<predicate_t> predicate;
    std::unique_ptr<search_condition_t> search_cond;
    std::unique_ptr<boolean_primary_t> primary;
    bool reverse_op = false; // true when NOT precedes the boolean test

    if (cur_sym == SYMBOL_NOT) {
        cur_tok = lex.next();
        reverse_op = true;
    }
    goto try_predicate;
try_predicate:
    if (parse_predicate(ctx, cur_tok, predicate))
        goto push_factor;
    // rewind and try the nested search condition
    ctx.result.code = PARSE_OK;
    ctx.result.error.assign("");
    cur_tok = start_tok;
    lex.cursor = start;
    goto try_nested_search_condition;
try_nested_search_condition:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        if (! parse_search_condition(ctx, cur_tok, search_cond))
            return false;
        goto expect_rparen;
    }
    return false;
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_factor;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_factor:
    if (ctx.opts.disable_statement_construction)
        return true;

    if (predicate)
        primary = std::make_unique<boolean_primary_t>(predicate);
    else
        primary = std::make_unique<boolean_primary_t>(search_cond);
    out = std::make_unique<boolean_factor_t>(primary, reverse_op);
    return true;
}

// <predicate> ::=
//     <comparison predicate>
//     | <between predicate>
//     | <in predicate>
//     | <like predicate>
//     | <null predicate>
//     | <quantified comparison predicate>
//     | <exists predicate>
//     | <match predicate>
//     | <overlaps predicate>
bool parse_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out) {
    bool reverse_op = false;
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    bool found_not = false;
    std::unique_ptr<row_value_constructor_t> left_most;

    if (parse_row_value_constructor(ctx, cur_tok, left_most)) {
        // The following types of predicates have a row-value constructor as
        // the left-most element of the predicate: comparison predicate,
        // between predicate, in predicate, like predicate, null predicate
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_NOT) {
            found_not = true;
            reverse_op = true;
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
        }
        switch (cur_sym) {
            case SYMBOL_BETWEEN:
                cur_tok = lex.next();
                return parse_between_predicate(ctx, cur_tok, out, left_most, reverse_op);
            case SYMBOL_IN:
                cur_tok = lex.next();
                return parse_in_predicate(ctx, cur_tok, out, left_most, reverse_op);
            case SYMBOL_IS:
                if (found_not)
                    goto err_expected_between_in_or_like;
                cur_tok = lex.next();
                return parse_null_predicate(ctx, cur_tok, out, left_most, reverse_op);
            case SYMBOL_LIKE:
                // TODO(jaypipes): Technically a LIKE predicate does not have a
                // left-most row-value-constructor. Its left-most element is a
                // character value expression. Rework this top-level switching
                // so a character value expression is parsed into the left_most
                // variable.
                cur_tok = lex.next();
                return parse_like_predicate(ctx, cur_tok, out, left_most, reverse_op);
            case SYMBOL_MATCH:
                if (found_not)
                    goto err_expected_between_in_or_like;
                cur_tok = lex.next();
                return parse_match_predicate(ctx, cur_tok, out, left_most);
            case SYMBOL_OVERLAPS:
                if (found_not)
                    goto err_expected_between_in_or_like;
                cur_tok = lex.next();
                return parse_overlaps_predicate(ctx, cur_tok, out, left_most);
            default:
                if (found_not)
                    goto err_expected_between_in_or_like;
                return parse_comparison_predicate(ctx, cur_tok, out, left_most);
        }
    }
    // If a row-value constructor wasn't able to be parsed, look for other
    // symbols representing the other kinds of predicates that don't have a
    // row-value constructor left-most element
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_EXISTS:
            cur_tok = lex.next();
            return parse_exists_predicate(ctx, cur_tok, out);
        case SYMBOL_UNIQUE:
            cur_tok = lex.next();
            return parse_unique_predicate(ctx, cur_tok, out);
        default:
            return false;
    }
err_expected_between_in_or_like:
    expect_any_error(ctx, {SYMBOL_BETWEEN, SYMBOL_IN, SYMBOL_LIKE});
    return false;
}

// <comparison predicate> ::=
//     <row value constructor> <comp op> <row value constructor>
//
// <quantified comparison predicate> ::=
//     <row value constructor> <comp op> <quantifier> <table subquery>
//
// <quantifier> ::= <all> | <some>
//
// <all> ::= ALL
//
// <some> ::= SOME | ANY
bool parse_comparison_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    comp_op_t op = COMP_OP_EQUAL;
    quantifier_t quantifier = QUANTIFIER_NONE;
    std::unique_ptr<row_value_constructor_t> right;
    std::unique_ptr<query_expression_t> subquery;

    // We get here after successfully parsing a row-value constructor and we
    // know that the current symbol is *not* BETWEEN, LIKE, IN, or IS. We now
    // expect to find an operator and a right side row value constructor or
    // table subquery if a quantifier is present.
    switch (cur_sym) {
        case SYMBOL_EQUAL:
            op = COMP_OP_EQUAL;
            cur_tok = lex.next();
            goto optional_quantifier;
        case SYMBOL_NOT_EQUAL:
            op = COMP_OP_NOT_EQUAL;
            cur_tok = lex.next();
            goto optional_quantifier;
        case SYMBOL_LESS_THAN:
            op = COMP_OP_LESS;
            cur_tok = lex.next();
            goto optional_equal;
        case SYMBOL_GREATER_THAN:
            op = COMP_OP_GREATER;
            cur_tok = lex.next();
            goto optional_equal;
        default:
            goto err_expect_operator;
    }
err_expect_operator:
    expect_any_error(ctx, {SYMBOL_EQUAL, SYMBOL_EXCLAMATION, SYMBOL_LESS_THAN, SYMBOL_GREATER_THAN});
    return false;
optional_equal:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_EQUAL) {
        switch (op) {
            case COMP_OP_LESS:
                op = COMP_OP_LESS_EQUAL;
                break;
            case COMP_OP_GREATER:
                op = COMP_OP_GREATER_EQUAL;
                break;
            default:
                break;
        }
        cur_tok = lex.next();
    }
    goto optional_quantifier;
optional_quantifier:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ALL) {
        cur_tok = lex.next();
        quantifier = QUANTIFIER_ALL;
    } else if (cur_sym == SYMBOL_SOME || cur_sym == SYMBOL_ANY) {
        cur_tok = lex.next();
        quantifier = QUANTIFIER_ANY;
    }
    if (quantifier != QUANTIFIER_NONE)
        goto expect_subquery;
    goto expect_right;
expect_right:
    // We get here after successfully parsing the left side of the predicate
    // and the operator and now expect to find the right side of the predicate
    if (! parse_row_value_constructor(ctx, cur_tok, right))
        goto err_expect_right;
    goto push_condition;
err_expect_right:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a << row value constructor >> for "
                "the right side of the comparison predicate." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_subquery:
    // We get here after successfully parsing the left side of the predicate,
    // the operator, a quantifier and now expect to find a table subquery.
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    if (! parse_query_expression(ctx, cur_tok, subquery))
        goto err_expect_subquery;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_condition;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
err_expect_subquery:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a << table subquery >> for "
                "the right side of the quantified comparison predicate."
             << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_condition:
    if (ctx.opts.disable_statement_construction)
        return true;

    if (quantifier == QUANTIFIER_NONE)
        out = std::make_unique<comp_predicate_t>(op, left, right);
    else
        out = std::make_unique<quantified_comparison_predicate_t>(
                op, quantifier, left, subquery);
    return true;
}

// <between predicate> ::=
//     <row value constructor>
//     [ NOT ] BETWEEN <row value constructor> AND <row value constructor>
bool parse_between_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<row_value_constructor_t> comp_left;
    std::unique_ptr<row_value_constructor_t> comp_right;

    // We get here if we've processed the left row value constructor and the
    // [NOT] BETWEEN symbol(s). We now expect a left comparison, an AND symbol,
    // and a right comparison
    if (! parse_row_value_constructor(ctx, cur_tok, comp_left))
        goto err_expect_left_comp;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_AND)
        goto err_expect_and;
    cur_tok = lex.next();
    if (! parse_row_value_constructor(ctx, cur_tok, comp_right))
        goto err_expect_right_comp;
    goto push_condition;
err_expect_and:
    expect_error(ctx, SYMBOL_AND);
    return false;
err_expect_left_comp:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a << row value constructor >> for "
                "the left comparison side of the between predicate." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
err_expect_right_comp:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a << row value constructor >> for "
                "the right comparison side of the between predicate." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_condition:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<between_predicate_t>(left, comp_left, comp_right, reverse_op);
    return true;
}

// <like predicate> ::=
//     <match value> [ NOT ] LIKE <pattern> [ ESCAPE <escape character> ]
//
// <match value> ::=
//     <character value expression>
//
// <pattern> ::=
//     <character value expression>
//
// <escape character> ::=
//     <character value expression>
bool parse_like_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> pattern;
    std::unique_ptr<value_expression_t> escape_char;

    // We get here if we've processed the left row value constructor, an
    // optional NOT symbol and the LIKE symbol(s). We now expect a character
    // value expression followed by an optional ESCAPE keyword with a character
    // value expression representing the escape char.
    if (! parse_character_value_expression(ctx, cur_tok, pattern))
        return false;
    goto optional_escape_char;
optional_escape_char:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ESCAPE) {
        cur_tok = lex.next();
        if (! parse_character_value_expression(ctx, cur_tok, escape_char))
            return false;
    }
    goto push_predicate;
push_predicate:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<like_predicate_t>(left, pattern, escape_char, reverse_op);
    return true;
}

// <null predicate> ::= <row value constructor> IS [ NOT ] NULL
bool parse_null_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;

    // We get here if we've processed the left row value constructor and the
    // IS symbol. We now expect either an optional NOT symbol follwed by the
    // NULL symbol
    if (cur_sym == SYMBOL_NOT) {
        reverse_op = ! reverse_op;
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
    }
    if (cur_sym != SYMBOL_NULL)
        goto err_expect_null;
    cur_tok = lex.next();
    goto push_condition;
err_expect_null:
    expect_error(ctx, SYMBOL_NULL);
    return false;
push_condition:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<null_predicate_t>(left, reverse_op);
    return true;
}

// <in predicate> :=
//     <row value constructor> [ NOT ] IN <in predicate value>
//
// <in predicate value> ::=
//     <table subquery> | <left paren> <in value list> <right paren>
//
// <in value list> ::=
//     <value expression> { <comma> <value expression> } ...
bool parse_in_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> value;
    std::vector<std::unique_ptr<value_expression_t>> values;
    std::unique_ptr<query_expression_t> subq;

    // We get here if we've processed the left row value constructor and the
    // [NOT] IN symbol(s). We now expect a LPAREN followed by either the SELECT
    // symbol or a list of value expressions
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SELECT)
        goto process_subquery;
    goto process_value_list_item;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_subquery:
    if (! parse_query_expression(ctx, cur_tok, subq))
        return false;
    goto expect_rparen;
process_value_list_item:
    if (! parse_value_expression(ctx, cur_tok, value))
        goto err_expect_value_expression;
    values.emplace_back(std::move(value));
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_value_list_item;
    }
    goto expect_rparen;
err_expect_value_expression:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a <value expression> as "
                "an element of the IN operator but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_condition;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_condition:
    if (ctx.opts.disable_statement_construction)
        return true;

    if (! values.empty())
        out = std::make_unique<in_values_predicate_t>(left, values, reverse_op);
    else
        out = std::make_unique<in_subquery_predicate_t>(left, subq, reverse_op);
    return true;
}

// <exists predicate> ::= EXISTS <table subquery>
bool parse_exists_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<query_expression_t> subq;

    // We get here if we've processed the left row value constructor and the
    // [NOT] EXISTS symbol(s). We now expect a parens-enclosed SELECT statement
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    if (! parse_query_expression(ctx, cur_tok, subq))
        goto err_expect_subquery;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_predicate;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
err_expect_subquery:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a <subquery> after EXISTS "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_predicate:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<exists_predicate_t>(subq);
    return true;
}

// <unique predicate> ::= UNIQUE <table subquery>
bool parse_unique_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<query_expression_t> subq;

    // We get here if we've processed the left row value constructor and the
    // [NOT] UNIQUE symbol(s). We now expect a parens-enclosed SELECT statement
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    if (! parse_query_expression(ctx, cur_tok, subq))
        goto err_expect_subquery;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_predicate;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
err_expect_subquery:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find a <subquery> after UNIQUE "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_predicate:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<unique_predicate_t>(subq);
    return true;
}

// <match predicate> ::=
//     <row value constructor>
//     MATCH [ UNIQUE ] [ PARTIAL | FULL ]
//     <table subquery>
bool parse_match_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    bool match_unique = false;
    bool match_partial = false;
    std::unique_ptr<query_expression_t> subq;

    // We get here if we've processed the left row value constructor and the
    // MATCH symbol.
    goto optional_unique;
optional_unique:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_UNIQUE) {
        match_unique = true;
        cur_tok = lex.next();
    }
    goto optional_partial_full;
optional_partial_full:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_PARTIAL) {
        match_partial = true;
        cur_tok = lex.next();
    } else if (cur_sym == SYMBOL_FULL)
        cur_tok = lex.next();
    goto expect_lparen;
expect_lparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_subquery;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_subquery:
    if (! parse_query_expression(ctx, cur_tok, subq))
        return false;
    goto expect_rparen;
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_predicate;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_predicate:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<match_predicate_t>(
            left, match_unique, match_partial, subq);
    return true;
}

// <overlaps predicate> ::=
//     <row value constructor 1> OVERLAPS <row value constructor 2>
bool parse_overlaps_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left) {
    std::unique_ptr<row_value_constructor_t> right;

    // We get here if we've processed the left row value constructor and the
    // OVERLAPS symbol. We now expect a row value constructor
    if (! parse_row_value_constructor(ctx, cur_tok, right))
        goto err_expect_rvc;
    goto push_predicate;
err_expect_rvc:
    {
        std::stringstream estr;
        estr << "Expected to find a <row value constructor> after OVERLAPS "
                "but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_predicate:
    if (ctx.opts.disable_statement_construction)
        return true;

    out = std::make_unique<overlaps_predicate_t>(left, right);
    return true;
}

} // namespace sqltoast
