/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "printers.h"
#include "../printers.h"

using namespace sqltoast;

namespace sqltoaster {

void to_yaml(printer_t& ptr, std::ostream& out, const statement_t& stmt) {
    ptr.indent_print(out);
    out << "statement:" << std::endl;
    ptr.indent_push(out);
    switch (stmt.type) {
        case STATEMENT_TYPE_CREATE_SCHEMA:
            ptr.indent_print(out);
            out << "type: CREATE_SCHEMA" << std::endl;
            {
                const create_schema_statement_t& sub =
                    static_cast<const create_schema_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DROP_SCHEMA:
            ptr.indent_print(out);
            out << "type: DROP_SCHEMA" << std::endl;
            {
                const drop_schema_statement_t& sub =
                    static_cast<const drop_schema_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_CREATE_TABLE:
            ptr.indent_print(out);
            out << "type: CREATE_TABLE" << std::endl;
            {
                const create_table_statement_t& sub =
                    static_cast<const create_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DROP_TABLE:
            ptr.indent_print(out);
            out << "type: DROP_TABLE" << std::endl;
            {
                const drop_table_statement_t& sub =
                    static_cast<const drop_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_ALTER_TABLE:
            ptr.indent_print(out);
            out << "type: ALTER_TABLE" << std::endl;
            {
                const alter_table_statement_t& sub =
                    static_cast<const alter_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_CREATE_VIEW:
            ptr.indent_print(out);
            out << "type: CREATE_VIEW" << std::endl;
            {
                const create_view_statement_t& sub =
                    static_cast<const create_view_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DROP_VIEW:
            ptr.indent_print(out);
            out << "type: DROP_VIEW" << std::endl;
            {
                const drop_view_statement_t& sub =
                    static_cast<const drop_view_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_SELECT:
            ptr.indent_print(out);
            out << "type: SELECT" << std::endl;
            {
                const select_statement_t& sub =
                    static_cast<const select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_INSERT:
            ptr.indent_print(out);
            out << "type: INSERT" << std::endl;
            {
                const insert_statement_t& sub =
                    static_cast<const insert_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_INSERT_SELECT:
            ptr.indent_print(out);
            out << "type: INSERT_SELECT" << std::endl;
            {
                const insert_select_statement_t& sub =
                    static_cast<const insert_select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DELETE:
            ptr.indent_print(out);
            out << "type: DELETE" << std::endl;
            {
                const delete_statement_t& sub = static_cast<const delete_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_UPDATE:
            ptr.indent_print(out);
            out << "type: UPDATE" << std::endl;
            {
                const update_statement_t& sub = static_cast<const update_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_COMMIT:
            ptr.indent_print(out);
            out << "type: COMMIT";
            break;
        case STATEMENT_TYPE_ROLLBACK:
            ptr.indent_print(out);
            out << "type: ROLLBACK";
            break;
        case STATEMENT_TYPE_GRANT:
            ptr.indent_print(out);
            out << "type: GRANT" << std::endl;
            {
                const grant_statement_t& sub = static_cast<const grant_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        default:
            break;
    }
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const create_schema_statement_t& stmt) {
    ptr.indent_print(out);
    out << "schema_name: " << stmt.schema_name;
    if (stmt.authorization_identifier) {
       out << std::endl;
       ptr.indent_print(out);
       out << "authorization_identifier: " << stmt.authorization_identifier;
    }
    if (stmt.default_charset) {
       out << std::endl;
       ptr.indent_print(out);
       out << "default_charset: " << stmt.default_charset;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_schema_statement_t& stmt) {
    out << "<statement: DROP SCHEMA" << std::endl
        << "   schema name: " << stmt.schema_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       out << "   behaviour: CASCADE";
    else
       out << "   behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const create_table_statement_t& stmt) {
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
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_table_statement_t& stmt) {
    out << "<statement: DROP TABLE" << std::endl
        << "   table name: " << stmt.table_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       out << "   behaviour: CASCADE";
    else
       out << "   behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const add_column_action_t& action) {
    out << "ADD COLUMN " << *action.column_definition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const alter_column_action_t& action) {
    if (action.alter_column_action_type == ALTER_COLUMN_ACTION_TYPE_SET_DEFAULT)
        out << "ALTER COLUMN " << action.column_name
            << " SET " << *action.default_descriptor;
    else
        out << "ALTER COLUMN " << action.column_name
            << " DROP DEFAULT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_column_action_t& action) {
    out << "DROP COLUMN " << action.column_name;
    if (action.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
        out << " CASCADE";
    else
        out << " RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const add_constraint_action_t& action) {
    out << "ADD" << *action.constraint;
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_constraint_action_t& action) {
    out << "DROP CONSTRAINT " << action.constraint_name;
    if (action.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
        out << " CASCADE";
    else
        out << " RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const alter_table_action_t& action) {
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
            {
                const drop_constraint_action_t& sub =
                    static_cast<const drop_constraint_action_t&>(action);
                out << sub;
            }
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const alter_table_statement_t& stmt) {
    out << "<statement: ALTER TABLE" << std::endl
        << "   table name: " << stmt.table_name << std::endl;
    out << "   action: " << *stmt.action;
}

void to_yaml(printer_t& ptr, std::ostream& out, const create_view_statement_t& stmt) {
    out << "<statement: CREATE VIEW" << std::endl
        << "   table name: " << stmt.table_name;
    if (! stmt.columns.empty()) {
       out << std::endl << "   columns:";
       size_t x = 0;
       for (const auto& column : stmt.columns)
           out << std::endl << "     " << x++ << ": " << column;
    }
    if (stmt.check_option != CHECK_OPTION_NONE) {
        if (stmt.check_option == CHECK_OPTION_LOCAL)
            out << std::endl << "   check option: LOCAL";
        else
            out << std::endl << "   check option: CASCADED";
    }
    out << std::endl << "   query: " << *stmt.query << '>';
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_view_statement_t& stmt) {
    out << "<statement: DROP VIEW" << std::endl
        << "   view name: " << stmt.table_name << std::endl;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       out << "   behaviour: CASCADE";
    else
       out << "   behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const select_statement_t& stmt) {
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
}

void to_yaml(printer_t& ptr, std::ostream& out, const insert_statement_t& stmt) {
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
}

void to_yaml(printer_t& ptr, std::ostream& out, const insert_select_statement_t& stmt) {
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
}

void to_yaml(printer_t& ptr, std::ostream& out, const delete_statement_t& stmt) {
    out << "<statement: DELETE" << std::endl
        << "   table name: " << stmt.table_name;

    if (stmt.where_condition)
        out << std::endl << "   where:" << std::endl << "     " << *stmt.where_condition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const update_statement_t& stmt) {
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
}

void to_yaml(printer_t& ptr, std::ostream& out, const grant_action_t& action) {
    switch (action.type) {
        case GRANT_ACTION_TYPE_SELECT:
            out << "SELECT";
            return;
        case GRANT_ACTION_TYPE_DELETE:
            out << "DELETE";
            return;
        case GRANT_ACTION_TYPE_USAGE:
            out << "USAGE";
            return;
        case GRANT_ACTION_TYPE_INSERT:
            out << "INSERT";
            break;
        case GRANT_ACTION_TYPE_UPDATE:
            out << "UPDATE";
            break;
        case GRANT_ACTION_TYPE_REFERENCES:
            out << "REFERENCES";
            break;
    }
    const column_list_grant_action_t& cla =
        static_cast<const column_list_grant_action_t&>(action);
    if (cla.columns.empty())
        return;
    out << " (";
    size_t x = 0;
    for (const lexeme_t& col : cla.columns) {
        if (x++ > 0)
            out << ',';
        out << col;
    }
    out << ')';
}

void to_yaml(printer_t& ptr, std::ostream& out, const grant_statement_t& stmt) {
    out << "<statement: GRANT" << std::endl
        << "   on: ";
    switch (stmt.object_type) {
        case GRANT_OBJECT_TYPE_TABLE:
            break;
        case GRANT_OBJECT_TYPE_DOMAIN:
            out << "DOMAIN ";
            break;
        case GRANT_OBJECT_TYPE_COLLATION:
            out << "COLLATION ";
            break;
        case GRANT_OBJECT_TYPE_CHARACTER_SET:
            out << "CHARACTER SET ";
            break;
        case GRANT_OBJECT_TYPE_TRANSLATION:
            out << "TRANSLATION ";
            break;
    }
    out << stmt.on << std::endl;
    if (stmt.to_public())
        out << "   to: PUBLIC" << std::endl;
    else
        out << "   to: " << stmt.to << std::endl;
    if (stmt.with_grant_option)
        out << "   with grant option: YES" << std::endl;
    if (stmt.all_privileges())
        out << "   privileges: ALL";
    else {
        out << "   privileges:";
        size_t x = 0;
        for (const std::unique_ptr<grant_action_t>& action : stmt.privileges)
            out << std::endl << "     " << x++ << ": " << *action;
    }
}

} // namespace sqltoast
