/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sqltoast/print.h>

#include "printers.h"

namespace sqltoaster {
namespace print {

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::statement_t& stmt) {
    switch (stmt.type) {
        case sqltoast::STATEMENT_TYPE_CREATE_SCHEMA:
            ptr.indent_noendl(out) << "type: CREATE_SCHEMA";
            {
                const sqltoast::create_schema_statement_t& sub =
                    static_cast<const sqltoast::create_schema_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DROP_SCHEMA:
            ptr.indent_noendl(out) << "type: DROP_SCHEMA";
            {
                const sqltoast::drop_schema_statement_t& sub =
                    static_cast<const sqltoast::drop_schema_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_CREATE_TABLE:
            ptr.indent_noendl(out) << "type: CREATE_TABLE";
            {
                const sqltoast::create_table_statement_t& sub =
                    static_cast<const sqltoast::create_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DROP_TABLE:
            ptr.indent_noendl(out) << "type: DROP_TABLE";
            {
                const sqltoast::drop_table_statement_t& sub =
                    static_cast<const sqltoast::drop_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_ALTER_TABLE:
            ptr.indent_noendl(out) << "type: ALTER_TABLE";
            {
                const sqltoast::alter_table_statement_t& sub =
                    static_cast<const sqltoast::alter_table_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_CREATE_VIEW:
            ptr.indent_noendl(out) << "type: CREATE_VIEW";
            {
                const sqltoast::create_view_statement_t& sub =
                    static_cast<const sqltoast::create_view_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DROP_VIEW:
            ptr.indent_noendl(out) << "type: DROP_VIEW";
            {
                const sqltoast::drop_view_statement_t& sub =
                    static_cast<const sqltoast::drop_view_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_SELECT:
            ptr.indent_noendl(out) << "type: SELECT";
            {
                const sqltoast::select_statement_t& sub =
                    static_cast<const sqltoast::select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_INSERT:
            ptr.indent_noendl(out) << "type: INSERT";
            {
                const sqltoast::insert_statement_t& sub =
                    static_cast<const sqltoast::insert_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_INSERT_SELECT:
            ptr.indent_noendl(out) << "type: INSERT_SELECT";
            {
                const sqltoast::insert_select_statement_t& sub =
                    static_cast<const sqltoast::insert_select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DELETE:
            ptr.indent_noendl(out) << "type: DELETE";
            {
                const sqltoast::delete_statement_t& sub =
                    static_cast<const sqltoast::delete_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_UPDATE:
            ptr.indent_noendl(out) << "type: UPDATE";
            {
                const sqltoast::update_statement_t& sub =
                    static_cast<const sqltoast::update_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::STATEMENT_TYPE_COMMIT:
            ptr.indent_noendl(out) << "type: COMMIT";
            break;
        case sqltoast::STATEMENT_TYPE_ROLLBACK:
            ptr.indent_noendl(out) << "type: ROLLBACK";
            break;
        case sqltoast::STATEMENT_TYPE_GRANT:
            ptr.indent_noendl(out) << "type: GRANT";
            {
                const sqltoast::grant_statement_t& sub =
                    static_cast<const sqltoast::grant_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        default:
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_schema_statement_t& stmt) {
    ptr.indent(out) << "schema_name: " << stmt.schema_name;
    if (stmt.authorization_identifier) {
       ptr.indent(out) << "authorization_identifier: " << stmt.authorization_identifier;
    }
    if (stmt.default_charset) {
       ptr.indent(out) << "default_charset: " << stmt.default_charset;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_schema_statement_t& stmt) {
    ptr.indent(out) << "schema_name: " << stmt.schema_name;
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_table_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.table_type != sqltoast::TABLE_TYPE_NORMAL) {
        ptr.indent(out) << "temporary: true (";
        if (stmt.table_type == sqltoast::TABLE_TYPE_TEMPORARY_GLOBAL)
            out << "global)";
        else
            out << "local)";
    }
    ptr.indent(out) << "column_definitions:";
    ptr.indent_push(out);
    for (auto cdef_it = stmt.column_definitions.cbegin();
            cdef_it != stmt.column_definitions.cend();
            cdef_it++) {
        const sqltoast::column_definition_t& cdef = *(*cdef_it);
        ptr.indent(out) << cdef.name << ": ";
        if (cdef.data_type.get()) {
            out << *cdef.data_type;
        } else {
            out << " UNKNOWN";
        }
        if (cdef.default_descriptor.get()) {
            out << " " << *cdef.default_descriptor;
        }
        for (const std::unique_ptr<sqltoast::constraint_t>& c: cdef.constraints)
            out << *c;
        if (cdef.collate) {
            out << " COLLATE " << cdef.collate;
        }
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

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_table_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::add_column_action_t& action) {
    out << "ADD COLUMN " << *action.column_definition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_column_action_t& action) {
    if (action.alter_column_action_type == sqltoast::ALTER_COLUMN_ACTION_TYPE_SET_DEFAULT)
        out << "ALTER COLUMN " << action.column_name
            << " SET " << *action.default_descriptor;
    else
        out << "ALTER COLUMN " << action.column_name
            << " DROP DEFAULT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_column_action_t& action) {
    out << "DROP COLUMN " << action.column_name;
    if (action.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
        out << " CASCADE";
    else
        out << " RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::add_constraint_action_t& action) {
    out << "ADD" << *action.constraint;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_constraint_action_t& action) {
    out << "DROP CONSTRAINT " << action.constraint_name;
    if (action.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
        out << " CASCADE";
    else
        out << " RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_table_action_t& action) {
    switch (action.type) {
        case sqltoast::ALTER_TABLE_ACTION_TYPE_ADD_COLUMN:
            {
                const sqltoast::add_column_action_t& sub =
                    static_cast<const sqltoast::add_column_action_t&>(action);
                out << sub;
            }
            break;
        case sqltoast::ALTER_TABLE_ACTION_TYPE_ALTER_COLUMN:
            {
                const sqltoast::alter_column_action_t& sub =
                    static_cast<const sqltoast::alter_column_action_t&>(action);
                out << sub;
            }
            break;
        case sqltoast::ALTER_TABLE_ACTION_TYPE_DROP_COLUMN:
            {
                const sqltoast::drop_column_action_t& sub =
                    static_cast<const sqltoast::drop_column_action_t&>(action);
                out << sub;
            }
            break;
        case sqltoast::ALTER_TABLE_ACTION_TYPE_ADD_CONSTRAINT:
            {
                const sqltoast::add_constraint_action_t& sub =
                    static_cast<const sqltoast::add_constraint_action_t&>(action);
                out << sub;
            }
            break;
        case sqltoast::ALTER_TABLE_ACTION_TYPE_DROP_CONSTRAINT:
            {
                const sqltoast::drop_constraint_action_t& sub =
                    static_cast<const sqltoast::drop_constraint_action_t&>(action);
                out << sub;
            }
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_table_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    ptr.indent(out) << "action: " << *stmt.action;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_view_statement_t& stmt) {
    ptr.indent(out) << "view_name: " << stmt.table_name;
    if (! stmt.columns.empty()) {
        ptr.indent(out) << "columns:";
        ptr.indent_push(out);
        for (const auto& column : stmt.columns)
            ptr.indent(out) << "- "<< column;
        ptr.indent_pop(out);
    }
    if (stmt.check_option != sqltoast::CHECK_OPTION_NONE) {
        if (stmt.check_option == sqltoast::CHECK_OPTION_LOCAL)
            ptr.indent(out) << "check_option: LOCAL";
        else
            ptr.indent(out) << "check_option: CASCADED";
    }
    ptr.indent(out) << "query:" << std::endl;
    ptr.indent_push(out);
    to_yaml(ptr, out, *stmt.query);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_view_statement_t& stmt) {
    ptr.indent(out) << "view_name: " << stmt.table_name;
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::select_statement_t& stmt) {
    if (stmt.distinct)
       ptr.indent(out) << "distinct: true";
    ptr.indent(out) << "selected_columns:";
    ptr.indent_push(out);
    for (const sqltoast::derived_column_t& dc : stmt.selected_columns)
        ptr.indent(out) << "- " << dc;
    ptr.indent_pop(out);
    ptr.indent(out) << "referenced_tables:";
    ptr.indent_push(out);
    for (const std::unique_ptr<sqltoast::table_reference_t>& tr : stmt.referenced_tables)
        ptr.indent(out) << "- " << *tr;
    ptr.indent_pop(out);
    if (stmt.where_condition)
        ptr.indent(out) << "where: " << *stmt.where_condition;
    if (! stmt.group_by_columns.empty()) {
        ptr.indent(out) << "group_by:";
        ptr.indent_push(out);
        for (const sqltoast::grouping_column_reference_t& gcr : stmt.group_by_columns)
            ptr.indent(out) << "- " << gcr;
        ptr.indent_pop(out);
    }
    if (stmt.having_condition)
        ptr.indent(out) << "having: " << *stmt.having_condition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::insert_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.use_default_columns())
        ptr.indent(out) << "default_columns: true";
    else {
        ptr.indent(out) << "columns:";
        ptr.indent_push(out);
        for (const sqltoast::lexeme_t& col : stmt.insert_columns)
            ptr.indent(out) << "- " << col;
        ptr.indent_pop(out);
    }
    if (stmt.use_default_values())
        ptr.indent(out) << "default_values: true";
    else {
        ptr.indent(out) << "values:";
        ptr.indent_push(out);
        for (const std::unique_ptr<sqltoast::row_value_constructor_t>& val : stmt.insert_values)
            ptr.indent(out) << "- " << *val;
        ptr.indent_pop(out);
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::insert_select_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.use_default_columns())
        ptr.indent(out) << "default_columns: true";
    else {
        ptr.indent(out) << "columns:";
        ptr.indent_push(out);
        for (const sqltoast::lexeme_t& col : stmt.insert_columns)
            ptr.indent(out) << "- " << col;
        ptr.indent_pop(out);
    }
    ptr.indent(out) << "select:";
    ptr.indent_push(out);
    ptr.indent(out) << *stmt.select;
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::delete_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    if (stmt.where_condition)
        ptr.indent(out) << "where: " << *stmt.where_condition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::update_statement_t& stmt) {
    ptr.indent(out) << "table_name: " << stmt.table_name;
    ptr.indent(out) << "set_columns:";
    ptr.indent_push(out);
    for (const sqltoast::set_column_t& set_col : stmt.set_columns) {
        ptr.indent(out) << set_col.column_name << ": ";
        if (set_col.type == sqltoast::SET_COLUMN_TYPE_NULL)
            out << "NULL";
        else if (set_col.type == sqltoast::SET_COLUMN_TYPE_DEFAULT)
            out << "DEFAULT";
        else
            out << *set_col.value;
    }
    ptr.indent_pop(out);

    if (stmt.where_condition)
        ptr.indent(out) << "where: " << *stmt.where_condition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::grant_action_t& action) {
    switch (action.type) {
        case sqltoast::GRANT_ACTION_TYPE_SELECT:
            out << "SELECT";
            return;
        case sqltoast::GRANT_ACTION_TYPE_DELETE:
            out << "DELETE";
            return;
        case sqltoast::GRANT_ACTION_TYPE_USAGE:
            out << "USAGE";
            return;
        case sqltoast::GRANT_ACTION_TYPE_INSERT:
            out << "INSERT";
            break;
        case sqltoast::GRANT_ACTION_TYPE_UPDATE:
            out << "UPDATE";
            break;
        case sqltoast::GRANT_ACTION_TYPE_REFERENCES:
            out << "REFERENCES";
            break;
    }
    const sqltoast::column_list_grant_action_t& cla =
        static_cast<const sqltoast::column_list_grant_action_t&>(action);
    if (cla.columns.empty())
        return;
    out << " (";
    size_t x = 0;
    for (const sqltoast::lexeme_t& col : cla.columns) {
        if (x++ > 0)
            out << ',';
        out << col;
    }
    out << ')';
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::grant_statement_t& stmt) {
    ptr.indent(out) << "on: ";
    switch (stmt.object_type) {
        case sqltoast::GRANT_OBJECT_TYPE_TABLE:
            break;
        case sqltoast::GRANT_OBJECT_TYPE_DOMAIN:
            out << "DOMAIN ";
            break;
        case sqltoast::GRANT_OBJECT_TYPE_COLLATION:
            out << "COLLATION ";
            break;
        case sqltoast::GRANT_OBJECT_TYPE_CHARACTER_SET:
            out << "CHARACTER SET ";
            break;
        case sqltoast::GRANT_OBJECT_TYPE_TRANSLATION:
            out << "TRANSLATION ";
            break;
    }
    out << stmt.on;
    if (stmt.to_public())
        ptr.indent(out) << "to: PUBLIC";
    else
        ptr.indent(out) << "to: " << stmt.to;
    if (stmt.with_grant_option)
        ptr.indent(out) << "with_grant_option: YES";
    if (stmt.all_privileges()) {
        ptr.indent(out) << "privileges:";
        ptr.indent_push(out);
        ptr.indent(out) << "- ALL";
        ptr.indent_pop(out);
    } else {
        ptr.indent(out) << "privileges:";
        ptr.indent_push(out);
        for (const std::unique_ptr<sqltoast::grant_action_t>& action : stmt.privileges)
            ptr.indent(out) << "- " << *action;
        ptr.indent_pop(out);
    }
}

} // namespace sqltoast::print
} // namespace sqltoast
