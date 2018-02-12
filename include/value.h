/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_VALUE_H
#define SQLTOAST_VALUE_H

#include <ostream>

#include "sqltoast.h"

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
} value_expression_t;

std::ostream& operator<< (std::ostream& out, const value_expression_t& ve);
} // namespace sqltoast

#endif /* SQLTOAST_VALUE_H */
