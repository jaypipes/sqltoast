/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_VALUE_H
#define SQLTOAST_VALUE_H

namespace sqltoast {

// A value expression primary is a variant that can contain one of several
// types of terms that evaluate into a scalar value
typedef enum vep_type {
    VEP_TYPE_UNSIGNED_VALUE_SPECIFICATION,
    VEP_TYPE_COLUMN_REFERENCE,
    VEP_TYPE_SET_FUNCTION_SPECIFICATION,
    VEP_TYPE_SCALAR_SUBQUERY,
    VEP_TYPE_CASE_EXPRESSION,
    VEP_TYPE_VALUE_EXPRESSION,
    VEP_TYPE_CAST_SPECIFICATION
} vep_type_t;

typedef struct value_expression_primary {
    vep_type_t vep_type;
    lexeme_t lexeme;
    value_expression_primary(
            vep_type_t vep_type,
            lexeme_t lexeme) :
        vep_type(vep_type),
        lexeme(lexeme)
    {}
} value_expression_primary_t;

std::ostream& operator<< (std::ostream& out, const value_expression_primary_t& vep);

// unsigned value specifications are unsigned numeric literals, string and
// date/time literals, parameters, variables and some common general
// value-producing keywords like CURRENT_USER
typedef enum uvs_type {
    UVS_TYPE_UNSIGNED_NUMERIC,
    UVS_TYPE_CHARACTER_STRING,
    UVS_TYPE_NATIONAL_CHARACTER_STRING,
    UVS_TYPE_BIT_STRING,
    UVS_TYPE_HEX_STRING,
    UVS_TYPE_DATETIME,
    UVS_TYPE_INTERVAL,
    UVS_TYPE_PARAMETER,
    UVS_TYPE_VARIABLE,
    UVS_TYPE_USER,
    UVS_TYPE_CURRENT_USER,
    UVS_TYPE_SESSION_USER,
    UVS_TYPE_SYSTEM_USER,
    UVS_TYPE_VALUE  // The keyword "VALUE"
} uvs_type_t;

typedef struct unsigned_value_specification : value_expression_primary_t {
    uvs_type_t uvs_type;
    unsigned_value_specification(
            uvs_type_t uvs_type,
            lexeme_t lexeme) :
        value_expression_primary_t(VEP_TYPE_UNSIGNED_VALUE_SPECIFICATION, lexeme),
        uvs_type(uvs_type)
    {}
} unsigned_value_specification_t;

std::ostream& operator<< (std::ostream& out, const unsigned_value_specification_t& uvs);

typedef enum set_function_type {
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

struct value_expression;
typedef struct set_function : value_expression_primary_t {
    set_function_type_t func_type;
    std::unique_ptr<struct value_expression> value;
    set_function(
            set_function_type_t func_type,
            lexeme_t lexeme) :
        value_expression_primary_t(VEP_TYPE_SET_FUNCTION_SPECIFICATION, lexeme),
        func_type(func_type)
    {}
    set_function(
            set_function_type_t func_type,
            lexeme_t lexeme,
            std::unique_ptr<struct value_expression>& value) :
        value_expression_primary_t(VEP_TYPE_SET_FUNCTION_SPECIFICATION, lexeme),
        func_type(func_type),
        value(std::move(value))
    {}
} set_function_t;

std::ostream& operator<< (std::ostream& out, const set_function_t& sf);

// This is a "subexpression" inside a value expression primary
typedef struct value_subexpression : value_expression_primary_t {
    std::unique_ptr<struct value_expression> value;
    value_subexpression(
            std::unique_ptr<struct value_expression>& value,
            lexeme_t lexeme) :
        value_expression_primary_t(VEP_TYPE_VALUE_EXPRESSION, lexeme),
        value(std::move(value))
    {}
} value_subexpression_t;

std::ostream& operator<< (std::ostream& out, const value_subexpression_t& uvs);

// A value expression primary that contains a subquery that evaluates to a
// scalar value
struct statement;
typedef struct scalar_subquery : value_expression_primary_t {
    // Always static_castable to a select_statement_t
    std::unique_ptr<struct statement> subquery;
    scalar_subquery(
            std::unique_ptr<struct statement>& subquery,
            lexeme_t lexeme) :
        value_expression_primary_t(VEP_TYPE_SCALAR_SUBQUERY, lexeme),
        subquery(std::move(subquery))
    {}
} scalar_subquery_t;

std::ostream& operator<< (std::ostream& out, const scalar_subquery_t& uvs);

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
    std::unique_ptr<value_expression_primary_t> value;
    numeric_factor(
            std::unique_ptr<value_expression_primary_t>& value,
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

typedef enum string_function_type {
    STRING_FUNCTION_TYPE_SUBSTRING,
    STRING_FUNCTION_TYPE_UPPER,
    STRING_FUNCTION_TYPE_LOWER,
    STRING_FUNCTION_TYPE_CONVERT,
    STRING_FUNCTION_TYPE_TRANSLATE,
    STRING_FUNCTION_TYPE_TRIM
} string_function_type_t;

typedef struct string_function {
    string_function_type_t type;
    string_function(string_function_type_t type) : type(type)
    {}
} string_function_t;

std::ostream& operator<< (std::ostream& out, const string_function_t& sf);

struct value_expression;
typedef struct substring_function : string_function_t {
    // Guaranteed to be static_castable to a character_value_expression_t
    std::unique_ptr<struct value_expression> operand;
    // Guaranteed to be static_castable to a numeric_value_expression_t
    std::unique_ptr<struct value_expression> start_position_value;
    // Guaranteed to be static_castable to a numeric_value_expression_t
    std::unique_ptr<struct value_expression> for_length_value;
    substring_function(
            std::unique_ptr<struct value_expression>& operand,
            std::unique_ptr<struct value_expression>& start_position_value) :
        string_function_t(STRING_FUNCTION_TYPE_SUBSTRING),
        operand(std::move(operand)),
        start_position_value(std::move(start_position_value))
    {}
    substring_function(
            std::unique_ptr<struct value_expression>& operand,
            std::unique_ptr<struct value_expression>& start_position_value,
            std::unique_ptr<struct value_expression>& for_length_value) :
        string_function_t(STRING_FUNCTION_TYPE_SUBSTRING),
        operand(std::move(operand)),
        start_position_value(std::move(start_position_value)),
        for_length_value(std::move(for_length_value))
    {}
} substring_function_t;

std::ostream& operator<< (std::ostream& out, const substring_function_t& sf);

// A character primary is a value expression primary or a string value function
typedef struct character_primary {
    std::unique_ptr<value_expression_primary_t> value;
    std::unique_ptr<string_function_t> string_function;
    character_primary(
            std::unique_ptr<value_expression_primary_t>& value) :
        value(std::move(value))
    {}
    character_primary(
            std::unique_ptr<string_function_t>& string_function) :
        string_function(std::move(string_function))
    {}
} character_primary_t;

std::ostream& operator<< (std::ostream& out, const character_primary_t& cp);

// A character factor is a character primary with an optional collation.
typedef struct character_factor {
    std::unique_ptr<character_primary_t> value;
    lexeme_t collation;
    character_factor(
            std::unique_ptr<character_primary_t>& value,
            lexeme_t collation) :
        value(std::move(value)),
        collation(collation)
    {}
} character_factor_t;

std::ostream& operator<< (std::ostream& out, const character_factor_t& cf);

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
