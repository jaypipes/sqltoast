/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_PRINT_YAML_PRINTERS_H
#define SQLTOASTER_PRINT_YAML_PRINTERS_H

#include <sqltoast/print.h>

#include "../printer.h"

namespace sqltoaster {
namespace print {

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::add_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::add_constraint_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_table_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::between_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::boolean_factor_t& bf);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::boolean_primary_t& bp);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::boolean_term_t& bt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::character_factor_t& factor);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::character_value_expression_t& cve);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::comp_op_t& op);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::comp_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_schema_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_view_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::datetime_factor_t& factor);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::datetime_term_t& term);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::datetime_value_expression_t& de);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::delete_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::derived_column_t& dc);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_schema_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_view_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::exists_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::extract_expression_t& expr);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::grant_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::grant_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::in_subquery_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::in_values_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::insert_select_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::insert_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::interval_factor_t& factor);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::interval_term_t& term);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::interval_value_expression_t& ie);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::joined_table_query_expression_t& qe);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::joined_table_t& jt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::length_expression_t& expr);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::like_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::match_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_expression_t& qe);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_primary_t& primary);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_term_t& term);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::null_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::numeric_expression_t& expr);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::numeric_factor_t& factor);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::numeric_function_t& func);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::numeric_primary_t& primary);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::numeric_term_t& term);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::overlaps_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::position_expression_t& expr);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::quantified_comparison_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_expression_t& qe);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_specification_non_join_query_primary_t& primary);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_specification_t& query);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::row_value_constructor_element_t& rvce);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::row_value_constructor_t& rvc);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::row_value_expression_t& rve);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::search_condition_t& sc);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::select_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::table_expression_t& table_exp);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::unique_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::update_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::value_expression_t& ve);

} // namespace sqltoaster::print
} // namespace sqltoaster

#endif /* SQLTOASTER_PRINT_YAML_PRINTERS_H */
