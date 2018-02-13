/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_LITERAL_H
#define SQLTOAST_PARSER_LITERAL_H

#include "parser/lexer.h"

namespace sqltoast {

// Moves the supplied parse context's cursor to the next literal found in the
// context's input stream and adds a token with type literal to the tokens
// stack.Returns whether a literal was found.
tokenize_result_t token_literal(
        parse_position_t cursor,
        const parse_position_t end);

tokenize_result_t token_numeric_literal(
        parse_position_t cursor,
        const parse_position_t end,
        bool found_sign);

tokenize_result_t token_character_string_literal(
        parse_position_t cursor,
        const parse_position_t end,
        symbol_t literal_sym);

tokenize_result_t token_bit_string_literal(
        parse_position_t cursor,
        const parse_position_t end);

tokenize_result_t token_hex_string_literal(
        parse_position_t cursor,
        const parse_position_t end);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LITERAL_H */
