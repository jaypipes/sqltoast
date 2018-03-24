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

std::ostream& operator<< (std::ostream& out, const numeric_term_t& nt) {
    if (! nt.right) {
        out << *nt.left->value;
    } else {
        out << *nt.left->value;
        if (nt.op == NUMERIC_OP_MULTIPLY)
            out << " * ";
        else
            out << " / ";
        out << *nt.right->value;
    }
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

std::ostream& operator<< (std::ostream& out, const string_function_t& sf) {
    switch (sf.type) {
        case STRING_FUNCTION_TYPE_SUBSTRING:
            {
                const substring_function_t& subs = static_cast<const substring_function_t&>(sf);
                out << subs;
            }
            break;
        case STRING_FUNCTION_TYPE_UPPER:
            out << "upper[" << *sf.operand << "]";
            break;
        case STRING_FUNCTION_TYPE_LOWER:
            out << "lower[" << *sf.operand << "]";
            break;
        case STRING_FUNCTION_TYPE_CONVERT:
            {
                const convert_function_t& cf = static_cast<const convert_function_t&>(sf);
                out << cf;
            }
            break;
        case STRING_FUNCTION_TYPE_TRANSLATE:
            {
                const translate_function_t& tf = static_cast<const translate_function_t&>(sf);
                out << tf;
            }
            break;
        case STRING_FUNCTION_TYPE_TRIM:
            {
                const trim_function_t& tf = static_cast<const trim_function_t&>(sf);
                out << tf;
            }
            break;
        default:
            out << "string-function[UNKNOWN]";
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const substring_function_t& sf) {
    out << "substring[" << *sf.operand << " FROM " << *sf.start_position_value;
    if (sf.for_length_value)
        out << " FOR " << *sf.for_length_value;
    out << "]";
    return out;
}

std::ostream& operator<< (std::ostream& out, const convert_function_t& cf) {
    out << "convert[" << *cf.operand << " USING " << cf.conversion_name << "]";
    return out;
}

std::ostream& operator<< (std::ostream& out, const translate_function_t& tf) {
    out << "translate[" << *tf.operand << " USING " << tf.translation_name << "]";
    return out;
}

std::ostream& operator<< (std::ostream& out, const trim_function_t& tf) {
    out << "trim[";
    if (tf.trim_character) {
        switch (tf.specification) {
            case TRIM_SPECIFICATION_LEADING:
                out << "LEADING ";
                break;
            case TRIM_SPECIFICATION_TRAILING:
                out << "TRAILING ";
                break;
            case TRIM_SPECIFICATION_BOTH:
                out << "BOTH ";
                break;
        }
        out << *tf.trim_character << " FROM ";
    }
    out << *tf.operand << "]";
    return out;
}

std::ostream& operator<< (std::ostream& out, const character_primary_t& cp) {
    if (cp.value)
        out << *cp.value;
    else
        out << *cp.string_function;
    return out;
}

std::ostream& operator<< (std::ostream& out, const character_factor_t& cf) {
    out << *cf.value;
    if (cf.collation)
        out << " COLLATE " << cf.collation;
    return out;
}

std::ostream& operator<< (std::ostream& out, const time_zone_specifier_t& tzs) {
    if (! tzs.local_tz)
        out << " AT TIME ZONE ";// << *tzs.interval_value;
    else
        out << " AT LOCAL";
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_primary_t& dp) {
    out << *dp.value;
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_factor_t& factor) {
    out << *factor.value;
    if (factor.time_zone_specifier)
        out << *factor.time_zone_specifier;
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_term_t& term) {
    out << *term.value;
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_value_expression_t& de) {
    // datetime expressions are the container for things evaluate to a datetime
    // value
    out << "datetime-expression[" << *de.left << "]";
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_field_t& df) {
    out << df.interval;
    if (df.interval != INTERVAL_UNIT_SECOND) {
        if (df.precision > 0)
            out << '(' << df.precision;
    } else {
        // We can have 0 leading precision and non-zero fractional precision
        // for second intervals...
        if (df.precision > 0 ||
                (df.precision == 0 && df.fractional_precision > 0)) {
            out << df.precision;
            if (df.fractional_precision > 0)
                out << ", " << df.fractional_precision;
            out << ')';
        }
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_qualifier_t& iq) {
    out << iq.start;
    if (iq.end)
        out << " TO " << *iq.end;
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_primary_t& primary) {
    out << *primary.value;
    if (primary.qualifier)
        out << *primary.qualifier;
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_factor_t& factor) {
    if (factor.sign != 0)
        out << factor.sign << ' ';
    out << *factor.value;
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_term_t& term) {
    out << *term.left;
    return out;
}

std::ostream& operator<< (std::ostream& out, const row_value_constructor_t& rvc) {
    switch (rvc.rvc_type) {
        case RVC_TYPE_DEFAULT:
            out <<"DEFAULT";
            break;
        case RVC_TYPE_NULL:
            out <<"NULL";
            break;
        case RVC_TYPE_VALUE_EXPRESSION:
            {
                const row_value_expression_t& val = static_cast<const row_value_expression_t&>(rvc);
                out << val;
            }
            break;
        default:
            out << "row-value-constructor[UNKNOWN]";
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const row_value_expression_t& rve) {
    out << *rve.value;
    return out;
}

} // namespace sqltoast
