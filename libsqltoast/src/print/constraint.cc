/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const constraint_t& constraint) {
    if (constraint.name)
        out << " CONSTRAINT " << constraint.name;
    switch (constraint.type) {
        case CONSTRAINT_TYPE_NOT_NULL:
            {
                const not_null_constraint_t& nnc =
                    static_cast<const not_null_constraint_t&>(constraint);
                out << nnc;
            }
            break;
        case CONSTRAINT_TYPE_UNIQUE:
        case CONSTRAINT_TYPE_PRIMARY_KEY:
            {
                const unique_constraint_t& uniq_c =
                    static_cast<const unique_constraint_t&>(constraint);
                out << uniq_c;
            }
            break;
        case CONSTRAINT_TYPE_FOREIGN_KEY:
            {
                const foreign_key_constraint_t& ref_c =
                    static_cast<const foreign_key_constraint_t&>(constraint);
                out << ref_c;
            }
            break;
        case CONSTRAINT_TYPE_CHECK:
            out << "CHECK ";
            // TODO
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const not_null_constraint_t& nnc) {
    out << " NOT NULL";
    return out;
}

std::ostream& operator<< (std::ostream& out, const unique_constraint_t& constraint) {
    switch (constraint.type) {
        case CONSTRAINT_TYPE_UNIQUE:
            out << " UNIQUE";
            break;
        case CONSTRAINT_TYPE_PRIMARY_KEY:
            out << " PRIMARY KEY";
            break;
        default:
            // Should never happen...
            break;
    }
    size_t x = 0;
    size_t num_columns = constraint.columns.size();
    if (num_columns > 0) {
        out << " (";
        for (auto col : constraint.columns) {
            out << col;
            if (x++ < (num_columns - 1)) {
                out << ",";
            }
        }
        out << ")";
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const foreign_key_constraint_t& constraint) {
        size_t num_columns = constraint.columns.size();
        size_t x = 0;
        out << " FOREIGN KEY (";
        for (const lexeme_t& col_name : constraint.columns) {
            out << col_name;
            if (x++ != (num_columns - 1))
                out << ",";
        }
        size_t num_referenced_columns = constraint.referenced_columns.size();
        out << ") REFERENCES " << constraint.referenced_table;
        if (num_referenced_columns > 0) {
            out << " (";
            x = 0;
            for (const lexeme_t& col_name : constraint.referenced_columns) {
                out << col_name;
                if (x++ != (num_referenced_columns - 1))
                    out << ",";
            }
            out << ")";
        }
        switch (constraint.match_type) {
            case MATCH_TYPE_FULL:
                out << " MATCH FULL";
                break;
            case MATCH_TYPE_PARTIAL:
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

} // namespace sqltoast
