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
    inline bool peek_char(const char c) {
        return ((cursor != end_pos && (*cursor == c)));
    }

    // Attempts to find the next token. If a token was found, returns a pointer
    // to that token, else NULL.
    token_t* next_token();
} lexer_t;

typedef enum tokenize_result_code {
    TOKEN_FOUND,
    TOKEN_NOT_FOUND,
    TOKEN_ERR_NO_CLOSING_DELIMITER
} tokenize_result_code_t;

typedef struct tokenize_result {
    tokenize_result_code_t code;
    token_t token;
    tokenize_result(tokenize_result_code_t code) :
        code(code), token()
    {}
    tokenize_result(symbol_t sym, parse_position_t start, parse_position_t end) :
        code(TOKEN_FOUND),
        token(sym, start, end)
    {}
} tokenize_result_t;

typedef tokenize_result_t (*tokenize_func_t) (lexer_t& lex);

void fill_lexeme(token_t* tok, lexeme_t& lexeme);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LEXER_H */
