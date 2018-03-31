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

typedef enum numeric_primary_type {
    NUMERIC_PRIMARY_TYPE_VALUE,
    NUMERIC_PRIMARY_TYPE_FUNCTION
} numeric_primary_type_t;

typedef struct numeric_primary {
    numeric_primary_type_t type;
    numeric_primary(numeric_primary_type_t type) :
        type(type)
    {}
} numeric_primary_t;

std::ostream& operator<< (std::ostream& out, const numeric_primary_t& np);

typedef struct numeric_value : numeric_primary_t {
    std::unique_ptr<value_expression_primary_t> value;
    numeric_value(std::unique_ptr<value_expression_primary_t>& value) :
        numeric_primary_t(NUMERIC_PRIMARY_TYPE_VALUE),
        value(std::move(value))
    {}
} numeric_value_t;

std::ostream& operator<< (std::ostream& out, const numeric_value_t& nv);

typedef enum numeric_function_type {
    NUMERIC_FUNCTION_TYPE_UNKNOWN,
    NUMERIC_FUNCTION_TYPE_POSITION,
    NUMERIC_FUNCTION_TYPE_EXTRACT,
    NUMERIC_FUNCTION_TYPE_CHAR_LENGTH,
    NUMERIC_FUNCTION_TYPE_OCTET_LENGTH,
    NUMERIC_FUNCTION_TYPE_BIT_LENGTH
} numeric_function_type_t;

typedef struct numeric_function : numeric_primary_t {
    numeric_function_type_t type;
    numeric_function(numeric_function_type_t type) :
        numeric_primary_t(NUMERIC_PRIMARY_TYPE_FUNCTION),
        type(type)
    {}
} numeric_function_t;

std::ostream& operator<< (std::ostream& out, const numeric_function_t& nf);

typedef struct position_expression : numeric_function_t {
    // Will always be static_castable to character_value_expression_t
    std::unique_ptr<struct value_expression> to_find;
    // Will always be static_castable to character_value_expression_t
    std::unique_ptr<struct value_expression> subject;
    position_expression(
            std::unique_ptr<struct value_expression>& to_find,
            std::unique_ptr<struct value_expression>& subject) :
        numeric_function_t(NUMERIC_FUNCTION_TYPE_POSITION),
        to_find(std::move(to_find)),
        subject(std::move(subject))
    {}
} position_expression_t;

std::ostream& operator<< (std::ostream& out, const position_expression_t& pe);

typedef struct extract_expression : numeric_function_t {
    interval_unit_t extract_field;
    // Will always be static_castable to either a datetime_value_expression_t
    // or an interval_value_expression_t
    std::unique_ptr<struct value_expression> extract_source;
    extract_expression(
            interval_unit_t extract_field,
            std::unique_ptr<struct value_expression>& extract_source) :
        numeric_function_t(NUMERIC_FUNCTION_TYPE_EXTRACT),
        extract_field(extract_field),
        extract_source(std::move(extract_source))
    {}
} extract_expression_t;

std::ostream& operator<< (std::ostream& out, const extract_expression_t& ee);

typedef struct length_expression : numeric_function_t {
    // Will always be static_castable to string_value_expression_t
    std::unique_ptr<struct value_expression> operand;
    length_expression(
            numeric_function_type_t type,
            std::unique_ptr<struct value_expression>& operand) :
        numeric_function_t(type),
        operand(std::move(operand))
    {}
} length_expression_t;

std::ostream& operator<< (std::ostream& out, const length_expression_t& le);

typedef struct numeric_factor {
    int8_t sign;
    std::unique_ptr<numeric_primary_t> value;
    numeric_factor(std::unique_ptr<numeric_primary_t>& value, int8_t sign) :
        sign(0),
        value(std::move(value))
    {}
} numeric_factor_t;

std::ostream& operator<< (std::ostream& out, const numeric_factor_t& nf);

typedef enum numeric_op {
    NUMERIC_OP_NONE,
    NUMERIC_OP_ADD,
    NUMERIC_OP_SUBTRACT,
    NUMERIC_OP_MULTIPLY,
    NUMERIC_OP_DIVIDE
} numeric_op_t;

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

