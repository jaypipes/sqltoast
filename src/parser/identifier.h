/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_IDENTIFIER_H
#define SQLTOAST_IDENTIFIER_H

#include <string>

#include "context.h"

namespace sqltoast {

typedef struct db_identifier {
    const std::string name;
    db_identifier(
        parse_input_t::const_iterator start_pos,
        parse_input_t::const_iterator end_pos) : name(start_pos, end_pos)
    {}
} db_identifier_t;

typedef struct table_identifier {
    const db_identifier& db;
    const std::string name;
    table_identifier(
        db_identifier& db,
        parse_input_t::const_iterator start_pos,
        parse_input_t::const_iterator end_pos) : db(db), name(start_pos, end_pos)
    {}
} table_identifier_t;

typedef struct column_identifier {
    const table_identifier& table;
    const std::string name;
    column_identifier(
        table_identifier& table,
        parse_input_t::const_iterator start_pos,
        parse_input_t::const_iterator end_pos) : table(table), name(start_pos, end_pos)
    {}
} column_identifier_t;

// Returns true if an identifier (of any kind) can be parsed from the parse
// context's cursor position
bool token_identifier(parse_context_t& ctx);

// Returns true if a delimited identifier can be parsed from the parse
// context's cursor position.
bool token_delimited_identifier(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_IDENTIFIER_H */
