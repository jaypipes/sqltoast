/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast.h"

#include "parser/error.h"
#include "parser/parse.h"
#include "predicate.h"

namespace sqltoast {

// <search condition> ::=
//     <boolean term>
//     | <search condition> OR <boolean term>
bool parse_search_condition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<search_condition_t>& cond_p) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<boolean_term_t> term_p;

    // We get here after getting one of the symbols that precede a search
    // condition's definition, which include the WHERE and HAVING symbols, as
    // well as the AND and OR symbols when constructing compound predicates

    if (parse_boolean_term(ctx, cur_tok, term_p))
        goto optional_or;
    return false;
optional_or:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_OR) {
        cur_tok = lex.next();
        goto ensure_or;
    }
    goto push_condition;
ensure_or:
    if (ctx.opts.disable_statement_construction)
        goto process_or_term;
    if (! cond_p) {
        cond_p = std::make_unique<search_condition_t>();
        cond_p->terms.emplace_back(std::move(term_p));
    }
    goto process_or_term;
process_or_term:
    if (! parse_boolean_term(ctx, cur_tok, term_p))
        return false;
    goto push_or;
push_or:
    if (ctx.opts.disable_statement_construction)
        goto optional_or;
    cond_p->terms.emplace_back(std::move(term_p));
    goto optional_or;
push_condition:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (! cond_p) {
        cond_p = std::make_unique<search_condition_t>();
        cond_p->terms.emplace_back(std::move(term_p));
    }
    return true;
}

// <boolean term> ::=
//     <boolean factor>
//     | <boolean term> AND <boolean factor>
bool parse_boolean_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& term_p) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;

    if (parse_boolean_factor(ctx, cur_tok, term_p))
        goto optional_and;
    return false;
optional_and:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AND) {
        cur_tok = lex.next();
        if (! parse_boolean_factor(ctx, cur_tok, term_p))
            return false;
        goto optional_and;
    }
    goto push_term;
push_term:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        return true;
    }
}

// <boolean factor> ::= [ NOT ] <boolean test>
//
// <boolean test> ::= <boolean primary> [ IS [ NOT ] <truth value> ]
//
// <boolean primary> ::= <predicate> | <left paren> <search condition> <right paren>
bool parse_boolean_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& term_p) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    bool reverse_op = false; // true when NOT precedes a predicate

    if (cur_sym == SYMBOL_NOT) {
        cur_tok = lex.next();
        reverse_op = true;
    }
    return parse_predicate(ctx, cur_tok, term_p, reverse_op);
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
        std::unique_ptr<boolean_term_t>& term_p,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    bool found_not = false;
    std::unique_ptr<row_value_constructor_t> left_most;

    if (parse_row_value_constructor(ctx, cur_tok, left_most)) {
        // The following types of predicates have a row-value constructor as
        // the left-most element of the predicate: comparison predicate,
        // between predicate, in predicate, like predicate, null predicate
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_NOT) {
            found_not = true;
            reverse_op = ! reverse_op;
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
        }
        switch (cur_sym) {
            case SYMBOL_BETWEEN:
                cur_tok = lex.next();
                return parse_between_predicate(ctx, cur_tok, term_p, left_most, reverse_op);
            case SYMBOL_IN:
                cur_tok = lex.next();
                return parse_in_predicate(ctx, cur_tok, term_p, left_most, reverse_op);
            case SYMBOL_IS:
                if (found_not)
                    goto err_expected_between_in_or_like;
                cur_tok = lex.next();
                return parse_null_predicate(ctx, cur_tok, term_p, left_most, reverse_op);
            case SYMBOL_LIKE:
                cur_tok = lex.next();
                return false; //return parse_like_predicate(ctx, cur_tok, term_p, left_most, reverse_op);
            default:
                if (found_not)
                    goto err_expected_between_in_or_like;
                return parse_comparison_predicate(ctx, cur_tok, term_p, left_most, reverse_op);
        }
    }
    // If a row-value constructor wasn't able to be parsed, look for other
    // symbols representing the other kinds of predicates that don't have a
    // row-value constructor left-most element
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_EXISTS:
            return false;  // return parse_exists_predicate(ctx, cur_tok, term_p, reverse_op);
        default:
            return false;
    }
