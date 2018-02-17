/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_PUNCTUATOR_H
#define SQLTOAST_PARSER_PUNCTUATOR_H

#include "parser/lexer.h"
#include "parser/symbol.h"

namespace sqltoast {

const unsigned int NUM_PUNCTUATORS = 12;

static const char punctuator_char_map[12] = {
    ',',
    '=',
    '!',
    '(',
    ')',
    '*',
    ';',
    '<',
    '>',
    ':',
    '?',
    '\0'
};

static const symbol_t punctuator_symbol_map[12] = {
    SYMBOL_COMMA,
    SYMBOL_EQUAL,
    SYMBOL_EXCLAMATION,
    SYMBOL_LPAREN,
    SYMBOL_RPAREN,
    SYMBOL_ASTERISK,
    SYMBOL_SEMICOLON,
    SYMBOL_LESS_THAN,
    SYMBOL_GREATER_THAN,
    SYMBOL_COLON,
    SYMBOL_QUESTION_MARK,
    SYMBOL_EOS
};

// Moves the supplied parse context's cursor to the next punctuator found in the
// context's input stream and sets the context's current symbol to the found
// punctuator symbol. Returns whether a punctuator was found.
tokenize_result_t token_punctuator(
        parse_position_t cursor,
        const parse_position_t end);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PUNCTUATOR_H */
