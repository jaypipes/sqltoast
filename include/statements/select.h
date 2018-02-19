/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_SELECT_H
#define SQLTOAST_STATEMENTS_SELECT_H

#include <string>
#include <memory>
#include <vector>

#include "derived_column.h"
#include "predicate.h"
#include "statement.h"
#include "table_reference.h"

namespace sqltoast {
namespace statements {

typedef struct select : statement_t {
    bool distinct;
    std::vector<derived_column_t> selected_columns;
    std::vector<table_reference_t> referenced_tables;
    std::unique_ptr<search_condition_t> where_condition;
    select() :
        statement_t(STATEMENT_TYPE_SELECT),
        distinct(false)
    {}
    virtual const std::string to_string();
} select_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_SELECT_H */
