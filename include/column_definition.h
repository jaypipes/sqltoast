/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_DEFINITION_H
#define SQLTOAST_COLUMN_DEFINITION_H

#include <ostream>

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef struct column_definition {
    std::unique_ptr<data_type_descriptor_t> data_type;
    identifier_t id;
    column_definition(std::unique_ptr<data_type_descriptor_t>& data_type, identifier_t& id) :
        data_type(std::move(data_type)),
        id(id)
    {}
} column_definition_t;

std::ostream& operator<< (std::ostream& out, const column_definition_t& column_def);

} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_DEFINITION_H */
