/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_PRINT_H
#define SQLTOASTER_PRINT_H

#include "sqltoast/sqltoast.h"

namespace sqltoast {

/* DDL/definition elements  */
std::ostream& operator<< (std::ostream& out, const approximate_numeric_t& num);
std::ostream& operator<< (std::ostream& out, const bit_string_t& bs);
std::ostream& operator<< (std::ostream& out, const char_string_t& cs);
std::ostream& operator<< (std::ostream& out, const column_definition_t& column_def);
std::ostream& operator<< (std::ostream& out, const constraint_t& constraint);
std::ostream& operator<< (std::ostream& out, const data_type_descriptor_t& dt);
std::ostream& operator<< (std::ostream& out, const datetime_t& dt);
std::ostream& operator<< (std::ostream& out, const default_descriptor_t& column_def);
std::ostream& operator<< (std::ostream& out, const derived_column_t& dc);
std::ostream& operator<< (std::ostream& out, const exact_numeric_t& num);
std::ostream& operator<< (std::ostream& out, const foreign_key_constraint_t& constraint);
std::ostream& operator<< (std::ostream& out, const grouping_column_reference_t& dc);
std::ostream& operator<< (std::ostream& out, const identifier_t& id);
std::ostream& operator<< (std::ostream& out, const interval_t& interval);
std::ostream& operator<< (std::ostream& out, const interval_unit_t& unit);
std::ostream& operator<< (std::ostream& out, const not_null_constraint_t& constraint);
std::ostream& operator<< (std::ostream& out, const unique_constraint_t& constraint);

/*  predicates */
std::ostream& operator<< (std::ostream& out, const between_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const boolean_factor_t& bf);
std::ostream& operator<< (std::ostream& out, const boolean_primary_t& bp);
std::ostream& operator<< (std::ostream& out, const boolean_term_t& bt);
std::ostream& operator<< (std::ostream& out, const comp_op_t& op);
std::ostream& operator<< (std::ostream& out, const comp_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const exists_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const in_subquery_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const in_values_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const like_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const match_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const null_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const overlaps_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const quantified_comparison_predicate_t& pred);
std::ostream& operator<< (std::ostream& out, const search_condition_t& sc);
std::ostream& operator<< (std::ostream& out, const unique_predicate_t& pred);

/*  statements */
std::ostream& operator<< (std::ostream& out, const add_column_action_t& action);
std::ostream& operator<< (std::ostream& out, const add_constraint_action_t& action);
std::ostream& operator<< (std::ostream& out, const alter_column_action_t& action);
std::ostream& operator<< (std::ostream& out, const alter_table_action_t& action);
std::ostream& operator<< (std::ostream& out, const alter_table_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const create_schema_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const create_table_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const create_view_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const delete_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const drop_column_action_t& action);
std::ostream& operator<< (std::ostream& out, const drop_column_action_t& action);
std::ostream& operator<< (std::ostream& out, const drop_schema_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const drop_table_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const drop_view_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const grant_action_t& action);
std::ostream& operator<< (std::ostream& out, const grant_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const insert_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const select_statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const statement_t& stmt);
std::ostream& operator<< (std::ostream& out, const update_statement_t& stmt);

/* query components */
std::ostream& operator<< (std::ostream& out, const case_expression_t& ce);
std::ostream& operator<< (std::ostream& out, const case_expression_t& ce);
std::ostream& operator<< (std::ostream& out, const character_factor_t& cf);
std::ostream& operator<< (std::ostream& out, const character_primary_t& cp);
std::ostream& operator<< (std::ostream& out, const character_value_expression_t& ve);
std::ostream& operator<< (std::ostream& out, const coalesce_function_t& cf);
std::ostream& operator<< (std::ostream& out, const coalesce_function_t& cf);
std::ostream& operator<< (std::ostream& out, const convert_function_t& cf);
std::ostream& operator<< (std::ostream& out, const current_datetime_function_t& df);
std::ostream& operator<< (std::ostream& out, const datetime_factor_t& factor);
std::ostream& operator<< (std::ostream& out, const datetime_field_t& df);
std::ostream& operator<< (std::ostream& out, const datetime_primary_t& np);
std::ostream& operator<< (std::ostream& out, const datetime_term_t& term);
std::ostream& operator<< (std::ostream& out, const datetime_value_expression_t& ve);
std::ostream& operator<< (std::ostream& out, const datetime_value_t& nv);
std::ostream& operator<< (std::ostream& out, const derived_table_t& dt);
std::ostream& operator<< (std::ostream& out, const extract_expression_t& ee);
std::ostream& operator<< (std::ostream& out, const interval_factor_t& factor);
std::ostream& operator<< (std::ostream& out, const interval_primary_t& primary);
std::ostream& operator<< (std::ostream& out, const interval_qualifier_t& iq);
std::ostream& operator<< (std::ostream& out, const interval_term_t& tern);
std::ostream& operator<< (std::ostream& out, const interval_value_expression_t& ve);
std::ostream& operator<< (std::ostream& out, const join_specification_t& js);
std::ostream& operator<< (std::ostream& out, const joined_table_query_expression_t& qe);
std::ostream& operator<< (std::ostream& out, const join_target_t& jt);
std::ostream& operator<< (std::ostream& out, const length_expression_t& le);
std::ostream& operator<< (std::ostream& out, const non_join_query_expression_t& qe);
std::ostream& operator<< (std::ostream& out, const non_join_query_primary_t& primary);
std::ostream& operator<< (std::ostream& out, const non_join_query_term_t& term);
std::ostream& operator<< (std::ostream& out, const nullif_function_t& nf);
std::ostream& operator<< (std::ostream& out, const numeric_expression_t& ne);
std::ostream& operator<< (std::ostream& out, const numeric_factor_t& nf);
std::ostream& operator<< (std::ostream& out, const numeric_function_t& nf);
std::ostream& operator<< (std::ostream& out, const numeric_primary_t& np);
std::ostream& operator<< (std::ostream& out, const numeric_term_t& nt);
std::ostream& operator<< (std::ostream& out, const numeric_value_t& nv);
std::ostream& operator<< (std::ostream& out, const position_expression_t& pe);
std::ostream& operator<< (std::ostream& out, const query_expression_t& qe);
std::ostream& operator<< (std::ostream& out, const query_specification_non_join_query_primary_t& primary);
std::ostream& operator<< (std::ostream& out, const query_specification_t& qs);
std::ostream& operator<< (std::ostream& out, const row_value_constructor_element_t& rvce);
std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc);
std::ostream& operator<< (std::ostream& out, const row_value_expression_t& rve);
std::ostream& operator<< (std::ostream& out, const scalar_subquery_t& uvs);
std::ostream& operator<< (std::ostream& out, const searched_case_expression_t& sce);
std::ostream& operator<< (std::ostream& out, const set_function_t& sf);
std::ostream& operator<< (std::ostream& out, const simple_case_expression_t& sce);
std::ostream& operator<< (std::ostream& out, const string_function_t& sf);
std::ostream& operator<< (std::ostream& out, const substring_function_t& sf);
std::ostream& operator<< (std::ostream& out, const table_expression_t& te);
std::ostream& operator<< (std::ostream& out, const table_reference_t& tr);
std::ostream& operator<< (std::ostream& out, const table_t& t);
std::ostream& operator<< (std::ostream& out, const table_value_constructor_non_join_query_primary_t& primary);
std::ostream& operator<< (std::ostream& out, const table_value_constructor_t& table_value);
std::ostream& operator<< (std::ostream& out, const translate_function_t& tf);
std::ostream& operator<< (std::ostream& out, const trim_function_t& tf);
std::ostream& operator<< (std::ostream& out, const unsigned_value_specification_t& uvs);
std::ostream& operator<< (std::ostream& out, const unsigned_value_specification_t& uvs);
std::ostream& operator<< (std::ostream& out, const value_expression_primary_t& vep);
std::ostream& operator<< (std::ostream& out, const value_expression_primary_t& vep);
std::ostream& operator<< (std::ostream& out, const value_expression_t& ve);
std::ostream& operator<< (std::ostream& out, const parenthesized_value_expression_t& vep);

} // namespace sqltoast

#endif /* SQLTOASTER_PRINT_H */
