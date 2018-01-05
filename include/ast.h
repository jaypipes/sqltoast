/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_AST_H
#define SQLTOAST_AST_H

#include "ast_node.h"

namespace sqltoast {

typedef struct ast {
    ast_node root;
    ast(ast_node& root) : root(root)
    {}
    const std::string to_string();
} ast_t;

} // namespace sqltoast

#endif /* SQLTOAST_AST_H */
