/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_CREATE_TABLE_H
#define SQLTOAST_STATEMENTS_CREATE_TABLE_H

#include <string>

#include "identifier.h"
#include "statement.h"

namespace sqltoast {
namespace statements {

typedef struct create_table : statement_t {
    identifier_t table_identifier;
    create_table(identifier_t& table_id) :
        statement_t(STATEMENT_TYPE_CREATE_TABLE),
        table_identifier(table_id)
    {}
    virtual const std::string to_string();
} create_table_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_CREATE_TABLE_H */