err_expected_between_in_or_like:
    expect_any_error(ctx, {SYMBOL_BETWEEN, SYMBOL_IN, SYMBOL_LIKE});
    return false;
}

// <comparison predicate> ::=
//     <row value constructor> <comp op> <row value constructor>
bool parse_comparison_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& term_p,
        std::unique_ptr<row_value_constructor_t>& left,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    comp_op_t op = COMP_OP_EQUAL;
    std::unique_ptr<row_value_constructor_t> right;

    // We get here after successfully parsing a row-value constructor and we
    // know that the current symbol is *not* BETWEEN, LIKE, IN, or IS. We now
    // expect to find an operator and a right side row value constructor.
    switch (cur_sym) {
        case SYMBOL_EQUAL:
            op = COMP_OP_EQUAL;
            cur_tok = lex.next();
            goto expect_right;
        case SYMBOL_EXCLAMATION:
            op = COMP_OP_NOT_EQUAL;
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
            if (cur_sym != SYMBOL_EQUAL)
                goto err_expect_equal;
            cur_tok = lex.next();
            goto expect_right;
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
err_expect_equal:
    expect_error(ctx, SYMBOL_EQUAL);
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
push_condition:
    if (ctx.opts.disable_statement_construction)
        return true;

    if (! term_p)
        term_p = std::make_unique<comp_predicate_t>(op, left, right, reverse_op);
    else
        term_p->and_term(std::make_unique<comp_predicate_t>(op, left, right, reverse_op));
    return true;
}

// <between predicate> ::=
//     <row value constructor>
//     [ NOT ] BETWEEN <row value constructor> AND <row value constructor>
bool parse_between_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& term_p,
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

    if (! term_p)
        term_p = std::make_unique<between_predicate_t>(left, comp_left, comp_right, reverse_op);
    else
        term_p->and_term(std::make_unique<between_predicate_t>(left, comp_left, comp_right, reverse_op));
    return true;
}

// <null predicate> ::= <row value constructor> IS [ NOT ] NULL
bool parse_null_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& term_p,
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

    if (! term_p)
        term_p = std::make_unique<null_predicate_t>(left, reverse_op);
    else
        term_p->and_term(std::make_unique<null_predicate_t>(left, reverse_op));
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
        std::unique_ptr<boolean_term_t>& term_p,
        std::unique_ptr<row_value_constructor_t>& left,
        bool reverse_op) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::vector<std::unique_ptr<row_value_constructor_t>> values;
    std::unique_ptr<statement_t> subq;

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
    if (! parse_select(ctx, cur_tok, subq))
        return false;
    goto expect_rparen;
process_value_list_item:
    {
        std::unique_ptr<row_value_constructor> value;
        if (! parse_value_expression(ctx, cur_tok, value))
            return false;
        values.emplace_back(std::move(value));
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        std::cout << "got next symbol: " << cur_tok << std::endl;
        if (cur_sym == SYMBOL_COMMA) {
            cur_tok = lex.next();
            goto process_value_list_item;
        }
        goto expect_rparen;
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

    if (! values.empty()) {
        if (! term_p)
            term_p = std::make_unique<in_values_predicate_t>(left, values, reverse_op);
        else
            term_p->and_term(std::make_unique<in_values_predicate_t>(left, values, reverse_op));
    }
    else {
        if (! term_p)
            term_p = std::make_unique<in_subquery_predicate_t>(left, subq, reverse_op);
        else
            term_p->and_term(std::make_unique<in_subquery_predicate_t>(left, subq, reverse_op));
    }
    return true;
}

} // namespace sqltoast
