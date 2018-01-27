/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_STATEMENTS_CREATE_TABLE_H
#define SQLTOAST_PARSER_STATEMENTS_CREATE_TABLE_H

#include "parser/context.h"

namespace sqltoast {

// Returns true if the CREATE TABLE statement was parsed successfully from
// the parse context
bool parse_create_table(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_STATEMENTS_CREATE_TABLE_H */
