/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/punctuator.h"

namespace sqltoast {

tokenize_result_t token_punctuator(parse_position_t cursor) {
    const char c = *cursor;
    for (unsigned int x = 0; x < NUM_PUNCTUATORS; x++) {
        if (c == punctuator_char_map[x]) {
            return tokenize_result_t(
                punctuator_symbol_map[x],
                parse_position_t(cursor),
                parse_position_t(cursor)
            );
        }
    }
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

} // namespace sqltoast
