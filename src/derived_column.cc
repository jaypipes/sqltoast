/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const derived_column_t& dc) {
    out << std::string(dc.value.start, dc.value.end);
    if (dc.has_alias()) {
        out << " AS " << std::string(dc.alias.start, dc.alias.end);
    }
    return out;
}

} // namespace sqltoast
