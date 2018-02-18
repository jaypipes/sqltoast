/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "predicate.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const search_condition_t& sc) {
    if (sc.term) {
        out << std::endl << "     " << *sc.term;
        boolean_term_t* next_term = sc.term->term_and.get();
        while (next_term != NULL) {
            out << std::endl << "     AND " << *next_term;
            next_term = next_term->term_and.get();
        }
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const boolean_term_t& bt) {
    if (bt.reverse_op)
        out << "NOT ";
    switch (bt.op) {
        case COMP_OP_EQUAL:
        case COMP_OP_NOT_EQUAL:
        case COMP_OP_LESS:
        case COMP_OP_GREATER:
        case COMP_OP_LESS_EQUAL:
        case COMP_OP_GREATER_EQUAL:
            {
                const comp_predicate_t& pred = static_cast<const comp_predicate_t&>(bt);
                out << pred;
            }
            break;
        case COMP_OP_BETWEEN:
            {
                const between_predicate_t& pred = static_cast<const between_predicate_t&>(bt);
                out << pred;
            }
            break;
        case COMP_OP_NULL:
            {
                const null_predicate_t& pred = static_cast<const null_predicate_t&>(bt);
                out << pred;
            }
            break;
        case COMP_OP_IN_VALUES:
            {
                const in_values_predicate_t& pred = static_cast<const in_values_predicate_t&>(bt);
                out << pred;
            }
            break;
        case COMP_OP_IN_SUBQUERY:
            {
                const in_subquery_predicate_t& pred = static_cast<const in_subquery_predicate_t&>(bt);
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
            break;
    }
    out << *pred.right;
    return out;
}

std::ostream& operator<< (std::ostream& out, const between_predicate_t& pred) {
    out << *pred.left << " BETWEEN " << *pred.comp_left << " AND " << *pred.comp_right;
};

std::ostream& operator<< (std::ostream& out, const null_predicate_t& pred) {
    out << *pred.left << " IS NULL";
};

std::ostream& operator<< (std::ostream& out, const in_values_predicate_t& pred) {
    out << *pred.left << " IN (";
    size_t x = 0;
    for (auto& ve : pred.values) {
        if (x++ > 0)
            out << ", ";
        out << *ve;
    }
    out << ")";
};

std::ostream& operator<< (std::ostream& out, const in_subquery_predicate_t& pred) {
    out << *pred.left << " IN <subquery>";
}

} // namespace sqltoast
