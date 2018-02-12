/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_CREATE_SCHEMA_H
#define SQLTOAST_STATEMENTS_CREATE_SCHEMA_H

#include <string>

#include "sqltoast.h"
#include "statement.h"

namespace sqltoast {
namespace statements {

typedef struct create_schema : statement_t {
    lexeme_t schema_name;
    // Optional elements
    lexeme_t authorization_identifier;
    lexeme_t default_charset;
    create_schema(lexeme_t& schema_name) :
        statement_t(STATEMENT_TYPE_CREATE_SCHEMA),
        schema_name(schema_name)
    {}
    virtual const std::string to_string();
} create_schema_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_CREATE_SCHEMA_H */
