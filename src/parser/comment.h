/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_COMMENT_H
#define SQLTOAST_PARSER_COMMENT_H

#include "context.h"

namespace sqltoast {

tokenize_result_t token_comment(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_COMMENT_H */
