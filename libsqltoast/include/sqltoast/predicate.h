/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PREDICATE_H
#define SQLTOAST_PREDICATE_H

namespace sqltoast {

// A predicate is anything that compares one or more columnar values or sets of
// columnar values with each other
typedef enum predicate_type {
    PREDICATE_TYPE_COMPARISON,
    PREDICATE_TYPE_BETWEEN,
    PREDICATE_TYPE_IN_VALUES,
    PREDICATE_TYPE_IN_SUBQUERY,
    PREDICATE_TYPE_LIKE,
    PREDICATE_TYPE_NULL,
    PREDICATE_TYPE_QUANTIFIED_COMPARISON,
    PREDICATE_TYPE_EXISTS,
    PREDICATE_TYPE_UNIQUE,
    PREDICATE_TYPE_MATCH,
    PREDICATE_TYPE_OVERLAPS
} predicate_type_t;

typedef struct predicate {
    predicate_type_t predicate_type;
    predicate(predicate_type_t pred_type) :
        predicate_type(pred_type)
    {}
} predicate_t;

typedef enum comp_op {
    COMP_OP_EQUAL,
    COMP_OP_NOT_EQUAL,
    COMP_OP_LESS,
    COMP_OP_GREATER,
    COMP_OP_LESS_EQUAL,
    COMP_OP_GREATER_EQUAL,
} comp_op_t;

typedef struct comp_predicate : predicate_t {
    comp_op_t op;
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<row_value_constructor_t> right;
    comp_predicate(
            comp_op_t op,
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<row_value_constructor_t>& right) :
        predicate_t(PREDICATE_TYPE_COMPARISON),
        op(op),
        left(std::move(left)),
        right(std::move(right))
    {}
} comp_predicate_t;

typedef struct between_predicate : predicate_t {
    bool reverse_op;
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<row_value_constructor_t> comp_left;
    std::unique_ptr<row_value_constructor_t> comp_right;
    between_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<row_value_constructor_t>& comp_left,
            std::unique_ptr<row_value_constructor_t>& comp_right,
            bool reverse_op) :
        predicate_t(PREDICATE_TYPE_BETWEEN),
        reverse_op(reverse_op),
        left(std::move(left)),
        comp_left(std::move(comp_left)),
        comp_right(std::move(comp_right))
    {}
} between_predicate_t;

typedef struct like_predicate : predicate_t {
    bool reverse_op;
    std::unique_ptr<row_value_constructor_t> match;
    // Guaranteed to always be static_castable to a
    // character_value_expression_t
    std::unique_ptr<value_expression_t> pattern;
    // Guaranteed to always be static_castable to a
    // character_value_expression_t
    std::unique_ptr<value_expression_t> escape_char;
    like_predicate(
            std::unique_ptr<row_value_constructor_t>& match,
            std::unique_ptr<value_expression_t>& pattern,
            std::unique_ptr<value_expression_t>& escape_char,
            bool reverse_op) :
        predicate_t(PREDICATE_TYPE_LIKE),
        reverse_op(reverse_op),
        match(std::move(match)),
        pattern(std::move(pattern)),
        escape_char(std::move(escape_char))
    {}
} like_predicate_t;

typedef struct null_predicate : predicate_t {
    bool reverse_op;
    std::unique_ptr<row_value_constructor_t> left;
    null_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            bool reverse_op) :
        predicate_t(PREDICATE_TYPE_NULL),
        reverse_op(reverse_op),
        left(std::move(left))
    {}
} null_predicate_t;

typedef struct in_values_predicate : predicate_t {
    bool reverse_op;
    std::unique_ptr<row_value_constructor_t> left;
    std::vector<std::unique_ptr<value_expression_t>> values;
    in_values_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::vector<std::unique_ptr<value_expression_t>>& values,
            bool reverse_op) :
        predicate_t(PREDICATE_TYPE_IN_VALUES),
        reverse_op(reverse_op),
        left(std::move(left)),
        values(std::move(values))
    {}
} in_values_predicate_t;

struct query_expression;
typedef struct in_subquery_predicate : predicate_t {
    bool reverse_op;
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<struct query_expression> subquery;
    in_subquery_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<struct query_expression>& subq,
            bool reverse_op) :
        predicate_t(PREDICATE_TYPE_IN_SUBQUERY),
        reverse_op(reverse_op),
        left(std::move(left)),
        subquery(std::move(subq))
    {}
} in_subquery_predicate_t;

