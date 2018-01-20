/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_AST_H
#define SQLTOAST_AST_H

#include <stack>

namespace sqltoast {

enum ast_node_type {
    NODE_TYPE_LITERAL,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STATEMENT,
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_FUNCTION,
    NODE_TYPE_UNKNOWN
};

typedef struct ast_node {
    const ast_node_type type;
    ast_node(ast_node_type type) : type(type)
    {}
    ast_node operator=(const ast_node& other) {
        return ast_node(other.type);
    }
    const std::string to_string();
} ast_node_t;

typedef struct ast {
    std::stack<ast_node_t> nodes;
    ast()
    {}
    void add_node(ast_node_t& node) {
        nodes.push(node);
    }
    const std::string to_string();
} ast_t;

} // namespace sqltoast

#endif /* SQLTOAST_AST_H */
