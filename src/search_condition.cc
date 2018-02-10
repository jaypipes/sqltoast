/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "search_condition.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const search_condition_t& sc) {
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
        default:
            // TODO
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const comp_predicate_t& pred) {
    out << std::string(pred.left.start, pred.left.end);
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
    out << std::string(pred.right.start, pred.right.end);
    return out;
}

} // namespace sqltoast
