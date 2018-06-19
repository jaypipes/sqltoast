/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_TABLE_REFERENCE_H
#define SQLTOAST_TABLE_REFERENCE_H

namespace sqltoast {

// A correlation specification is the parse element that indicates a table
// reference's alias (technically called a "correlation name") and the optional
// list of correlated column names.
typedef struct correlation_spec {
    lexeme_t alias;
    std::vector<lexeme_t> columns;
    correlation_spec(lexeme_t& alias) :
        alias(alias)
    {}
    correlation_spec(
            lexeme_t& alias,
            std::vector<lexeme_t>& columns) :
        alias(alias),
        columns(std::move(columns))
    {}
} correlation_spec_t;

typedef enum join_type {
    JOIN_TYPE_NONE,
    JOIN_TYPE_CROSS,
    JOIN_TYPE_INNER,
    JOIN_TYPE_LEFT,
    JOIN_TYPE_RIGHT,
    JOIN_TYPE_FULL,
    JOIN_TYPE_NATURAL,
    JOIN_TYPE_UNION
} join_type_t;

typedef struct join_specification {
    std::unique_ptr<search_condition_t> condition;
    std::vector<lexeme_t> named_columns;
    join_specification()
    {}
    join_specification(std::unique_ptr<search_condition_t>& join_cond) :
        condition(std::move(join_cond))
    {}
    join_specification(std::vector<lexeme_t>& named_columns) :
        named_columns(std::move(named_columns))
    {}
} join_specification_t;

typedef struct join_target {
    join_type_t join_type;
    std::unique_ptr<struct table_reference> table_ref;
    std::unique_ptr<join_specification_t> join_spec;
    join_target(
            join_type_t join_type,
            std::unique_ptr<struct table_reference>& table_ref,
            std::unique_ptr<join_specification_t>& join_spec) :
        join_type(join_type),
        table_ref(std::move(table_ref)),
        join_spec(std::move(join_spec))
    {}
    join_target(
            join_type_t join_type,
            std::unique_ptr<struct table_reference>& table_ref) :
        join_type(join_type),
        table_ref(std::move(table_ref))
    {}
} join_target_t;

// A table reference is an object whose columns are referenced in the <select
// list>, <group by>, <having>, <where> or <join condition> clauses of a
// SEELECT statement. There are two types of table references. A table is
// simply that: a table in the database that has been directly referenced in
// the FROM clause. And a derived table is a subquery in the FROM clause.
// Either type of table reference may be joined to another table reference.

typedef enum table_reference_type_t {
    TABLE_REFERENCE_TYPE_TABLE,
    TABLE_REFERENCE_TYPE_DERIVED_TABLE
} table_reference_type_t;

typedef struct table_reference {
    table_reference_type_t type;
    std::unique_ptr<join_target_t> joined;
    table_reference(table_reference_type_t type) :
        type(type)
    {}
    // Joins a supplied table reference to this one
    inline void join(std::unique_ptr<join_target_t>& target) {
        joined = std::move(target);
    }
} table_reference_t;

typedef struct table : table_reference_t {
    lexeme_t table_name;
    std::unique_ptr<correlation_spec_t> correlation_spec;
    table(lexeme_t& table_name, lexeme_t& alias) :
        table_reference_t(TABLE_REFERENCE_TYPE_TABLE),
        table_name(table_name),
        correlation_spec(std::make_unique<correlation_spec_t>(alias))
    {}
    inline bool has_alias() const {
        if (correlation_spec)
            return correlation_spec->alias;
        return false;
    }
} table_t;

struct query_expression;
typedef struct derived_table : table_reference_t {
    correlation_spec_t correlation_spec;
    std::unique_ptr<struct query_expression> query;
    derived_table(
            lexeme_t& table_name,
            std::unique_ptr<struct query_expression>& query) :
        table_reference_t(TABLE_REFERENCE_TYPE_DERIVED_TABLE),
        correlation_spec(table_name),
        query(std::move(query))
    {}
} derived_table_t;

} // namespace sqltoast

#endif /* SQLTOAST_TABLE_REFERENCE_H */
