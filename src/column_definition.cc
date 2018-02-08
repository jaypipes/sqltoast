/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "column_definition.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const column_constraint_t& constraint) {
    if (constraint.name.get())
        out << "CONSTRAINT " << *constraint.name << " ";
    switch (constraint.type) {
        case COLUMN_CONSTRAINT_TYPE_NOT_NULL:
            out << "NOT NULL";
            break;
        case COLUMN_CONSTRAINT_TYPE_UNIQUE:
            out << "UNIQUE";
            break;
        case COLUMN_CONSTRAINT_TYPE_PRIMARY_KEY:
            out << "PRIMARY KEY";
            break;
        case COLUMN_CONSTRAINT_TYPE_REFERENCES:
            {
                const references_constraint_t& ref_c = static_cast<const references_constraint_t&>(constraint);
                out << ref_c;
            }
            break;
        case COLUMN_CONSTRAINT_TYPE_CHECK:
            out << "CHECK ";
            // TODO
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const references_constraint_t& constraint) {
        out << "REFERENCES " << constraint.table_name;
        size_t num_columns = constraint.column_names.size();
        if (num_columns > 0) {
            out << "(";
            size_t x = 0;
            for (const identifier_t& col_name : constraint.column_names) {
                out << col_name;
                if (x++ != (num_columns - 1))
                    out << ",";
            }
            out << ")";
        }
        switch (constraint.match_type) {
            case REFERENCES_MATCH_TYPE_FULL:
                out << " MATCH FULL";
                break;
            case REFERENCES_MATCH_TYPE_PARTIAL:
                out << " MATCH PARTIAL";
                break;
            default:
                break;
        }
        switch (constraint.on_update) {
            case REFERENTIAL_ACTION_SET_NULL:
                out << " ON UPDATE SET NULL";
                break;
            case REFERENTIAL_ACTION_SET_DEFAULT:
                out << " ON UPDATE SET DEFAULT";
                break;
            case REFERENTIAL_ACTION_CASCADE:
                out << " ON UPDATE CASCADE";
                break;
            default:
                break;
        }
        switch (constraint.on_delete) {
            case REFERENTIAL_ACTION_SET_NULL:
                out << " ON DELETE SET NULL";
                break;
            case REFERENTIAL_ACTION_SET_DEFAULT:
                out << " ON DELETE SET DEFAULT";
                break;
            case REFERENTIAL_ACTION_CASCADE:
                out << " ON DELETE CASCADE";
                break;
            default:
                break;
        }
        return out;
}

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
    return out;
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
    if (column_def.constraints.size() > 0) {
        for (auto constraint_it = column_def.constraints.begin();
             constraint_it != column_def.constraints.end();
             constraint_it++) {
            out << " " << *(*constraint_it);
        }
    }
    if (column_def.collate.get()) {
        out << " COLLATE " << *column_def.collate;
    }
    return out;
}

} // namespace sqltoast
