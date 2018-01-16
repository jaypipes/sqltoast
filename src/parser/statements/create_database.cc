/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "create_database.h"
#include "parser/token.h"

namespace sqltoast {

//
// The CREATE DATABASE statement follows this form:
//
// CREATE DATABASE [IF NOT EXISTS] identifier;
//
bool parse_create_database(parse_context_t& ctx) {
    next_symbol(ctx);
    if (expect(ctx, TOKEN_TYPE_IDENTIFIER)) {
        next_symbol(ctx);
        ast_node node(NODE_TYPE_STATEMENT);
        ctx.ast.add_node(node);
        return true;
    }
    return false;
}

} // namespace sqltoast
