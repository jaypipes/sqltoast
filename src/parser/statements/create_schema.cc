/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "create_schema.h"
#include "parser/token.h"
#include "parser/lexer.h"

namespace sqltoast {

//
// The CREATE SCHEMA statement follows this form:
//
// CREATE SCHEMA [IF NOT EXISTS] identifier;
//
bool parse_create_schema(parse_context_t& ctx) {
    next_token(ctx);
    if (expect(ctx, TOKEN_TYPE_IDENTIFIER)) {
        next_token(ctx);
        ast_node node(NODE_TYPE_STATEMENT);
        ctx.ast.add_node(node);
        return true;
    }
    return false;
}

} // namespace sqltoast
