/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "derived_column.h"
#include "statements/select.h"

namespace sqltoast {
namespace statements {

const std::string select_t::to_string() {
    std::stringstream ss;
    ss << "<statement: SELECT";
    if (distinct)
       ss << std::endl << "   distinct: true";
    ss << std::endl << "   select list: ";
    size_t x = 0;
    for (const derived_column_t& dc : selected_columns) {
        ss << std::endl << "     " << x++ << ": " << dc;
    }
    ss << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast::statements
} // namespace sqltoast
