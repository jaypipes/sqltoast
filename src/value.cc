/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "value.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const value_expression_t& ve) {
    switch (ve.type) {
        case VALUE_EXPRESSION_TYPE_LITERAL:
            out << "literal[" << std::string(ve.lexeme.start, ve.lexeme.end) << ']';
            break;
        case VALUE_EXPRESSION_TYPE_COLUMN:
            out << "column-reference[" << std::string(ve.lexeme.start, ve.lexeme.end) << ']';
            break;
        case VALUE_EXPRESSION_TYPE_GENERAL:
            out << "general-value[" << std::string(ve.lexeme.start, ve.lexeme.end) << ']';
            break;
        case VALUE_EXPRESSION_TYPE_SCALAR_SUBQUERY:
            out << "scalar-subquery";
            break;
        default:
            out << "value-expression[" << std::string(ve.lexeme.start, ve.lexeme.end) << ']';
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc) {
    switch (rvc.rvc_type) {
        case RVC_TYPE_VALUE_EXPRESSION:
            {
                const value_expression_t& ve = static_cast<const value_expression_t&>(rvc);
                out << ve;
            }
            break;
        default:
            out << "row-value-constructor[" << std::string(rvc.lexeme.start, rvc.lexeme.end) << ']';
            break;
    }
    return out;
}

} // namespace sqltoast
