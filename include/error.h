/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_ERROR_H
#define SQLTOAST_ERROR_H

#include <sstream>
#include <string>

#include "parse.h"

namespace sqltoast {

void create_syntax_error_marker(parser_context_t& ctx, std::stringstream& es, parser_position_t err_pos);

} // namespace sqltoast

#endif /* SQLTOAST_ERROR_H */
