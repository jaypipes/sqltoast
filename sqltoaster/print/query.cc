/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "printers.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const table_expression_t& te) {
    out << "<table expression";
    if (stmt.distinct)
       out << std::endl << "   distinct: true";
    out << std::endl << "   selected columns:";
    size_t x = 0;
    for (const derived_column_t& dc : stmt.selected_columns) {
        out << std::endl << "     " << x++ << ": " << dc;
    }
    out << std::endl << "   referenced tables:";
    x = 0;
    for (const std::unique_ptr<table_reference_t>& tr : stmt.referenced_tables) {
        out << std::endl << "     " << x++ << ": " << *tr;
    }
    if (stmt.where_condition) {
        out << std::endl << "   where:" << std::endl << "     ";
        out << *stmt.where_condition;
    }
    if (! stmt.group_by_columns.empty()) {
        out << std::endl << "   group by:";
        x = 0;
        for (const grouping_column_reference_t& gcr : stmt.group_by_columns) {
            out << std::endl << "     " << x++ << ": " << gcr;
        }
    }
    if (stmt.having_condition) {
        out << std::endl << "   having:" << std::endl << "     ";
        out << *stmt.having_condition;
    }
    out << ">";

    return out;
}

std::ostream& operator<< (std::ostream& out, const query_specification_t& qs) {
    out << "<query specification";
    if (stmt.distinct)
       out << std::endl << "   distinct: true";
    out << std::endl << "   selected columns:";
    size_t x = 0;
    for (const derived_column_t& dc : stmt.selected_columns) {
        out << std::endl << "     " << x++ << ": " << dc;
    }
    out << std::endl << "   table expression:";
    x = 0;
    for (const std::unique_ptr<table_reference_t>& tr : stmt.referenced_tables) {
        out << std::endl << "     " << x++ << ": " << *tr;
    }
    if (stmt.where_condition) {
        out << std::endl << "   where:" << std::endl << "     ";
        out << *stmt.where_condition;
    }
    if (! stmt.group_by_columns.empty()) {
        out << std::endl << "   group by:";
        x = 0;
        for (const grouping_column_reference_t& gcr : stmt.group_by_columns) {
            out << std::endl << "     " << x++ << ": " << gcr;
        }
    }
    if (stmt.having_condition) {
        out << std::endl << "   having:" << std::endl << "     ";
        out << *stmt.having_condition;
    }
    out << ">";

    return out;
}

} // namespace sqltoast
