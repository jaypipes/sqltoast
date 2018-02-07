/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_DEFINITION_H
#define SQLTOAST_COLUMN_DEFINITION_H

#include <memory>
#include <ostream>

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef enum default_type {
    DEFAULT_TYPE_LITERAL,
    DEFAULT_TYPE_USER,
    DEFAULT_TYPE_CURRENT_USER,
    DEFAULT_TYPE_SESSION_USER,
    DEFAULT_TYPE_SYSTEM_USER,
    DEFAULT_TYPE_CURRENT_DATE,
    DEFAULT_TYPE_CURRENT_TIME,
    DEFAULT_TYPE_CURRENT_TIMESTAMP,
    DEFAULT_TYPE_NULL
} default_type_t;

typedef struct default_descriptor {
    default_type_t type;
    // TODO(jaypipes): Maybe make this a std::unique_ptr<> to some concrete
    // struct representing a literal OR a datetime function type?
    lexeme_t lexeme;
    size_t precision;
    default_descriptor(default_type_t type, lexeme_t lexeme, size_t prec) :
        type(type),
        lexeme(lexeme),
        precision(prec)
    {}
} default_descriptor_t;

std::ostream& operator<< (std::ostream& out, const default_descriptor_t& column_def);

typedef struct column_definition {
    identifier_t id;
    std::unique_ptr<data_type_descriptor_t> data_type;
    std::unique_ptr<default_descriptor_t> default_descriptor;
    column_definition(identifier_t& id) :
        id(id)
    {}
} column_definition_t;

std::ostream& operator<< (std::ostream& out, const column_definition_t& column_def);

} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_DEFINITION_H */
