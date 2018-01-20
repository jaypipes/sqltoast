/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_STATEMENT_H
#define SQLTOAST_PARSER_STATEMENT_H

#include "context.h"

namespace sqltoast {

bool parse_statement(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_STATEMENT_H */
