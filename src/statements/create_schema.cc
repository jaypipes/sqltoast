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
       << "   schema identifier: " << identifier << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast::statements
} // namespace sqltoast
