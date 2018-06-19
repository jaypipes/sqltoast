/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_QUERY_H
#define SQLTOAST_QUERY_H

namespace sqltoast {

// A table expression describes the tables involved in a query expression along
// with filtering, grouping and aggregate expressions on those tables

typedef struct table_expression {
    std::vector<std::unique_ptr<table_reference_t>> referenced_tables;
    std::unique_ptr<search_condition_t> where_condition;
    std::vector<grouping_column_reference_t> group_by_columns;
    std::unique_ptr<search_condition_t> having_condition;
    table_expression(
            std::vector<std::unique_ptr<table_reference_t>>& ref_tables,
            std::unique_ptr<search_condition_t>& where_cond,
            std::vector<grouping_column_reference_t>& group_by_cols,
            std::unique_ptr<search_condition_t>& having_cond) :
        referenced_tables(std::move(ref_tables)),
        where_condition(std::move(where_cond)),
        group_by_columns(std::move(group_by_cols)),
        having_condition(std::move(having_cond))
    {}
} table_expression_t;

typedef struct query_specification {
    bool distinct;
    std::vector<derived_column_t> selected_columns;
    std::unique_ptr<table_expression_t> table_expression;
    query_specification(
            bool distinct,
            std::vector<derived_column_t>& selected_cols,
            std::unique_ptr<table_expression_t>& table_expression) :
        distinct(distinct),
        selected_columns(std::move(selected_cols)),
        table_expression(std::move(table_expression))
    {}
} query_specification_t;

typedef enum query_expression_type_t {
    QUERY_EXPRESSION_TYPE_NON_JOIN_QUERY_EXPRESSION,
    QUERY_EXPRESSION_TYPE_JOINED_TABLE
} query_expression_type_t;

// A query expression produces a table-like selection of rows.

typedef struct query_expression {
    query_expression_type_t query_expression_type;
    query_expression(query_expression_type_t qe_type) :
        query_expression_type(qe_type)
    {}
} query_expression_t;

typedef enum non_join_query_primary_type {
    NON_JOIN_QUERY_PRIMARY_TYPE_QUERY_SPECIFICATION,
    NON_JOIN_QUERY_PRIMARY_TYPE_TABLE_VALUE_CONSTRUCTOR,
    NON_JOIN_QUERY_PRIMARY_TYPE_EXPLICIT_TABLE,
    NON_JOIN_QUERY_PRIMARY_TYPE_SUBEXPRESSION
} non_join_query_primary_type_t;

typedef struct non_join_query_primary {
    non_join_query_primary_type primary_type;
    non_join_query_primary(
            non_join_query_primary_type_t primary_type) :
        primary_type(primary_type)
    {}
} non_join_query_primary_t;

typedef struct query_specification_non_join_query_primary : non_join_query_primary_t {
    std::unique_ptr<query_specification_t> query_spec;
    query_specification_non_join_query_primary(
            std::unique_ptr<query_specification_t>& query_spec) :
        non_join_query_primary_t(NON_JOIN_QUERY_PRIMARY_TYPE_QUERY_SPECIFICATION),
        query_spec(std::move(query_spec))
    {}
} query_specification_non_join_query_primary_t;

typedef struct table_value_constructor {
    std::vector<std::unique_ptr<row_value_constructor_t>> values;
    table_value_constructor(
            std::vector<std::unique_ptr<row_value_constructor_t>>& values) :
        values(std::move(values))
    {}
} table_value_constructor_t;

typedef struct table_value_constructor_non_join_query_primary : non_join_query_primary_t {
    std::unique_ptr<table_value_constructor_t> table_value;
    table_value_constructor_non_join_query_primary(
            std::unique_ptr<table_value_constructor_t>& table_value) :
        non_join_query_primary_t(NON_JOIN_QUERY_PRIMARY_TYPE_TABLE_VALUE_CONSTRUCTOR),
        table_value(std::move(table_value))
    {}
} table_value_constructor_non_join_query_primary_t;

typedef struct non_join_query_term {
    std::unique_ptr<non_join_query_primary_t> primary;
    non_join_query_term(
            std::unique_ptr<non_join_query_primary_t>& primary) :
        primary(std::move(primary))
    {}
} non_join_query_term_t;

typedef struct non_join_query_expression : query_expression_t {
    std::unique_ptr<non_join_query_term_t> term;
    non_join_query_expression(
            std::unique_ptr<non_join_query_term_t>& term) :
        query_expression_t(QUERY_EXPRESSION_TYPE_NON_JOIN_QUERY_EXPRESSION),
        term(std::move(term))
    {}
} non_join_query_expression_t;

typedef struct joined_table_query_expression : query_expression_t {
    std::unique_ptr<table_reference_t> joined_table;
    joined_table_query_expression(
            std::unique_ptr<table_reference_t>& joined_table) :
        query_expression_t(QUERY_EXPRESSION_TYPE_JOINED_TABLE),
        joined_table(std::move(joined_table))
    {}
} joined_table_query_expression_t;

} // namespace sqltoast

#endif /* SQLTOAST_QUERY_H */
