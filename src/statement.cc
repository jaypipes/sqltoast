/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast.h"
#include "statement.h"

namespace sqltoast {

const std::string create_schema_t::to_string() {
    std::stringstream ss;
    ss << "<statement: CREATE SCHEMA" << std::endl
       << "   schema name: " << schema_name;
    if (authorization_identifier) {
       ss << std::endl
          << "   authorization identifier: " << authorization_identifier;
    }
    if (default_charset) {
       ss << std::endl
          << "   default charset: " << default_charset;
    }
    ss << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast
