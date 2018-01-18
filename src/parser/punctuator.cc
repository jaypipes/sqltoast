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
        if (c == punctuator_map[x]) {
            token_t tok(
                TOKEN_TYPE_PUNCTUATOR,
                parse_position_t(ctx.cursor),
                parse_position_t(ctx.cursor+1)
            );
            ctx.tokens.emplace_back(tok);
            return true;
        }
    }
    ctx.cursor--; // rewind... we didn't find a punctuator
    return false;
}

} // namespace sqltoast
