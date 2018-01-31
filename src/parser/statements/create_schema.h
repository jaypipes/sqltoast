/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_STATEMENTS_CREATE_SCHEMA_H
#define SQLTOAST_PARSER_STATEMENTS_CREATE_SCHEMA_H

#include "parser/context.h"

namespace sqltoast {

// Returns true if the DEFAULT CHARACTER SET <charset identifier> clause was
// successfully parsed. SYMBOL_DELETE symbol is assumed to have already been
// matched and the supplied token iterator is pointing at the token
// representing the DEFAULT keyword. If true is returned, cur_tok will be
// pointing to the token representing the charset identifier
bool require_default_charset_clause(parse_context_t& ctx);

// Returns true if the CREATE SCHEMA statement was parsed successfully from
// the parse context
bool parse_create_schema(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_STATEMENTS_CREATE_SCHEMA_H */
