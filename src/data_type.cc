/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <string>

#include "data_type.h"

namespace sqltoast {

const std::string character_string_t::to_string() {
    return std::string("CHAR");
}

} // namespace sqltoast
