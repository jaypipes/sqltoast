/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_IDENTIFIER_H
#define SQLTOAST_IDENTIFIER_H

#include <ostream>
#include <string>

#include "sqltoast.h"

namespace sqltoast {

typedef struct schema_identifier {
    const std::string name;
    schema_identifier(
        parse_input_t::const_iterator start_pos,
        parse_input_t::const_iterator end_pos) : name(start_pos, end_pos)
    {}
} schema_identifier_t;

std::ostream& operator<< (std::ostream& out, const schema_identifier_t& id);

} // namespace sqltoast

#endif /* SQLTOAST_IDENTIFIER_H */
