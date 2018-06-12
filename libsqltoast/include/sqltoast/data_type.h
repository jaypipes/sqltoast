/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_DATA_TYPE_H
#define SQLTOAST_DATA_TYPE_H

namespace sqltoast {

typedef enum data_type {
    DATA_TYPE_CHAR,
    DATA_TYPE_VARCHAR,
    DATA_TYPE_NCHAR,
    DATA_TYPE_NVARCHAR,
    DATA_TYPE_BIT,
    DATA_TYPE_VARBIT,
    DATA_TYPE_NUMERIC,
    DATA_TYPE_INT,
    DATA_TYPE_SMALLINT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_DATE,
    DATA_TYPE_TIME,
    DATA_TYPE_TIMESTAMP,
    DATA_TYPE_INTERVAL
} data_type_t;

typedef struct data_type_descriptor {
    data_type_t type;
    data_type_descriptor(data_type_t type) : type(type)
    {}
} data_type_descriptor_t;

typedef struct char_string : data_type_descriptor_t {
    size_t size;
    lexeme_t charset;
    char_string(data_type_t type, size_t size) :
        data_type_descriptor_t(type),
        size(size)
    {}
    char_string(data_type_t type, size_t size, lexeme_t charset) :
        data_type_descriptor_t(type),
        size(size),
        charset(charset)
    {}
} char_string_t;

typedef struct bit_string : data_type_descriptor_t {
    size_t size;
    bit_string(data_type_t type, size_t size) :
        data_type_descriptor_t(type),
        size(size)
    {}
} bit_string_t;

typedef struct exact_numeric : data_type_descriptor_t {
    size_t precision;
    size_t scale;
    exact_numeric(data_type_t type, size_t prec, size_t scale) :
        data_type_descriptor_t(type),
        precision(prec),
        scale(scale)
    {}
} exact_numeric_t;

typedef struct approximate_numeric : data_type_descriptor_t {
    size_t precision;
    approximate_numeric(data_type_t type, size_t prec) :
        data_type_descriptor_t(type),
        precision(prec)
    {}
} approximate_numeric_t;

typedef struct datetime : data_type_descriptor_t {
    size_t precision;
    bool with_tz;
    datetime(data_type_t type, size_t prec, bool with_tz) :
        data_type_descriptor_t(type),
        precision(prec),
        with_tz(with_tz)
    {}
} datetime_t;

// TOOD(jaypipes): Move datetime and interval stuff to a separate temporal.h
// file
typedef enum interval_unit {
    INTERVAL_UNIT_YEAR,
    INTERVAL_UNIT_MONTH,
    INTERVAL_UNIT_DAY,
    INTERVAL_UNIT_HOUR,
    INTERVAL_UNIT_MINUTE,
    INTERVAL_UNIT_SECOND
} interval_unit_t;

typedef struct interval : data_type_descriptor_t {
    interval_unit_t unit;
    size_t precision;
    interval(interval_unit_t unit, size_t prec) :
        data_type_descriptor_t(DATA_TYPE_INTERVAL),
        unit(unit),
        precision(prec)
    {}
} interval_t;

} // namespace sqltoast

#endif /* SQLTOAST_DATA_TYPE_H */
