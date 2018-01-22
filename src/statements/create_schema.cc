/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "statements/create_schema.h"

namespace sqltoast {
namespace statements {

const std::string create_schema::to_string() {
    std::stringstream ss;
    ss << "<statement: CREATE SCHEMA" << std::endl
       << "   schema identifier: " << schema_identifier;
    if (authorization_identifier.get()) {
       ss << std::endl
          << "   authorization identifier: " << *authorization_identifier;
    }
    if (default_charset.get()) {
       ss << std::endl
          << "   default charset: " << *default_charset;
    }
    ss << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast::statements
} // namespace sqltoast
