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
    table_reference(table_reference_type_t type) :
        type(type)
    {}
} table_reference_t;

std::ostream& operator<< (std::ostream& out, const table_reference_t& tr);

typedef struct table : table_reference_t {
    lexeme_t table_name;
    lexeme_t alias;
    table(lexeme_t& table_name, lexeme_t& alias) :
        table_reference_t(TABLE_REFERENCE_TYPE_TABLE),
        table_name(table_name),
        alias(alias)
    {}
    inline bool has_alias() const {
        return alias.start != parse_position_t(0);
    }
} table_t;

std::ostream& operator<< (std::ostream& out, const table_t& t);

typedef struct derived_table : table_reference_t {
    lexeme_t table_name;
    // Will always be static_castable to select_statement_t
    std::unique_ptr<statement_t> subquery;
    derived_table(
            lexeme_t& table_name,
            std::unique_ptr<statement_t>& subquery) :
        table_reference_t(TABLE_REFERENCE_TYPE_DERIVED_TABLE),
        table_name(table_name),
        subquery(std::move(subquery))
    {}
} derived_table_t;

std::ostream& operator<< (std::ostream& out, const derived_table_t& dt);

typedef enum join_type {
    JOIN_TYPE_UNKNOWN,
    JOIN_TYPE_CROSS,
    JOIN_TYPE_INNER,
    JOIN_TYPE_LEFT_OUTER,
    JOIN_TYPE_RIGHT_OUTER,
    JOIN_TYPE_FULL_OUTER,
    JOIN_TYPE_UNION
} join_type_t;

typedef struct join_specification {
    std::unique_ptr<search_condition_t> condition;
    // std::vector<lexeme_t> named_columns;`
    join_specification()
    {}
    join_specification(std::unique_ptr<search_condition_t>& join_cond) :
        condition(std::move(join_cond))
    {}
} join_specification_t;

std::ostream& operator<< (std::ostream& out, const join_specification_t& js);

typedef struct joined_table : table_reference_t {
    join_type_t join_type;
    std::unique_ptr<table_reference_t> left;
    std::unique_ptr<table_reference_t> right;
    std::unique_ptr<join_specification_t> spec;
    joined_table(
            std::unique_ptr<table_reference_t>& left,
            std::unique_ptr<table_reference_t>& right) :
        table_reference_t(TABLE_REFERENCE_TYPE_JOINED_TABLE),
        join_type(JOIN_TYPE_CROSS),
        left(std::move(left)),
        right(std::move(right))
    {}
    joined_table(
            join_type_t join_type,
            std::unique_ptr<table_reference_t>& left,
            std::unique_ptr<table_reference_t>& right,
            std::unique_ptr<search_condition_t>& join_cond) :
        table_reference_t(TABLE_REFERENCE_TYPE_JOINED_TABLE),
        join_type(join_type),
        left(std::move(left)),
        right(std::move(right)),
        spec(std::make_unique<join_specification_t>(join_cond))
    {}
} joined_table_t;

std::ostream& operator<< (std::ostream& out, const joined_table_t& jt);

} // namespace sqltoast

#endif /* SQLTOAST_TABLE_REFERENCE_H */
