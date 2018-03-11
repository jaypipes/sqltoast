/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PREDICATE_H
#define SQLTOAST_PREDICATE_H

namespace sqltoast {

typedef enum comp_op {
    COMP_OP_EQUAL,
    COMP_OP_NOT_EQUAL,
    COMP_OP_LESS,
    COMP_OP_GREATER,
    COMP_OP_LESS_EQUAL,
    COMP_OP_GREATER_EQUAL,
    COMP_OP_BETWEEN,
    COMP_OP_IN_VALUES,
    COMP_OP_IN_SUBQUERY,
    COMP_OP_LIKE,
    COMP_OP_NULL,
    COMP_OP_EXISTS,
    COMP_OP_UNIQUE
} comp_op_t;

// A boolean factor is anything that evaluates to a boolean. This could be a
// simple comparison predicate or a complex IN (<subquery>) predicate or it
// could be a pointer to another search_condition_t
typedef struct boolean_factor {
    comp_op_t op;
    bool reverse_op;
    boolean_factor(comp_op_t op, bool reverse_op) :
        op(op),
        reverse_op(reverse_op)
    {}
} boolean_factor_t;

typedef struct comp_predicate : boolean_factor_t {
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<row_value_constructor_t> right;
    comp_predicate(
            comp_op_t op,
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<row_value_constructor_t>& right,
            bool reverse_op) :
        boolean_factor_t(op, reverse_op),
        left(std::move(left)),
        right(std::move(right))
    {}
} comp_predicate_t;

std::ostream& operator<< (std::ostream& out, const comp_predicate_t& pred);

typedef struct between_predicate : boolean_factor_t {
    std::unique_ptr<row_value_constructor_t> left;
    std::unique_ptr<row_value_constructor_t> comp_left;
    std::unique_ptr<row_value_constructor_t> comp_right;
    between_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<row_value_constructor_t>& comp_left,
            std::unique_ptr<row_value_constructor_t>& comp_right,
            bool reverse_op) :
        boolean_factor_t(COMP_OP_BETWEEN, reverse_op),
        left(std::move(left)),
        comp_left(std::move(comp_left)),
        comp_right(std::move(comp_right))
    {}
} between_predicate_t;

std::ostream& operator<< (std::ostream& out, const between_predicate_t& pred);

typedef struct null_predicate : boolean_factor_t {
    std::unique_ptr<row_value_constructor_t> left;
    null_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            bool reverse_op) :
        boolean_factor_t(COMP_OP_NULL, reverse_op),
        left(std::move(left))
    {}
} null_predicate_t;

std::ostream& operator<< (std::ostream& out, const null_predicate_t& pred);

typedef struct in_values_predicate : boolean_factor_t {
    std::unique_ptr<row_value_constructor_t> left;
    std::vector<std::unique_ptr<value_expression_t>> values;
    in_values_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::vector<std::unique_ptr<value_expression_t>>& values,
            bool reverse_op) :
        boolean_factor_t(COMP_OP_IN_VALUES, reverse_op),
        left(std::move(left)),
        values(std::move(values))
    {}
} in_values_predicate_t;

std::ostream& operator<< (std::ostream& out, const in_values_predicate_t& pred);

typedef struct statement statement_t;
typedef struct in_subquery_predicate : boolean_factor_t {
    std::unique_ptr<row_value_constructor_t> left;
    // Guaranteed to always be static_castable to a select_t
    std::unique_ptr<statement_t> subquery;
    in_subquery_predicate(
            std::unique_ptr<row_value_constructor_t>& left,
            std::unique_ptr<statement_t>& subq,
            bool reverse_op) :
        boolean_factor_t(COMP_OP_IN_SUBQUERY, reverse_op),
        left(std::move(left)),
        subquery(std::move(subq))
    {}
} in_subquery_predicate_t;

std::ostream& operator<< (std::ostream& out, const in_subquery_predicate_t& pred);

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

std::ostream& operator<< (std::ostream& out, const boolean_term_t& bt);

// A container for processing boolean terms found in WHERE and HAVING clause
// conditions
typedef struct search_condition {
    // A collection of boolean terms that are OR'd together
    std::vector<std::unique_ptr<boolean_term_t>> terms;
} search_condition_t;

std::ostream& operator<< (std::ostream& out, const search_condition_t& sc);
} // namespace sqltoast

#endif /* SQLTOAST_PREDICATE_H */
