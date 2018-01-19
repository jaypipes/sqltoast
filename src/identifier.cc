/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <ostream>

#include "identifier.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const schema_identifier_t& id) {
    out << id.name;
    return out;
}

} // namespace sqltoast
