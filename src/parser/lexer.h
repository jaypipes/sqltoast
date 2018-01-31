/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_LEXER_H
#define SQLTOAST_PARSER_LEXER_H

#include <iostream>
#include <cctype>

#include "sqltoast.h"
#include "parser/token.h"

namespace sqltoast {

// Possible escape mode for literals and identifiers
enum escape_mode {
    ESCAPE_NONE = 0,
    ESCAPE_SINGLE_QUOTE = 1,
    ESCAPE_DOUBLE_QUOTE = 2,
    ESCAPE_TILDE = 3,
    ESCAPE_UNICODE_AMPERSAND = 4
};

typedef struct lexer {
    parse_position_t start_pos;
    parse_position_t end_pos;
    parse_cursor_t cursor;
    error_t error;
    token_t current_token;
    lexer(parse_input_t& subject) :
        start_pos(subject.cbegin()),
        end_pos(subject.cend()),
        cursor(subject.begin()),
        error(ERR_NONE),
        current_token(SYMBOL_SOS, subject.cbegin(), subject.cbegin())
    {}
    void skip_simple_comments();
    // Simply advances the lexer's cursor over any whitespace or simple
    // comments
    inline void skip() {
        while (std::isspace(*cursor))
            cursor++;
        skip_simple_comments();
        return;
    }
    inline bool peek_char(const char c) {
        return ((cursor != end_pos && (*cursor == c)));
    }
    inline void set_token(symbol_t sym, parse_position_t start, parse_position_t end) {
        current_token.symbol = sym;
        current_token.lexeme.start = start;
        current_token.lexeme.end = end;
#ifdef SQLTOAST_DEBUG
        std::cout << current_token << std::endl;
#endif
        return;
    }
} lexer_t;

void fill_lexeme(token_t* tok, lexeme_t& lexeme);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LEXER_H */
