/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_FILL_H
#define SQLTOASTER_FILL_H

#include <sqltoast/sqltoast.h>

#include "printer.h"
#include "node.h"

namespace sqltoaster {

void fill(mapping_t& node, const sqltoast::alter_table_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::between_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::boolean_factor_t& bf);
void fill(mapping_t& node, const sqltoast::boolean_primary_t& bp);
void fill(mapping_t& node, const sqltoast::boolean_term_t& bt);
void fill(mapping_t& node, const sqltoast::case_expression_t& expr);
void fill(mapping_t& node, const sqltoast::character_factor_t& factor);
void fill(mapping_t& node, const sqltoast::character_primary_t& cp);
void fill(mapping_t& node, const sqltoast::character_value_expression_t& cve);
void fill(mapping_t& node, const sqltoast::coalesce_function_t& func);
void fill(mapping_t& node, const sqltoast::comp_op_t& op);
void fill(mapping_t& node, const sqltoast::comp_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::convert_function_t& func);
void fill(mapping_t& node, const sqltoast::create_schema_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::create_table_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::create_view_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::datetime_factor_t& factor);
void fill(mapping_t& node, const sqltoast::datetime_field_t& field);
void fill(mapping_t& node, const sqltoast::datetime_primary_t& primary);
void fill(mapping_t& node, const sqltoast::datetime_term_t& term);
void fill(mapping_t& node, const sqltoast::datetime_value_t& value);
void fill(mapping_t& node, const sqltoast::datetime_value_expression_t& de);
void fill(mapping_t& node, const sqltoast::delete_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::derived_column_t& dc);
void fill(mapping_t& node, const sqltoast::derived_table_t& t);
void fill(mapping_t& node, const sqltoast::drop_column_action_t& action);
void fill(mapping_t& node, const sqltoast::drop_column_action_t& action);
void fill(mapping_t& node, const sqltoast::drop_schema_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::drop_table_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::drop_view_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::exists_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::extract_expression_t& expr);
void fill(mapping_t& node, const sqltoast::grant_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::in_subquery_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::in_values_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::insert_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::interval_factor_t& factor);
void fill(mapping_t& node, const sqltoast::interval_primary_t& primary);
void fill(mapping_t& node, const sqltoast::interval_qualifier_t& qualifier);
void fill(mapping_t& node, const sqltoast::interval_term_t& term);
void fill(mapping_t& node, const sqltoast::interval_value_expression_t& ie);
void fill(mapping_t& node, const sqltoast::join_specification_t& spec);
void fill(mapping_t& node, const sqltoast::joined_table_query_expression_t& qe);
void fill(mapping_t& node, const sqltoast::join_target_t& jt);
void fill(mapping_t& node, const sqltoast::length_expression_t& expr);
void fill(mapping_t& node, const sqltoast::like_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::match_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::non_join_query_expression_t& qe);
void fill(mapping_t& node, const sqltoast::non_join_query_primary_t& primary);
void fill(mapping_t& node, const sqltoast::non_join_query_term_t& term);
void fill(mapping_t& node, const sqltoast::nullif_function_t& func);
void fill(mapping_t& node, const sqltoast::null_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::numeric_expression_t& expr);
void fill(mapping_t& node, const sqltoast::numeric_factor_t& factor);
void fill(mapping_t& node, const sqltoast::numeric_function_t& func);
void fill(mapping_t& node, const sqltoast::numeric_primary_t& primary);
void fill(mapping_t& node, const sqltoast::numeric_term_t& term);
void fill(mapping_t& node, const sqltoast::numeric_value_t& value);
void fill(mapping_t& node, const sqltoast::overlaps_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::parenthesized_value_expression_t& expr);
void fill(mapping_t& node, const sqltoast::position_expression_t& expr);
void fill(mapping_t& node, const sqltoast::predicate_t& pred);
void fill(mapping_t& node, const sqltoast::quantified_comparison_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::query_expression_t& qe);
void fill(mapping_t& node, const sqltoast::query_specification_non_join_query_primary_t& primary);
void fill(mapping_t& node, const sqltoast::query_specification_t& query);
void fill(mapping_t& node, const sqltoast::row_value_constructor_element_t& rvce);
void fill(mapping_t& node, const sqltoast::row_value_constructor_t& rvc);
void fill(mapping_t& node, const sqltoast::row_value_expression_t& rve);
void fill(mapping_t& node, const sqltoast::scalar_subquery_t& subq);
void fill(mapping_t& node, const sqltoast::search_condition_t& sc);
void fill(mapping_t& node, const sqltoast::searched_case_expression_t& expr);
void fill(mapping_t& node, const sqltoast::select_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::set_function_t& func);
void fill(mapping_t& node, const sqltoast::simple_case_expression_t& expr);
void fill(mapping_t& node, const sqltoast::statement_t& stmt);
void fill(mapping_t& node, const sqltoast::string_function_t& func);
void fill(mapping_t& node, const sqltoast::substring_function_t& func);
void fill(mapping_t& node, const sqltoast::table_t& t);
void fill(mapping_t& node, const sqltoast::table_expression_t& table_exp);
void fill(mapping_t& node, const sqltoast::table_reference_t& tr);
void fill(mapping_t& node, const sqltoast::table_value_constructor_t& table_value);
void fill(mapping_t& node, const sqltoast::table_value_constructor_non_join_query_primary_t& primary);
void fill(mapping_t& node, const sqltoast::translate_function_t& func);
void fill(mapping_t& node, const sqltoast::trim_function_t& func);
void fill(mapping_t& node, const sqltoast::unique_predicate_t& pred);
void fill(mapping_t& node, const sqltoast::update_statement_t& stmt);
void fill(mapping_t& node, const sqltoast::value_expression_t& ve);
void fill(mapping_t& node, const sqltoast::value_expression_primary_t& primary);

} // namespace sqltoaster

#endif /* SQLTOASTER_FILL_H */
