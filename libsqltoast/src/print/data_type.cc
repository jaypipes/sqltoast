/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const data_type_descriptor_t& dt) {
    switch (dt.type) {
        case DATA_TYPE_CHAR:
        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_NCHAR:
        case DATA_TYPE_NVARCHAR:
            {
                const char_string_t& sub = static_cast<const char_string_t&>(dt);
                out << sub;
            }
            break;
        case DATA_TYPE_BIT:
        case DATA_TYPE_VARBIT:
            {
                const bit_string_t& sub = static_cast<const bit_string_t&>(dt);
                out << sub;
            }
            break;
        case DATA_TYPE_INT:
        case DATA_TYPE_SMALLINT:
        case DATA_TYPE_NUMERIC:
            {
                const exact_numeric_t& sub = static_cast<const exact_numeric_t&>(dt);
                out << sub;
            }
            break;
        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_FLOAT:
            {
                const approximate_numeric_t& sub = static_cast<const approximate_numeric_t&>(dt);
                out << sub;
            }
            break;
        case DATA_TYPE_DATE:
        case DATA_TYPE_TIME:
        case DATA_TYPE_TIMESTAMP:
            {
                const datetime_t& sub = static_cast<const datetime_t&>(dt);
                out << sub;
            }
            break;
        case DATA_TYPE_INTERVAL:
            {
                const interval_t& sub = static_cast<const interval_t&>(dt);
                out << sub;
            }
            break;
        default:
            return out;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const char_string_t& cs) {
    switch (cs.type) {
        case DATA_TYPE_CHAR:
            out << "CHAR";
            break;
        case DATA_TYPE_NCHAR:
            out << "NCHAR";
            break;
        case DATA_TYPE_VARCHAR:
            out << "VARCHAR";
            break;
        case DATA_TYPE_NVARCHAR:
            out << "NVARCHAR";
            break;
        default:
            return out;
    }
    if (cs.size > 0)
        out << "(" << cs.size << ")";
    if (cs.charset)
        out << " CHARACTER SET " << cs.charset;
    return out;
}

std::ostream& operator<< (std::ostream& out, const bit_string_t& bs) {
    out << "BIT";
    if (bs.type == DATA_TYPE_VARBIT)
        out << " VARYING";
    if (bs.size > 0)
        out << "(" << bs.size << ")";
    return out;
}

std::ostream& operator<< (std::ostream& out, const exact_numeric_t& num) {
    switch (num.type) {
        case DATA_TYPE_INT:
            out << "INT";
            break;
        case DATA_TYPE_SMALLINT:
            out << "SMALLINT";
            break;
        case DATA_TYPE_NUMERIC:
            out << "NUMERIC";
            break;
        default:
            return out;
    }
    if (num.precision > 0) {
        out << "(" << num.precision;
        if (num.scale > 0) {
            out << "," << num.scale;
        }
        out << ")";
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const approximate_numeric_t& num) {
    switch (num.type) {
        case DATA_TYPE_FLOAT:
            out << "FLOAT";
            break;
        case DATA_TYPE_DOUBLE:
            out << "DOUBLE PRECISION";
            break;
        default:
            return out;
    }
    if (num.precision > 0)
        out << "(" << num.precision << ")";
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_t& dt) {
    switch (dt.type) {
        case DATA_TYPE_DATE:
            out << "DATE";
            break;
        case DATA_TYPE_TIME:
            out << "TIME";
            break;
        case DATA_TYPE_TIMESTAMP:
            out << "TIMESTAMP";
            break;
        default:
            return out;
    }
    if (dt.precision > 0)
        out << "(" << dt.precision << ")";
    if (dt.with_tz)
        out << " WITH TIME ZONE";
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_t& interval) {
    out << "INTERVAL (" << interval.unit;
    if (interval.unit == INTERVAL_UNIT_SECOND && interval.precision > 0)
        out << "(" << interval.precision << ")";
    out << ")";
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_unit_t& unit) {
    switch (unit) {
        case INTERVAL_UNIT_YEAR:
            out << "YEAR";
            break;
        case INTERVAL_UNIT_MONTH:
            out << "MONTH";
            break;
        case INTERVAL_UNIT_DAY:
            out << "DAY";
            break;
        case INTERVAL_UNIT_HOUR:
            out << "HOUR";
            break;
        case INTERVAL_UNIT_MINUTE:
            out << "MINUTE";
            break;
        case INTERVAL_UNIT_SECOND:
            out << "SECOND";
            break;
    }
    return out;
}

} // namespace sqltoast
