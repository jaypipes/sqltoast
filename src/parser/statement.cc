/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/parse.h"
#include "parser/statement.h"
#include "parser/statements/create_schema.h"
#include "parser/symbol.h"
#include "parser/token.h"

namespace sqltoast {

static const size_t NUM_CREATE_STATEMENT_PARSERS = 1;
static const parse_func_t create_statement_parsers[1] = {
    &parse_create_schema
};

bool parse_statement(parse_context_t& ctx) {
    // Assumption: the top token in the stack will be of type
    // TOKEN_TYPE_KEYWORD
    token_t& top_tok = ctx.tokens.front();
    symbol_t& top_sym = top_tok.symbol;

    switch (top_sym) {
        case SYMBOL_CREATE:
        {
            size_t x = 0;
            bool parse_res = false;
            while (ctx.result.code == PARSE_OK &&
                    ! parse_res &&
                    x < NUM_CREATE_STATEMENT_PARSERS) {
                parse_res = create_statement_parsers[x](ctx);
                x++;
            }
            return parse_res;
        }
        default:
            return false;
    }
}

} // namespace sqltoast
