/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_ERROR_H
#define SQLTOAST_ERROR_H

#include <initializer_list>
#include <sstream>

#include "context.h"

namespace sqltoast {

// Populates the parse context's error member with a crafted syntax error that
// shows where the syntax error occurred
void create_syntax_error_marker(parse_context_t& ctx, std::stringstream& es);

// Helper function to generate a syntax error about expecting to find a
// particular symbol
void expect_error(parse_context_t& ctx, symbol_t expected);
void expect_any_error(parse_context_t& ctx, std::initializer_list<symbol_t> expected);
} // namespace sqltoast

#endif /* SQLTOAST_ERROR_H */
