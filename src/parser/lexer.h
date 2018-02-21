/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_LEXER_H
#define SQLTOAST_PARSER_LEXER_H

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
    parse_position_t start;
    parse_position_t end;
    parse_position_t cursor;
    token_t current_token;
    lexer(parse_input_t& subject) :
        start(subject.cbegin()),
        end(subject.cend()),
        cursor(subject.cbegin()),
        current_token(SYMBOL_SOS, subject.cbegin(), subject.cbegin())
    {}
    // Returns the next symbol after the lexer's current cursor.
    symbol_t peek() const;
    // Populates a supplied symbol pointer with the value of the symbol found
    // after the supplied cursor. If not symbol was found, the symbol pointer
    // will contain SYMBOL_EOS. Returns the cursor's position after having
    // found the next symbol.
    parse_position_t peek_from(parse_position_t cur, symbol_t* sym) const;

    // Attempts to find the next token. If a token was found, returns a pointer
    // to that token, else NULL.
    token_t& next();
} lexer_t;

typedef enum tokenize_result_code {
    TOKEN_NOT_FOUND,
    TOKEN_FOUND,
    TOKEN_ERR_NO_CLOSING_DELIMITER
} tokenize_result_code_t;

typedef struct tokenize_result {
    tokenize_result_code_t code;
    token_t token;
    tokenize_result(tokenize_result_code_t code) :
        code(code), token()
    {}
    tokenize_result(
            tokenize_result_code_t errcode,
            parse_position_t start,
            parse_position_t end) :
        code(errcode),
        token(SYMBOL_ERROR, start, end)
    {}
    tokenize_result(
            symbol_t sym,
            parse_position_t start,
            parse_position_t end) :
        code(TOKEN_FOUND),
        token(sym, start, end)
    {}
} tokenize_result_t;

typedef tokenize_result_t (*tokenize_func_t) (
        parse_position_t cursor,
        const parse_position_t end);

// Advances the supplied cursor past any whitespace and simple SQL comments and
// returns the location of the cursor after skipping
parse_position_t skip(parse_position_t cur);
// Advances the supplied cursor past any simple SQL comments and returns the
// location of the cursor after skipping
parse_position_t skip_simple_comments(parse_position_t cursor);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LEXER_H */
