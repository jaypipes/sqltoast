/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const statement_t& stmt) {
    switch (stmt.type) {
        case STATEMENT_TYPE_CREATE_SCHEMA:
            {
                const create_schema_t& sub = static_cast<const create_schema_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_DROP_SCHEMA:
            {
                const drop_schema_t& sub = static_cast<const drop_schema_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_CREATE_TABLE:
            {
                const create_table_t& sub = static_cast<const create_table_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_DROP_TABLE:
            {
                const drop_table_t& sub = static_cast<const drop_table_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_SELECT:
            {
                const select_t& sub = static_cast<const select_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_INSERT:
            {
                const insert_t& sub = static_cast<const insert_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_INSERT_SELECT:
            {
                const insert_select_t& sub = static_cast<const insert_select_t&>(stmt);
                out << sub;
            }
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const create_schema_t& stmt) {
    out << "<statement: CREATE SCHEMA" << std::endl
        << "   schema name: " << stmt.schema_name;
    if (stmt.authorization_identifier) {
       out << std::endl << "   authorization identifier: " << stmt.authorization_identifier;
    }
    if (stmt.default_charset) {
       out << std::endl << "   default charset: " << stmt.default_charset;
    }
    out << ">" << std::endl;
    return out;
}

std::ostream& operator<< (std::ostream& out, const drop_schema_t& stmt) {
    out << "<statement: DROP SCHEMA" << std::endl
        << "   schema name: " << stmt.schema_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE) {
       out << "   behaviour: CASCADE";
    } else {
       out << "   behaviour: RESTRICT";
    }
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const create_table_t& stmt) {
    out << "<statement: CREATE TABLE" << std::endl
        << "    table name: " << stmt.table_name;
    if (stmt.table_type != TABLE_TYPE_NORMAL) {
        out << std::endl << "    temporary: true (";
        if (stmt.table_type == TABLE_TYPE_TEMPORARY_GLOBAL) {
            out << "global)";
        } else {
            out << "local)";
        }
    }
    out << std::endl << "    column definitions:";
    for (auto cdef_it = stmt.column_definitions.begin();
            cdef_it != stmt.column_definitions.end();
            cdef_it++) {
        out << std::endl << "      " << *(*cdef_it);
    }
    if (stmt.constraints.size() > 0) {
        out << std::endl << "    constraints:";
        for (auto constraint_it = stmt.constraints.begin();
             constraint_it != stmt.constraints.end();
             constraint_it++) {
            out << std::endl << "      " << *(*constraint_it);
        }
    }
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const drop_table_t& stmt) {
    out << "<statement: DROP TABLE" << std::endl
        << "   table name: " << stmt.table_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       out << "   behaviour: CASCADE";
    else
       out << "   behaviour: RESTRICT";
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const select_t& stmt) {
    out << "<statement: SELECT";
    if (stmt.distinct)
       out << std::endl << "   distinct: true";
    out << std::endl << "   selected columns: ";
    size_t x = 0;
    for (const derived_column_t& dc : stmt.selected_columns) {
        out << std::endl << "     " << x++ << ": " << dc;
    }
    out << std::endl << "   referenced tables: ";
    x = 0;
    for (const table_reference_t& tr : stmt.referenced_tables) {
        out << std::endl << "     " << x++ << ": " << tr;
    }
    if (stmt.where_condition) {
        out << std::endl << "   where conditions: ";
        out << *stmt.where_condition;
    }
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const insert_t& stmt) {
    out << "<statement: INSERT " << std::endl
        << "   table name: " << stmt.table_name;

    if (stmt.use_default_columns())
        out << std::endl << "   default columns: true";
    else {
        out << std::endl << "   columns:";
        size_t x = 0;
        for (const lexeme_t& col : stmt.insert_columns) {
            out << std::endl << "     " << x++ << ": " << col;
        }
    }
    if (stmt.use_default_values())
        out << std::endl << "   default values: true";
    else {
        size_t x = 0;
        out << std::endl << "   values:";
        for (const lexeme_t& val : stmt.insert_values) {
            out << std::endl << "     " << x++ << ": " << val;
        }
    }
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const insert_select_t& stmt) {
    out << "<statement: INSERT " << std::endl
        << "   table name: " << stmt.table_name;

    if (stmt.use_default_columns())
        out << std::endl << "   default columns: true";
    else {
        out << std::endl << "   columns:";
        size_t x = 0;
        for (const lexeme_t& col : stmt.insert_columns) {
            out << std::endl << "     " << x++ << ": " << col;
        }
    }
    out << std::endl << "   select: " << *stmt.select;
    out << ">" << std::endl;

    return out;
}

} // namespace sqltoast
