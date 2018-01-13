/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_IDENTIFIER_H
#define SQLTOAST_IDENTIFIER_H

#include <string>

#include "parse.h"

namespace sqltoast {

typedef struct db_identifier {
    const std::string name;
    db_identifier(
        parser_input_t::const_iterator start_pos,
        parser_input_t::const_iterator end_pos) : name(start_pos, end_pos)
    {}
} db_identifier_t;

typedef struct table_identifier {
    const db_identifier& db;
    const std::string name;
} table_identifier_t;

typedef struct column_identifier {
    const table_identifier& table;
    const std::string name;
} column_identifier_t;

// Returns true if an identifier (of any kind) can be parsed from the parser
// context's cursor position
bool parse_identifier(parser_context_t& ctx);

// Returns true if a quoted identifier can be parsed from the parser
// context's cursor position.
bool parse_quoted_identifier(parser_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_IDENTIFIER_H */
