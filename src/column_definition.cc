/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "column_definition.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const column_definition_t& column_def) {
    out << column_def.id.name;
    return out;
}

} // namespace sqltoast
