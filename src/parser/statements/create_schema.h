/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_STATEMENTS_CREATE_SCHEMA_H
#define SQLTOAST_PARSER_STATEMENTS_CREATE_SCHEMA_H

#include <string>

#include "parser/context.h"

namespace sqltoast {

// Returns true if the CREATE SCHEMA statement was parsed successfully from
// the parse context
bool parse_create_schema(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_STATEMENTS_CREATE_SCHEMA_H */
