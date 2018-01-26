/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_DEFINITION_H
#define SQLTOAST_COLUMN_DEFINITION_H

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef struct column_definition {
    data_type_t data_type;
    identifier_t id;
    column_definition(data_type_t data_type, identifier_t& id) :
        data_type(data_type),
        id(id)
    {}
    virtual std::ostream& operator<< (std::ostream& out) = 0;
} column_definition_t;

} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_DEFINITION_H */
