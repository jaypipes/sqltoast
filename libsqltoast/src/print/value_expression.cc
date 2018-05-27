/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc) {
    switch (rvc.rvc_type) {
        case RVC_TYPE_ELEMENT:
            {
                const row_value_constructor_element_t& el =
                    static_cast<const row_value_constructor_element_t&>(rvc);
                out << el;
            }
            break;
        case RVC_TYPE_LIST:
            {
                const row_value_constructor_list_t& els =
                    static_cast<const row_value_constructor_list_t&>(rvc);
                out << '(';
                size_t x = 0;
                for (const auto& el : els.elements) {
                    if (x++ > 0)
                        out << ',';
                    out << *el;
                }
                out << ')';
            }
            break;
        default:
            out << "row-value-constructor[UNKNOWN]";
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const row_value_constructor_element_t& rvce) {
    switch (rvce.rvc_element_type) {
        case RVC_ELEMENT_TYPE_DEFAULT:
            out << "DEFAULT";
            break;
        case RVC_ELEMENT_TYPE_NULL:
            out << "NULL";
            break;
        case RVC_ELEMENT_TYPE_VALUE_EXPRESSION:
            {
                const row_value_expression_t& val = static_cast<const row_value_expression_t&>(rvce);
                out << val;
            }
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const row_value_expression_t& rve) {
    out << *rve.value;
    return out;
}

std::ostream& operator<< (std::ostream& out, const value_expression_t& ve) {
    switch (ve.type) {
        case VALUE_EXPRESSION_TYPE_NUMERIC_EXPRESSION:
            {
                const numeric_expression_t& ne =
                    static_cast<const numeric_expression_t&>(ve);
                out << ne;
            }
            break;
        case VALUE_EXPRESSION_TYPE_STRING_EXPRESSION:
            {
                const character_value_expression_t& cve =
                    static_cast<const character_value_expression_t&>(ve);
                out << cve;
            }
            break;
        case VALUE_EXPRESSION_TYPE_DATETIME_EXPRESSION:
            {
                const datetime_value_expression_t& dtve =
                    static_cast<const datetime_value_expression_t&>(ve);
                out << dtve;
            }
            break;
        case VALUE_EXPRESSION_TYPE_INTERVAL_EXPRESSION:
            {
                const interval_value_expression_t& ive =
                    static_cast<const interval_value_expression_t&>(ve);
                out << ive;
            }
            break;
        default:
            out << "unknown-value-expression";
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const numeric_expression_t& ne) {
    // numeric expressions are the container for things that may be evaluated
    // to a number. However, numeric expressions that have only a single
    // element can be reduced to just that one element
    if (! ne.right) {
        if (! ne.left->right)
            out << *ne.left;
        else
            out << "numeric-expression[" << *ne.left << "]";
    } else {
        out << "numeric-expression[";
        out << *ne.left;
        if (ne.op == NUMERIC_OP_ADD)
            out << " + ";
        else
            out << " - ";
        out << *ne.right << "]";
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const character_value_expression_t& cve) {
    if (cve.values.size() > 1) {
        out << "concatenate[";
        size_t x = 0;
        for (const std::unique_ptr<character_factor_t>& val : cve.values) {
            if (x++ > 0)
                out << ", ";
            out << *val;
        }
        out << "]";
    } else
        out << *cve.values[0];
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_value_expression_t& de) {
    // datetime expressions are the container for things that may be evaluated
    // to a number. However, datetime expressions that have only a single
    // element can be reduced to just that one element
    if (! de.right)
        out << "datetime-expression[" << *de.left << "]";
    else {
        out << "datetime-expression[";
        out << *de.left;
        if (de.op == NUMERIC_OP_ADD)
            out << " + ";
        else
            out << " - ";
        out << *de.right << "]";
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_value_expression_t& ie) {
    // interval expressions are the container for things that may be evaluated
    // to a number. However, interval expressions that have only a single
    // element can be reduced to just that one element
    if (! ie.right)
        out << "interval-expression[" << *ie.left << "]";
    else {
        out << "interval-expression[";
        out << *ie.left;
        if (ie.op == NUMERIC_OP_ADD)
            out << " + ";
        else
            out << " - ";
        out << *ie.right << "]";
    }
    return out;
}

} // namespace sqltoast
