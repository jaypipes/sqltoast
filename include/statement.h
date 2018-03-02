/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENT_H
#define SQLTOAST_STATEMENT_H

namespace sqltoast {

typedef enum statement_type {
    STATEMENT_TYPE_CREATE_SCHEMA,
    STATEMENT_TYPE_CREATE_TABLE,
    STATEMENT_TYPE_DROP_SCHEMA,
    STATEMENT_TYPE_DROP_TABLE,
    STATEMENT_TYPE_INSERT,
    STATEMENT_TYPE_INSERT_SELECT,
    STATEMENT_TYPE_SELECT
} statement_type_t;

typedef struct statement {
    statement_type_t type;
    statement(statement_type_t type) : type(type)
    {}
} statement_t;

std::ostream& operator<< (std::ostream& out, const statement_t& stmt);

typedef struct create_schema : statement_t {
    lexeme_t schema_name;
    // Optional elements
    lexeme_t authorization_identifier;
    lexeme_t default_charset;
    create_schema(
            lexeme_t& schema_name,
            lexeme_t& auth_ident,
            lexeme_t& def_charset) :
        statement_t(STATEMENT_TYPE_CREATE_SCHEMA),
        schema_name(schema_name),
        authorization_identifier(auth_ident),
        default_charset(def_charset)
    {}
} create_schema_t;

std::ostream& operator<< (std::ostream& out, const create_schema_t& stmt);

typedef enum drop_behaviour {
    DROP_BEHAVIOUR_CASCADE,
    DROP_BEHAVIOUR_RESTRICT
} drop_behaviour_t;

typedef struct drop_schema : statement_t {
    lexeme_t schema_name;
    drop_behaviour_t drop_behaviour;
    drop_schema(lexeme_t& schema_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_SCHEMA),
        schema_name(schema_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_schema_t;

std::ostream& operator<< (std::ostream& out, const drop_schema_t& stmt);

typedef enum table_type {
    TABLE_TYPE_NORMAL,
    TABLE_TYPE_TEMPORARY_GLOBAL,
    TABLE_TYPE_TEMPORARY_LOCAL
} table_type_t;

typedef struct create_table : statement_t {
    table_type_t table_type;
    lexeme_t table_name;
    std::vector<std::unique_ptr<column_definition_t>> column_definitions;
    std::vector<std::unique_ptr<constraint_t>> constraints;
    create_table(
            table_type_t table_type,
            lexeme_t& table_name,
            std::vector<std::unique_ptr<column_definition_t>>& column_defs,
            std::vector<std::unique_ptr<constraint_t>>& constraints) :
        statement_t(STATEMENT_TYPE_CREATE_TABLE),
        table_type(table_type),
        table_name(table_name),
        column_definitions(std::move(column_defs)),
        constraints(std::move(constraints))
    {}
} create_table_t;

std::ostream& operator<< (std::ostream& out, const create_table_t& stmt);

typedef struct drop_table : statement_t {
    lexeme_t table_name;
    drop_behaviour_t drop_behaviour;
    drop_table(lexeme_t& table_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_TABLE),
        table_name(table_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_table_t;

std::ostream& operator<< (std::ostream& out, const drop_table_t& stmt);

typedef struct select : statement_t {
    bool distinct;
    std::vector<derived_column_t> selected_columns;
    std::vector<table_reference_t> referenced_tables;
    std::unique_ptr<search_condition_t> where_condition;
    select(
            bool distinct,
            std::vector<derived_column_t>& selected_cols,
            std::vector<table_reference_t>& ref_tables,
            std::unique_ptr<search_condition_t>& where_cond) :
        statement_t(STATEMENT_TYPE_SELECT),
        distinct(distinct),
        selected_columns(std::move(selected_cols)),
        referenced_tables(std::move(ref_tables)),
        where_condition(std::move(where_cond))
    {}
} select_t;

std::ostream& operator<< (std::ostream& out, const select_t& stmt);

typedef struct insert : statement_t {
    lexeme_t table_name;
    std::vector<lexeme_t> insert_columns;
    std::vector<lexeme_t> insert_values;
    insert(
            lexeme_t& table_name,
            std::vector<lexeme_t>& col_list,
            std::vector<lexeme_t>& val_list) :
        statement_t(STATEMENT_TYPE_INSERT),
        table_name(table_name),
        insert_columns(std::move(col_list)),
        insert_values(std::move(val_list))
    {}
    inline bool use_default_columns() const {
        return insert_columns.empty();
    }
    inline bool use_default_values() const {
        return insert_values.empty();
    }
} insert_t;

std::ostream& operator<< (std::ostream& out, const insert_t& stmt);

typedef struct insert_select : statement_t {
    lexeme_t table_name;
    std::vector<lexeme_t> insert_columns;
    // Guaranteed to always be static_castable to a select_t
    std::unique_ptr<statement_t> select;
    insert_select(
            lexeme_t& table_name,
            std::vector<lexeme_t>& col_list,
            std::unique_ptr<statement_t>& select) :
        statement_t(STATEMENT_TYPE_INSERT_SELECT),
        table_name(table_name),
        insert_columns(std::move(col_list)),
        select(std::move(select))
    {}
    inline bool use_default_columns() const {
        return insert_columns.empty();
    }
} insert_select_t;

std::ostream& operator<< (std::ostream& out, const insert_select_t& stmt);

} // namespace sqltoast

#endif /* SQLTOAST_STATEMENT_H */
