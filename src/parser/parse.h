/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_PARSE_H
#define SQLTOAST_PARSER_PARSE_H

#include "context.h"

namespace sqltoast {

typedef bool (*parse_func_t) (parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PARSE_H */
