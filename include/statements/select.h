/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENTS_SELECT_H
#define SQLTOAST_STATEMENTS_SELECT_H

#include <string>

#include "statement.h"

namespace sqltoast {
namespace statements {

typedef struct select : statement_t {
    bool distinct;
    select() :
        statement_t(STATEMENT_TYPE_SELECT),
        distinct(false)
    {}
    virtual const std::string to_string();
} select_t;

} // namespace sqltoast::statements
} // namespace sqltoast

#endif /* SQLTOAST_STATEMENTS_SELECT_H */
