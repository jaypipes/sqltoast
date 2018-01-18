/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "statement.h"
#include "statements/create_schema.h"
#include "token.h"
#include "symbol.h"

namespace sqltoast {

void parse_statement(parse_context_t& ctx) {
    token_t& top_tok = ctx.tokens.front();
    symbol_t& top_sym = top_tok.symbol;

    switch (top_sym) {
        case SYMBOL_CREATE:
            if (parse_create_schema(ctx)) {
                ctx.result.code = PARSE_SUCCESS;
            }
            break;
        default:
            break;
    }
}

} // namespace sqltoast
