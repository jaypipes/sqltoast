/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "predicate.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const search_condition_t& sc) {
    if (sc.reverse_op)
        out << "NOT ";
    switch (sc.op) {
        case COMP_OP_EQUAL:
        case COMP_OP_NOT_EQUAL:
        case COMP_OP_LESS:
        case COMP_OP_GREATER:
        case COMP_OP_LESS_EQUAL:
        case COMP_OP_GREATER_EQUAL:
            {
                const comp_predicate_t& pred = static_cast<const comp_predicate_t&>(sc);
                out << pred;
            }
            break;
        case COMP_OP_BETWEEN:
            {
                const between_predicate_t& pred = static_cast<const between_predicate_t&>(sc);
                out << pred;
            }
            break;
        default:
            // TODO
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const comp_predicate_t& pred) {
    out << *pred.left;
    switch (pred.op) {
        case COMP_OP_EQUAL:
            out << " = ";
            break;
        case COMP_OP_NOT_EQUAL:
            out << " != ";
            break;
        case COMP_OP_LESS:
            out << " < ";
            break;
        case COMP_OP_GREATER:
            out << " > ";
            break;
        case COMP_OP_LESS_EQUAL:
            out << " <= ";
            break;
        case COMP_OP_GREATER_EQUAL:
            out << " >= ";
            break;
        default:
            // TODO
            break;
    }
    out << *pred.right;
    return out;
}

std::ostream& operator<< (std::ostream& out, const between_predicate_t& pred) {
    out << *pred.left << " BETWEEN " << *pred.comp_left << " AND " << *pred.comp_right;
};

} // namespace sqltoast
