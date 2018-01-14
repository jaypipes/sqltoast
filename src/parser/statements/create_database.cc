/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "internal/identifier.h"
#include "ast.h"
#include "parse.h"
#include "create_database.h"

namespace sqltoast {

//
// The CREATE DATABASE statement follows this form:
//
// CREATE DATABASE [IF NOT EXISTS] identifier;
//
bool parse_create_database(parse_context_t& ctx) {
    next_symbol(ctx);
    if (expect(ctx, IDENTIFIER)) {
        next_symbol(ctx);
        ast_node node(STMT_TYPE_CREATE_DATABASE);
        ctx.ast.add_node(node);
        return true;
    }
    return false;
}

} // namespace sqltoast
