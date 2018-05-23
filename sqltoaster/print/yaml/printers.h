/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_PRINT_YAML_PRINTERS_H
#define SQLTOASTER_PRINT_YAML_PRINTERS_H

#include "../../printer.h"

using namespace sqltoast;

namespace sqltoaster {

/* DDL/definition elements  */
void to_yaml(printer_t& ptr, std::ostream& out, const default_descriptor_t& column_def);
void to_yaml(printer_t& ptr, std::ostream& out, const column_definition_t& column_def);
void to_yaml(printer_t& ptr, std::ostream& out, const grouping_column_reference_t& dc);
void to_yaml(printer_t& ptr, std::ostream& out, const constraint_t& constraint);
void to_yaml(printer_t& ptr, std::ostream& out, const not_null_constraint_t& constraint);
void to_yaml(printer_t& ptr, std::ostream& out, const unique_constraint_t& constraint);
void to_yaml(printer_t& ptr, std::ostream& out, const foreign_key_constraint_t& constraint);
void to_yaml(printer_t& ptr, std::ostream& out, const data_type_descriptor_t& dt);
void to_yaml(printer_t& ptr, std::ostream& out, const char_string_t& cs);
void to_yaml(printer_t& ptr, std::ostream& out, const bit_string_t& bs);
void to_yaml(printer_t& ptr, std::ostream& out, const exact_numeric_t& num);
void to_yaml(printer_t& ptr, std::ostream& out, const approximate_numeric_t& num);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_t& dt);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_unit_t& unit);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_t& interval);
void to_yaml(printer_t& ptr, std::ostream& out, const identifier_t& id);
void to_yaml(printer_t& ptr, std::ostream& out, const derived_column_t& dc);

/*  predicates */
void to_yaml(printer_t& ptr, std::ostream& out, const predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const comp_op_t& op);
void to_yaml(printer_t& ptr, std::ostream& out, const comp_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const between_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const like_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const null_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const in_values_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const in_subquery_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const quantified_comparison_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const exists_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const unique_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const match_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const overlaps_predicate_t& pred);
void to_yaml(printer_t& ptr, std::ostream& out, const boolean_term_t& bt);
void to_yaml(printer_t& ptr, std::ostream& out, const boolean_factor_t& bf);
void to_yaml(printer_t& ptr, std::ostream& out, const boolean_primary_t& bp);
void to_yaml(printer_t& ptr, std::ostream& out, const search_condition_t& sc);

/*  statements */
void to_yaml(printer_t& ptr, std::ostream& out, const statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const create_schema_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const drop_schema_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const create_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const drop_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const alter_table_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const add_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const alter_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const drop_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const add_constraint_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const drop_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const alter_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const select_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const insert_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const insert_select_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const delete_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const update_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const create_view_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const drop_view_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const grant_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const grant_statement_t& stmt);

/* query components */
void to_yaml(printer_t& ptr, std::ostream& out, const table_reference_t& tr);
void to_yaml(printer_t& ptr, std::ostream& out, const table_t& t);
void to_yaml(printer_t& ptr, std::ostream& out, const derived_table_t& dt);
void to_yaml(printer_t& ptr, std::ostream& out, const join_specification_t& js);
void to_yaml(printer_t& ptr, std::ostream& out, const joined_table_t& jt);
void to_yaml(printer_t& ptr, std::ostream& out, const value_expression_primary_t& vep);
void to_yaml(printer_t& ptr, std::ostream& out, const unsigned_value_specification_t& uvs);
void to_yaml(printer_t& ptr, std::ostream& out, const case_expression_t& ce);
void to_yaml(printer_t& ptr, std::ostream& out, const coalesce_function_t& cf);
void to_yaml(printer_t& ptr, std::ostream& out, const nullif_function_t& nf);
void to_yaml(printer_t& ptr, std::ostream& out, const simple_case_expression_t& sce);
void to_yaml(printer_t& ptr, std::ostream& out, const searched_case_expression_t& sce);
void to_yaml(printer_t& ptr, std::ostream& out, const set_function_t& sf);
void to_yaml(printer_t& ptr, std::ostream& out, const value_subexpression_t& uvs);
void to_yaml(printer_t& ptr, std::ostream& out, const scalar_subquery_t& uvs);
void to_yaml(printer_t& ptr, std::ostream& out, const numeric_primary_t& np);
void to_yaml(printer_t& ptr, std::ostream& out, const numeric_value_t& nv);
void to_yaml(printer_t& ptr, std::ostream& out, const numeric_function_t& nf);
void to_yaml(printer_t& ptr, std::ostream& out, const position_expression_t& pe);
void to_yaml(printer_t& ptr, std::ostream& out, const extract_expression_t& ee);
void to_yaml(printer_t& ptr, std::ostream& out, const length_expression_t& le);
void to_yaml(printer_t& ptr, std::ostream& out, const numeric_factor_t& nf);
void to_yaml(printer_t& ptr, std::ostream& out, const numeric_term_t& nt);
void to_yaml(printer_t& ptr, std::ostream& out, const string_function_t& sf);
void to_yaml(printer_t& ptr, std::ostream& out, const substring_function_t& sf);
void to_yaml(printer_t& ptr, std::ostream& out, const convert_function_t& cf);
void to_yaml(printer_t& ptr, std::ostream& out, const translate_function_t& tf);
void to_yaml(printer_t& ptr, std::ostream& out, const trim_function_t& tf);
void to_yaml(printer_t& ptr, std::ostream& out, const character_primary_t& cp);
void to_yaml(printer_t& ptr, std::ostream& out, const character_factor_t& cf);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_primary_t& np);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_value_t& nv);
void to_yaml(printer_t& ptr, std::ostream& out, const current_datetime_function_t& df);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_factor_t& factor);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_term_t& term);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_field_t& df);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_qualifier_t& iq);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_primary_t& primary);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_factor_t& factor);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_term_t& tern);
void to_yaml(printer_t& ptr, std::ostream& out, const value_expression_t& ve);
void to_yaml(printer_t& ptr, std::ostream& out, const numeric_expression_t& ne);
void to_yaml(printer_t& ptr, std::ostream& out, const character_value_expression_t& ve);
void to_yaml(printer_t& ptr, std::ostream& out, const datetime_value_expression_t& ve);
void to_yaml(printer_t& ptr, std::ostream& out, const interval_value_expression_t& ve);
void to_yaml(printer_t& ptr, std::ostream& out, const row_value_constructor_t& rvc);
void to_yaml(printer_t& ptr, std::ostream& out, const row_value_constructor_element_t& rvce);
void to_yaml(printer_t& ptr, std::ostream& out, const row_value_expression_t& rve);

} // namespace sqltoast

#endif /* SQLTOASTER_PRINT_YAML_PRINTERS_H */
