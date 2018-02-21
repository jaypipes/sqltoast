/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENT_H
#define SQLTOAST_STATEMENT_H

#include "sqltoast.h"

namespace sqltoast {

typedef enum statement_type {
    STATEMENT_TYPE_CREATE_SCHEMA,
    STATEMENT_TYPE_CREATE_TABLE,
    STATEMENT_TYPE_DROP_SCHEMA,
    STATEMENT_TYPE_SELECT
} statement_type_t;

typedef struct statement {
    statement_type_t type;
    statement(statement_type_t type) : type(type)
    {}
    virtual const std::string to_string() = 0;
} statement_t;

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

} // namespace sqltoast

#endif /* SQLTOAST_STATEMENT_H */
