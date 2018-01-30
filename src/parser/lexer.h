/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_LEXER_H
#define SQLTOAST_PARSER_LEXER_H

#include <vector>

#include "sqltoast.h"

namespace sqltoast {

// Possible escape mode for literals and identifiers
enum escape_mode {
    ESCAPE_NONE = 0,
    ESCAPE_SINGLE_QUOTE = 1,
    ESCAPE_DOUBLE_QUOTE = 2,
    ESCAPE_TILDE = 3,
    ESCAPE_UNICODE_AMPERSAND = 4
};

typedef std::vector<char>::const_iterator parse_position_t;
typedef std::vector<char>::iterator parse_cursor_t;

typedef struct lexer {
    parse_position_t start_pos;
    parse_position_t end_pos;
    parse_cursor_t cursor;
    escape_mode current_escape;
    lexer(parse_input_t& subject) :
        start_pos(subject.cbegin()),
        end_pos(subject.cend()),
        cursor(subject.begin()),
        current_escape(ESCAPE_NONE)
    {}
    // Simply advances the parse context's cursor over any whitespace
    inline void skip_ws() {
        while (std::isspace(*cursor))
            cursor++;
        return;
    }
    inline bool peek_char(const char c) {
        return ((cursor != end_pos && (*cursor == c)));
    }
} lexer_t;

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LEXER_H */
