/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const set_function_t& sf) {
    switch (sf.func_type) {
        case SET_FUNCTION_TYPE_COUNT_STAR:
            out << "COUNT(*)";
            break;
        case SET_FUNCTION_TYPE_COUNT_DISTINCT:
            out << "COUNT(DISTINCT " << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_COUNT:
            out << "COUNT(" << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_AVG_DISTINCT:
            out << "AVG(DISTINCT " << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_AVG:
            out << "AVG(" << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_MIN_DISTINCT:
            out << "MIN(DISTINCT " << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_MIN:
            out << "MIN(" << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_MAX_DISTINCT:
            out << "MAX(DISTINCT " << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_MAX:
            out << "MAX(" << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_SUM_DISTINCT:
            out << "SUM(DISTINCT " << *sf.value << ")";
            break;
        case SET_FUNCTION_TYPE_SUM:
            out << "SUM(" << *sf.value << ")";
            break;
        default:
            break;
    }
    return out;
}
std::ostream& operator<< (std::ostream& out, const value_expression_primary_t& vep) {
    switch (vep.vep_type) {
        case VEP_TYPE_UNSIGNED_VALUE_SPECIFICATION:
            {
                const unsigned_value_specification_t& uvs = static_cast<const unsigned_value_specification_t&>(vep);
                out << uvs;
            }
            break;
        case VEP_TYPE_COLUMN_REFERENCE:
            out << "column-reference[" << std::string(vep.lexeme.start, vep.lexeme.end) << ']';
            break;
        case VEP_TYPE_SET_FUNCTION_SPECIFICATION:
            {
                const set_function_t& sf = static_cast<const set_function_t&>(vep);
                out << sf;
            }
            break;
        case VEP_TYPE_VALUE_EXPRESSION:
            {
                const value_subexpression_t& ve = static_cast<const value_subexpression_t&>(vep);
                out << "(" << *ve.value << ")";
            }
            break;
        case VEP_TYPE_SCALAR_SUBQUERY:
            {
                const scalar_subquery_t& subq = static_cast<const scalar_subquery_t&>(vep);
                out << "scalar-subquery[" << std::endl << *subq.subquery << "]";
            }
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const unsigned_value_specification_t& uvs) {
    switch (uvs.uvs_type) {
        case UVS_TYPE_UNSIGNED_NUMERIC:
        case UVS_TYPE_CHARACTER_STRING:
        case UVS_TYPE_NATIONAL_CHARACTER_STRING:
        case UVS_TYPE_BIT_STRING:
        case UVS_TYPE_HEX_STRING:
        case UVS_TYPE_DATETIME:
        case UVS_TYPE_INTERVAL:
            out << "literal[" << std::string(uvs.lexeme.start, uvs.lexeme.end) << ']';
            break;
        case UVS_TYPE_USER:
            out << "USER";
            break;
        case UVS_TYPE_CURRENT_USER:
            out << "CURRENT_USER";
            break;
        case UVS_TYPE_SESSION_USER:
            out << "SESSION_USER";
            break;
        case UVS_TYPE_SYSTEM_USER:
            out << "SYSTEM_USER";
            break;
        case UVS_TYPE_VALUE:
            out << "VALUE";
            break;
        case UVS_TYPE_PARAMETER:
            out << "parameter[" << std::string(uvs.lexeme.start, uvs.lexeme.end) << ']';
            break;
        default:
            out << "unknown-unsigned-value-expression";
            break;
    }
    return out;
}

} // namespace sqltoast
