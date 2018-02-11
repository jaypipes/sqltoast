/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>
#include <string>

#include "data_type.h"

namespace sqltoast {

const std::string char_string_t::to_string() {
    std::stringstream ss;
    switch (type) {
        case DATA_TYPE_CHAR:
            ss << "CHAR";
            break;
        case DATA_TYPE_NCHAR:
            ss << "NCHAR";
            break;
        case DATA_TYPE_VARCHAR:
            ss << "VARCHAR";
            break;
        case DATA_TYPE_NVARCHAR:
            ss << "NVARCHAR";
            break;
        default:
            SQLTOAST_UNREACHABLE();
    }
    if (size > 0)
        ss << "(" << size << ")";
    if (charset)
        ss << " CHARACTER SET " << charset;
    return ss.str();
}

const std::string bit_string_t::to_string() {
    std::stringstream ss;
    ss << "BIT";
    if (type == DATA_TYPE_VARBIT)
        ss << " VARYING";
    if (size > 0)
        ss << "(" << size << ")";
    return ss.str();
}

const std::string exact_numeric_t::to_string() {
    std::stringstream ss;
    switch (type) {
        case DATA_TYPE_INT:
            ss << "INT";
            break;
        case DATA_TYPE_SMALLINT:
            ss << "SMALLINT";
            break;
        case DATA_TYPE_NUMERIC:
            ss << "NUMERIC";
            break;
        default:
            SQLTOAST_UNREACHABLE();
    }
    if (precision > 0) {
        ss << "(" << precision;
        if (scale > 0) {
            ss << "," << scale;
        }
        ss << ")";
    }
    return ss.str();
}

const std::string approximate_numeric_t::to_string() {
    std::stringstream ss;
    switch (type) {
        case DATA_TYPE_FLOAT:
            ss << "FLOAT";
            break;
        case DATA_TYPE_DOUBLE:
            ss << "DOUBLE PRECISION";
            break;
        default:
            SQLTOAST_UNREACHABLE();
    }
    if (precision > 0)
        ss << "(" << precision << ")";
    return ss.str();
}

const std::string datetime_t::to_string() {
    std::stringstream ss;
    switch (type) {
        case DATA_TYPE_DATE:
            ss << "DATE";
            break;
        case DATA_TYPE_TIME:
            ss << "TIME";
            break;
        case DATA_TYPE_TIMESTAMP:
            ss << "TIMESTAMP";
            break;
        default:
            SQLTOAST_UNREACHABLE();
    }
    if (precision > 0)
        ss << "(" << precision << ")";
    if (with_tz)
        ss << " WITH TIME ZONE";
    return ss.str();
}

const std::string interval_t::to_string() {
    std::stringstream ss;
    ss << "INTERVAL (";
    switch (unit) {
        case INTERVAL_UNIT_YEAR:
            ss << "YEAR)";
            break;
        case INTERVAL_UNIT_MONTH:
            ss << "MONTH)";
            break;
        case INTERVAL_UNIT_DAY:
            ss << "DAY)";
            break;
        case INTERVAL_UNIT_HOUR:
            ss << "HOUR)";
            break;
        case INTERVAL_UNIT_MINUTE:
            ss << "MINUTE)";
            break;
        case INTERVAL_UNIT_SECOND:
            ss << "SECOND";
            if (precision > 0)
                ss << "(" << precision << ")";
            ss << ")";
            break;
        default:
            SQLTOAST_UNREACHABLE();
    }
    return ss.str();
}

} // namespace sqltoast
