/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_VALUE_H
#define SQLTOAST_VALUE_H

namespace sqltoast {

typedef enum value_expression_type {
    VALUE_EXPRESSION_TYPE_UNKNOWN,
    VALUE_EXPRESSION_TYPE_LITERAL,
    VALUE_EXPRESSION_TYPE_LITERAL_DATE,
    VALUE_EXPRESSION_TYPE_LITERAL_TIME,
    VALUE_EXPRESSION_TYPE_LITERAL_TIMESTAMP,
    VALUE_EXPRESSION_TYPE_LITERAL_INTERVAL,
    VALUE_EXPRESSION_TYPE_PARAMETER,
    VALUE_EXPRESSION_TYPE_VARIABLE,
    VALUE_EXPRESSION_TYPE_GENERAL,
    VALUE_EXPRESSION_TYPE_COLUMN,
    VALUE_EXPRESSION_TYPE_NUMERIC_EXPRESSION,
    VALUE_EXPRESSION_TYPE_STRING_EXPRESSION,
    VALUE_EXPRESSION_TYPE_DATETIME_EXPRESSION,
    VALUE_EXPRESSION_TYPE_INTERVAL_EXPRESSION,
    VALUE_EXPRESSION_TYPE_SET_FUNCTION,
    VALUE_EXPRESSION_TYPE_SCALAR_SUBQUERY,
    VALUE_EXPRESSION_TYPE_CASE,
    VALUE_EXPRESSION_TYPE_CAST
} value_expression_type_t;

// A value expression is anything that can be deduced into a scalar value.  It
// is the most common type of row value constructor element.  A value
// expression may be a column reference, a numeric expression like "2 * 5 +
// col_value", a datetime or interval expression like "date_col - INTERVAL 1
// DAY", a string expression like "CONCAT(some_col, '-', some_other_col)" or
// even the results of a CASE expression or scalar subquery
typedef struct value_expression {
    value_expression_type_t type;
    lexeme_t lexeme;
    value_expression(value_expression_type_t ve_type) :
        type(ve_type)
    {}
    value_expression(value_expression_type_t ve_type, lexeme_t lexeme) :
        type(ve_type),
        lexeme(lexeme)
    {}
} value_expression_t;

std::ostream& operator<< (std::ostream& out, const value_expression_t& ve);

// A numeric value expression is a series of value expressions and literals
// that evaluate to a numeric value. A numeric factor is one of the parts of a
// numeric value expression. A numeric operator applies an operation to two
// factors.
typedef enum numeric_op {
    NUMERIC_OP_NONE,
    NUMERIC_OP_ADD,
    NUMERIC_OP_SUBTRACT,
    NUMERIC_OP_MULTIPLY,
    NUMERIC_OP_DIVIDE
} numeric_op_t;

typedef enum numeric_factor_type {
    NUMERIC_FACTOR_TYPE_VALUE,
    NUMERIC_FACTOR_TYPE_FUNCTION
} numeric_factor_type_t;

typedef struct numeric_factor {
    int sign;
    numeric_factor_type_t type;
    std::unique_ptr<value_expression_t> value;
    numeric_factor(
            std::unique_ptr<value_expression_t>& value,
            int sign) :
        sign(sign),
        type(NUMERIC_FACTOR_TYPE_VALUE),
        value(std::move(value))
    {}
} numeric_factor_t;

typedef struct numeric_term {
    std::unique_ptr<numeric_factor_t> left;
    numeric_op_t op;
    std::unique_ptr<numeric_factor_t> right;
    numeric_term(std::unique_ptr<numeric_factor_t>& left) :
        left(std::move(left)),
        op(NUMERIC_OP_NONE)
    {}
    inline void multiply(std::unique_ptr<numeric_factor_t>& operand) {
        op = NUMERIC_OP_MULTIPLY;
        right = std::move(operand);
    }
    inline void divide(std::unique_ptr<numeric_factor_t>& operand) {
        op = NUMERIC_OP_DIVIDE;
        right = std::move(operand);
    }
} numeric_term_t;

std::ostream& operator<< (std::ostream& out, const numeric_term_t& nt);

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

typedef enum set_function_type {
    SET_FUNCTION_TYPE_UNKNOWN,
    SET_FUNCTION_TYPE_COUNT,
    SET_FUNCTION_TYPE_COUNT_DISTINCT,
    SET_FUNCTION_TYPE_COUNT_STAR,
    SET_FUNCTION_TYPE_AVG,
    SET_FUNCTION_TYPE_AVG_DISTINCT,
    SET_FUNCTION_TYPE_MIN,
    SET_FUNCTION_TYPE_MIN_DISTINCT,
    SET_FUNCTION_TYPE_MAX,
    SET_FUNCTION_TYPE_MAX_DISTINCT,
    SET_FUNCTION_TYPE_SUM,
    SET_FUNCTION_TYPE_SUM_DISTINCT
} set_function_type_t;

typedef struct set_function : value_expression_t {
    set_function_type_t func_type;
    std::unique_ptr<value_expression_t> value;
    set_function(set_function_type_t func_type) :
        value_expression_t(VALUE_EXPRESSION_TYPE_SET_FUNCTION),
        func_type(func_type)
    {}
    set_function(
            set_function_type_t func_type,
            std::unique_ptr<value_expression_t>& value) :
        value_expression_t(VALUE_EXPRESSION_TYPE_SET_FUNCTION, value->lexeme),
        func_type(func_type),
        value(std::move(value))
    {}
} set_function_t;

std::ostream& operator<< (std::ostream& out, const set_function_t& sf);

typedef struct character_value_expression : value_expression_t {
    std::unique_ptr<value_expression_t> value;
    lexeme_t collation;
    character_value_expression(
            std::unique_ptr<value_expression_t>& value,
            lexeme_t collation) :
        value_expression_t(VALUE_EXPRESSION_TYPE_STRING_EXPRESSION, value->lexeme),
        value(std::move(value)),
        collation(collation)
    {}
} character_value_expression_t;

std::ostream& operator<< (std::ostream& out, const character_value_expression_t& ve);

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

#endif /* SQLTOAST_VALUE_H */
