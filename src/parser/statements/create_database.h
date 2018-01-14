/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_CREATE_DATABASE_H
#define SQLTOAST_CREATE_DATABASE_H

#include <string>

#include "parser/context.h"
#include "parser/identifier.h"

namespace sqltoast {

// Returns true if the CREATE DATABASE statement was parsed successfully from
// the parse context
bool parse_create_database(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_CREATE_DATABASE_H */
