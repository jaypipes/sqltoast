/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/print.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const query_specification_t& query) {
    out << "query[";
    if (query.distinct)
       out << "distinct: true ";
    out << "selected-columns[";
    size_t x = 0;
    for (const derived_column_t& dc : query.selected_columns) {
        if (x++ > 0)
            out << ',';
        out << dc;
    }
    out << "] ";
    out << *query.table_expression << "]";

    return out;
}

std::ostream& operator<< (std::ostream& out, const table_expression_t& table_exp) {
    out << "table-expression[";
    out << "referenced-tables[";
    size_t x = 0;
    for (const std::unique_ptr<table_reference_t>& tr : table_exp.referenced_tables) {
        if (x++ > 0)
            out << ',';
        out << *tr;
    }
    if (table_exp.where_condition)
        out << " where[" << *table_exp.where_condition << ']';
    if (! table_exp.group_by_columns.empty()) {
        out << " group-by[";
        x = 0;
        for (const grouping_column_reference_t& gcr : table_exp.group_by_columns) {
            if (x++ > 0)
                out << ',';
            out << gcr;
        }
        out << ']';
    }
    if (table_exp.having_condition)
        out << " having[" << *table_exp.having_condition << ']';
    out << "]";

    return out;
}

std::ostream& operator<< (std::ostream& out, const query_expression_t& qe) {
    switch (qe.query_expression_type) {
        case QUERY_EXPRESSION_TYPE_NON_JOIN_QUERY_EXPRESSION:
            {
                const non_join_query_expression_t& sub =
                    static_cast<const non_join_query_expression_t&>(qe);
                out << sub;
            }
            break;
        case QUERY_EXPRESSION_TYPE_JOINED_TABLE:
            {
                const joined_table_query_expression_t& sub =
                    static_cast<const joined_table_query_expression_t&>(qe);
                out << sub;
            }
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const non_join_query_expression_t& qe) {
    out << *qe.term;
    return out;
}

std::ostream& operator<< (std::ostream& out, const non_join_query_term_t& term) {
    out << *term.primary;
    return out;
}

std::ostream& operator<< (std::ostream& out, const non_join_query_primary_t& primary) {
    switch (primary.primary_type) {
        case NON_JOIN_QUERY_PRIMARY_TYPE_QUERY_SPECIFICATION:
            {
                const query_specification_non_join_query_primary_t& sub =
                    static_cast<const query_specification_non_join_query_primary_t&>(primary);
                out << sub;
            }
            break;
        case NON_JOIN_QUERY_PRIMARY_TYPE_TABLE_VALUE_CONSTRUCTOR:
            {
                const table_value_constructor_non_join_query_primary_t& sub =
                    static_cast<const table_value_constructor_non_join_query_primary_t&>(primary);
                out << sub;
            }
            break;
        case NON_JOIN_QUERY_PRIMARY_TYPE_EXPLICIT_TABLE:
        case NON_JOIN_QUERY_PRIMARY_TYPE_SUBEXPRESSION:
            // TODO
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const query_specification_non_join_query_primary_t& primary) {
    out << *primary.query_spec;
    return out;
}

std::ostream& operator<< (std::ostream& out, const table_value_constructor_non_join_query_primary_t& primary) {
    out << "values[" << *primary.table_value << ']';
    return out;
}

std::ostream& operator<< (std::ostream& out, const table_value_constructor_t& table_value) {
    size_t x = 0;
    for (const std::unique_ptr<row_value_constructor_t>& value : table_value.values) {
        if (x++ > 0)
            out << ',';
        out << *value;
    }
    return out;
}

std::ostream& operator<< (std::ostream& out, const joined_table_query_expression_t& qe) {
    out << *qe.joined_table;
    return out;
}

} // namespace sqltoast
