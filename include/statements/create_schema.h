/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_CREATE_SCHEMA_H
#define SQLTOAST_STATEMENTS_CREATE_SCHEMA_H

#include <string>

#include "identifier.h"
#include "statement.h"

namespace sqltoast {
namespace statements {

typedef struct create_schema : statement_t {
    schema_identifier_t schema_identifier;
    create_schema(schema_identifier_t id) :
        statement_t(STATEMENT_TYPE_CREATE_SCHEMA),
        schema_identifier(id)
    {}
    virtual const std::string to_string();
} create_schema_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_CREATE_SCHEMA_H */
