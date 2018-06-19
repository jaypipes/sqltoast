/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_PARSE_H
#define SQLTOAST_PARSER_PARSE_H

#include "sqltoast/sqltoast.h"

#include "parser/context.h"
#include "parser/token.h"

namespace sqltoast {

typedef bool (*parse_func_t) (
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Top-level statement parser that is called from within the primary parse()
// loop over found tokens.
void parse_statement(parse_context_t& ctx);

/// Returns true if the COMMIT statement was parsed successfully from
// the parse context
bool parse_commit(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

/// Returns true if the ROLLBACK statement was parsed successfully from
// the parse context
bool parse_rollback(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the CREATE VIEW statement was parsed successfully from
// the parse context
bool parse_create_view(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the CREATE SCHEMA statement was parsed successfully from
// the parse context
bool parse_create_schema(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the CREATE TABLE statement was parsed successfully from
// the parse context
bool parse_create_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the DROP SCHEMA statement was parsed successfully from
// the parse context
bool parse_drop_schema(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the DROP TABLE statement was parsed successfully from
// the parse context
bool parse_drop_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the DROP VIEW statement was parsed successfully from
// the parse context
bool parse_drop_view(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the ALTER TABLE statement was parsed successfully from
// the parse context
bool parse_alter_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if a SELECT statement was parsed successfully from
// the parse context. If true, then the out parameter will be populated with an
// allocated select_t statement struct
bool parse_select(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if an INSERT statement was parsed successfully from the parse
// context. If true, then the out parameter will be populated with an allocated
// insert_t or insert_select_t statement struct
bool parse_insert(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if a DELETE statement was parsed successfully from the parse
// context. If true, then the out parameter will be populated with an allocated
// delete_statement_t struct
bool parse_delete(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if an UPDATE statement was parsed successfully from the parse
// context. If true, then the out parameter will be populated with an allocated
// update_statement_t struct
bool parse_update(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if the GRANT statement was parsed successfully from
// the parse context
bool parse_grant(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out);

// Returns true if a query expression can be parsed. If true, the out argument
// will contain a new pointer to a query_expression_t.
bool parse_query_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_expression_t>& out);
bool parse_non_join_query_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_expression_t>& out);
bool parse_non_join_query_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<non_join_query_term_t>& out);
bool parse_non_join_query_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<non_join_query_primary_t>& out);

// Returns true if a query specification can be parsed. If true, the out
// argument will contain a new pointer to a query_specification_t.
bool parse_query_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_specification_t>& out);
bool parse_table_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_expression_t>& out);
bool parse_table_value_constructor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_value_constructor_t>& out);

// Returns true if a table reference can be parsed. If true, the out argument
// will contain a new pointer to a table_reference_t.
bool parse_table_reference(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_reference_t>& out);
bool parse_derived_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_reference_t>& out);
bool parse_joined_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_reference_t>& out);

// Returns true if a search condition could be parsed. If true, the out
// argument will have a new pointer to a search_condition_t added to it.
bool parse_search_condition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<search_condition_t>& out);

bool parse_boolean_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_term_t>& out);

bool parse_boolean_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<boolean_factor_t>& out);

// Returns true if a oredicate could be parsed. If true, the out
// argument will have a new pointer to a boolean_factor_t subclass that
// represents a prdicate added to it.
bool parse_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out);
bool parse_exists_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out);
bool parse_unique_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out);
bool parse_like_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left_p,
        bool reverse_op);
bool parse_match_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left_p);
bool parse_overlaps_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left);

// Returns true if a comp_predicate_t could be parsed. If true, the out
// argument will be a new pointer to a comp_predicate_t
bool parse_comparison_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left_p);

// Returns true if a between_predicate_t could be parsed. If true, the out
// argument will be a new pointer to a between_predicate_t
bool parse_between_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left_p,
        bool reverse_op);

// Returns true if a null_predicate_t could be parsed. If true, the out
// argument will be a new pointer to a null_predicate_t
bool parse_null_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left_p,
        bool reverse_op);

// Returns true if an IN predicate (either a subquery or values predicate)
// could be parsed. If true, the out argument will be a new pointer to
// either an in_values_predicate_t or an in_subquery_predicate_t
bool parse_in_predicate(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<predicate_t>& out,
        std::unique_ptr<row_value_constructor_t>& left_p,
        bool reverse_op);

// Returns true if a row value constructor could be parsed. If true, the out
// argument will be filled appropriately.
bool parse_row_value_constructor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor>& out);
bool parse_row_value_constructor_element(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<row_value_constructor>& out);

// Returns true if a value_expression_t could be parsed. If true, the out
// argument will be filled appropriately.
bool parse_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out);

// Returns true if a value_expression_t of one of the unsigned value
// specifications could be parsed. If true, the out argument will be filled
// appropriately.
bool parse_numeric_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out);
bool parse_numeric_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_term_t>& out);
bool parse_numeric_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_factor_t>& out);
bool parse_numeric_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<numeric_primary_t>& out);

bool parse_value_expression_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out);
bool parse_unsigned_value_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out);
bool parse_set_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out);
bool parse_case_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out);
bool parse_simple_case_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out);
bool parse_searched_case_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_primary_t>& out);

