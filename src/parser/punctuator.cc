/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/punctuator.h"

namespace sqltoast {

tokenize_result_t token_punctuator(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    const char c = *lex.cursor++;
    for (unsigned int x = 0; x < NUM_PUNCTUATORS; x++) {
        if (c == punctuator_char_map[x]) {
            lex.set_token(
                punctuator_symbol_map[x],
                parse_position_t(lex.cursor),
                parse_position_t(lex.cursor+1)
            );
            return TOKEN_FOUND;
        }
    }
    lex.cursor--; // rewind... we didn't find a punctuator
    return TOKEN_NOT_FOUND;
}

} // namespace sqltoast
