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
    STATEMENT_TYPE_ALTER_TABLE,
    STATEMENT_TYPE_INSERT,
    STATEMENT_TYPE_INSERT_SELECT,
    STATEMENT_TYPE_DELETE,
    STATEMENT_TYPE_UPDATE,
    STATEMENT_TYPE_SELECT
} statement_type_t;

typedef struct statement {
    statement_type_t type;
    statement(statement_type_t type) : type(type)
    {}
} statement_t;

std::ostream& operator<< (std::ostream& out, const statement_t& stmt);

typedef struct create_schema_statement : statement_t {
    lexeme_t schema_name;
    // Optional elements
    lexeme_t authorization_identifier;
    lexeme_t default_charset;
    create_schema_statement(
            lexeme_t& schema_name,
            lexeme_t& auth_ident,
            lexeme_t& def_charset) :
        statement_t(STATEMENT_TYPE_CREATE_SCHEMA),
        schema_name(schema_name),
        authorization_identifier(auth_ident),
        default_charset(def_charset)
    {}
} create_schema_statement_t;

std::ostream& operator<< (std::ostream& out, const create_schema_statement_t& stmt);

typedef enum drop_behaviour {
    DROP_BEHAVIOUR_CASCADE,
    DROP_BEHAVIOUR_RESTRICT
} drop_behaviour_t;

