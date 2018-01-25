/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "statements/drop_schema.h"

namespace sqltoast {
namespace statements {

const std::string drop_schema::to_string() {
    std::stringstream ss;
    ss << "<statement: DROP SCHEMA" << std::endl
       << "   schema identifier: " << schema_identifier << std::endl;
    if (drop_behaviour == DROP_BEHAVIOUR_CASCADE) {
       ss << "   behaviour: CASCADE";
    } else {
       ss << "   behaviour: RESTRICT";
    }
    ss << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast::statements
} // namespace sqltoast