typedef enum quantifier {
    QUANTIFIER_NONE,
    QUANTIFIER_ALL,
    QUANTIFIER_ANY
} quantifier_t;

typedef struct quantified_comparison_predicate : predicate_t {
    comp_op_t op;
    quantifier_t quantifier;
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<struct query_expression> subquery;
    quantified_comparison_predicate(
            comp_op_t op,
            quantifier_t quantifier,
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<struct query_expression>& subq) :
        predicate_t(PREDICATE_TYPE_QUANTIFIED_COMPARISON),
        op(op),
        quantifier(quantifier),
        left(std::move(left)),
        subquery(std::move(subq))
    {}
} quantified_comparison_predicate_t;

typedef struct exists_predicate : predicate_t {
    std::unique_ptr<struct query_expression> subquery;
    exists_predicate(std::unique_ptr<struct query_expression>& subq) :
        predicate_t(PREDICATE_TYPE_EXISTS),
        subquery(std::move(subq))
    {}
} exists_predicate_t;

typedef struct unique_predicate : predicate_t {
    // Guaranteed to always be static_castable to a select_t
    std::unique_ptr<struct query_expression> subquery;
    unique_predicate(std::unique_ptr<struct query_expression>& subq) :
        predicate_t(PREDICATE_TYPE_UNIQUE),
        subquery(std::move(subq))
    {}
} unique_predicate_t;

typedef struct match_predicate : predicate_t {
    std::unique_ptr<row_value_constructor_t> left;
    bool match_unique;
    bool match_partial;
    // Guaranteed to always be static_castable to a select_t
    std::unique_ptr<struct query_expression> subquery;
    match_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            bool match_unique,
            bool match_partial,
            std::unique_ptr<struct query_expression>& subq) :
        predicate_t(PREDICATE_TYPE_MATCH),
        left(std::move(left)),
        match_unique(match_unique),
        match_partial(match_partial),
        subquery(std::move(subq))
    {}
} match_predicate_t;

typedef struct overlaps_predicate : predicate_t {
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<row_value_constructor_t> right;
    overlaps_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<row_value_constructor_t>& right) :
        predicate_t(PREDICATE_TYPE_OVERLAPS),
        left(std::move(left)),
        right(std::move(right))
    {}
} overlaps_predicate_t;

// A boolean primary evaluates to a boolean. This could be a simple comparison
// predicate or a complex IN (<subquery>) predicate or it could be a pointer to
// another search_condition_t
struct search_condition;
typedef struct boolean_primary {
    std::unique_ptr<predicate_t> predicate;
    std::unique_ptr<struct search_condition> search_condition;
    boolean_primary(std::unique_ptr<predicate_t>& predicate) :
        predicate(std::move(predicate))
    {}
    boolean_primary(std::unique_ptr<struct search_condition>& search_cond) :
        search_condition(std::move(search_cond))
    {}
} boolean_primary_t;

// A boolean factor is anything that evaluates to a boolean. This could be a
// simple comparison predicate or a complex IN (<subquery>) predicate or it
// could be a pointer to another search_condition_t
typedef struct boolean_factor {
    bool reverse_op;
    std::unique_ptr<boolean_primary_t> primary;
    boolean_factor(std::unique_ptr<boolean_primary_t>& primary, bool reverse_op) :
        reverse_op(reverse_op),
        primary(std::move(primary))
    {}
} boolean_factor_t;

typedef struct boolean_term {
    std::unique_ptr<boolean_factor_t> factor;
    std::unique_ptr<boolean_term> and_operand;
    boolean_term(std::unique_ptr<boolean_factor_t>& factor) :
        factor(std::move(factor))
    {}
    inline boolean_term* and_(std::unique_ptr<boolean_factor_t>& and_factor) {
        boolean_term* next_term = this;
        while (next_term->and_operand)
            next_term = next_term->and_operand.get();
        next_term->and_operand = std::move(std::make_unique<boolean_term>(and_factor));
        return next_term;
    }
} boolean_term_t;

// A container for processing boolean terms found in WHERE and HAVING clause
// conditions
typedef struct search_condition {
    // A collection of boolean terms that are OR'd together
    std::vector<std::unique_ptr<boolean_term_t>> terms;
} search_condition_t;

} // namespace sqltoast

#endif /* SQLTOAST_PREDICATE_H */