bool parse_string_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out);
bool parse_character_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out);
bool parse_character_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<character_factor_t>& out);
bool parse_character_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<character_primary_t>& out);
bool parse_string_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out);
bool parse_substring_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out);
bool parse_convert_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out);
bool parse_translate_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out);
bool parse_trim_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<string_function_t>& out);

bool parse_datetime_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out);
bool parse_datetime_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_term_t>& out);
bool parse_datetime_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_factor_t>& out);
bool parse_datetime_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_primary_t>& out);
bool parse_datetime_function(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<datetime_primary_t>& out);

bool parse_interval_value_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<value_expression_t>& out);
bool parse_interval_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_term_t>& out);
bool parse_interval_factor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_factor_t>& out);
bool parse_interval_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_primary_t>& out);
bool parse_interval_qualifier(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<interval_qualifier_t>& out);

// Returns true if a table constraint can be parsed from the supplied token
// iterator. If the function returns true, constraints will have a new member
// (if ctx.options.disable_statement_construction is false
bool parse_constraint(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out);

// Returns true if a foreign key constraint can be parsed from the supplied
// token iterator. If the function returns true, constraint_p will be filled
// appropriately with attributes of the referential constraint
bool parse_foreign_key_constraint(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out);

// Returns true if a column definition clause can be parsed from the supplied
// token iterator. If the function returns true, column_defs will have a new
// member (if ctx.options.disable_statement_construction is false
bool parse_column_definition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<column_definition_t>& out);

// Returns true if a default clause can be parsed from the supplied
// token iterator. If the function returns true, the out argument will contain
// a new pointer to a default_descriptor_t.
bool parse_default_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<default_descriptor_t>& out);

// Returns true if a column constraint can be parsed from the supplied token
// iterator. If the function returns true, the out argument will contain a new
// pointer to a constraint_t.
bool parse_column_constraint(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out);

// Returns true if a references specification can be parsed from the supplied
// token iterator. If the function returns true, the out argument will contain
// a new pointer to a constraint_t.
bool parse_references_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out);

// Returns true if one or more identifiers, delimited by commas, can be parsed
// from the supplied token iterator. If the function returns true, identifiers
// will have been populated with lexeme_t structs.
bool parse_identifier_list(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::vector<lexeme_t>& identifiers);

// Returns true if a match type can be parsed from the
// supplied token iterator. If the function returns true, the match_type
// argument will be set to the found match type.
bool parse_match_type(
        parse_context_t& ctx,
        token_t& cur_tok,
        match_type* match_type);

// Returns true if a referential action can be parsed from the
// supplied token iterator. If the function returns true, the action
// argument will be set to the found referential action.
bool parse_referential_action(
        parse_context_t& ctx,
        token_t& cur_tok,
        referential_action_t* action);

// Returns true if a collate clause can be parsed from the supplied
// token iterator. If the function returns true, column_def will have a its
// collate member populated.
bool parse_collate_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def);

// Returns true if a data type descriptor clause can be parsed from the supplied
// token iterator. If the function returns true, the out argument will
// be a pointer to a filled-in data_type_descriptor_t struct.
bool parse_data_type_descriptor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

// Returns true if a character string descriptor clause can be parsed from the
// supplied token iterator. If the function returns true, the out argument will
// be a pointer to a filled-in data_type_descriptor_t struct.
bool parse_character_string(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);
bool parse_national_character_string(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

// Returns true if a bit descriptor clause can be parsed from the supplied
// token iterator. If the function returns true, the out argument will be a
// pointer to a filled-in data_type_descriptor_t struct.
bool parse_bit_string(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

// Returns true if a length specifier clause can be parsed from the
// supplied token iterator. If the function returns true, the len
// argument will be set to the parsed length.
bool parse_length_specifier(
        parse_context_t& ctx,
        token_t& cur_tok,
        size_t* len);

// Returns true if an exact numeric descriptor clause can be parsed from the
// supplied token iterator. If the function returns true, the out argument will
// be a pointer to a filled-in data_type_descriptor_t struct
bool parse_exact_numeric(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

// Returns true if a precision/scale specifier clause can be parsed from the
// supplied token iterator. If the function returns true, the scale and precision
// arguments will be set to the parsed length.
bool parse_precision_scale(
        parse_context_t& ctx,
        token_t& cur_tok,
        size_t* prec,
        size_t* scale);

// Returns true if an approximate numeric descriptor clause can be parsed from
// the supplied token iterator. If the function returns true, the out argument
// will be a pointer to a filled-in data_type_descriptor_t struct
bool parse_approximate_numeric(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

// Returns true if a datetime descriptor clause can be parsed from the supplied
// token iterator. If the function returns true, the out argument will be a
// pointer to a filled-in data_type_descriptor_t struct
bool parse_datetime(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

// Returns true if an interval descriptor clause can be parsed from the supplied
// token iterator. If the function returns true, the out argument will be a
// pointer to a filled-in data_type_descriptor_t struct
bool parse_interval(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<data_type_descriptor_t>& out);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PARSE_H */
