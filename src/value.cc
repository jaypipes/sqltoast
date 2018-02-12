/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "value.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc) {
    out << std::string(rvc.lexeme.start, rvc.lexeme.end);
    return out;
}

} // namespace sqltoast