typedef enum string_function_type {
    STRING_FUNCTION_TYPE_SUBSTRING,
    STRING_FUNCTION_TYPE_UPPER,
    STRING_FUNCTION_TYPE_LOWER,
    STRING_FUNCTION_TYPE_CONVERT,
    STRING_FUNCTION_TYPE_TRANSLATE,
    STRING_FUNCTION_TYPE_TRIM
} string_function_type_t;

struct value_expression;
typedef struct string_function {
    string_function_type_t type;
    // Guaranteed to be static_castable to a character_value_expression_t
    std::unique_ptr<struct value_expression> operand;
    string_function(
            string_function_type_t type,
            std::unique_ptr<struct value_expression>& operand) :
        type(type),
        operand(std::move(operand))
    {}
} string_function_t;

std::ostream& operator<< (std::ostream& out, const string_function_t& sf);

typedef struct substring_function : string_function_t {
    // Guaranteed to be static_castable to a numeric_value_expression_t
    std::unique_ptr<struct value_expression> start_position_value;
    // Guaranteed to be static_castable to a numeric_value_expression_t
    std::unique_ptr<struct value_expression> for_length_value;
    substring_function(
            std::unique_ptr<struct value_expression>& operand,
            std::unique_ptr<struct value_expression>& start_position_value) :
        string_function_t(STRING_FUNCTION_TYPE_SUBSTRING, operand),
        start_position_value(std::move(start_position_value))
    {}
    substring_function(
            std::unique_ptr<struct value_expression>& operand,
            std::unique_ptr<struct value_expression>& start_position_value,
            std::unique_ptr<struct value_expression>& for_length_value) :
        string_function_t(STRING_FUNCTION_TYPE_SUBSTRING, operand),
        start_position_value(std::move(start_position_value)),
        for_length_value(std::move(for_length_value))
    {}
} substring_function_t;

std::ostream& operator<< (std::ostream& out, const substring_function_t& sf);

typedef struct convert_function : string_function_t {
    lexeme_t conversion_name;
    convert_function(
            std::unique_ptr<struct value_expression>& operand,
            lexeme_t conversion_name) :
        string_function_t(STRING_FUNCTION_TYPE_CONVERT, operand),
        conversion_name(conversion_name)
    {}
} convert_function_t;

std::ostream& operator<< (std::ostream& out, const convert_function_t& cf);

typedef struct translate_function : string_function_t {
    lexeme_t translation_name;
    translate_function(
            std::unique_ptr<struct value_expression>& operand,
            lexeme_t translation_name) :
        string_function_t(STRING_FUNCTION_TYPE_TRANSLATE, operand),
        translation_name(translation_name)
    {}
} translate_function_t;

std::ostream& operator<< (std::ostream& out, const translate_function_t& tf);

typedef enum trim_specification {
    TRIM_SPECIFICATION_LEADING,
    TRIM_SPECIFICATION_TRAILING,
    TRIM_SPECIFICATION_BOTH
} trim_specification_t;

typedef struct trim_function : string_function_t {
    trim_specification_t specification;
    // Guaranteed to be static_castable to a character_value_expression_t
    std::unique_ptr<struct value_expression> trim_character;
    trim_function(
            std::unique_ptr<struct value_expression>& operand,
            trim_specification_t specification,
            std::unique_ptr<struct value_expression>& trim_character) :
        string_function_t(STRING_FUNCTION_TYPE_TRIM, operand),
        specification(specification),
        trim_character(std::move(trim_character))
    {}
} trim_function_t;

std::ostream& operator<< (std::ostream& out, const trim_function_t& tf);

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

typedef enum datetime_function_type {
    DATETIME_FUNCTION_TYPE_CURRENT_DATE,
    DATETIME_FUNCTION_TYPE_CURRENT_TIME,
    DATETIME_FUNCTION_TYPE_CURRENT_TIMESTAMP
} datetime_function_type_t;

typedef struct datetime_function {
    datetime_function_type_t type;
    size_t time_precision;
    datetime_function(
            datetime_function_type_t type,
            size_t time_precision) :
        type(type),
        time_precision(time_precision)
    {}
} datetime_function_t;

