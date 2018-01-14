/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <sstream>
#include <string>

#include "ast.h"

namespace sqltoast {

const std::string ast::to_string() {
    std::stringstream ss;
    while (! nodes.empty()) {
        ast_node& node = nodes.top();
        ss << node.to_string();
        nodes.pop();
        if (! nodes.empty()) {
            ss << " ==> ";
        }
    }
    return ss.str();
}

const std::string ast_node::to_string() {
    switch (type) {
        case NODE_TYPE_STATEMENT:
            return std::string("<statement node>");
        case NODE_TYPE_IDENTIFIER:
            return std::string("<identifier node>");
        default:
            return std::string("<unknown node>");
    }
}

} // namespace sqltoast
