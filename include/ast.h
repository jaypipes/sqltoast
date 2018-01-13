/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_AST_H
#define SQLTOAST_AST_H

namespace sqltoast {

enum ast_node_type {
    NODE_TYPE_LITERAL,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STATEMENT,
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_FUNCTION
};

enum statement_type {
    STMT_TYPE_CREATE_DATABASE
};

struct ast_node {
    ast_node_type type;
    ast_node(ast_node_type type) : type(type)
    {}
    const std::string to_string();
};

struct statement_node : ast_node {
    const statement_type stype;
    statement_node(statement_type stype) : ast_node(NODE_TYPE_STATEMENT), stype(stype)
    {}
    const std::string to_string();
};

typedef struct ast {
    ast_node root;
    ast(ast_node& root) : root(root)
    {}
    const std::string to_string();
} ast_t;

} // namespace sqltoast

#endif /* SQLTOAST_AST_H */
