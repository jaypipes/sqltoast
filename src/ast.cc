/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <string>

#include "ast.h"

namespace sqltoast {

const std::string ast::to_string() {
    return std::string("<ast>");
}

} // namespace sqltoast
