/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const set_function_t& sf) {
    switch (sf.func_type) {
        case SET_FUNCTION_TYPE_COUNT:
            out << "COUNT";
            break;
        case SET_FUNCTION_TYPE_AVG:
            out << "AVG";
            break;
        case SET_FUNCTION_TYPE_MIN:
            out << "MIN";
            break;
        case SET_FUNCTION_TYPE_MAX:
            out << "MAX";
            break;
        case SET_FUNCTION_TYPE_SUM:
            out << "SUM";
            break;
    }
    out << '(';
    if (sf.star)
        out << '*';
    else {
        if (sf.distinct)
            out << "DISTINCT ";
        out << *sf.value;
    }
    out << ')';
    return out;
}

std::ostream& operator<< (std::ostream& out, const case_expression_t& ce) {
    switch (ce.case_type) {
        case CASE_EXPRESSION_TYPE_COALESCE_FUNCTION:
            {
                const coalesce_function_t& cf =
                    static_cast<const coalesce_function_t&>(ce);
                out << cf;
            }
            break;
        case CASE_EXPRESSION_TYPE_NULLIF_FUNCTION:
            {
                const nullif_function_t& nf =
                    static_cast<const nullif_function_t&>(ce);
                out << nf;
            }
            break;
        case CASE_EXPRESSION_TYPE_SIMPLE_CASE:
            {
                const simple_case_expression_t& sce =
                    static_cast<const simple_case_expression_t&>(ce);
                out << sce;
            }
            break;
        case CASE_EXPRESSION_TYPE_SEARCHED_CASE:
            {
                const searched_case_expression_t& sce =
                    static_cast<const searched_case_expression_t&>(ce);
                out << sce;
            }
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const simple_case_expression_t& sce) {
    out << "simple-case-expression[" << *sce.operand;
    for (const auto& when_clause : sce.when_clauses) {
        out << " WHEN " << *when_clause.operand
            << " THEN " << *when_clause.result;
    }
    if (sce.else_value)
        out << " ELSE " << *sce.else_value;
    out << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const searched_case_expression_t& sce) {
    out << "searched-case-expression[";
    size_t x = 0;
    for (const auto& when_clause : sce.when_clauses) {
        if (x++ > 0)
            out << ' ';
        out << "WHEN " << *when_clause.condition
            << " THEN " << *when_clause.result;
    }
    if (sce.else_value)
        out << " ELSE " << *sce.else_value;
    out << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const coalesce_function_t& cf) {
    out << "coalesce[";
    size_t x = 0;
    for (const auto& val : cf.values) {
        if (x++ > 0)
            out << ',';
        out << *val;
    }
    out << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const nullif_function_t& nf) {
    out << "nullif[" << *nf.left << ',' << *nf.right << ']';
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
        case VEP_TYPE_PARENTHESIZED_VALUE_EXPRESSION:
            {
                const parenthesized_value_expression_t& ve =
                    static_cast<const parenthesized_value_expression_t&>(vep);
                out << "parenthesized-value-expression[" << *ve.value << "]";
            }
            break;
        case VEP_TYPE_CASE_EXPRESSION:
            {
                const case_expression_t& ce = static_cast<const case_expression_t&>(vep);
                out << ce;
            }
            break;
        case VEP_TYPE_SCALAR_SUBQUERY:
            {
                const scalar_subquery_t& subq = static_cast<const scalar_subquery_t&>(vep);
                out << "scalar-subquery[" << std::endl << *subq.query << "]";
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

std::ostream& operator<< (std::ostream& out, const numeric_value_t& nv) {
    out << *nv.primary;
    return out;
}

std::ostream& operator<< (std::ostream& out, const length_expression_t& le) {
    switch (le.type) {
        case NUMERIC_FUNCTION_TYPE_CHAR_LENGTH:
            out << "char-length[";
            break;
        case NUMERIC_FUNCTION_TYPE_BIT_LENGTH:
            out << "bit-length[";
            break;
        case NUMERIC_FUNCTION_TYPE_OCTET_LENGTH:
            out << "octet-length[";
            break;
        default:
            // TODO(jaypipes)
            break;
    }
    out << *le.operand << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const position_expression_t& pe) {
    out << "position[" << *pe.to_find
        << " IN " << *pe.subject << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const extract_expression_t& ee) {
    out << "extract[" << ee.extract_field
        << " FROM " << *ee.extract_source << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const numeric_function_t& nf) {
    switch (nf.type) {
        case NUMERIC_FUNCTION_TYPE_POSITION:
            {
                const position_expression_t& pe =
                    static_cast<const position_expression_t&>(nf);
                out << pe;
            }
            break;
        case NUMERIC_FUNCTION_TYPE_EXTRACT:
            {
                const extract_expression_t& ee =
                    static_cast<const extract_expression_t&>(nf);
                out << ee;
            }
            break;
        case NUMERIC_FUNCTION_TYPE_CHAR_LENGTH:
        case NUMERIC_FUNCTION_TYPE_BIT_LENGTH:
        case NUMERIC_FUNCTION_TYPE_OCTET_LENGTH:
            {
                const length_expression_t& le =
                    static_cast<const length_expression_t&>(nf);
                out << le;
            }
            break;
        default:
            // TODO(jaypipes)
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const numeric_primary_t& np) {
    switch (np.type) {
        case NUMERIC_PRIMARY_TYPE_VALUE:
            {
                const numeric_value_t& nv =
                    static_cast<const numeric_value_t&>(np);
                out << nv;
            }
            break;
        case NUMERIC_PRIMARY_TYPE_FUNCTION:
            {
                const numeric_function_t& nf =
                    static_cast<const numeric_function_t&>(np);
                out << nf;
            }
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const numeric_factor_t& nf) {
    if (nf.sign != 0)
        out << nf.sign << ' ';
    out << *nf.primary;
    return out;
}

std::ostream& operator<< (std::ostream& out, const numeric_term_t& nt) {
    if (! nt.right) {
        out << *nt.left;
    } else {
        out << *nt.left;
        if (nt.op == NUMERIC_OP_MULTIPLY)
            out << " * ";
        else
            out << " / ";
        out << *nt.right;
    }
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
    out << *cf.primary;
    if (cf.collation)
        out << " COLLATE " << cf.collation;
    return out;
}

std::ostream& operator<< (std::ostream& out, const current_datetime_function_t& df) {
    switch (df.func_type) {
        case DATETIME_FUNCTION_TYPE_CURRENT_DATE:
            out << "current-date[]";
            break;
        case DATETIME_FUNCTION_TYPE_CURRENT_TIME:
            out << "current-time[";
            if (df.precision != 0)
                out << df.precision;
            out << ']';
            break;
        case DATETIME_FUNCTION_TYPE_CURRENT_TIMESTAMP:
            out << "current-timestamp[";
            if (df.precision != 0)
                out << df.precision;
            out << ']';
            break;
        default:
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_value_t& dv) {
    out << *dv.primary;
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_primary_t& dp) {
    switch (dp.type) {
        case DATETIME_PRIMARY_TYPE_VALUE:
            {
                const datetime_value_t& dv =
                    static_cast<const datetime_value_t&>(dp);
                out << dv;
            }
            break;
        case DATETIME_PRIMARY_TYPE_FUNCTION:
            {
                const current_datetime_function_t& df =
                    static_cast<const current_datetime_function_t&>(dp);
                out << df;
            }
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_factor_t& factor) {
    out << *factor.primary;
    if (factor.is_local_tz())
        out << " AT LOCAL";
    else
        out << " AT TIME ZONE " << factor.tz;
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_term_t& term) {
    out << *term.value;
    return out;
}

std::ostream& operator<< (std::ostream& out, const datetime_field_t& df) {
    out << df.interval;
    if (df.interval != INTERVAL_UNIT_SECOND) {
        if (df.precision > 0)
            out << '(' << df.precision << ')';
    } else {
        // We can have 0 leading precision and non-zero fractional precision
        // for second intervals...
        if (df.precision > 0 ||
                (df.precision == 0 && df.fractional_precision > 0)) {
            out << '(' << df.precision;
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
        out << ' ' << *primary.qualifier;
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_factor_t& factor) {
    if (factor.sign != 0)
        out << factor.sign << ' ';
    out << *factor.primary;
    return out;
}

std::ostream& operator<< (std::ostream& out, const interval_term_t& term) {
    if (! term.right) {
        out << *term.left;
    } else {
        out << *term.left;
        if (term.op == NUMERIC_OP_MULTIPLY)
            out << " * ";
        else
            out << " / ";
        out << *term.right;
    }
    return out;
}

} // namespace sqltoast
