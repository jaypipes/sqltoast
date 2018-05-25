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
    switch (stmt.type) {
        case STATEMENT_TYPE_CREATE_SCHEMA:
            ptr.indent_noendl(out) << "type: CREATE_SCHEMA";
            {
                const create_schema_statement_t& sub =
                    static_cast<const create_schema_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DROP_SCHEMA:
            ptr.indent_noendl(out) << "type: DROP_SCHEMA";
            {
                const drop_schema_statement_t& sub =
                    static_cast<const drop_schema_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_CREATE_TABLE:
            ptr.indent_noendl(out) << "type: CREATE_TABLE";
            {
                const create_table_statement_t& sub =
                    static_cast<const create_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DROP_TABLE:
            ptr.indent_noendl(out) << "type: DROP_TABLE";
            {
                const drop_table_statement_t& sub =
                    static_cast<const drop_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_ALTER_TABLE:
            ptr.indent_noendl(out) << "type: ALTER_TABLE";
            {
                const alter_table_statement_t& sub =
                    static_cast<const alter_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_CREATE_VIEW:
            ptr.indent_noendl(out) << "type: CREATE_VIEW";
            {
                const create_view_statement_t& sub =
                    static_cast<const create_view_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DROP_VIEW:
            ptr.indent_noendl(out) << "type: DROP_VIEW";
            {
                const drop_view_statement_t& sub =
                    static_cast<const drop_view_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_SELECT:
            ptr.indent_noendl(out) << "type: SELECT";
            {
                const select_statement_t& sub =
                    static_cast<const select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_INSERT:
            ptr.indent_noendl(out) << "type: INSERT";
            {
                const insert_statement_t& sub =
                    static_cast<const insert_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_INSERT_SELECT:
            ptr.indent_noendl(out) << "type: INSERT_SELECT";
            {
                const insert_select_statement_t& sub =
                    static_cast<const insert_select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_DELETE:
            ptr.indent_noendl(out) << "type: DELETE";
            {
                const delete_statement_t& sub = static_cast<const delete_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_UPDATE:
            ptr.indent_noendl(out) << "type: UPDATE";
            {
                const update_statement_t& sub = static_cast<const update_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case STATEMENT_TYPE_COMMIT:
            ptr.indent_noendl(out) << "type: COMMIT";
            break;
        case STATEMENT_TYPE_ROLLBACK:
            ptr.indent_noendl(out) << "type: ROLLBACK";
            break;
        case STATEMENT_TYPE_GRANT:
            ptr.indent_noendl(out) << "type: GRANT";
            {
                const grant_statement_t& sub = static_cast<const grant_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        default:
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const create_schema_statement_t& stmt) {
    ptr.indent(out) << "schema_name: " << stmt.schema_name;
    if (stmt.authorization_identifier) {
       ptr.indent(out) << "authorization_identifier: " << stmt.authorization_identifier;
    }
    if (stmt.default_charset) {
       ptr.indent(out) << "default_charset: " << stmt.default_charset;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_schema_statement_t& stmt) {
    ptr.indent(out) << "schema_name: " << stmt.schema_name;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const create_table_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.table_type != TABLE_TYPE_NORMAL) {
        ptr.indent(out) << "temporary: true (";
        if (stmt.table_type == TABLE_TYPE_TEMPORARY_GLOBAL)
            out << "global)";
        else
            out << "local)";
    }
    ptr.indent(out) << "column_definitions:";
    ptr.indent_push(out);
    for (auto cdef_it = stmt.column_definitions.begin();
            cdef_it != stmt.column_definitions.end();
            cdef_it++) {
        ptr.indent(out) << "- "<< *(*cdef_it);
    }
    ptr.indent_pop(out);
    if (stmt.constraints.size() > 0) {
        ptr.indent(out) << "constraints:";
        ptr.indent_push(out);
        for (auto constraint_it = stmt.constraints.begin();
             constraint_it != stmt.constraints.end();
             constraint_it++) {
            ptr.indent(out) << "- " << *(*constraint_it);
        }
        ptr.indent_pop(out);
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_table_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
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
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (! stmt.columns.empty()) {
        ptr.indent(out) << "column_definitions:";
        ptr.indent_push(out);
        for (const auto& column : stmt.columns)
            ptr.indent(out) << "- "<< column;
        ptr.indent_pop(out);
    }
    if (stmt.check_option != CHECK_OPTION_NONE) {
        if (stmt.check_option == CHECK_OPTION_LOCAL)
            ptr.indent(out) << "check_option: LOCAL";
        else
            ptr.indent(out) << "check_option: CASCADED";
    }
    ptr.indent(out) << "query:" << std::endl;
    ptr.indent_push(out);
    to_yaml(ptr, out, *stmt.query);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const drop_view_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.drop_behaviour == DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const select_statement_t& stmt) {
    if (stmt.distinct)
       ptr.indent(out) << "distinct: true";
    ptr.indent(out) << "selected_columns:";
    ptr.indent_push(out);
    for (const derived_column_t& dc : stmt.selected_columns)
        ptr.indent(out) << "- " << dc;
    ptr.indent_pop(out);
    ptr.indent(out) << "referenced_tables:";
    ptr.indent_push(out);
    for (const std::unique_ptr<table_reference_t>& tr : stmt.referenced_tables)
        ptr.indent(out) << "- " << *tr;
    ptr.indent_pop(out);
    if (stmt.where_condition)
        ptr.indent(out) << "where:" << *stmt.where_condition;
    if (! stmt.group_by_columns.empty()) {
        ptr.indent(out) << "group_by:";
        ptr.indent_push(out);
        for (const grouping_column_reference_t& gcr : stmt.group_by_columns)
            ptr.indent(out) << "- " << gcr;
        ptr.indent_pop(out);
    }
    if (stmt.having_condition)
        ptr.indent(out) << "having:" << *stmt.having_condition;
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
