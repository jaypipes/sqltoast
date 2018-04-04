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
                const create_schema_statement_t& sub =
                    static_cast<const create_schema_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_DROP_SCHEMA:
            {
                const drop_schema_statement_t& sub =
                    static_cast<const drop_schema_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_CREATE_TABLE:
            {
                const create_table_statement_t& sub =
                    static_cast<const create_table_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_DROP_TABLE:
            {
                const drop_table_statement_t& sub =
                    static_cast<const drop_table_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_ALTER_TABLE:
            {
                const alter_table_statement_t& sub =
                    static_cast<const alter_table_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_SELECT:
            {
                const select_statement_t& sub =
                    static_cast<const select_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_INSERT:
            {
                const insert_statement_t& sub =
                    static_cast<const insert_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_INSERT_SELECT:
            {
                const insert_select_statement_t& sub =
                    static_cast<const insert_select_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_DELETE:
            {
                const delete_statement_t& sub = static_cast<const delete_statement_t&>(stmt);
                out << sub;
            }
            break;
        case STATEMENT_TYPE_UPDATE:
            {
                const update_statement_t& sub = static_cast<const update_statement_t&>(stmt);
                out << sub;
            }
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const create_schema_statement_t& stmt) {
    out << "<statement: CREATE SCHEMA" << std::endl
        << "   schema name: " << stmt.schema_name;
    if (stmt.authorization_identifier)
       out << std::endl << "   authorization identifier: " << stmt.authorization_identifier;
    if (stmt.default_charset)
       out << std::endl << "   default charset: " << stmt.default_charset;
    out << ">" << std::endl;
    return out;
}

std::ostream& operator<< (std::ostream& out, const drop_schema_statement_t& stmt) {
    out << "<statement: DROP SCHEMA" << std::endl
        << "   schema name: " << stmt.schema_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       out << "   behaviour: CASCADE";
    else
       out << "   behaviour: RESTRICT";
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const create_table_statement_t& stmt) {
    out << "<statement: CREATE TABLE" << std::endl
        << "    table name: " << stmt.table_name;
    if (stmt.table_type != TABLE_TYPE_NORMAL) {
        out << std::endl << "    temporary: true (";
        if (stmt.table_type == TABLE_TYPE_TEMPORARY_GLOBAL)
            out << "global)";
        else
            out << "local)";
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

std::ostream& operator<< (std::ostream& out, const drop_table_statement_t& stmt) {
    out << "<statement: DROP TABLE" << std::endl
        << "   table name: " << stmt.table_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       out << "   behaviour: CASCADE";
    else
       out << "   behaviour: RESTRICT";
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const add_column_action_t& action) {
    out << "ADD COLUMN " << *action.column_definition;
    return out;
}

std::ostream& operator<< (std::ostream& out, const alter_column_action_t& action) {
    if (action.alter_column_action_type == ALTER_COLUMN_ACTION_TYPE_SET_DEFAULT)
        out << "ALTER COLUMN " << action.column_name
            << " SET " << *action.default_descriptor;
    else
        out << "ALTER COLUMN " << action.column_name
            << " DROP DEFAULT";
    return out;
}

std::ostream& operator<< (std::ostream& out, const drop_column_action_t& action) {
    out << "DROP COLUMN " << action.column_name;
    if (action.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
        out << " CASCADE";
    else
        out << " RESTRICT";
    return out;
}

std::ostream& operator<< (std::ostream& out, const add_constraint_action_t& action) {
    out << "ADD " << *action.constraint;
    return out;
}

std::ostream& operator<< (std::ostream& out, const alter_table_action_t& action) {
    switch (action.type) {
        case ALTER_TABLE_ACTION_TYPE_ADD_COLUMN:
            {
                const add_column_action_t& sub =
                    static_cast<const add_column_action_t&>(action);
                out << sub;
            }
            break;
        case ALTER_TABLE_ACTION_TYPE_ALTER_COLUMN:
            {
                const alter_column_action_t& sub =
                    static_cast<const alter_column_action_t&>(action);
                out << sub;
            }
            break;
        case ALTER_TABLE_ACTION_TYPE_DROP_COLUMN:
            {
                const drop_column_action_t& sub =
                    static_cast<const drop_column_action_t&>(action);
                out << sub;
            }
            break;
        case ALTER_TABLE_ACTION_TYPE_ADD_CONSTRAINT:
            {
                const add_constraint_action_t& sub =
                    static_cast<const add_constraint_action_t&>(action);
                out << sub;
            }
            break;
        case ALTER_TABLE_ACTION_TYPE_DROP_CONSTRAINT:
            out << "DROP CONSTRAINT";
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const alter_table_statement_t& stmt) {
    out << "<statement: ALTER TABLE" << std::endl
        << "   table name: " << stmt.table_name << std::endl;
    out << "   action: " << *stmt.action;
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const select_statement_t& stmt) {
    out << "<statement: SELECT";
    if (stmt.distinct)
       out << std::endl << "   distinct: true";
    out << std::endl << "   selected columns:";
    size_t x = 0;
    for (const derived_column_t& dc : stmt.selected_columns) {
        out << std::endl << "     " << x++ << ": " << dc;
    }
    out << std::endl << "   referenced tables:";
    x = 0;
    for (const std::unique_ptr<table_reference_t>& tr : stmt.referenced_tables) {
        out << std::endl << "     " << x++ << ": " << *tr;
    }
    if (stmt.where_condition) {
        out << std::endl << "   where:" << std::endl << "     ";
        out << *stmt.where_condition;
    }
    if (! stmt.group_by_columns.empty()) {
        out << std::endl << "   group by:";
        x = 0;
        for (const grouping_column_reference_t& gcr : stmt.group_by_columns) {
            out << std::endl << "     " << x++ << ": " << gcr;
        }
    }
    if (stmt.having_condition) {
        out << std::endl << "   having:" << std::endl << "     ";
        out << *stmt.having_condition;
    }
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const insert_statement_t& stmt) {
    out << "<statement: INSERT" << std::endl
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
        for (const std::unique_ptr<row_value_constructor_t>& val : stmt.insert_values) {
            out << std::endl << "     " << x++ << ": " << *val;
        }
    }
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const insert_select_statement_t& stmt) {
    out << "<statement: INSERT" << std::endl
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
    out << std::endl << "   select:" << std::endl << "     " << *stmt.select;
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const delete_statement_t& stmt) {
    out << "<statement: DELETE" << std::endl
        << "   table name: " << stmt.table_name;

    if (stmt.where_condition)
        out << std::endl << "   where:" << std::endl << "     " << *stmt.where_condition;
    out << ">" << std::endl;

    return out;
}

std::ostream& operator<< (std::ostream& out, const update_statement_t& stmt) {
    out << "<statement: UPDATE" << std::endl
        << "   table name: " << stmt.table_name;

    out << std::endl << "   set columns:";
    for (const set_column_t& set_col : stmt.set_columns) {
        out << std::endl << "     " << set_col.column_name << " = ";
        if (set_col.type == SET_COLUMN_TYPE_NULL)
            out << "NULL";
        else if (set_col.type == SET_COLUMN_TYPE_DEFAULT)
            out << "DEFAULT";
        else
            out << *set_col.value;
    }

    if (stmt.where_condition)
        out << std::endl << "   where:" << std::endl << "     " << *stmt.where_condition;
    out << ">" << std::endl;

    return out;
}

} // namespace sqltoast