typedef struct drop_schema_statement : statement_t {
    lexeme_t schema_name;
    drop_behaviour_t drop_behaviour;
    drop_schema_statement(lexeme_t& schema_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_SCHEMA),
        schema_name(schema_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_schema_statement_t;

std::ostream& operator<< (std::ostream& out, const drop_schema_statement_t& stmt);

typedef enum table_type {
    TABLE_TYPE_NORMAL,
    TABLE_TYPE_TEMPORARY_GLOBAL,
    TABLE_TYPE_TEMPORARY_LOCAL
} table_type_t;

typedef struct create_table_statement : statement_t {
    table_type_t table_type;
    lexeme_t table_name;
    std::vector<std::unique_ptr<column_definition_t>> column_definitions;
    std::vector<std::unique_ptr<constraint_t>> constraints;
    create_table_statement(
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
} create_table_statement_t;

std::ostream& operator<< (std::ostream& out, const create_table_statement_t& stmt);

typedef struct drop_table_statement : statement_t {
    lexeme_t table_name;
    drop_behaviour_t drop_behaviour;
    drop_table_statement(lexeme_t& table_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_TABLE),
        table_name(table_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_table_statement_t;

std::ostream& operator<< (std::ostream& out, const drop_table_statement_t& stmt);

typedef enum alter_table_action_type {
    ALTER_TABLE_ACTION_TYPE_ADD_COLUMN,
    ALTER_TABLE_ACTION_TYPE_ALTER_COLUMN,
    ALTER_TABLE_ACTION_TYPE_DROP_COLUMN,
    ALTER_TABLE_ACTION_TYPE_ADD_TABLE_CONSTRAINT,
    ALTER_TABLE_ACTION_TYPE_DROP_TABLE_CONSTRAINT
} alter_table_action_type_t;

typedef struct alter_table_action {
    alter_table_action_type_t type;
    alter_table_action(alter_table_action_type_t type) :
        type(type)
    {}
} alter_table_action_t;

std::ostream& operator<< (std::ostream& out, const alter_table_action_t& action);

typedef struct add_column_action : alter_table_action_t {
    std::unique_ptr<column_definition_t> column_definition;
    add_column_action(std::unique_ptr<column_definition_t>& column_def) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_ADD_COLUMN),
        column_definition(std::move(column_def))
    {}
} add_column_action_t;

std::ostream& operator<< (std::ostream& out, const add_column_action_t& action);

typedef enum alter_column_action_type {
    ALTER_COLUMN_ACTION_TYPE_SET_DEFAULT,
    ALTER_COLUMN_ACTION_TYPE_DROP_DEFAULT
} alter_column_action_type_t;

typedef struct alter_column_action : alter_table_action_t {
    alter_column_action_type_t alter_column_action_type;
    lexeme_t column_name;
    std::unique_ptr<default_descriptor_t> default_descriptor;
    alter_column_action(lexeme_t column_name) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_ALTER_COLUMN),
        alter_column_action_type(ALTER_COLUMN_ACTION_TYPE_DROP_DEFAULT),
        column_name(column_name)
    {}
    alter_column_action(
            lexeme_t column_name,
            std::unique_ptr<default_descriptor_t>& default_descriptor) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_ALTER_COLUMN),
        alter_column_action_type(ALTER_COLUMN_ACTION_TYPE_SET_DEFAULT),
        column_name(column_name),
        default_descriptor(std::move(default_descriptor))
    {}
} alter_column_action_t;

std::ostream& operator<< (std::ostream& out, const alter_column_action_t& action);

typedef struct alter_table_statement : statement_t {
    lexeme_t table_name;
    std::unique_ptr<alter_table_action_t> action;
    alter_table_statement(
            lexeme_t& table_name,
            std::unique_ptr<alter_table_action_t>& action) :
        statement_t(STATEMENT_TYPE_ALTER_TABLE),
        table_name(table_name),
        action(std::move(action))
    {}
} alter_table_statement_t;

std::ostream& operator<< (std::ostream& out, const alter_table_statement_t& stmt);

typedef struct select_statement : statement_t {
    bool distinct;
    std::vector<derived_column_t> selected_columns;
    std::vector<std::unique_ptr<table_reference_t>> referenced_tables;
    std::unique_ptr<search_condition_t> where_condition;
    std::vector<grouping_column_reference_t> group_by_columns;
    std::unique_ptr<search_condition_t> having_condition;
    select_statement(
            bool distinct,
            std::vector<derived_column_t>& selected_cols,
            std::vector<std::unique_ptr<table_reference_t>>& ref_tables,
            std::unique_ptr<search_condition_t>& where_cond,
            std::vector<grouping_column_reference_t>& group_by_cols,
            std::unique_ptr<search_condition_t>& having_cond) :
        statement_t(STATEMENT_TYPE_SELECT),
        distinct(distinct),
        selected_columns(std::move(selected_cols)),
        referenced_tables(std::move(ref_tables)),
        where_condition(std::move(where_cond)),
        group_by_columns(std::move(group_by_cols)),
        having_condition(std::move(having_cond))
    {}
} select_statement_t;

std::ostream& operator<< (std::ostream& out, const select_statement_t& stmt);

typedef struct insert_statement : statement_t {
    lexeme_t table_name;
    std::vector<lexeme_t> insert_columns;
    std::vector<std::unique_ptr<row_value_constructor_t>> insert_values;
    insert_statement(
            lexeme_t& table_name,
            std::vector<lexeme_t>& col_list,
            std::vector<std::unique_ptr<row_value_constructor_t>>& val_list) :
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
} insert_statement_t;

std::ostream& operator<< (std::ostream& out, const insert_statement_t& stmt);

typedef struct insert_select_statement : statement_t {
    lexeme_t table_name;
    std::vector<lexeme_t> insert_columns;
    // Guaranteed to always be static_castable to a select_t
    std::unique_ptr<statement_t> select;
    insert_select_statement(
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
} insert_select_statement_t;

std::ostream& operator<< (std::ostream& out, const insert_select_statement_t& stmt);

typedef struct delete_statement : statement_t {
    lexeme_t table_name;
    std::unique_ptr<search_condition_t> where_condition;
    delete_statement(lexeme_t& table_name, std::unique_ptr<search_condition_t>& where) :
        statement_t(STATEMENT_TYPE_DELETE),
        table_name(table_name),
        where_condition(std::move(where))
    {}
} delete_statement_t;

std::ostream& operator<< (std::ostream& out, const delete_statement_t& stmt);

typedef enum set_column_type {
    SET_COLUMN_TYPE_NULL,
    SET_COLUMN_TYPE_DEFAULT,
    SET_COLUMN_TYPE_VALUE_EXPRESSION
} set_column_type_t;

// A set_column_t is a struct representing the column to update in an UPDATE
// statement along with the value to set the column to
typedef struct set_column {
    set_column_type_t type;
    lexeme_t column_name;
    std::unique_ptr<value_expression_t> value;
    set_column(set_column_type_t type, lexeme_t column_name) :
        type(type),
        column_name(column_name)
    {}
    set_column(
            lexeme_t column_name,
            std::unique_ptr<value_expression_t>& value) :
        type(SET_COLUMN_TYPE_VALUE_EXPRESSION),
        column_name(column_name),
        value(std::move(value))
    {}
} set_column_t;

typedef struct update_statement : statement_t {
    lexeme_t table_name;
    std::vector<set_column_t> set_columns;
    std::unique_ptr<search_condition_t> where_condition;
    update_statement(
            lexeme_t& table_name,
            std::vector<set_column_t>& set_columns) :
        statement_t(STATEMENT_TYPE_UPDATE),
        table_name(table_name),
        set_columns(std::move(set_columns))
    {}
    update_statement(
            lexeme_t& table_name,
            std::vector<set_column_t>& set_columns,
            std::unique_ptr<search_condition_t>& where_cond) :
        statement_t(STATEMENT_TYPE_UPDATE),
        table_name(table_name),
        set_columns(std::move(set_columns)),
        where_condition(std::move(where_cond))
    {}
} update_statement_t;

std::ostream& operator<< (std::ostream& out, const update_statement_t& stmt);

} // namespace sqltoast

#endif /* SQLTOAST_STATEMENT_H */
