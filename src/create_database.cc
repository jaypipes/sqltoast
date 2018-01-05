/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <memory>

#include "ast.h"
#include "ast_node.h"
#include "create_database.h"
#include "identifier.h"
#include "parse.h"

namespace sqltoast {

//
// The CREATE DATABASE statement follows this form:
//
// CREATE DATABASE [IF NOT EXISTS] identifier;
//
bool parse_create_database(parser_context_t& ctx) {
    next_symbol(ctx);
    if (expect(ctx, IDENTIFIER)) {
        next_symbol(ctx);
        statement_node st_node(STMT_TYPE_CREATE_DATABASE);
        if (! ctx.ast) {
            ctx.ast = std::make_unique<ast_t>(ast(st_node));
        }
        return true;
    }
    return false;
}

} // namespace sqltoast
