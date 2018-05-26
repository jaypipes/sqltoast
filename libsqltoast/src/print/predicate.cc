/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const search_condition_t& sc) {
    if (! sc.terms.empty()) {
        size_t x = 0;
        for (const std::unique_ptr<boolean_term_t>& or_term_p : sc.terms) {
            if (x++ > 0)
                out << " OR " << *or_term_p;
            else
                out << *or_term_p;
        }
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const predicate_t& pred) {
    switch (pred.predicate_type) {
        case PREDICATE_TYPE_COMPARISON:
            {
                const comp_predicate_t& cp =
                    static_cast<const comp_predicate_t&>(pred);
                out << cp;
            }
            break;
        case PREDICATE_TYPE_BETWEEN:
            {
                const between_predicate_t& bp =
                    static_cast<const between_predicate_t&>(pred);
                out << bp;
            }
            break;
        case PREDICATE_TYPE_LIKE:
            {
                const like_predicate_t& lp =
                    static_cast<const like_predicate_t&>(pred);
                out << lp;
            }
            break;
        case PREDICATE_TYPE_NULL:
            {
                const null_predicate_t& np =
                    static_cast<const null_predicate_t&>(pred);
                out << np;
            }
            break;
        case PREDICATE_TYPE_IN_VALUES:
            {
                const in_values_predicate_t& ivp =
                    static_cast<const in_values_predicate_t&>(pred);
                out << ivp;
            }
            break;
        case PREDICATE_TYPE_IN_SUBQUERY:
            {
                const in_subquery_predicate_t& isp =
                    static_cast<const in_subquery_predicate_t&>(pred);
                out << isp;
            }
            break;
        case PREDICATE_TYPE_QUANTIFIED_COMPARISON:
            {
                const quantified_comparison_predicate_t& qp =
                    static_cast<const quantified_comparison_predicate_t&>(pred);
                out << qp;
            }
            break;
        case PREDICATE_TYPE_EXISTS:
            {
                const exists_predicate_t& ep =
                    static_cast<const exists_predicate_t&>(pred);
                out << ep;
            }
            break;
        case PREDICATE_TYPE_UNIQUE:
            {
                const unique_predicate_t& ep =
                    static_cast<const unique_predicate_t&>(pred);
                out << ep;
            }
            break;
        case PREDICATE_TYPE_MATCH:
            {
                const match_predicate_t& mp =
                    static_cast<const match_predicate_t&>(pred);
                out << mp;
            }
            break;
        case PREDICATE_TYPE_OVERLAPS:
            {
                const overlaps_predicate_t& op =
                    static_cast<const overlaps_predicate_t&>(pred);
                out << op;
            }
            break;
        default:
            // TODO
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const boolean_primary_t& bp) {
    if (bp.predicate)
        out << *bp.predicate;
    else
        out << '(' << *bp.search_condition << ')';
    return out;
}

std::ostream& operator<< (std::ostream& out, const boolean_factor_t& bf) {
    if (bf.reverse_op)
        out << "NOT ";
    out << *bf.primary;
    return out;
}

std::ostream& operator<< (std::ostream& out, const boolean_term_t& bt) {
    out << *bt.factor;
    if (bt.and_operand)
        out << " AND " << *bt.and_operand;
    return out;
}

std::ostream& operator<< (std::ostream& out, const comp_op_t& op) {
    switch (op) {
        case COMP_OP_EQUAL:
            out << " = ";
            break;
        case COMP_OP_NOT_EQUAL:
            out << " <> ";
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
    return out;
}

std::ostream& operator<< (std::ostream& out, const comp_predicate_t& pred) {
    out << *pred.left << pred.op << *pred.right;
    return out;
}

std::ostream& operator<< (std::ostream& out, const between_predicate_t& pred) {
    out << *pred.left << " BETWEEN " << *pred.comp_left << " AND " << *pred.comp_right;
    return out;
}

std::ostream& operator<< (std::ostream& out, const like_predicate_t& pred) {
    out << *pred.match;
    if (pred.reverse_op)
        out << " NOT";
    out << " LIKE " << *pred.pattern;
    if (pred.escape_char)
        out << " ESCAPE " << *pred.escape_char;
    return out;
}

std::ostream& operator<< (std::ostream& out, const null_predicate_t& pred) {
    out << *pred.left << " IS";
    if (pred.reverse_op)
        out << " NOT";
    out << " NULL";
    return out;
}

std::ostream& operator<< (std::ostream& out, const in_values_predicate_t& pred) {
    out << *pred.left;
    if (pred.reverse_op)
        out << " NOT";
    out << " IN (";
    size_t x = 0;
    for (auto& ve : pred.values) {
        if (x++ > 0)
            out << ",";
        out << *ve;
    }
    out << ")";
    return out;
}

std::ostream& operator<< (std::ostream& out, const in_subquery_predicate_t& pred) {
    out << *pred.left << " IN " << *pred.subquery;
    return out;
}

std::ostream& operator<< (std::ostream& out, const quantified_comparison_predicate_t& pred) {
    out << *pred.left << pred.op;
    if (pred.quantifier == QUANTIFIER_ALL)
        out << "ALL ";
    else
        out << "ANY ";
    out  << *pred.subquery;
    return out;
}

std::ostream& operator<< (std::ostream& out, const exists_predicate_t& pred) {
    out << "EXISTS " << *pred.subquery;
    return out;
}

std::ostream& operator<< (std::ostream& out, const unique_predicate_t& pred) {
    out << "UNIQUE " << *pred.subquery;
    return out;
}

std::ostream& operator<< (std::ostream& out, const match_predicate_t& pred) {
    out << *pred.left << " MATCH ";
    if (pred.match_unique)
        out << "UNIQUE ";
    if (pred.match_partial)
        out << "PARTIAL ";
    else
        out << "FULL ";
    out << *pred.subquery;
    return out;
}

std::ostream& operator<< (std::ostream& out, const overlaps_predicate_t& pred) {
    out << *pred.left << " OVERLAPS " << *pred.right;
    return out;
}

} // namespace sqltoast
