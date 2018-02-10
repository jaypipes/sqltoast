/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_DERIVED_COLUMN_H
#define SQLTOAST_DERIVED_COLUMN_H

#include <memory>
#include <vector>
#include <ostream>

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef struct derived_column {
    lexeme_t value;
    lexeme_t alias;
    derived_column(lexeme_t& value) :
        value(value)
    {}
    inline bool has_alias() const {
        return alias.start != parse_position_t(0);
    }
} derived_column_t;

std::ostream& operator<< (std::ostream& out, const derived_column_t& column_def);

} // namespace sqltoast

#endif /* SQLTOAST_DERIVED_COLUMN_H */
