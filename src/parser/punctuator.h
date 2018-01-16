/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_PUNCTUATOR_H
#define SQLTOAST_PARSER_PUNCTUATOR_H

#include "context.h"

namespace sqltoast {

typedef enum punctuator {
    PUNCTUATOR_SEMICOLON,
    PUNCTUATOR_COMMA,
    PUNCTUATOR_LPAREN,
    PUNCTUATOR_RPAREN,
} punctuator_t;

const unsigned int NUM_PUNCTUATORS = 4;

static const char punctuator_map[4] = {
    ';', // PUNCTUATOR_SEMICOLON
    ',', // PUNCTUATOR_COMMA
    '(', // PUNCTUATOR_LPAREN
    ')', // PUNCTUATOR_RPAREN
};

// Moves the supplied parse context's cursor to the next punctuator found in the
// context's input stream and sets the context's current symbol to the found
// punctuator symbol. Returns whether a punctuator was found.
bool parse_punctuator(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PUNCTUATOR_H */
