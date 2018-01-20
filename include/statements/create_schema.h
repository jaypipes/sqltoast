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
    identifier_t schema_identifier;
    std::unique_ptr<identifier_t> authorization_identifier;
    create_schema(identifier_t& schema_id, std::unique_ptr<identifier_t>& authorization_id) :
        statement_t(STATEMENT_TYPE_CREATE_SCHEMA),
        schema_identifier(schema_id),
        authorization_identifier(std::move(authorization_id))
    {}
    virtual const std::string to_string();
} create_schema_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_CREATE_SCHEMA_H */
