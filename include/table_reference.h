/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_TABLE_REFERENCE_H
#define SQLTOAST_TABLE_REFERENCE_H

namespace sqltoast {

// A table reference is an object whose columns are referenced in the <select
// list>, <group by>, <having>, <where> or <join condition> clauses of a
// SEELECT statement. There are three types of table references. A table is
// simply that: a table in the database that has been directly referenced in
// the FROM clause. A joined table is a table reference that has been referred
// to via a JOIN expression. And a derived table is a subquery in the FROM
// clause.

typedef enum table_reference_type_t {
    TABLE_REFERENCE_TYPE_TABLE,
    TABLE_REFERENCE_TYPE_DERIVED_TABLE,
    TABLE_REFERENCE_TYPE_JOINED_TABLE
} table_reference_type_t;

typedef struct table_reference {
    table_reference_type_t type;
    lexeme_t alias;
    table_reference(table_reference_type_t type, lexeme_t& alias) :
        type(type), alias(alias)
    {}
    inline bool has_alias() const {
        return alias.start != parse_position_t(0);
    }
} table_reference_t;

std::ostream& operator<< (std::ostream& out, const table_reference_t& tr);

typedef struct table : table_reference_t {
    lexeme_t table_name;
    table(lexeme_t& table_name, lexeme_t& alias) :
        table_reference_t(TABLE_REFERENCE_TYPE_TABLE, alias),
        table_name(table_name)
    {}
} table_t;

std::ostream& operator<< (std::ostream& out, const table_t& t);

} // namespace sqltoast

#endif /* SQLTOAST_TABLE_REFERENCE_H */
