/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "punctuator.h"
#include "token.h"

namespace sqltoast {

bool token_punctuator(parse_context_t& ctx) {
    const char c = *ctx.cursor++;
    for (unsigned int x = 0; x < NUM_PUNCTUATORS; x++) {
        if (c == punctuator_char_map[x]) {
            token_t tok(
                punctuator_symbol_map[x],
                parse_position_t(ctx.cursor),
                parse_position_t(ctx.cursor+1)
            );
            ctx.push_token(tok);
            return true;
        }
    }
    ctx.cursor--; // rewind... we didn't find a punctuator
    return false;
}

} // namespace sqltoast
