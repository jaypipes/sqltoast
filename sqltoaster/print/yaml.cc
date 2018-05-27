/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sqltoast/print.h>

#include "yaml.h"

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
    ptr.indent(out) << "query:";
    ptr.indent_push(out);
    to_yaml(ptr, out, *stmt.query);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_specification_t& query) {
    if (query.distinct)
       ptr.indent(out) << "distinct: true";
    ptr.indent(out) << "selected_columns:";
    ptr.indent_push(out);
    for (const sqltoast::derived_column_t& dc : query.selected_columns)
        ptr.indent(out) << "- " << dc;
    ptr.indent_pop(out);
    to_yaml(ptr, out, *query.table_expression);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::table_expression_t& table_exp) {
    ptr.indent(out) << "referenced_tables:";
    ptr.indent_push(out);
    for (const std::unique_ptr<sqltoast::table_reference_t>& tr : table_exp.referenced_tables)
        ptr.indent(out) << "- " << *tr;
    ptr.indent_pop(out);
    if (table_exp.where_condition) {
        ptr.indent(out) << "where:";
        ptr.indent_push(out);
        to_yaml(ptr, out, *table_exp.where_condition);
        ptr.indent_pop(out);
    }
    if (! table_exp.group_by_columns.empty()) {
        ptr.indent(out) << " group_by:";
        ptr.indent_push(out);
        for (const sqltoast::grouping_column_reference_t& gcr : table_exp.group_by_columns)
            ptr.indent(out) << "- " << gcr;
        ptr.indent_pop(out);
    }
    if (table_exp.having_condition)
        ptr.indent(out) << "having: " << *table_exp.having_condition;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_expression_t& qe) {
    switch (qe.query_expression_type) {
        case sqltoast::QUERY_EXPRESSION_TYPE_NON_JOIN_QUERY_EXPRESSION:
            {
                const sqltoast::non_join_query_expression_t& sub =
                    static_cast<const sqltoast::non_join_query_expression_t&>(qe);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::QUERY_EXPRESSION_TYPE_JOINED_TABLE:
            {
                const sqltoast::joined_table_query_expression_t& sub =
                    static_cast<const sqltoast::joined_table_query_expression_t&>(qe);
                to_yaml(ptr, out, sub);
            }
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_expression_t& qe) {
    to_yaml(ptr, out, *qe.term);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_term_t& term) {
    to_yaml(ptr, out, *term.primary);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_primary_t& primary) {
    switch (primary.primary_type) {
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_QUERY_SPECIFICATION:
            {
                const sqltoast::query_specification_non_join_query_primary_t& sub =
                    static_cast<const sqltoast::query_specification_non_join_query_primary_t&>(primary);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_TABLE_VALUE_CONSTRUCTOR:
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_EXPLICIT_TABLE:
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_SUBEXPRESSION:
            // TODO
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_specification_non_join_query_primary_t& primary) {
    to_yaml(ptr, out, *primary.query_spec);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::joined_table_query_expression_t& qe) {
    to_yaml(ptr, out, *qe.joined_table);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::joined_table_t& jt) {
    switch (jt.join_type) {
        case sqltoast::JOIN_TYPE_INNER:
            ptr.indent(out) << "inner-join[";
            break;
        case sqltoast::JOIN_TYPE_LEFT:
            ptr.indent(out) << "left-join[";
            break;
        case sqltoast::JOIN_TYPE_RIGHT:
            ptr.indent(out) << "right-join[";
            break;
        case sqltoast::JOIN_TYPE_FULL:
            ptr.indent(out) << "full-join[";
            break;
        case sqltoast::JOIN_TYPE_CROSS:
            ptr.indent(out) << "cross-join[";
            break;
        case sqltoast::JOIN_TYPE_NATURAL:
            ptr.indent(out) << "natural-join[";
            break;
        case sqltoast::JOIN_TYPE_UNION:
            ptr.indent(out) << "union-join[";
            break;
        default:
            break;
    }
    out << *jt.left << ',' << *jt.right;
    if (jt.spec)
        out << *jt.spec;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_view_statement_t& stmt) {
    ptr.indent(out) << "view_name: " << stmt.table_name;
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
       ptr.indent(out) << "drop_behaviour: CASCADE";
    else
       ptr.indent(out) << "drop_behaviour: RESTRICT";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::select_statement_t& stmt) {
    const sqltoast::query_specification& query = *stmt.query;
    if (query.distinct)
       ptr.indent(out) << "distinct: true";
    ptr.indent(out) << "selected_columns:";
    ptr.indent_push(out);
    for (const sqltoast::derived_column_t& dc : query.selected_columns)
        ptr.indent(out) << "- " << dc;
    ptr.indent_pop(out);

    const sqltoast::table_expression_t& table_exp = *query.table_expression;
    ptr.indent(out) << "referenced_tables:";
    ptr.indent_push(out);
    for (const std::unique_ptr<sqltoast::table_reference_t>& tr : table_exp.referenced_tables)
        ptr.indent(out) << "- " << *tr;
    ptr.indent_pop(out);
    if (table_exp.where_condition) {
        ptr.indent(out) << "where:";
        ptr.indent_push(out);
        to_yaml(ptr, out, *table_exp.where_condition);
        ptr.indent_pop(out);
    }
    if (! table_exp.group_by_columns.empty()) {
        ptr.indent(out) << "group_by:";
        ptr.indent_push(out);
        for (const sqltoast::grouping_column_reference_t& gcr : table_exp.group_by_columns)
            ptr.indent(out) << "- " << gcr;
        ptr.indent_pop(out);
    }
    if (table_exp.having_condition) {
        ptr.indent(out) << "having:";
        ptr.indent_push(out);
        to_yaml(ptr, out, *table_exp.having_condition);
        ptr.indent_pop(out);
    }
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

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::search_condition_t& sc) {
    // OR'd operands are on the same "level" as each other for evaluation
    // purposes, which is why we don't attempt to indent here and just
    // output the terms in a list
    for (const std::unique_ptr<sqltoast::boolean_term_t>& or_term_p : sc.terms)
        to_yaml(ptr, out, *or_term_p);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::predicate_t& pred) {
    switch (pred.predicate_type) {
        case sqltoast::PREDICATE_TYPE_COMPARISON:
            ptr.indent(out) << "- predicate_type: COMPARISON";
            ptr.indent_push(out);
            {
                const sqltoast::comp_predicate_t& sub =
                    static_cast<const sqltoast::comp_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_BETWEEN:
            ptr.indent(out) << "- predicate_type: BETWEEN";
            ptr.indent_push(out);
            {
                const sqltoast::between_predicate_t& sub =
                    static_cast<const sqltoast::between_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_LIKE:
            ptr.indent(out) << "- predicate_type: LIKE";
            ptr.indent_push(out);
            {
                const sqltoast::like_predicate_t& sub =
                    static_cast<const sqltoast::like_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_NULL:
            ptr.indent(out) << "- predicate_type: NULL";
            ptr.indent_push(out);
            {
                const sqltoast::null_predicate_t& sub =
                    static_cast<const sqltoast::null_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_IN_VALUES:
            ptr.indent(out) << "- predicate_type: IN_VALUES";
            ptr.indent_push(out);
            {
                const sqltoast::in_values_predicate_t& sub =
                    static_cast<const sqltoast::in_values_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_IN_SUBQUERY:
            ptr.indent(out) << "- predicate_type: IN_SUBQUERY";
            ptr.indent_push(out);
            {
                const sqltoast::in_subquery_predicate_t& sub =
                    static_cast<const sqltoast::in_subquery_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_QUANTIFIED_COMPARISON:
            ptr.indent(out) << "- predicate_type: QUANTIFIED_COMPARISON";
            ptr.indent_push(out);
            {
                const sqltoast::quantified_comparison_predicate_t& sub =
                    static_cast<const sqltoast::quantified_comparison_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_EXISTS:
            ptr.indent(out) << "- predicate_type: EXISTS";
            ptr.indent_push(out);
            {
                const sqltoast::exists_predicate_t& sub =
                    static_cast<const sqltoast::exists_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_UNIQUE:
            ptr.indent(out) << "- predicate_type: UNIQUE";
            ptr.indent_push(out);
            {
                const sqltoast::unique_predicate_t& sub =
                    static_cast<const sqltoast::unique_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_MATCH:
            ptr.indent(out) << "- predicate_type: MATCH";
            ptr.indent_push(out);
            {
                const sqltoast::match_predicate_t& sub =
                    static_cast<const sqltoast::match_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
        case sqltoast::PREDICATE_TYPE_OVERLAPS:
            ptr.indent(out) << "- predicate_type: OVERLAPS";
            ptr.indent_push(out);
            {
                const sqltoast::overlaps_predicate_t& sub =
                    static_cast<const sqltoast::overlaps_predicate_t&>(pred);
                to_yaml(ptr, out, sub);
            }
            break;
    }
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::comp_op_t& op) {
    ptr.indent(out) << "op: ";
    switch (op) {
        case sqltoast::COMP_OP_EQUAL:
            out << "EQUAL";
            break;
        case sqltoast::COMP_OP_NOT_EQUAL:
            out << "NOT_EQUAL";
            break;
        case sqltoast::COMP_OP_LESS:
            out << "LESS_THAN";
            break;
        case sqltoast::COMP_OP_GREATER:
            out << "GREATER_THAN";
            break;
        case sqltoast::COMP_OP_LESS_EQUAL:
            out << "LESS_THAN_OR_EQUAL";
            break;
        case sqltoast::COMP_OP_GREATER_EQUAL:
            out << "GREATER_THAN_OR_EQUAL";
            break;
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::comp_predicate_t& pred) {
    to_yaml(ptr, out, pred.op);
    ptr.indent(out) << "left: " << *pred.left;
    ptr.indent(out) << "right: "<< *pred.right;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::between_predicate_t& pred) {
    ptr.indent(out) << "left: " << *pred.left;
    ptr.indent(out) << "comp_left: " << *pred.comp_left;
    ptr.indent(out) << "comp_right: "<< *pred.comp_right;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::like_predicate_t& pred) {
    ptr.indent(out) << "match: " << *pred.match;
    if (pred.reverse_op)
        ptr.indent(out) << "predicate_negate: true";
    ptr.indent(out) << "pattern: " << *pred.pattern;
    if (pred.escape_char)
        ptr.indent(out) << "escape_char: " << *pred.escape_char;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::null_predicate_t& pred) {
    if (pred.reverse_op)
        ptr.indent(out) << "negate: true";
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::in_values_predicate_t& pred) {
    ptr.indent(out) << "left: " << *pred.left;
    if (pred.reverse_op)
        ptr.indent(out) << "predicate_negate: true";
    ptr.indent(out) << "values:";
    ptr.indent_push(out);
    for (auto& ve : pred.values)
        ptr.indent(out) << "- " << *ve;
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::in_subquery_predicate_t& pred) {
    ptr.indent(out) << "left: " << *pred.left;
    ptr.indent(out) << "query:";
    ptr.indent_push(out);
    to_yaml(ptr, out, *pred.subquery);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::quantified_comparison_predicate_t& pred) {
    to_yaml(ptr, out, pred.op);
    ptr.indent(out) << "left: " << *pred.left;
    if (pred.quantifier == sqltoast::QUANTIFIER_ALL)
        ptr.indent(out) << "quantifier: ALL";
    else
        ptr.indent(out) << "quantifier: ANY";
    ptr.indent(out) << "query:";
    ptr.indent_push(out);
    to_yaml(ptr, out, *pred.subquery);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::exists_predicate_t& pred) {
    ptr.indent(out) << "query:";
    ptr.indent_push(out);
    to_yaml(ptr, out, *pred.subquery);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::unique_predicate_t& pred) {
    ptr.indent(out) << "query:";
    ptr.indent_push(out);
    to_yaml(ptr, out, *pred.subquery);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::match_predicate_t& pred) {
    ptr.indent(out) << "left: " << *pred.left;
    if (pred.match_unique)
        ptr.indent(out) << "unique: true";
    if (pred.match_partial)
        ptr.indent(out) << "partial: true";
    ptr.indent(out) << "query:";
    ptr.indent_push(out);
    to_yaml(ptr, out, *pred.subquery);
    ptr.indent_pop(out);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::overlaps_predicate_t& pred) {
    ptr.indent(out) << "left: " << *pred.left;
    ptr.indent(out) << "right: " << *pred.right;
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::boolean_primary_t& bp) {
    if (bp.predicate)
        to_yaml(ptr, out, *bp.predicate);
    else
        to_yaml(ptr, out, *bp.search_condition);
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::boolean_factor_t& bf) {
    to_yaml(ptr, out, *bf.primary);
    if (bf.reverse_op) {
        ptr.indent_push(out);
        ptr.indent(out) << "factor_negate: true";
        ptr.indent_pop(out);
    }
}

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::boolean_term_t& bt) {
    to_yaml(ptr, out, *bt.factor);
    if (bt.and_operand) {
        ptr.indent_push(out);
        ptr.indent(out) << "and:";
        ptr.indent_push(out);
        to_yaml(ptr, out, *bt.and_operand);
        ptr.indent_pop(out);
        ptr.indent_pop(out);
    }
}

} // namespace sqltoast::print
} // namespace sqltoast
