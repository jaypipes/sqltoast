/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "column_definition.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const default_descriptor_t& default_desc) {
    out << "DEFAULT ";
    switch (default_desc.type) {
        case DEFAULT_TYPE_NULL:
            out << "NULL";
            break;
        case DEFAULT_TYPE_CURRENT_USER:
            out << "CURRENT_USER";
            break;
        case DEFAULT_TYPE_SESSION_USER:
            out << "SESSION_USER";
            break;
        case DEFAULT_TYPE_SYSTEM_USER:
            out << "SYSTEM_USER";
            break;
        case DEFAULT_TYPE_USER:
            out << "USER";
            break;
        case DEFAULT_TYPE_CURRENT_DATE:
            out << "CURRENT_DATE";
            break;
        case DEFAULT_TYPE_CURRENT_TIME:
            out << "CURRENT_TIME";
            if (default_desc.precision > 0)
                out << "(" << default_desc.precision << ")";
            break;
        case DEFAULT_TYPE_CURRENT_TIMESTAMP:
            out << "CURRENT_TIMESTAMP";
            if (default_desc.precision > 0)
                out << "(" << default_desc.precision << ")";
            break;
        case DEFAULT_TYPE_LITERAL:
            out << "'" << std::string(default_desc.lexeme.start, default_desc.lexeme.end) << "'";
            break;
        default:
            break;
    }
}

std::ostream& operator<< (std::ostream& out, const column_definition_t& column_def) {
    out << column_def.id.name << " ";
    if (column_def.data_type.get()) {
        out << column_def.data_type->to_string();
    } else {
        out << " UNKNOWN";
    }
    if (column_def.default_descriptor.get()) {
        out << " " << *column_def.default_descriptor;
    }
    return out;
}

} // namespace sqltoast
