/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_TABLE_REFERENCE_H
#define SQLTOAST_TABLE_REFERENCE_H

#include <memory>
#include <vector>
#include <ostream>

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef struct table_reference {
    lexeme_t value;
    lexeme_t alias;
    table_reference(lexeme_t& value) :
        value(value)
    {}
    inline bool has_alias() const {
        return alias.start != parse_position_t(0);
    }
} table_reference_t;

std::ostream& operator<< (std::ostream& out, const table_reference_t& tr);

} // namespace sqltoast

#endif /* SQLTOAST_TABLE_REFERENCE_H */
