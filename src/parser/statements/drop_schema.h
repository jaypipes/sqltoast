/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_STATEMENTS_DROP_SCHEMA_H
#define SQLTOAST_PARSER_STATEMENTS_DROP_SCHEMA_H

#include "parser/context.h"

namespace sqltoast {

// Returns true if the DROP SCHEMA statement was parsed successfully from
// the parse context
bool parse_drop_schema(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_STATEMENTS_DROP_SCHEMA_H */
