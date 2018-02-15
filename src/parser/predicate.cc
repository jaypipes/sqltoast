/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"
#include "predicate.h"

namespace sqltoast {

// <search condition> ::=
//     <boolean term>
//     | <search condition> OR <boolean term>
//
// <boolean term> ::=
//     <boolean factor>
//     | <boolean term> AND <boolean factor>
//
// <boolean factor> ::= [ NOT ] <boolean test>
//
// <boolean test> ::= <boolean primary> [ IS [ NOT ] <truth value> ]
//
// <boolean primary> ::= <predicate> | <left paren> <search condition> <right paren>
//
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
bool parse_search_condition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::vector<std::unique_ptr<search_condition_t>>& conditions) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    bool reverse_op = false; // true when NOT precedes a predicate
    std::unique_ptr<search_condition_t> cond_p;

    // We get here after getting one of the symbols that precede a search
    // condition's definition, which include the WHERE and HAVING symbols, as
    // well as the AND and OR symbols when constructing compound predicates

    if (cur_sym == SYMBOL_NOT) {
        cur_tok = lex.next();
        reverse_op = true;
    }
    goto process_left;
process_left:
    if (cur_tok.is_literal()  || cur_tok.is_identifier()) {
        // A number of different types of predicates start with an identifier
        // or literal, including simple comparison predicates, between
        // predicates, in predicates, etc.
        //
        // NOTE(jaypipes): We deliberately don't advance the cursor with a call
        // to lex.next() here because the current token is pointing at an
        // identifier, and the parse_xxx_predicate() functions will use this
        // identifier as their "left" attributes for the constructed predicate
        // structs.
        if (! parse_comparison_predicate(ctx, cur_tok, cond_p))
            return false;
        goto push_condition;
    }
    return false;
push_condition:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        cond_p->reverse_op = reverse_op;
        conditions.emplace_back(std::move(cond_p));
        return true;
    }
}

// <comparison predicate> ::=
//     <row value constructor> <comp op> <row value constructor>
bool parse_comparison_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<search_condition_t>& cond_p) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    comp_op_t op = COMP_OP_EQUAL;
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<row_value_constructor_t> right;

    // We get here after getting an identifier or literal while trying to parse
    // a search condition. This identifier or literal can be followed by one of
    // the comparison operators and then an identifier for the right side of
    // the comparison operation. The cursor is currently pointing at the left
    // side of the predicate.

    if (! parse_row_value_constructor(ctx, cur_tok, left))
        return false;

    // If we get here, the left side of the predicate has been consumed and the
    // row value constructor for the left side has been filled. We now expect
    // to find an operator and a right side row value constructor.
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_EQUAL:
            op = COMP_OP_EQUAL;
            cur_tok = lex.next();
            goto expect_right;
        default:
            return false;
    }
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

    cond_p = std::make_unique<comp_predicate_t>(op, left, right);
    return true;
}

} // namespace sqltoast

