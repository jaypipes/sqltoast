/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_SEARCH_CONDITION_H
#define SQLTOAST_SEARCH_CONDITION_H

#include <memory>
#include <vector>
#include <ostream>

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef enum comp_op {
    COMP_OP_EQUAL,
    COMP_OP_NOT_EQUAL,
    COMP_OP_LESS,
    COMP_OP_GREATER,
    COMP_OP_LESS_EQUAL,
    COMP_OP_GREATER_EQUAL,
    COMP_OP_BETWEEN,
    COMP_OP_IN,
    COMP_OP_LIKE,
    COMP_OP_NULL,
    COMP_OP_EXISTS,
    COMP_OP_UNIQUE
} comp_op_t;

typedef struct search_condition {
    comp_op_t op;
    bool reverse_op;
    search_condition(comp_op_t op) : op(op), reverse_op(false)
    {}
} search_condition_t;

std::ostream& operator<< (std::ostream& out, const search_condition_t& sc);

typedef struct comp_predicate : search_condition_t {
    lexeme_t left;
    lexeme_t right;
    comp_predicate(comp_op_t op, lexeme_t& left, lexeme_t& right) :
        search_condition_t(op),
        left(left),
        right(right)
    {}
} comp_predicate_t;

std::ostream& operator<< (std::ostream& out, const comp_predicate_t& pred);

} // namespace sqltoast

#endif /* SQLTOAST_SEARCH_CONDITION_H */
