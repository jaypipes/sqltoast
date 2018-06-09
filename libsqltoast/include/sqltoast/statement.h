/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_STATEMENT_H
#define SQLTOAST_STATEMENT_H

namespace sqltoast {

typedef enum statement_type {
    STATEMENT_TYPE_ALTER_TABLE,
    STATEMENT_TYPE_COMMIT,
    STATEMENT_TYPE_CREATE_SCHEMA,
    STATEMENT_TYPE_CREATE_TABLE,
    STATEMENT_TYPE_CREATE_VIEW,
    STATEMENT_TYPE_DELETE,
    STATEMENT_TYPE_DROP_SCHEMA,
    STATEMENT_TYPE_DROP_TABLE,
    STATEMENT_TYPE_DROP_VIEW,
    STATEMENT_TYPE_GRANT,
    STATEMENT_TYPE_INSERT,
    STATEMENT_TYPE_ROLLBACK,
    STATEMENT_TYPE_SELECT,
    STATEMENT_TYPE_UPDATE
} statement_type_t;

typedef struct statement {
    statement_type_t type;
    statement(statement_type_t type) : type(type)
    {}
} statement_t;

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

typedef struct drop_table_statement : statement_t {
    lexeme_t table_name;
    drop_behaviour_t drop_behaviour;
    drop_table_statement(lexeme_t& table_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_TABLE),
        table_name(table_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_table_statement_t;

typedef enum alter_table_action_type {
    ALTER_TABLE_ACTION_TYPE_ADD_COLUMN,
    ALTER_TABLE_ACTION_TYPE_ALTER_COLUMN,
    ALTER_TABLE_ACTION_TYPE_DROP_COLUMN,
    ALTER_TABLE_ACTION_TYPE_ADD_CONSTRAINT,
    ALTER_TABLE_ACTION_TYPE_DROP_CONSTRAINT
} alter_table_action_type_t;

typedef struct alter_table_action {
    alter_table_action_type_t type;
    alter_table_action(alter_table_action_type_t type) :
        type(type)
    {}
} alter_table_action_t;

typedef struct add_column_action : alter_table_action_t {
    std::unique_ptr<column_definition_t> column_definition;
    add_column_action(std::unique_ptr<column_definition_t>& column_def) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_ADD_COLUMN),
        column_definition(std::move(column_def))
    {}
} add_column_action_t;

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

typedef struct drop_column_action : alter_table_action_t {
    lexeme_t column_name;
    drop_behaviour_t drop_behaviour;
    drop_column_action(
            lexeme_t column_name,
            drop_behaviour_t drop_behaviour) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_DROP_COLUMN),
        column_name(column_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_column_action_t;

typedef struct add_constraint_action : alter_table_action_t {
    std::unique_ptr<constraint_t> constraint;
    add_constraint_action(std::unique_ptr<constraint_t>& constraint) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_ADD_CONSTRAINT),
        constraint(std::move(constraint))
    {}
} add_constraint_action_t;

typedef struct drop_constraint_action : alter_table_action_t {
    lexeme_t constraint_name;
    drop_behaviour_t drop_behaviour;
    drop_constraint_action(
            lexeme_t constraint_name,
            drop_behaviour_t drop_behaviour) :
        alter_table_action_t(ALTER_TABLE_ACTION_TYPE_DROP_CONSTRAINT),
        constraint_name(constraint_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_constraint_action_t;

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

typedef struct select_statement : statement_t {
    std::unique_ptr<query_specification_t> query;
    select_statement(
            std::unique_ptr<query_specification_t>& query) :
        statement_t(STATEMENT_TYPE_SELECT),
        query(std::move(query))
    {}
} select_statement_t;

typedef struct insert_statement : statement_t {
    lexeme_t table_name;
    std::vector<lexeme_t> insert_columns;
    std::unique_ptr<query_expression_t> query;
    insert_statement(
            lexeme_t& table_name,
            std::vector<lexeme_t>& col_list,
            std::unique_ptr<query_expression_t>& query) :
        statement_t(STATEMENT_TYPE_INSERT),
        table_name(table_name),
        insert_columns(std::move(col_list)),
        query(std::move(query))
    {}
} insert_statement_t;

typedef struct delete_statement : statement_t {
    lexeme_t table_name;
    std::unique_ptr<search_condition_t> where_condition;
    delete_statement(lexeme_t& table_name, std::unique_ptr<search_condition_t>& where) :
        statement_t(STATEMENT_TYPE_DELETE),
        table_name(table_name),
        where_condition(std::move(where))
    {}
} delete_statement_t;

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

typedef enum check_option {
    CHECK_OPTION_NONE,
    CHECK_OPTION_LOCAL,
    CHECK_OPTION_CASCADED
} check_option_t;

typedef struct create_view_statement : statement_t {
    lexeme_t table_name;
    check_option_t check_option;
    std::vector<lexeme_t> columns;
    std::unique_ptr<query_expression_t> query;
    create_view_statement(
            lexeme_t& table_name,
            check_option_t check_option,
            std::vector<lexeme_t>& columns,
            std::unique_ptr<query_expression_t>& query) :
        statement_t(STATEMENT_TYPE_CREATE_VIEW),
        table_name(table_name),
        check_option(check_option),
        columns(std::move(columns)),
        query(std::move(query))
    {}
} create_view_statement_t;

typedef struct drop_view_statement : statement_t {
    lexeme_t table_name;
    drop_behaviour_t drop_behaviour;
    drop_view_statement(lexeme_t& table_name, drop_behaviour_t drop_behaviour) :
        statement_t(STATEMENT_TYPE_DROP_VIEW),
        table_name(table_name),
        drop_behaviour(drop_behaviour)
    {}
} drop_view_statement_t;

typedef enum grant_action_type {
    GRANT_ACTION_TYPE_SELECT,
    GRANT_ACTION_TYPE_DELETE,
    GRANT_ACTION_TYPE_INSERT,
    GRANT_ACTION_TYPE_UPDATE,
    GRANT_ACTION_TYPE_REFERENCES,
    GRANT_ACTION_TYPE_USAGE
} grant_action_type_t;

typedef struct grant_action {
    grant_action_type_t type;
    grant_action(grant_action_type_t type) :
        type(type)
    {}
} grant_action_t;

typedef struct column_list_grant_action : grant_action_t {
    std::vector<lexeme_t> columns;
    column_list_grant_action(
            grant_action_type_t type,
            std::vector<lexeme_t>& columns) :
        grant_action_t(type),
        columns(std::move(columns))
    {}
} column_list_grant_action_t;

typedef enum grant_object_type {
    GRANT_OBJECT_TYPE_TABLE,
    GRANT_OBJECT_TYPE_DOMAIN,
    GRANT_OBJECT_TYPE_COLLATION,
    GRANT_OBJECT_TYPE_CHARACTER_SET,
    GRANT_OBJECT_TYPE_TRANSLATION
} grant_object_type_t;

typedef struct grant_statement : statement_t {
    grant_object_type_t object_type;
    lexeme_t on;
    lexeme_t to;
    bool with_grant_option;
    std::vector<std::unique_ptr<grant_action_t>> privileges;
    grant_statement(
            grant_object_type_t object_type,
            lexeme_t& on,
            lexeme_t& to,
            bool with_grant_option,
            std::vector<std::unique_ptr<grant_action_t>>& privileges) :
        statement_t(STATEMENT_TYPE_GRANT),
        object_type(object_type),
        on(on),
        to(to),
        with_grant_option(with_grant_option),
        privileges(std::move(privileges))
    {}
    inline bool to_public() const {
        // When TO PUBLIC is specified, the "to" lexeme_t argument in the
        // constructor is false-valued
        return to == false;
    }
    inline bool all_privileges() const {
        // When ALL PRIVILEGES is specified, we don't add any actions to the
        // vector of privileges, so that's empty, indicating all privileges
        return privileges.empty();
    }
} grant_statement_t;

} // namespace sqltoast

#endif /* SQLTOAST_STATEMENT_H */
