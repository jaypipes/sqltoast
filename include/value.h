/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_VALUE_H
#define SQLTOAST_VALUE_H

namespace sqltoast {

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
    lexeme_t lexeme;
    row_value_constructor() : rvc_type(RVC_TYPE_UNKNOWN)
    {}
    row_value_constructor(rvc_type_t rvc_type) : rvc_type(rvc_type)
    {}
    row_value_constructor(lexeme_t lexeme) :
        rvc_type(RVC_TYPE_UNKNOWN),
        lexeme(lexeme)
    {}
    row_value_constructor(lexeme_t lexeme, rvc_type_t type) :
        rvc_type(type),
        lexeme(lexeme)
    {}
} row_value_constructor_t;

std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc);

typedef enum value_expression_type {
    VALUE_EXPRESSION_TYPE_UNKNOWN,
    VALUE_EXPRESSION_TYPE_LITERAL,
    VALUE_EXPRESSION_TYPE_LITERAL_DATE,
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
// col_value", a datetime or interval expression like "date_col <= NOW() -
// INTERVAL 1 DAY", a string expression like "CONCAT(some_col, '-',
// some_other_col)" or even the results of a CASE expression or scalar subquery
typedef struct value_expression : row_value_constructor_t {
    value_expression_type_t type;
    value_expression() :
        row_value_constructor_t(),
        type(VALUE_EXPRESSION_TYPE_UNKNOWN)
    {}
    value_expression(value_expression_type_t ve_type) :
        row_value_constructor_t(RVC_TYPE_VALUE_EXPRESSION),
        type(ve_type)
    {}
    value_expression(value_expression_type_t ve_type, lexeme_t lexeme) :
        row_value_constructor_t(lexeme, RVC_TYPE_VALUE_EXPRESSION),
        type(ve_type)
    {}
} value_expression_t;

std::ostream& operator<< (std::ostream& out, const value_expression_t& ve);

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
    // If set, will always be static_castable to a value_expression_t
    std::unique_ptr<row_value_constructor_t> value;
    set_function(set_function_type_t func_type) :
        value_expression_t(VALUE_EXPRESSION_TYPE_SET_FUNCTION),
        func_type(func_type)
    {}
    set_function(
            set_function_type_t func_type,
            std::unique_ptr<row_value_constructor_t>& value) :
        value_expression_t(VALUE_EXPRESSION_TYPE_SET_FUNCTION, value->lexeme),
        func_type(func_type),
        value(std::move(value))
    {}
} set_function_t;

std::ostream& operator<< (std::ostream& out, const set_function_t& sf);

typedef struct null_value : row_value_constructor_t {
    null_value(lexeme_t lexeme) :
        row_value_constructor_t(lexeme, RVC_TYPE_NULL)
    {}
} null_value_t;

std::ostream& operator<< (std::ostream& out, const null_value_t& ve);

typedef struct default_value : row_value_constructor_t {
    default_value(lexeme_t lexeme) :
        row_value_constructor_t(lexeme, RVC_TYPE_DEFAULT)
    {}
} default_value_t;

std::ostream& operator<< (std::ostream& out, const default_value_t& ve);

} // namespace sqltoast

#endif /* SQLTOAST_VALUE_H */
