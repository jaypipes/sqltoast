/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast.h"

namespace sqltoast {

const std::string create_schema_t::to_string() {
    std::stringstream ss;
    ss << "<statement: CREATE SCHEMA" << std::endl
       << "   schema name: " << schema_name;
    if (authorization_identifier) {
       ss << std::endl
          << "   authorization identifier: " << authorization_identifier;
    }
    if (default_charset) {
       ss << std::endl
          << "   default charset: " << default_charset;
    }
    ss << ">" << std::endl;

    return ss.str();
}

const std::string drop_schema::to_string() {
    std::stringstream ss;
    ss << "<statement: DROP SCHEMA" << std::endl
       << "   schema name: " << schema_name << std::endl;
    if (drop_behaviour == DROP_BEHAVIOUR_CASCADE) {
       ss << "   behaviour: CASCADE";
    } else {
       ss << "   behaviour: RESTRICT";
    }
    ss << ">" << std::endl;

    return ss.str();
}

const std::string create_table::to_string() {
    std::stringstream ss;
    ss << "<statement: CREATE TABLE" << std::endl
       << "    table name: " << table_name;
    if (table_type != TABLE_TYPE_NORMAL) {
        ss << std::endl << "    temporary: true (";
        if (table_type == TABLE_TYPE_TEMPORARY_GLOBAL) {
            ss << "global)";
        } else {
            ss << "local)";
        }
    }
    ss << std::endl << "    column definitions:";
    for (auto cdef_it = column_definitions.begin();
            cdef_it != column_definitions.end();
            cdef_it++) {
        ss << std::endl << "      " << *(*cdef_it);
    }
    if (constraints.size() > 0) {
        ss << std::endl << "    constraints:";
        for (auto constraint_it = constraints.begin();
             constraint_it != constraints.end();
             constraint_it++) {
            ss << std::endl << "      " << *(*constraint_it);
        }
    }
    ss << ">" << std::endl;

    return ss.str();
}

const std::string select_t::to_string() {
    std::stringstream ss;
    ss << "<statement: SELECT";
    if (distinct)
       ss << std::endl << "   distinct: true";
    ss << std::endl << "   selected columns: ";
    size_t x = 0;
    for (const derived_column_t& dc : selected_columns) {
        ss << std::endl << "     " << x++ << ": " << dc;
    }
    ss << std::endl << "   referenced tables: ";
    x = 0;
    for (const table_reference_t& tr : referenced_tables) {
        ss << std::endl << "     " << x++ << ": " << tr;
    }
    if (where_condition) {
        ss << std::endl << "   where conditions: ";
        ss << *where_condition;
    }
    ss << ">" << std::endl;

    return ss.str();
}

} // namespace sqltoast
