/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_CREATE_DATABASE_H
#define SQLTOAST_CREATE_DATABASE_H

#include <string>

#include "internal/identifier.h"
#include "parse.h"

namespace sqltoast {

typedef struct create_database_statement {
    const db_identifier_t db_id;
    create_database_statement(
        parse_input_t::const_iterator start_pos,
        parse_input_t::const_iterator end_pos) : db_id(start_pos, end_pos)
    {}
} create_database_statement_t;

// Returns true if the CREATE DATABASE statement was parsed successfully from
// the parse context
bool parse_create_database(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_CREATE_DATABASE_H */
