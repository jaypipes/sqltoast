/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "statements/create_table.h"

namespace sqltoast {
namespace statements {

const std::string create_table::to_string() {
    std::stringstream ss;
    ss << "<statement: CREATE TABLE" << std::endl
       << "   table identifier: " << table_identifier;
    if (table_type != TABLE_TYPE_NORMAL) {
        ss << std::endl << "   temporary: true (";
        if (table_type == TABLE_TYPE_TEMPORARY_GLOBAL) {
            ss << "global)";
        } else {
            ss << "local)";
        }
    }
    ss << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast::statements
} // namespace sqltoast
