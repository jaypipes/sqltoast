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
// token iterator. If the function returns true, then tok_it will be pointing
// at the next token to be parsed and column_defs will have a new member (if
// ctx.options.disable_statement_construction is false
bool parse_column_definition(
        parse_context_t& ctx,
        tokens_t::iterator tok_it,
        std::vector<std::unique_ptr<column_definition_t>>& column_defs);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PARSE_H */
