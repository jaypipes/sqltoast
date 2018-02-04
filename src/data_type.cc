/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>
#include <string>

#include "data_type.h"

namespace sqltoast {

const std::string char_string_t::to_string() {
    std::stringstream ss;
    if (type == DATA_TYPE_VARCHAR)
        ss << "VAR";
    ss << "CHAR";
    if (size > 0)
        ss << "(" << size << ")";
    if (charset.get())
        ss << " CHARACTER SET " << *charset;
    return ss.str();
}

} // namespace sqltoast