std::ostream& operator<< (std::ostream& out, const datetime_function_t& df);

// The datetime factor may be either a value expression primary or a datetime
// function
typedef struct datetime_primary {
    std::unique_ptr<value_expression_primary_t> value;
    std::unique_ptr<datetime_function_t> datetime_function;
    datetime_primary(std::unique_ptr<value_expression_primary_t>& value) :
        value(std::move(value))
    {}
    datetime_primary(std::unique_ptr<datetime_function_t>& datetime_function) :
        datetime_function(std::move(datetime_function))
    {}
} datetime_primary_t;

std::ostream& operator<< (std::ostream& out, const datetime_primary_t& dp);

// A datetime factor evaluates to a datetime value. It contains a datetime
// primary and has an optional timezone component.
typedef struct datetime_factor {
    std::unique_ptr<datetime_primary_t> value;
    lexeme_t tz;
    datetime_factor(
            std::unique_ptr<datetime_primary_t>& value,
            lexeme_t tz) :
        value(std::move(value)),
        tz(tz)
    {}
    inline bool is_local_tz() const {
        return ! tz;
    }
} datetime_factor_t;

std::ostream& operator<< (std::ostream& out, const datetime_factor_t& factor);

typedef struct datetime_term {
    std::unique_ptr<datetime_factor_t> value;
    datetime_term(std::unique_ptr<datetime_factor_t>& value) :
        value(std::move(value))
    {}
} datetime_term_t;

std::ostream& operator<< (std::ostream& out, const datetime_term_t& term);

typedef struct datetime_field {
    interval_unit_t interval;
    size_t precision;
    size_t fractional_precision;
    datetime_field(
            interval_unit_t interval,
            size_t precision,
            size_t fractional_precision) :
        interval(interval),
        precision(precision),
        fractional_precision(fractional_precision)
    {}
} datetime_field_t;

std::ostream& operator<< (std::ostream& out, const datetime_field_t& df);

typedef struct interval_qualifier {
    datetime_field_t start;
    std::unique_ptr<datetime_field_t> end;
    interval_qualifier(
            interval_unit_t start_interval,
            size_t start_precision,
            size_t start_fractional_precision) :
        start(start_interval, start_precision, start_fractional_precision)
    {}
    interval_qualifier(
            interval_unit_t start_interval,
            size_t start_precision,
            size_t start_fractional_precision,
            interval_unit_t end_interval,
            size_t end_fractional_precision) :
        start(start_interval, start_precision, start_fractional_precision),
        end(std::make_unique<datetime_field_t>(
                    end_interval, 0, end_fractional_precision))
    {}
} interval_qualifier_t;

std::ostream& operator<< (std::ostream& out, const interval_qualifier_t& iq);

typedef struct interval_primary {
    std::unique_ptr<value_expression_primary_t> value;
    std::unique_ptr<interval_qualifier_t> qualifier;
    interval_primary(
            std::unique_ptr<value_expression_primary_t>& value,
            std::unique_ptr<interval_qualifier_t>& qualifier) :
        value(std::move(value)),
        qualifier(std::move(qualifier))
    {}
} interval_primary_t;

std::ostream& operator<< (std::ostream& out, const interval_primary_t& primary);

typedef struct interval_factor {
    int8_t sign;
    std::unique_ptr<interval_primary_t> value;
    interval_factor(int8_t sign, std::unique_ptr<interval_primary_t>& value) :
        sign(sign),
        value(std::move(value))
    {}
} interval_factor_t;

std::ostream& operator<< (std::ostream& out, const interval_factor_t& factor);

typedef struct interval_term {
    std::unique_ptr<interval_factor_t> left;
    // Operating on an interval term with a numeric factor results in an
    // interval term
    numeric_op_t op;
    std::unique_ptr<numeric_factor_t> right;
    interval_term(std::unique_ptr<interval_factor_t>& left) :
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
} interval_term_t;

std::ostream& operator<< (std::ostream& out, const interval_term_t& tern);

} // namespace sqltoast

#endif /* SQLTOAST_VALUE_H */
