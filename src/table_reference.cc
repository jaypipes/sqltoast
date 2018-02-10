/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "table_reference.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const table_reference_t& tr) {
    out << std::string(tr.value.start, tr.value.end);
    if (tr.has_alias()) {
        out << " AS " << std::string(tr.alias.start, tr.alias.end);
    }
    return out;
}

} // namespace sqltoast
