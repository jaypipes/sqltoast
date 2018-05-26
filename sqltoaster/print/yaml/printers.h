/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOASTER_PRINT_YAML_PRINTERS_H
#define SQLTOASTER_PRINT_YAML_PRINTERS_H

#include <sqltoast/print.h>

#include "../../printer.h"

namespace sqltoaster {
namespace print {

void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::add_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::add_constraint_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_table_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::alter_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_schema_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::create_view_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::delete_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_column_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_schema_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_table_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::drop_view_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::grant_action_t& action);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::grant_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::insert_select_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::insert_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::joined_table_t& jt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::joined_table_query_expression_t& qe);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_expression_t& qe);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_primary_t& primary);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::non_join_query_term_t& term);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_expression_t& qe);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_specification_non_join_query_primary_t& primary);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::query_specification_t& query);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::select_statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::statement_t& stmt);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::table_expression_t& table_exp);
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::update_statement_t& stmt);

} // namespace sqltoast::print
} // namespace sqltoast

#endif /* SQLTOASTER_PRINT_YAML_PRINTERS_H */
