/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <ostream>

#include "column_definitions/character_string.h"

namespace sqltoast {
namespace column_definitions {

std::ostream& character_string::operator<< (std::ostream& out) {
    out << id.name << " CHAR";
    return out;
}

} // namespace sqltoast::column_definitions
} // namespace sqltoast
