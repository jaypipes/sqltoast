/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_CREATE_TABLE_H
#define SQLTOAST_STATEMENTS_CREATE_TABLE_H

#include <string>

#include "column_definition.h"
#include "identifier.h"
#include "statement.h"

namespace sqltoast {
namespace statements {

typedef enum table_type {
    TABLE_TYPE_NORMAL,
    TABLE_TYPE_TEMPORARY_GLOBAL,
    TABLE_TYPE_TEMPORARY_LOCAL
} table_type_t;

typedef struct create_table : statement_t {
    table_type_t table_type;
    identifier_t table_identifier;
    std::vector<column_definition_t> column_definitions;
    create_table(table_type_t table_type, identifier_t& table_id) :
        statement_t(STATEMENT_TYPE_CREATE_TABLE),
        table_type(table_type),
        table_identifier(table_id)
    {}
    virtual const std::string to_string();
} create_table_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_CREATE_TABLE_H */
