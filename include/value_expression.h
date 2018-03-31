/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_VALUE_EXPRESSION_H
#define SQLTOAST_VALUE_EXPRESSION_H

namespace sqltoast {

// A value expression is anything that can be deduced into a scalar value.  It
// is the most common type of row value constructor element.  A value
// expression may be a column reference, a numeric expression like "2 * 5 +
// col_value", a datetime or interval expression like "date_col - INTERVAL 1
// DAY", a string expression like "CONCAT(some_col, '-', some_other_col)" or
// even the results of a CASE expression or scalar subquery
typedef enum value_expression_type {
    VALUE_EXPRESSION_TYPE_NUMERIC_EXPRESSION,
    VALUE_EXPRESSION_TYPE_STRING_EXPRESSION,
    VALUE_EXPRESSION_TYPE_DATETIME_EXPRESSION,
    VALUE_EXPRESSION_TYPE_INTERVAL_EXPRESSION,
} value_expression_type_t;

typedef struct value_expression {
    value_expression_type_t type;
    value_expression(value_expression_type_t ve_type) :
        type(ve_type)
    {}
} value_expression_t;

std::ostream& operator<< (std::ostream& out, const value_expression_t& ve);

// A numeric value expression is a series of value expressions and literals
// that evaluate to a numeric value. A numeric factor is one of the parts of a
// numeric value expression. A numeric operator applies an operation to two
// factors.
typedef struct numeric_expression : value_expression_t {
    std::unique_ptr<numeric_term_t> left;
    numeric_op_t op;
    std::unique_ptr<numeric_term_t> right;
    numeric_expression(
            std::unique_ptr<numeric_term_t>& left) :
        value_expression_t(VALUE_EXPRESSION_TYPE_NUMERIC_EXPRESSION),
        left(std::move(left)),
        op(NUMERIC_OP_NONE)
    {}
    inline void add(std::unique_ptr<numeric_term_t>& operand) {
        op = NUMERIC_OP_ADD;
        right = std::move(operand);
    }
    inline void subtract(std::unique_ptr<numeric_term_t>& operand) {
        op = NUMERIC_OP_SUBTRACT;
        right = std::move(operand);
    }
} numeric_expression_t;

std::ostream& operator<< (std::ostream& out, const numeric_expression_t& ne);

// A character value expression is composed of one or more character factors
// concatenated together using either the concatenation operator (||) or the
// CONCAT string function
typedef struct character_value_expression : value_expression_t {
    std::vector<std::unique_ptr<character_factor_t>> values;
    character_value_expression(std::vector<std::unique_ptr<character_factor_t>>& values) :
        value_expression_t(VALUE_EXPRESSION_TYPE_STRING_EXPRESSION),
        values(std::move(values))
    {}
} character_value_expression_t;

std::ostream& operator<< (std::ostream& out, const character_value_expression_t& ve);

// A datetime value expression is composed of one or more datetime factors
// that, similar to how a numeric expression produces a numeric value, produce
// a datetime value. datetime factors may only be added to or subtracted from
// each other, which is different from interval factors, which in addition to
// add/subtract, may also be multiplied and divided into each other and with
// datetime factors
//
// A datetime value expression evaluates to a single datetime scalar value. It
// contains a datetime term called "left" that may be added to or subtracted
// from an interval term or interval value expression.
typedef struct datetime_value_expression : value_expression_t {
    std::unique_ptr<datetime_term_t> left;
    numeric_op_t op;
    std::unique_ptr<interval_term_t> right;
    datetime_value_expression(
            std::unique_ptr<datetime_term_t>& left) :
        value_expression_t(VALUE_EXPRESSION_TYPE_DATETIME_EXPRESSION),
        left(std::move(left)),
        op(NUMERIC_OP_NONE)
    {}
    inline void add(std::unique_ptr<interval_term_t>& operand) {
        op = NUMERIC_OP_ADD;
        right = std::move(operand);
    }
    inline void subtract(std::unique_ptr<interval_term_t>& operand) {
        op = NUMERIC_OP_SUBTRACT;
        right = std::move(operand);
    }
} datetime_value_expression_t;

std::ostream& operator<< (std::ostream& out, const datetime_value_expression_t& ve);

// An interval value expression evaluates to an interval value. It may be
// added and subtracted with an interval term and a datetime value expression
// may subtract an interval value expression
//
// An interval value expression evaluates to a single interval scalar value. It
// contains a datetime term called "left" that may be added to or subtracted
// from an interval term or interval value expression.
typedef struct interval_value_expression : value_expression_t {
    std::unique_ptr<interval_term_t> left;
    numeric_op_t op;
    std::unique_ptr<interval_term_t> right;
    interval_value_expression(
            std::unique_ptr<interval_term_t>& left) :
        value_expression_t(VALUE_EXPRESSION_TYPE_INTERVAL_EXPRESSION),
        left(std::move(left)),
        op(NUMERIC_OP_NONE)
    {}
    inline void add(std::unique_ptr<interval_term_t>& operand) {
        op = NUMERIC_OP_ADD;
        right = std::move(operand);
    }
    inline void subtract(std::unique_ptr<interval_term_t>& operand) {
        op = NUMERIC_OP_SUBTRACT;
        right = std::move(operand);
    }
} interval_value_expression_t;

std::ostream& operator<< (std::ostream& out, const interval_value_expression_t& ve);

typedef enum rvc_type {
    RVC_TYPE_UNKNOWN,
    RVC_TYPE_VALUE_EXPRESSION,
    RVC_TYPE_NULL,
    RVC_TYPE_DEFAULT,
    RVC_TYPE_VALUE_LIST,
    RVC_TYPE_ROW_SUBQUERY
} rvc_type_t;

// A row-value constructor is a struct that represents something that can be
// deduced into a row value. Examples of where row-value constructors can be
// found in the SQL grammar include either or both sides of a predicate
// expression or the contents of the VALUES clause
typedef struct row_value_constructor {
    rvc_type_t rvc_type;
    row_value_constructor(rvc_type_t rvc_type) : rvc_type(rvc_type)
    {}
} row_value_constructor_t;

std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc);

typedef struct row_value_expression : row_value_constructor_t {
    std::unique_ptr<value_expression_t> value;
    row_value_expression(std::unique_ptr<value_expression_t>& ve) :
        row_value_constructor_t(RVC_TYPE_VALUE_EXPRESSION),
        value(std::move(ve))
    {}
} row_value_expression_t;

std::ostream& operator<< (std::ostream& out, const row_value_expression_t& rve);

} // namespace sqltoast

#endif /* SQLTOAST_VALUE_EXPRESSION_H */
