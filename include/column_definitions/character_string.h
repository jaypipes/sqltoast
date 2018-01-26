/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_DEFINITIONS_CHARACTER_STRING_H
#define SQLTOAST_COLUMN_DEFINITIONS_CHARACTER_STRING_H

#include <string>

#include "identifier.h"
#include "column_definition.h"

namespace sqltoast {
namespace column_definitions {

typedef struct character_string : column_definition_t {
    character_string(identifier_t& id) :
        column_definition_t(DATA_TYPE_CHAR, id)
    {}
    virtual std::ostream& operator<< (std::ostream& out);
} create_schema_t;

} // namespace sqltoast::column_definitions
} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_DEFINITIONS_CHARACTER_STRING_H */
