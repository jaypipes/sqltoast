/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_PARSE_H
#define SQLTOAST_PARSER_PARSE_H

#include <memory>
#include <vector>

#include "column_definition.h"
#include "context.h"
#include "token.h"

namespace sqltoast {

typedef bool (*parse_func_t) (parse_context_t& ctx);

// Returns true if a column definition clause can be parsed from the supplied
// token iterator. If the function returns true, column_defs will have a new
// member (if ctx.options.disable_statement_construction is false
bool parse_column_definition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::vector<std::unique_ptr<column_definition_t>>& column_defs);

// Returns true if a data type descriptor clause can be parsed from the supplied
// token iterator. If the function returns true, the column_def argument will
// have its data_type attribute set to an allocated data type descriptor
bool parse_data_type_descriptor(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def);

// Returns true if a character string descriptor clause can be parsed from the
// supplied token iterator. If the function returns true, the column_def
// argument will have its data_type attribute set to an allocated char_string_t
// or varchar_string_t descriptor
bool parse_character_string(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PARSE_H */
