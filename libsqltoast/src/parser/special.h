/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_SPECIAL_H
#define SQLTOAST_PARSER_SPECIAL_H

#include "parser/lexer.h"
#include "parser/symbol.h"

namespace sqltoast {

// Moves the supplied parse context's cursor to the next special found in the
// context's input stream and sets the context's current symbol to the found
// special symbol. Returns whether a special symbol was found.
inline tokenize_result_t token_special(
        parse_position_t cursor,
        const parse_position_t end) {
    const char c = *cursor;
    switch (c) {
        case '\0':
            return tokenize_result_t(SYMBOL_EOS, cursor, cursor + 1);
        case ',':
            return tokenize_result_t(SYMBOL_COMMA, cursor, cursor + 1);
        case '=':
            return tokenize_result_t(SYMBOL_EQUAL, cursor, cursor + 1);
        case '(':
            return tokenize_result_t(SYMBOL_LPAREN, cursor, cursor + 1);
        case ')':
            return tokenize_result_t(SYMBOL_RPAREN, cursor, cursor + 1);
        case '*':
            return tokenize_result_t(SYMBOL_ASTERISK, cursor, cursor + 1);
        case '<':
            if ((cursor + 1) != end && *(cursor + 1) == '>')
                return tokenize_result_t(SYMBOL_NOT_EQUAL, cursor, cursor + 2);
            return tokenize_result_t(SYMBOL_LESS_THAN, cursor, cursor + 1);
        case '>':
            return tokenize_result_t(SYMBOL_GREATER_THAN, cursor, cursor + 1);
        case '!':
            return tokenize_result_t(SYMBOL_EXCLAMATION, cursor, cursor + 1);
        case '+':
            return tokenize_result_t(SYMBOL_PLUS, cursor, cursor + 1);
        case '-':
            return tokenize_result_t(SYMBOL_MINUS, cursor, cursor + 1);
        case '/':
            return tokenize_result_t(SYMBOL_SOLIDUS, cursor, cursor + 1);
        case '|':
            if ((cursor + 1) != end && *(cursor + 1) == '|')
                return tokenize_result_t(SYMBOL_CONCATENATION, cursor, cursor + 2);
            return tokenize_result_t(SYMBOL_VERTICAL_BAR, cursor, cursor + 1);
        case ';':
            return tokenize_result_t(SYMBOL_SEMICOLON, cursor, cursor + 1);
        case '?':
            return tokenize_result_t(SYMBOL_QUESTION_MARK, cursor, cursor + 1);
        case ':':
            return tokenize_result_t(SYMBOL_COLON, cursor, cursor + 1);
    }
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SPECIAL_H */
