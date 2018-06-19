/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include <sqltoast/print.h>

#include "../node.h"
#include "../fill.h"

namespace sqltoaster {

void fill(mapping_t& node, const sqltoast::statement_t& stmt) {
    std::unique_ptr<node_t> subnode = std::make_unique<mapping_t>();
    mapping_t& submap = static_cast<mapping_t&>(*subnode);
    switch (stmt.type) {
        case sqltoast::STATEMENT_TYPE_CREATE_SCHEMA:
            node.setattr("type", "CREATE_SCHEMA");
            {
                const sqltoast::create_schema_statement_t& sub =
                    static_cast<const sqltoast::create_schema_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("create_schema_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DROP_SCHEMA:
            node.setattr("type", "DROP_SCHEMA");
            {
                const sqltoast::drop_schema_statement_t& sub =
                    static_cast<const sqltoast::drop_schema_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("drop_schema_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_CREATE_TABLE:
            node.setattr("type", "CREATE_TABLE");
            {
                const sqltoast::create_table_statement_t& sub =
                    static_cast<const sqltoast::create_table_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("create_table_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DROP_TABLE:
            node.setattr("type", "DROP_TABLE");
            {
                const sqltoast::drop_table_statement_t& sub =
                    static_cast<const sqltoast::drop_table_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("drop_table_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_ALTER_TABLE:
            node.setattr("type", "ALTER_TABLE");
            {
                const sqltoast::alter_table_statement_t& sub =
                    static_cast<const sqltoast::alter_table_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("alter_table_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_CREATE_VIEW:
            node.setattr("type", "CREATE_VIEW");
            {
                const sqltoast::create_view_statement_t& sub =
                    static_cast<const sqltoast::create_view_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("create_view_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DROP_VIEW:
            node.setattr("type", "DROP_VIEW");
            {
                const sqltoast::drop_view_statement_t& sub =
                    static_cast<const sqltoast::drop_view_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("drop_view_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_SELECT:
            node.setattr("type", "SELECT");
            {
                const sqltoast::select_statement_t& sub =
                    static_cast<const sqltoast::select_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("select_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_INSERT:
            node.setattr("type", "INSERT");
            {
                const sqltoast::insert_statement_t& sub =
                    static_cast<const sqltoast::insert_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("insert_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_DELETE:
            node.setattr("type", "DELETE");
            {
                const sqltoast::delete_statement_t& sub =
                    static_cast<const sqltoast::delete_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("delete_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_UPDATE:
            node.setattr("type", "UPDATE");
            {
                const sqltoast::update_statement_t& sub =
                    static_cast<const sqltoast::update_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("update_statement", subnode);
            }
            break;
        case sqltoast::STATEMENT_TYPE_COMMIT:
            node.setattr("type", "COMMIT");
            break;
        case sqltoast::STATEMENT_TYPE_ROLLBACK:
            node.setattr("type", "ROLLBACK");
            break;
        case sqltoast::STATEMENT_TYPE_GRANT:
            node.setattr("type", "GRANT");
            {
                const sqltoast::grant_statement_t& sub =
                    static_cast<const sqltoast::grant_statement_t&>(stmt);
                fill(submap, sub);
                node.setattr("grant_statement", subnode);
            }
            break;
        default:
            break;
    }
}

void fill(mapping_t& node, const sqltoast::create_schema_statement_t& stmt) {
    node.setattr("schema_name", stmt.schema_name);
    if (stmt.authorization_identifier)
        node.setattr("authorization_identifier", stmt.authorization_identifier);
    if (stmt.default_charset)
        node.setattr("default_charset", stmt.default_charset);
}

void fill(mapping_t& node, const sqltoast::drop_schema_statement_t& stmt) {
    node.setattr("schema_name", stmt.schema_name);
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
        node.setattr("drop_behaviour", "CASCADE");
    else
        node.setattr("drop_behaviour", "RESTRICT");
}

void fill(mapping_t& node, const sqltoast::create_table_statement_t& stmt) {
    node.setattr("table_name", stmt.table_name);
    if (stmt.table_type != sqltoast::TABLE_TYPE_NORMAL) {
        if (stmt.table_type == sqltoast::TABLE_TYPE_TEMPORARY_GLOBAL)
            node.setattr("temporary", "GLOBAL");
        else
            node.setattr("temporary", "LOCAL");
    }
    std::unique_ptr<node_t> cdefs_node = std::make_unique<mapping_t>();
    mapping_t& cdefs_map = static_cast<mapping_t&>(*cdefs_node);
    for (auto cdef_it = stmt.column_definitions.cbegin();
            cdef_it != stmt.column_definitions.cend();
            cdef_it++) {
        const sqltoast::column_definition_t& cdef = *(*cdef_it);
        std::stringstream val;
        if (cdef.data_type.get()) {
            val << *cdef.data_type;
        } else {
            val << " UNKNOWN";
        }
        if (cdef.default_descriptor.get()) {
            val << " " << *cdef.default_descriptor;
        }
        for (const std::unique_ptr<sqltoast::constraint_t>& c: cdef.constraints)
            val << *c;
        if (cdef.collate) {
            val << " COLLATE " << cdef.collate;
        }
        cdefs_map.setattr(std::string(cdef.name.start, cdef.name.end).c_str(), val.str());
    }
    node.setattr("column_definitions", cdefs_node);
    if (stmt.constraints.size() > 0) {
        std::unique_ptr<node_t> constrs_node = std::make_unique<sequence_t>();
        sequence_t& constrs_seq = static_cast<sequence_t&>(*constrs_node);
        for (auto constraint_it = stmt.constraints.begin();
             constraint_it != stmt.constraints.end();
             constraint_it++) {
            std::stringstream val;
            val << *(*constraint_it);
            constrs_seq.append(val.str());
        }
        node.setattr("constraints", constrs_node);
    }
}

void fill(mapping_t& node, const sqltoast::drop_table_statement_t& stmt) {
    node.setattr("table_name", stmt.table_name);
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
        node.setattr("drop_behaviour", "CASCADE");
    else
        node.setattr("drop_behaviour", "RESTRICT");
}

void fill(mapping_t& node, const sqltoast::alter_table_statement_t& stmt) {
    node.setattr("table_name", stmt.table_name);
    std::stringstream val;
    val << *stmt.action;
    node.setattr("action", val.str());
}

void fill(mapping_t& node, const sqltoast::create_view_statement_t& stmt) {
    node.setattr("view_name", stmt.table_name);
    if (! stmt.columns.empty()) {
        std::unique_ptr<node_t> cols_node = std::make_unique<sequence_t>();
        sequence_t& cols_seq = static_cast<sequence_t&>(*cols_node);
        for (const auto& column : stmt.columns)
            cols_seq.append(column);
        node.setattr("columns", cols_node);
    }
    if (stmt.check_option != sqltoast::CHECK_OPTION_NONE) {
        if (stmt.check_option == sqltoast::CHECK_OPTION_LOCAL)
            node.setattr("check_option", "LOCAL");
        else
            node.setattr("check_option", "CASCADED");
    }
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *stmt.query);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::query_specification_t& query) {
    if (query.distinct)
        node.setattr("distinct", "true");
    std::unique_ptr<node_t> cols_node = std::make_unique<sequence_t>();
    sequence_t& cols_seq = static_cast<sequence_t&>(*cols_node);
    for (const sqltoast::derived_column_t& dc : query.selected_columns) {
        std::unique_ptr<node_t> col_node = std::make_unique<mapping_t>();
        mapping_t& col_map = static_cast<mapping_t&>(*col_node);
        fill(col_map, dc);
        cols_seq.append(col_node);
    }
    node.setattr("selected_columns", cols_node);
    fill(node, *query.table_expression);
}

void fill(mapping_t& node, const sqltoast::derived_column_t& dc) {
    if (dc.value) {
        fill(node, *dc.value);
        if (dc.alias)
            node.setattr("alias", dc.alias);
    }
    else
        // NOTE(jaypipes): the * cannot be aliased in a production
        node.setattr("asterisk", "true");
}

void fill(mapping_t& node, const sqltoast::table_expression_t& table_exp) {
    std::unique_ptr<node_t> tables_node = std::make_unique<sequence_t>();
    sequence_t& tables_seq = static_cast<sequence_t&>(*tables_node);
    for (const std::unique_ptr<sqltoast::table_reference_t>& tr : table_exp.referenced_tables) {
        std::unique_ptr<node_t> tbl_node = std::make_unique<mapping_t>();
        mapping_t& tbl_map = static_cast<mapping_t&>(*tbl_node);
        fill(tbl_map, *tr);
        tables_seq.append(tbl_node);
    }
    node.setattr("referenced_tables", tables_node);
    if (table_exp.where_condition) {
        std::unique_ptr<node_t> where_node = std::make_unique<mapping_t>();
        mapping_t& where_map = static_cast<mapping_t&>(*where_node);
        fill(where_map, *table_exp.where_condition);
        node.setattr("where", where_node);
    }
    if (! table_exp.group_by_columns.empty()) {
        std::unique_ptr<node_t> gcr_node = std::make_unique<sequence_t>();
        sequence_t& gcr_seq = static_cast<sequence_t&>(*gcr_node);
        for (const sqltoast::grouping_column_reference_t& gcr : table_exp.group_by_columns) {
            std::stringstream val;
            val << gcr;
            gcr_seq.append(val.str());
        }
        node.setattr("group_by", gcr_node);
    }
    if (table_exp.having_condition) {
        std::unique_ptr<node_t> having_node = std::make_unique<mapping_t>();
        mapping_t& having_map = static_cast<mapping_t&>(*having_node);
        fill(having_map, *table_exp.having_condition);
        node.setattr("having", having_node);
    }
}

void fill(mapping_t& node, const sqltoast::query_expression_t& qe) {
    switch (qe.query_expression_type) {
        case sqltoast::QUERY_EXPRESSION_TYPE_NON_JOIN_QUERY_EXPRESSION:
            {
                const sqltoast::non_join_query_expression_t& sub =
                    static_cast<const sqltoast::non_join_query_expression_t&>(qe);
                fill(node, sub);
            }
            break;
        case sqltoast::QUERY_EXPRESSION_TYPE_JOINED_TABLE:
            {
                const sqltoast::joined_table_query_expression_t& sub =
                    static_cast<const sqltoast::joined_table_query_expression_t&>(qe);
                fill(node, sub);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::non_join_query_expression_t& qe) {
    fill(node, *qe.term);
}

void fill(mapping_t& node, const sqltoast::non_join_query_term_t& term) {
    fill(node, *term.primary);
}

void fill(mapping_t& node, const sqltoast::non_join_query_primary_t& primary) {
    switch (primary.primary_type) {
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_QUERY_SPECIFICATION:
            {
                const sqltoast::query_specification_non_join_query_primary_t& sub =
                    static_cast<const sqltoast::query_specification_non_join_query_primary_t&>(primary);
                fill(node, sub);
            }
            break;
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_TABLE_VALUE_CONSTRUCTOR:
            {
                const sqltoast::table_value_constructor_non_join_query_primary_t& sub =
                    static_cast<const sqltoast::table_value_constructor_non_join_query_primary_t&>(primary);
                fill(node, sub);
            }
            break;
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_EXPLICIT_TABLE:
        case sqltoast::NON_JOIN_QUERY_PRIMARY_TYPE_SUBEXPRESSION:
            // TODO
            break;
    }
}

void fill(mapping_t& node, const sqltoast::query_specification_non_join_query_primary_t& primary) {
    fill(node, *primary.query_spec);
}

void fill(mapping_t& node, const sqltoast::table_value_constructor_non_join_query_primary_t& primary) {
    fill(node, *primary.table_value);
}

void fill(mapping_t& node, const sqltoast::table_value_constructor_t& table_value) {
    std::unique_ptr<node_t> values_node = std::make_unique<sequence_t>();
    sequence_t& values_seq = static_cast<sequence_t&>(*values_node);
    for (const std::unique_ptr<sqltoast::row_value_constructor_t>& value : table_value.values) {
        std::unique_ptr<node_t> rvc_node = std::make_unique<mapping_t>();
        mapping_t& rvc_map = static_cast<mapping_t&>(*rvc_node);
        fill(rvc_map, *value);
        values_seq.append(rvc_node);
    }
    node.setattr("values", values_node);
}

void fill(mapping_t& node, const sqltoast::joined_table_query_expression_t& qe) {
    fill(node, *qe.joined_table);
}

void fill(mapping_t& node, const sqltoast::table_reference_t& tr) {
    std::unique_ptr<node_t> subnode = std::make_unique<mapping_t>();
    mapping_t& submap = static_cast<mapping_t&>(*subnode);
    switch (tr.type) {
        case sqltoast::TABLE_REFERENCE_TYPE_TABLE:
            node.setattr("type", "TABLE");
            {
                const sqltoast::table_t& sub =
                    static_cast<const sqltoast::table_t&>(tr);
                fill(submap, sub);
                node.setattr("table", subnode);
            }
            break;
        case sqltoast::TABLE_REFERENCE_TYPE_DERIVED_TABLE:
            node.setattr("type", "DERIVED_TABLE");
            {
                const sqltoast::derived_table_t& sub =
                    static_cast<const sqltoast::derived_table_t&>(tr);
                fill(submap, sub);
                node.setattr("derived_table", subnode);
            }
            break;
    }
    if (tr.joined) {
        std::unique_ptr<node_t> joined_node = std::make_unique<mapping_t>();
        mapping_t& joined_map = static_cast<mapping_t&>(*joined_node);
        fill(joined_map, *tr.joined);
        node.setattr("joined", joined_node);
    }
}

void fill(mapping_t& node, const sqltoast::table_t& t) {
    node.setattr("name", t.table_name);
    if (t.has_alias())
        node.setattr("alias", t.correlation_spec->alias);
}

void fill(mapping_t& node, const sqltoast::derived_table_t& t) {
    // NOTE(jaypipes): derived tables always have a <correlation spec> which
    // contains an alias attribute
    node.setattr("name", t.correlation_spec.alias);
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *t.query);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::join_target_t& jt) {
    switch (jt.join_type) {
        case sqltoast::JOIN_TYPE_INNER:
            node.setattr("type", "INNER_JOIN");
            break;
        case sqltoast::JOIN_TYPE_LEFT:
            node.setattr("type", "LEFT_JOIN");
            break;
        case sqltoast::JOIN_TYPE_RIGHT:
            node.setattr("type", "RIGHT_JOIN");
            break;
        case sqltoast::JOIN_TYPE_FULL:
            node.setattr("type", "FULL_JOIN");
            break;
        case sqltoast::JOIN_TYPE_CROSS:
            node.setattr("type", "CROSS_JOIN");
            break;
        case sqltoast::JOIN_TYPE_NATURAL:
            node.setattr("type", "NATURAL_JOIN");
            break;
        case sqltoast::JOIN_TYPE_UNION:
            node.setattr("type", "UNION_JOIN");
            break;
        default:
            break;
    }
    std::unique_ptr<node_t> table_ref_node = std::make_unique<mapping_t>();
    mapping_t& table_ref_map = static_cast<mapping_t&>(*table_ref_node);
    fill(table_ref_map, *jt.table_ref);
    node.setattr("table_reference", table_ref_node);
    if (jt.join_spec) {
        std::unique_ptr<node_t> spec_node = std::make_unique<mapping_t>();
        mapping_t& spec_map = static_cast<mapping_t&>(*spec_node);
        fill(spec_map, *jt.join_spec);
        node.setattr("specification", spec_node);
    }
}

void fill(mapping_t& node, const sqltoast::join_specification_t& spec) {
    if (spec.condition) {
        std::unique_ptr<node_t> on_node = std::make_unique<mapping_t>();
        mapping_t& on_map = static_cast<mapping_t&>(*on_node);
        fill(on_map, *spec.condition);
        node.setattr("on", on_node);
    } else if (! spec.named_columns.empty()) {
        std::unique_ptr<node_t> cols_node = std::make_unique<sequence_t>();
        sequence_t& cols_seq = static_cast<sequence_t&>(*cols_node);
        for (const sqltoast::lexeme_t& col : spec.named_columns)
            cols_seq.append(col);
        node.setattr("using", cols_node);
    }
}

void fill(mapping_t& node, const sqltoast::drop_view_statement_t& stmt) {
    node.setattr("view_name", stmt.table_name);
    if (stmt.drop_behaviour == sqltoast::DROP_BEHAVIOUR_CASCADE)
        node.setattr("drop_behaviour", "CASCADE");
    else
        node.setattr("drop_behaviour", "RESTRICT");
}

void fill(mapping_t& node, const sqltoast::select_statement_t& stmt) {
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *stmt.query);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::insert_statement_t& stmt) {
    node.setattr("table_name", stmt.table_name);
    if (! stmt.insert_columns.empty()) {
        std::unique_ptr<node_t> cols_node = std::make_unique<sequence_t>();
        sequence_t& cols_seq = static_cast<sequence_t&>(*cols_node);
        for (const sqltoast::lexeme_t& col : stmt.insert_columns)
            cols_seq.append(col);
        node.setattr("columns", cols_node);
    }
    if (stmt.query) {
        std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
        mapping_t& query_map = static_cast<mapping_t&>(*query_node);
        fill(query_map, *stmt.query);
        node.setattr("query", query_node);
    } else {
        node.setattr("default_values", "true");
    }
}

void fill(mapping_t& node, const sqltoast::delete_statement_t& stmt) {
    node.setattr("table_name", stmt.table_name);
    if (stmt.where_condition) {
        std::unique_ptr<node_t> where_node = std::make_unique<mapping_t>();
        mapping_t& where_map = static_cast<mapping_t&>(*where_node);
        fill(where_map, *stmt.where_condition);
        node.setattr("where", where_node);
    }
}

void fill(mapping_t& node, const sqltoast::update_statement_t& stmt) {
    node.setattr("table_name", stmt.table_name);
    std::unique_ptr<node_t> cols_node = std::make_unique<mapping_t>();
    mapping_t& cols_map = static_cast<mapping_t&>(*cols_node);
    for (const sqltoast::set_column_t& set_col : stmt.set_columns) {
        if (set_col.type == sqltoast::SET_COLUMN_TYPE_NULL)
            cols_map.setattr(set_col.column_name, "NULL");
        else if (set_col.type == sqltoast::SET_COLUMN_TYPE_DEFAULT)
            cols_map.setattr(set_col.column_name, "DEFAULT");
        else {
            std::stringstream val;
            val << *set_col.value;
            cols_map.setattr(set_col.column_name, val.str());
        }
    }
    node.setattr("set_columns", cols_node);

    if (stmt.where_condition) {
        std::unique_ptr<node_t> where_node = std::make_unique<mapping_t>();
        mapping_t& where_map = static_cast<mapping_t&>(*where_node);
        fill(where_map, *stmt.where_condition);
        node.setattr("where", where_node);
    }
}

void fill(mapping_t& node, const sqltoast::grant_statement_t& stmt) {
    std::stringstream val;
    switch (stmt.object_type) {
        case sqltoast::GRANT_OBJECT_TYPE_TABLE:
            break;
        case sqltoast::GRANT_OBJECT_TYPE_DOMAIN:
            val << "DOMAIN ";
            break;
        case sqltoast::GRANT_OBJECT_TYPE_COLLATION:
            val << "COLLATION ";
            break;
        case sqltoast::GRANT_OBJECT_TYPE_CHARACTER_SET:
            val << "CHARACTER SET ";
            break;
        case sqltoast::GRANT_OBJECT_TYPE_TRANSLATION:
            val << "TRANSLATION ";
            break;
    }
    val << stmt.on;
    node.setattr("on", val.str());
    if (stmt.to_public())
        node.setattr("to", "PUBLIC");
    else
        node.setattr("to", stmt.to);
    if (stmt.with_grant_option)
        node.setattr("with_grant_option", "YES");
    std::unique_ptr<node_t> privs_node = std::make_unique<sequence_t>();
    sequence_t& privs_seq = static_cast<sequence_t&>(*privs_node);
    if (stmt.all_privileges())
        privs_seq.append("ALL");
    else {
        for (const std::unique_ptr<sqltoast::grant_action_t>& action : stmt.privileges) {
            val.str(std::string()); val.clear();
            val << *action;
            privs_seq.append(val.str());
        }
    }
    node.setattr("privileges", privs_node);
}

void fill(mapping_t& node, const sqltoast::search_condition_t& sc) {
    // OR'd operands are on the same "level" as each other for evaluation
    // purposes, which is why we don't attempt to indent here and just
    // output the terms in a list
    std::unique_ptr<node_t> terms_node = std::make_unique<sequence_t>();
    sequence_t& terms_seq = static_cast<sequence_t&>(*terms_node);
    for (const std::unique_ptr<sqltoast::boolean_term_t>& or_term_p : sc.terms) {
        std::unique_ptr<node_t> term_node = std::make_unique<mapping_t>();
        mapping_t& term_map = static_cast<mapping_t&>(*term_node);
        fill(term_map, *or_term_p);
        terms_seq.append(term_node);
    }
    node.setattr("terms", terms_node);
}

void fill(mapping_t& node, const sqltoast::predicate_t& pred) {
    std::unique_ptr<node_t> subnode = std::make_unique<mapping_t>();
    mapping_t& submap = static_cast<mapping_t&>(*subnode);
    switch (pred.predicate_type) {
        case sqltoast::PREDICATE_TYPE_COMPARISON:
            node.setattr("type", "COMPARISON");
            {
                const sqltoast::comp_predicate_t& sub =
                    static_cast<const sqltoast::comp_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("comparison_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_BETWEEN:
            node.setattr("type", "BETWEEN");
            {
                const sqltoast::between_predicate_t& sub =
                    static_cast<const sqltoast::between_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("between_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_LIKE:
            node.setattr("type", "LIKE");
            {
                const sqltoast::like_predicate_t& sub =
                    static_cast<const sqltoast::like_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("like_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_NULL:
            node.setattr("type", "NULL");
            {
                const sqltoast::null_predicate_t& sub =
                    static_cast<const sqltoast::null_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("null_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_IN_VALUES:
            node.setattr("type", "IN_VALUES");
            {
                const sqltoast::in_values_predicate_t& sub =
                    static_cast<const sqltoast::in_values_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("in_values_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_IN_SUBQUERY:
            node.setattr("type", "IN_SUBQUERY");
            {
                const sqltoast::in_subquery_predicate_t& sub =
                    static_cast<const sqltoast::in_subquery_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("in_subquery_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_QUANTIFIED_COMPARISON:
            node.setattr("type", "QUANTIFIED_COMPARISON");
            {
                const sqltoast::quantified_comparison_predicate_t& sub =
                    static_cast<const sqltoast::quantified_comparison_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("quantified_comparison_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_EXISTS:
            node.setattr("type", "EXISTS");
            {
                const sqltoast::exists_predicate_t& sub =
                    static_cast<const sqltoast::exists_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("exists_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_UNIQUE:
            node.setattr("type", "UNIQUE");
            {
                const sqltoast::unique_predicate_t& sub =
                    static_cast<const sqltoast::unique_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("unique_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_MATCH:
            node.setattr("type", "MATCH");
            {
                const sqltoast::match_predicate_t& sub =
                    static_cast<const sqltoast::match_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("match_predicate", subnode);
            }
            break;
        case sqltoast::PREDICATE_TYPE_OVERLAPS:
            submap.setattr("type", "OVERLAPS");
            {
                const sqltoast::overlaps_predicate_t& sub =
                    static_cast<const sqltoast::overlaps_predicate_t&>(pred);
                fill(submap, sub);
                node.setattr("overlaps_predicate", subnode);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::comp_op_t& op) {
    switch (op) {
        case sqltoast::COMP_OP_EQUAL:
            node.setattr("op", "EQUAL");
            break;
        case sqltoast::COMP_OP_NOT_EQUAL:
            node.setattr("op", "NOT_EQUAL");
            break;
        case sqltoast::COMP_OP_LESS:
            node.setattr("op", "LESS_THAN");
            break;
        case sqltoast::COMP_OP_GREATER:
            node.setattr("op", "GREATER_THAN");
            break;
        case sqltoast::COMP_OP_LESS_EQUAL:
            node.setattr("op", "LESS_THAN_OR_EQUAL");
            break;
        case sqltoast::COMP_OP_GREATER_EQUAL:
            node.setattr("op", "GREATER_THAN_OR_EQUAL");
            break;
    }
}

void fill(mapping_t& node, const sqltoast::comp_predicate_t& pred) {
    fill(node, pred.op);
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
    mapping_t& right_map = static_cast<mapping_t&>(*right_node);
    fill(right_map, *pred.right);
    node.setattr("right", right_node);
}

void fill(mapping_t& node, const sqltoast::between_predicate_t& pred) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    std::unique_ptr<node_t> comp_left_node = std::make_unique<mapping_t>();
    mapping_t& comp_left_map = static_cast<mapping_t&>(*comp_left_node);
    fill(comp_left_map, *pred.comp_left);
    node.setattr("comp_left", comp_left_node);
    std::unique_ptr<node_t> comp_right_node = std::make_unique<mapping_t>();
    mapping_t& comp_right_map = static_cast<mapping_t&>(*comp_right_node);
    fill(comp_right_map, *pred.comp_right);
    node.setattr("comp_right", comp_right_node);
}

void fill(mapping_t& node, const sqltoast::like_predicate_t& pred) {
    std::unique_ptr<node_t> match_node = std::make_unique<mapping_t>();
    mapping_t& match_map = static_cast<mapping_t&>(*match_node);
    fill(match_map, *pred.match);
    node.setattr("match", match_node);
    if (pred.reverse_op)
        node.setattr("negate", "true");
    const sqltoast::character_value_expression_t& pattern_val =
        static_cast<const sqltoast::character_value_expression_t&>(*pred.pattern);
    std::unique_ptr<node_t> pattern_node = std::make_unique<mapping_t>();
    mapping_t& pattern_map = static_cast<mapping_t&>(*pattern_node);
    fill(pattern_map, pattern_val);
    node.setattr("pattern", pattern_node);
    if (pred.escape_char) {
        const sqltoast::character_value_expression_t& escape_char_val =
            static_cast<const sqltoast::character_value_expression_t&>(*pred.escape_char);
        std::unique_ptr<node_t> escape_char_node = std::make_unique<mapping_t>();
        mapping_t& escape_char_map = static_cast<mapping_t&>(*escape_char_node);
        fill(escape_char_map, escape_char_val);
        node.setattr("escape_char", escape_char_node);
    }
}

void fill(mapping_t& node, const sqltoast::null_predicate_t& pred) {
    if (pred.reverse_op)
        node.setattr("negate", "true");
}

void fill(mapping_t& node, const sqltoast::in_values_predicate_t& pred) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    if (pred.reverse_op)
        node.setattr("negate", "true");
    std::unique_ptr<node_t> values_node = std::make_unique<sequence_t>();
    sequence_t& values_seq = static_cast<sequence_t&>(*values_node);
    for (auto& ve : pred.values) {
        std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
        mapping_t& value_map = static_cast<mapping_t&>(*value_node);
        fill(value_map, *ve);
        values_seq.append(value_node);
    }
    node.setattr("values", values_node);
}

void fill(mapping_t& node, const sqltoast::in_subquery_predicate_t& pred) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *pred.subquery);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::quantified_comparison_predicate_t& pred) {
    fill(node, pred.op);
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    if (pred.quantifier == sqltoast::QUANTIFIER_ALL)
        node.setattr("quantifier", "ALL");
    else
        node.setattr("quantifier", "ANY");
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *pred.subquery);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::exists_predicate_t& pred) {
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *pred.subquery);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::unique_predicate_t& pred) {
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *pred.subquery);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::match_predicate_t& pred) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    if (pred.match_unique)
        node.setattr("unique", "true");
    if (pred.match_partial)
        node.setattr("partial", "true");
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *pred.subquery);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::overlaps_predicate_t& pred) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *pred.left);
    node.setattr("left", left_node);
    std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
    mapping_t& right_map = static_cast<mapping_t&>(*right_node);
    fill(right_map, *pred.right);
    node.setattr("right", right_node);
}

void fill(mapping_t& node, const sqltoast::boolean_primary_t& bp) {
    std::unique_ptr<node_t> subnode = std::make_unique<mapping_t>();
    mapping_t& submap = static_cast<mapping_t&>(*subnode);
    if (bp.predicate) {
        fill(submap, *bp.predicate);
        node.setattr("predicate", subnode);
    }
    else {
        fill(submap, *bp.search_condition);
        node.setattr("search_condition", subnode);
    }
}

void fill(mapping_t& node, const sqltoast::boolean_factor_t& bf) {
    fill(node, *bf.primary);
    if (bf.reverse_op)
        node.setattr("negate", "true");
}

void fill(mapping_t& node, const sqltoast::boolean_term_t& bt) {
    std::unique_ptr<node_t> subnode = std::make_unique<mapping_t>();
    mapping_t& submap = static_cast<mapping_t&>(*subnode);
    fill(submap, *bt.factor);
    if (bt.and_operand) {
        std::unique_ptr<node_t> and_node = std::make_unique<mapping_t>();
        mapping_t& and_map = static_cast<mapping_t&>(*and_node);
        fill(and_map, *bt.and_operand);
        submap.setattr("and", and_node);
    }
    node.setattr("factor", subnode);
}

void fill(mapping_t& node, const sqltoast::row_value_constructor_t& rvc) {
    switch (rvc.rvc_type) {
        case sqltoast::RVC_TYPE_ELEMENT:
            node.setattr("type", "ELEMENT");
            {
                std::unique_ptr<node_t> element_node = std::make_unique<mapping_t>();
                mapping_t& element_map = static_cast<mapping_t&>(*element_node);
                const sqltoast::row_value_constructor_element_t& sub =
                    static_cast<const sqltoast::row_value_constructor_element_t&>(rvc);
                fill(element_map, sub);
                node.setattr("element", element_node);
            }
            break;
        case sqltoast::RVC_TYPE_LIST:
            node.setattr("type", "LIST");
            {
                std::unique_ptr<node_t> elements_node = std::make_unique<sequence_t>();
                sequence_t& elements_seq = static_cast<sequence_t&>(*elements_node);
                const sqltoast::row_value_constructor_list_t& sub =
                    static_cast<const sqltoast::row_value_constructor_list_t&>(rvc);
                for (const auto& rvc_el : sub.elements) {
                    const sqltoast::row_value_constructor_element_t& el =
                        static_cast<const sqltoast::row_value_constructor_element_t&>(*rvc_el);
                    std::unique_ptr<node_t> element_node = std::make_unique<mapping_t>();
                    mapping_t& element_map = static_cast<mapping_t&>(*element_node);
                    fill(element_map, el);
                    elements_seq.append(element_node);
                }
                node.setattr("elements", elements_node);
            }
            break;
        case sqltoast::RVC_TYPE_SUBQUERY:
            // TODO
            break;
    }
}

void fill(mapping_t& node, const sqltoast::row_value_constructor_element_t& rvce) {
    switch (rvce.rvc_element_type) {
        case sqltoast::RVC_ELEMENT_TYPE_DEFAULT:
            node.setattr("type", "DEFAULT");
            break;
        case sqltoast::RVC_ELEMENT_TYPE_NULL:
            node.setattr("type", "NULL");
            break;
        case sqltoast::RVC_ELEMENT_TYPE_VALUE_EXPRESSION:
            node.setattr("type", "VALUE_EXPRESSION");
            {
                std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
                mapping_t& value_map = static_cast<mapping_t&>(*value_node);
                const sqltoast::row_value_expression_t& val =
                    static_cast<const sqltoast::row_value_expression_t&>(rvce);
                fill(value_map, val);
                node.setattr("value_expression", value_node);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::row_value_expression_t& rve) {
    // row_value_expressions only have a single attribute -- the value, so just
    // pass through to that and condense the YAML output accordingly
    fill(node, *rve.value);
}

void fill(mapping_t& node, const sqltoast::value_expression_t& ve) {
    switch (ve.type) {
        case sqltoast::VALUE_EXPRESSION_TYPE_NUMERIC_EXPRESSION:
            node.setattr("type", "NUMERIC_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::numeric_expression_t& sub =
                    static_cast<const sqltoast::numeric_expression_t&>(ve);
                fill(expr_map, sub);
                node.setattr("numeric_expression", expr_node);
            }
            break;
        case sqltoast::VALUE_EXPRESSION_TYPE_STRING_EXPRESSION:
            node.setattr("type", "STRING_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::character_value_expression_t& sub =
                    static_cast<const sqltoast::character_value_expression_t&>(ve);
                fill(expr_map, sub);
                node.setattr("string_expression", expr_node);
            }
            break;
        case sqltoast::VALUE_EXPRESSION_TYPE_DATETIME_EXPRESSION:
            node.setattr("type", "DATETIME_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::datetime_value_expression_t& sub =
                    static_cast<const sqltoast::datetime_value_expression_t&>(ve);
                fill(expr_map, sub);
                node.setattr("datetime_expression", expr_node);
            }
            break;
        case sqltoast::VALUE_EXPRESSION_TYPE_INTERVAL_EXPRESSION:
            node.setattr("type", "INTERVAL_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::interval_value_expression_t& sub =
                    static_cast<const sqltoast::interval_value_expression_t&>(ve);
                fill(expr_map, sub);
                node.setattr("interval_expression", expr_node);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::numeric_expression_t& ne) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *ne.left);
    node.setattr("left", left_node);
    if (ne.right) {
        if (ne.op == sqltoast::NUMERIC_OP_ADD)
            node.setattr("op", "ADD");
        else
            node.setattr("op", "SUBTRACT");
        std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
        mapping_t& right_map = static_cast<mapping_t&>(*right_node);
        fill(right_map, *ne.right);
        node.setattr("right", right_node);
    }
}

void fill(mapping_t& node, const sqltoast::numeric_term_t& term) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *term.left);
    node.setattr("left", left_node);
    if (term.right) {
        if (term.op == sqltoast::NUMERIC_OP_MULTIPLY)
            node.setattr("op", "MULTIPLY");
        else
            node.setattr("op", "DIVIDE");
        std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
        mapping_t& right_map = static_cast<mapping_t&>(*right_node);
        fill(right_map, *term.right);
        node.setattr("right", right_node);
    }
}

void fill(mapping_t& node, const sqltoast::numeric_factor_t& factor) {
    if (factor.sign != 0) {
        std::stringstream val;
        val << factor.sign;
        node.setattr("sign", val.str());
    }
    std::unique_ptr<node_t> primary_node = std::make_unique<mapping_t>();
    mapping_t& primary_map = static_cast<mapping_t&>(*primary_node);
    fill(primary_map, *factor.primary);
    node.setattr("primary", primary_node);
}

void fill(mapping_t& node, const sqltoast::numeric_primary_t& primary) {
    switch (primary.type) {
        case sqltoast::NUMERIC_PRIMARY_TYPE_VALUE:
            node.setattr("type", "VALUE");
            {
                std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
                mapping_t& value_map = static_cast<mapping_t&>(*value_node);
                const sqltoast::numeric_value_t& sub =
                    static_cast<const sqltoast::numeric_value_t&>(primary);
                std::stringstream val;
                fill(value_map, sub);
                node.setattr("value", value_node);
            }
            break;
        case sqltoast::NUMERIC_PRIMARY_TYPE_FUNCTION:
            node.setattr("type", "FUNCTION");
            {
                std::unique_ptr<node_t> function_node = std::make_unique<mapping_t>();
                mapping_t& function_map = static_cast<mapping_t&>(*function_node);
                const sqltoast::numeric_function_t& sub =
                    static_cast<const sqltoast::numeric_function_t&>(primary);
                fill(function_map, sub);
                node.setattr("function", function_node);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::numeric_value_t& value) {
    std::unique_ptr<node_t> primary_node = std::make_unique<mapping_t>();
    mapping_t& primary_map = static_cast<mapping_t&>(*primary_node);
    fill(primary_map, *value.primary);
    node.setattr("primary", primary_node);
}

void fill(mapping_t& node, const sqltoast::value_expression_primary_t& primary) {
    std::stringstream val;
    switch (primary.vep_type) {
        case sqltoast::VEP_TYPE_UNSIGNED_VALUE_SPECIFICATION:
            node.setattr("type", "UNSIGNED_VALUE_SPECIFICATION");
            {
                const sqltoast::unsigned_value_specification_t& sub =
                    static_cast<const sqltoast::unsigned_value_specification_t&>(primary);
                val << sub;
                node.setattr("unsigned_value_specification", val.str());
            }
            break;
        case sqltoast::VEP_TYPE_COLUMN_REFERENCE:
            node.setattr("type", "COLUMN_REFERENCE");
            val << primary.lexeme;
            node.setattr("column_reference", val.str());
            break;
        case sqltoast::VEP_TYPE_SET_FUNCTION_SPECIFICATION:
            node.setattr("type", "SET_FUNCTION_SPECIFICATION");
            {
                std::unique_ptr<node_t> function_node = std::make_unique<mapping_t>();
                mapping_t& function_map = static_cast<mapping_t&>(*function_node);
                const sqltoast::set_function_t& sub =
                    static_cast<const sqltoast::set_function_t&>(primary);
                fill(function_map, sub);
                node.setattr("set_function_specification", function_node);
            }
            break;
        case sqltoast::VEP_TYPE_PARENTHESIZED_VALUE_EXPRESSION:
            node.setattr("type", "PARENTHESIZED_VALUE_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::parenthesized_value_expression_t& sub =
                    static_cast<const sqltoast::parenthesized_value_expression_t&>(primary);
                fill(expr_map, sub);
                node.setattr("parenthesized_value_expression", expr_node);
            }
            break;
        case sqltoast::VEP_TYPE_CASE_EXPRESSION:
            node.setattr("type", "CASE_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::case_expression_t& sub =
                    static_cast<const sqltoast::case_expression_t&>(primary);
                fill(expr_map, sub);
                node.setattr("case_expression", expr_node);
            }
            break;
        case sqltoast::VEP_TYPE_SCALAR_SUBQUERY:
            node.setattr("type", "SCALAR_SUBQUERY");
            {
                std::unique_ptr<node_t> subq_node = std::make_unique<mapping_t>();
                mapping_t& subq_map = static_cast<mapping_t&>(*subq_node);
                const sqltoast::scalar_subquery_t& sub =
                    static_cast<const sqltoast::scalar_subquery_t&>(primary);
                fill(subq_map, sub);
                node.setattr("scalar_subquery", subq_node);
            }
            break;
        default:
            break;
    }
}

void fill(mapping_t& node, const sqltoast::set_function_t& func) {
    switch (func.func_type) {
        case sqltoast::SET_FUNCTION_TYPE_COUNT:
            node.setattr("type", "COUNT");
            break;
        case sqltoast::SET_FUNCTION_TYPE_AVG:
            node.setattr("type", "AVG");
            break;
        case sqltoast::SET_FUNCTION_TYPE_MIN:
            node.setattr("type", "MIN");
            break;
        case sqltoast::SET_FUNCTION_TYPE_MAX:
            node.setattr("type", "MAX");
            break;
        case sqltoast::SET_FUNCTION_TYPE_SUM:
            node.setattr("type", "SUM");
            break;
    }
    if (func.star)
        node.setattr("star", "true");
    if (func.distinct)
        node.setattr("distinct", "true");
    if (func.value) {
        std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
        mapping_t& value_map = static_cast<mapping_t&>(*value_node);
        fill(value_map, *func.value);
        node.setattr("value", value_node);
    }
}

void fill(mapping_t& node, const sqltoast::parenthesized_value_expression_t& expr) {
    std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
    mapping_t& value_map = static_cast<mapping_t&>(*value_node);
    fill(value_map, *expr.value);
    node.setattr("value", value_node);
}

void fill(mapping_t& node, const sqltoast::case_expression_t& expr) {
    std::stringstream val;
    switch (expr.case_type) {
        case sqltoast::CASE_EXPRESSION_TYPE_COALESCE_FUNCTION:
            node.setattr("type", "COALESCE_FUNCTION");
            {
                std::unique_ptr<node_t> function_node = std::make_unique<mapping_t>();
                mapping_t& function_map = static_cast<mapping_t&>(*function_node);
                const sqltoast::coalesce_function_t& sub =
                    static_cast<const sqltoast::coalesce_function_t&>(expr);
                fill(function_map, sub);
                node.setattr("coalesce_function", function_node);
            }
            break;
        case sqltoast::CASE_EXPRESSION_TYPE_NULLIF_FUNCTION:
            node.setattr("type", "NULLIF_FUNCTION");
            {
                std::unique_ptr<node_t> function_node = std::make_unique<mapping_t>();
                mapping_t& function_map = static_cast<mapping_t&>(*function_node);
                const sqltoast::nullif_function_t& sub =
                    static_cast<const sqltoast::nullif_function_t&>(expr);
                fill(function_map, sub);
                node.setattr("nullif_function", function_node);
            }
            break;
        case sqltoast::CASE_EXPRESSION_TYPE_SIMPLE_CASE:
            node.setattr("type", "SIMPLE_CASE_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::simple_case_expression_t& sub =
                    static_cast<const sqltoast::simple_case_expression_t&>(expr);
                fill(expr_map, sub);
                node.setattr("simple_case_expression", expr_node);
            }
            break;
        case sqltoast::CASE_EXPRESSION_TYPE_SEARCHED_CASE:
            node.setattr("type", "SEARCHED_CASE_EXPRESSION");
            {
                std::unique_ptr<node_t> expr_node = std::make_unique<mapping_t>();
                mapping_t& expr_map = static_cast<mapping_t&>(*expr_node);
                const sqltoast::searched_case_expression_t& sub =
                    static_cast<const sqltoast::searched_case_expression_t&>(expr);
                fill(expr_map, sub);
                node.setattr("searched_case_expression", expr_node);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::coalesce_function_t& func) {
    std::unique_ptr<node_t> values_node = std::make_unique<sequence_t>();
    sequence_t& values_seq = static_cast<sequence_t&>(*values_node);
    for (const auto& val : func.values) {
        std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
        mapping_t& value_map = static_cast<mapping_t&>(*value_node);
        fill(value_map, *val);
        values_seq.append(value_node);
    }
    node.setattr("values", values_node);
}

void fill(mapping_t& node, const sqltoast::nullif_function_t& func) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *func.left);
    node.setattr("left", left_node);
    std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
    mapping_t& right_map = static_cast<mapping_t&>(*right_node);
    fill(right_map, *func.right);
    node.setattr("right", right_node);
}

void fill(mapping_t& node, const sqltoast::simple_case_expression_t& expr) {
    std::unique_ptr<node_t> operand_node = std::make_unique<mapping_t>();
    mapping_t& operand_map = static_cast<mapping_t&>(*operand_node);
    fill(operand_map, *expr.operand);
    node.setattr("operand", operand_node);
    std::unique_ptr<node_t> when_clauses_node = std::make_unique<sequence_t>();
    sequence_t& when_clauses_seq = static_cast<sequence_t&>(*when_clauses_node);
    for (const auto& when_clause : expr.when_clauses) {
        std::unique_ptr<node_t> when_node = std::make_unique<mapping_t>();
        mapping_t& when_map = static_cast<mapping_t&>(*when_node);
        std::unique_ptr<node_t> when_operand_node = std::make_unique<mapping_t>();
        mapping_t& when_operand_map = static_cast<mapping_t&>(*when_operand_node);
        fill(when_operand_map, *when_clause.operand);
        when_map.setattr("operand", when_operand_node);
        std::unique_ptr<node_t> when_result_node = std::make_unique<mapping_t>();
        mapping_t& when_result_map = static_cast<mapping_t&>(*when_result_node);
        fill(when_result_map, *when_clause.result);
        when_map.setattr("result", when_result_node);
        when_clauses_seq.append(when_node);
    }
    node.setattr("when_clauses", when_clauses_node);
    if (expr.else_value) {
        std::unique_ptr<node_t> else_node = std::make_unique<mapping_t>();
        mapping_t& else_map = static_cast<mapping_t&>(*else_node);
        fill(else_map, *expr.else_value);
        node.setattr("else", else_node);
    }
}

void fill(mapping_t& node, const sqltoast::searched_case_expression_t& expr) {
    std::unique_ptr<node_t> when_clauses_node = std::make_unique<sequence_t>();
    sequence_t& when_clauses_seq = static_cast<sequence_t&>(*when_clauses_node);
    for (const auto& when_clause : expr.when_clauses) {
        std::unique_ptr<node_t> when_node = std::make_unique<mapping_t>();
        mapping_t& when_map = static_cast<mapping_t&>(*when_node);
        std::unique_ptr<node_t> when_condition_node = std::make_unique<mapping_t>();
        mapping_t& when_condition_map = static_cast<mapping_t&>(*when_condition_node);
        fill(when_condition_map, *when_clause.condition);
        when_map.setattr("condition", when_condition_node);
        std::unique_ptr<node_t> when_result_node = std::make_unique<mapping_t>();
        mapping_t& when_result_map = static_cast<mapping_t&>(*when_result_node);
        fill(when_result_map, *when_clause.result);
        when_map.setattr("result", when_result_node);
        when_clauses_seq.append(when_node);
    }
    node.setattr("when_clauses", when_clauses_node);
    if (expr.else_value) {
        std::unique_ptr<node_t> else_node = std::make_unique<mapping_t>();
        mapping_t& else_map = static_cast<mapping_t&>(*else_node);
        fill(else_map, *expr.else_value);
        node.setattr("else", else_node);
    }
}

void fill(mapping_t& node, const sqltoast::scalar_subquery_t& subq) {
    std::unique_ptr<node_t> query_node = std::make_unique<mapping_t>();
    mapping_t& query_map = static_cast<mapping_t&>(*query_node);
    fill(query_map, *subq.query);
    node.setattr("query", query_node);
}

void fill(mapping_t& node, const sqltoast::numeric_function_t& func) {
    switch (func.type) {
        case sqltoast::NUMERIC_FUNCTION_TYPE_POSITION:
            node.setattr("type", "POSITION");
            {
                std::unique_ptr<node_t> position_node = std::make_unique<mapping_t>();
                mapping_t& position_map = static_cast<mapping_t&>(*position_node);
                const sqltoast::position_expression_t& sub =
                    static_cast<const sqltoast::position_expression_t&>(func);
                fill(position_map, sub);
                node.setattr("char_position", position_node);
            }
            break;
        case sqltoast::NUMERIC_FUNCTION_TYPE_EXTRACT:
            node.setattr("type", "EXTRACT");
            {
                std::unique_ptr<node_t> extract_node = std::make_unique<mapping_t>();
                mapping_t& extract_map = static_cast<mapping_t&>(*extract_node);
                const sqltoast::extract_expression_t& sub =
                    static_cast<const sqltoast::extract_expression_t&>(func);
                fill(extract_map, sub);
                node.setattr("extract", extract_node);
            }
            break;
        case sqltoast::NUMERIC_FUNCTION_TYPE_CHAR_LENGTH:
            node.setattr("type", "CHAR_LENGTH");
            {
                std::unique_ptr<node_t> length_node = std::make_unique<mapping_t>();
                mapping_t& length_map = static_cast<mapping_t&>(*length_node);
                const sqltoast::length_expression_t& sub =
                    static_cast<const sqltoast::length_expression_t&>(func);
                fill(length_map, sub);
                node.setattr("char_length", length_node);
            }
            break;
        case sqltoast::NUMERIC_FUNCTION_TYPE_BIT_LENGTH:
            node.setattr("type", "BIT_LENGTH");
            {
                std::unique_ptr<node_t> length_node = std::make_unique<mapping_t>();
                mapping_t& length_map = static_cast<mapping_t&>(*length_node);
                const sqltoast::length_expression_t& sub =
                    static_cast<const sqltoast::length_expression_t&>(func);
                fill(length_map, sub);
                node.setattr("bit_length", length_node);
            }
            break;
        case sqltoast::NUMERIC_FUNCTION_TYPE_OCTET_LENGTH:
            node.setattr("type", "OCTET_LENGTH");
            {
                std::unique_ptr<node_t> length_node = std::make_unique<mapping_t>();
                mapping_t& length_map = static_cast<mapping_t&>(*length_node);
                const sqltoast::length_expression_t& sub =
                    static_cast<const sqltoast::length_expression_t&>(func);
                fill(length_map, sub);
                node.setattr("octet_length", length_node);
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::length_expression_t& expr) {
    std::unique_ptr<node_t> operand_node = std::make_unique<mapping_t>();
    mapping_t& operand_map = static_cast<mapping_t&>(*operand_node);
    fill(operand_map, *expr.operand);
    node.setattr("operand", operand_node);
}

void fill(mapping_t& node, const sqltoast::position_expression_t& expr) {
    std::unique_ptr<node_t> to_find_node = std::make_unique<mapping_t>();
    mapping_t& to_find_map = static_cast<mapping_t&>(*to_find_node);
    fill(to_find_map, *expr.to_find);
    node.setattr("find", to_find_node);
    std::unique_ptr<node_t> in_node = std::make_unique<mapping_t>();
    mapping_t& in_map = static_cast<mapping_t&>(*in_node);
    fill(in_map, *expr.subject);
    node.setattr("in", in_node);
}

void fill(mapping_t& node, const sqltoast::extract_expression_t& expr) {
    std::stringstream val;
    val << expr.extract_field;
    node.setattr("field", val.str());
    std::unique_ptr<node_t> source_node = std::make_unique<mapping_t>();
    mapping_t& source_map = static_cast<mapping_t&>(*source_node);
    fill(source_map, *expr.extract_source);
    node.setattr("source", source_node);
}

void fill(mapping_t& node, const sqltoast::character_value_expression_t& cve) {
    std::unique_ptr<node_t> factors_node = std::make_unique<sequence_t>();
    sequence_t& factors_seq = static_cast<sequence_t&>(*factors_node);
    for (const std::unique_ptr<sqltoast::character_factor_t>& factor : cve.values) {
        std::unique_ptr<node_t> factor_node = std::make_unique<mapping_t>();
        mapping_t& factor_map = static_cast<mapping_t&>(*factor_node);
        fill(factor_map, *factor);
        factors_seq.append(factor_node);
    }
    node.setattr("factors", factors_node);
}

void fill(mapping_t& node, const sqltoast::character_factor_t& factor) {
    std::unique_ptr<node_t> primary_node = std::make_unique<mapping_t>();
    mapping_t& primary_map = static_cast<mapping_t&>(*primary_node);
    fill(primary_map, *factor.primary);
    node.setattr("primary", primary_node);
    if (factor.collation)
        node.setattr("collation", factor.collation);
}

void fill(mapping_t& node, const sqltoast::character_primary_t& primary) {
    if (primary.value) {
        std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
        mapping_t& value_map = static_cast<mapping_t&>(*value_node);
        fill(value_map, *primary.value);
        node.setattr("value", value_node);
    }
    else {
        std::unique_ptr<node_t> function_node = std::make_unique<mapping_t>();
        mapping_t& function_map = static_cast<mapping_t&>(*function_node);
        fill(function_map, *primary.string_function);
        node.setattr("function", function_node);
    }
}

void fill(mapping_t& node, const sqltoast::string_function_t& func) {
    std::unique_ptr<node_t> operand_node = std::make_unique<mapping_t>();
    mapping_t& operand_map = static_cast<mapping_t&>(*operand_node);
    const sqltoast::character_value_expression_t& operand =
        static_cast<const sqltoast::character_value_expression_t&>(*func.operand);
    fill(operand_map, operand);
    switch (func.type) {
        case sqltoast::STRING_FUNCTION_TYPE_SUBSTRING:
            node.setattr("type", "SUBSTRING");
            node.setattr("operand", operand_node);
            {
                std::unique_ptr<node_t> substring_node = std::make_unique<mapping_t>();
                mapping_t& substring_map = static_cast<mapping_t&>(*substring_node);
                const sqltoast::substring_function_t& sub =
                    static_cast<const sqltoast::substring_function_t&>(func);
                fill(substring_map, sub);
                node.setattr("substring", substring_node);
            }
            break;
        case sqltoast::STRING_FUNCTION_TYPE_UPPER:
            node.setattr("type", "UPPER");
            node.setattr("operand", operand_node);
            break;
        case sqltoast::STRING_FUNCTION_TYPE_LOWER:
            node.setattr("type", "LOWER");
            node.setattr("operand", operand_node);
            break;
        case sqltoast::STRING_FUNCTION_TYPE_CONVERT:
            node.setattr("type", "CONVERT");
            node.setattr("operand", operand_node);
            {
                std::unique_ptr<node_t> convert_node = std::make_unique<mapping_t>();
                mapping_t& convert_map = static_cast<mapping_t&>(*convert_node);
                const sqltoast::convert_function_t& sub =
                    static_cast<const sqltoast::convert_function_t&>(func);
                fill(convert_map, sub);
                node.setattr("convert", convert_node);
            }
            break;
        case sqltoast::STRING_FUNCTION_TYPE_TRANSLATE:
            node.setattr("type", "TRANSLATE");
            node.setattr("operand", operand_node);
            {
                std::unique_ptr<node_t> translate_node = std::make_unique<mapping_t>();
                mapping_t& translate_map = static_cast<mapping_t&>(*translate_node);
                const sqltoast::translate_function_t& sub =
                    static_cast<const sqltoast::translate_function_t&>(func);
                fill(translate_map, sub);
                node.setattr("translate", translate_node);
            }
            break;
        case sqltoast::STRING_FUNCTION_TYPE_TRIM:
            node.setattr("type", "TRIM");
            node.setattr("operand", operand_node);
            {
                std::unique_ptr<node_t> trim_node = std::make_unique<mapping_t>();
                mapping_t& trim_map = static_cast<mapping_t&>(*trim_node);
                const sqltoast::trim_function_t& sub =
                    static_cast<const sqltoast::trim_function_t&>(func);
                fill(trim_map, sub);
                node.setattr("trim", trim_node);
            }
            break;
        default:
            // TODO
            break;
    }
}

void fill(mapping_t& node, const sqltoast::substring_function_t& func) {
    std::unique_ptr<node_t> start_pos_node = std::make_unique<mapping_t>();
    mapping_t& start_pos_map = static_cast<mapping_t&>(*start_pos_node);
    const sqltoast::numeric_expression_t& start_pos_val =
        static_cast<sqltoast::numeric_expression_t&>(*func.start_position_value);
    fill(start_pos_map, start_pos_val);
    node.setattr("start_position", start_pos_node);
    if (func.for_length_value) {
        std::unique_ptr<node_t> for_length_node = std::make_unique<mapping_t>();
        mapping_t& for_length_map = static_cast<mapping_t&>(*for_length_node);
        const sqltoast::numeric_expression_t& for_length_val =
            static_cast<sqltoast::numeric_expression_t&>(*func.for_length_value);
        fill(for_length_map, for_length_val);
        node.setattr("for_length", for_length_node);
    }
}

void fill(mapping_t& node, const sqltoast::convert_function_t& func) {
    node.setattr("using", func.conversion_name);
}

void fill(mapping_t& node, const sqltoast::translate_function_t& func) {
    node.setattr("using", func.translation_name);
}

void fill(mapping_t& node, const sqltoast::trim_function_t& func) {
    switch (func.specification) {
        case sqltoast::TRIM_SPECIFICATION_LEADING:
            node.setattr("specification", "LEADING");
            break;
        case sqltoast::TRIM_SPECIFICATION_TRAILING:
            node.setattr("specification", "TRAILING");
            break;
        default:
            node.setattr("specification", "BOTH");
            break;
    }
    if (func.trim_character) {
        std::unique_ptr<node_t> trim_char_node = std::make_unique<mapping_t>();
        mapping_t& trim_char_map = static_cast<mapping_t&>(*trim_char_node);
        const sqltoast::character_value_expression_t& trim_char_val =
            static_cast<sqltoast::character_value_expression_t&>(*func.trim_character);
        fill(trim_char_map, trim_char_val);
        node.setattr("trim_character", trim_char_node);
    }
}

void fill(mapping_t& node, const sqltoast::datetime_value_expression_t& de) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *de.left);
    node.setattr("left", left_node);
    if (de.right) {
        if (de.op == sqltoast::NUMERIC_OP_ADD)
            node.setattr("op", "ADD");
        else
            node.setattr("op", "SUBTRACT");
        std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
        mapping_t& right_map = static_cast<mapping_t&>(*right_node);
        fill(right_map, *de.right);
        node.setattr("right", right_node);
    }
}

void fill(mapping_t& node, const sqltoast::datetime_term_t& term) {
    std::unique_ptr<node_t> factor_node = std::make_unique<mapping_t>();
    mapping_t& factor_map = static_cast<mapping_t&>(*factor_node);
    fill(factor_map, *term.value); // TODO this should be term.factor...
    node.setattr("factor", factor_node);
}

void fill(mapping_t& node, const sqltoast::datetime_factor_t& factor) {
    if (factor.is_local_tz())
        node.setattr("time_zone", "LOCAL");
    else
        node.setattr("time_zone", factor.tz);
    std::unique_ptr<node_t> primary_node = std::make_unique<mapping_t>();
    mapping_t& primary_map = static_cast<mapping_t&>(*primary_node);
    fill(primary_map, *factor.primary);
    node.setattr("primary", primary_node);
}

void fill(mapping_t& node, const sqltoast::datetime_primary_t& primary) {
    std::stringstream val;
    switch (primary.type) {
        case sqltoast::DATETIME_PRIMARY_TYPE_VALUE:
            node.setattr("type", "VALUE");
            {
                std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
                mapping_t& value_map = static_cast<mapping_t&>(*value_node);
                const sqltoast::datetime_value_t& sub =
                    static_cast<const sqltoast::datetime_value_t&>(primary);
                fill(value_map, sub);
                node.setattr("value", value_node);
            }
            break;
        case sqltoast::DATETIME_PRIMARY_TYPE_FUNCTION:
            node.setattr("type", "FUNCTION");
            {
                const sqltoast::current_datetime_function_t& sub =
                    static_cast<const sqltoast::current_datetime_function_t&>(primary);
                val << sub;
                node.setattr("function", val.str());
            }
            break;
    }
}

void fill(mapping_t& node, const sqltoast::datetime_value_t& value) {
    std::unique_ptr<node_t> primary_node = std::make_unique<mapping_t>();
    mapping_t& primary_map = static_cast<mapping_t&>(*primary_node);
    fill(primary_map, *value.primary);
    node.setattr("primary", primary_node);
}

void fill(mapping_t& node, const sqltoast::datetime_field_t& field) {
    std::stringstream val;
    val << field.interval;
    node.setattr("interval", val.str());
    if (field.interval != sqltoast::INTERVAL_UNIT_SECOND) {
        if (field.precision > 0) {
            val.str(std::string()); val.clear();
            val << field.precision;
            node.setattr("leading_precision", val.str());
        }
    } else {
        // We can have 0 leading precision and non-zero fractional precision
        // for second intervals...
        if (field.precision > 0 ||
                (field.precision == 0 && field.fractional_precision > 0)) {
            val.str(std::string()); val.clear();
            val << field.precision;
            node.setattr("leading_precision", val.str());
            if (field.fractional_precision > 0) {
                val.str(std::string()); val.clear();
                val << field.fractional_precision;
                node.setattr("fractional_precision", val.str());
            }
        }
    }
}

void fill(mapping_t& node, const sqltoast::interval_value_expression_t& expr) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *expr.left);
    node.setattr("left", left_node);
    if (expr.right) {
        if (expr.op == sqltoast::NUMERIC_OP_ADD)
            node.setattr("op", "ADD");
        else
            node.setattr("op", "SUBTRACT");
        std::unique_ptr<node_t> right_node = std::make_unique<mapping_t>();
        mapping_t& right_map = static_cast<mapping_t&>(*right_node);
        fill(right_map, *expr.right);
        node.setattr("right", right_node);
    }
}

void fill(mapping_t& node, const sqltoast::interval_term_t& term) {
    std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
    mapping_t& left_map = static_cast<mapping_t&>(*left_node);
    fill(left_map, *term.left);
    node.setattr("left", left_node);
    if (term.right) {
        if (term.op == sqltoast::NUMERIC_OP_MULTIPLY)
            node.setattr("op", "MULTIPLY");
        else
            node.setattr("op", "DIVIDE");
        std::unique_ptr<node_t> left_node = std::make_unique<mapping_t>();
        mapping_t& left_map = static_cast<mapping_t&>(*left_node);
        fill(left_map, *term.left);
        node.setattr("left", left_node);
    }
}

void fill(mapping_t& node, const sqltoast::interval_factor_t& factor) {
    if (factor.sign != 0) {
        std::stringstream val;
        val << factor.sign;
        node.setattr("sign", val.str());
    }
    std::unique_ptr<node_t> primary_node = std::make_unique<mapping_t>();
    mapping_t& primary_map = static_cast<mapping_t&>(*primary_node);
    fill(primary_map, *factor.primary);
    node.setattr("primary", primary_node);
}

void fill(mapping_t& node, const sqltoast::interval_primary_t& primary) {
    std::unique_ptr<node_t> value_node = std::make_unique<mapping_t>();
    mapping_t& value_map = static_cast<mapping_t&>(*value_node);
    fill(value_map, *primary.value);
    node.setattr("value", value_node);
    if (primary.qualifier) {
        std::unique_ptr<node_t> qualifier_node = std::make_unique<mapping_t>();
        mapping_t& qualifier_map = static_cast<mapping_t&>(*qualifier_node);
        fill(qualifier_map, *primary.qualifier);
        node.setattr("qualifier", qualifier_node);
    }
}

void fill(mapping_t& node, const sqltoast::interval_qualifier_t& qualifier) {
    std::unique_ptr<node_t> start_node = std::make_unique<mapping_t>();
    mapping_t& start_map = static_cast<mapping_t&>(*start_node);
    fill(start_map, qualifier.start);
    node.setattr("start", start_node);
    if (qualifier.end) {
        std::unique_ptr<node_t> end_node = std::make_unique<mapping_t>();
        mapping_t& end_map = static_cast<mapping_t&>(*end_node);
        fill(end_map, *qualifier.end);
        node.setattr("end", end_node);
    }
}

} // namespace sqltoast
