/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_CONSTRAINT_H
#define SQLTOAST_CONSTRAINT_H

namespace sqltoast {

typedef enum constraint_type {
    CONSTRAINT_TYPE_UNKNOWN,
    CONSTRAINT_TYPE_NOT_NULL,
    CONSTRAINT_TYPE_UNIQUE,
    CONSTRAINT_TYPE_PRIMARY_KEY,
    CONSTRAINT_TYPE_FOREIGN_KEY,
    CONSTRAINT_TYPE_CHECK
} constraint_type_t;

typedef struct constraint {
    constraint_type_t type;
    lexeme_t name;
    std::vector<lexeme_t> columns;
    constraint(constraint_type_t type) :
        type(type)
    {}
} constraint_t;

typedef struct not_null_constraint : constraint_t {
    not_null_constraint() :
        constraint_t(CONSTRAINT_TYPE_NOT_NULL)
    {}
} not_null_constraint_t;

typedef struct unique_constraint : constraint_t {
    unique_constraint(bool is_primary) :
        constraint_t(is_primary ? CONSTRAINT_TYPE_PRIMARY_KEY : CONSTRAINT_TYPE_UNIQUE)
    {}
} unique_constraint_t;

typedef enum match_type {
    MATCH_TYPE_NONE,
    MATCH_TYPE_FULL,
    MATCH_TYPE_PARTIAL
} match_type_t;

typedef enum referential_action {
    REFERENTIAL_ACTION_NONE,
    REFERENTIAL_ACTION_CASCADE,
    REFERENTIAL_ACTION_SET_NULL,
    REFERENTIAL_ACTION_SET_DEFAULT
} referential_action_t;

typedef struct foreign_key_constraint : constraint_t {
    lexeme_t referenced_table;
    std::vector<lexeme_t> referenced_columns;
    match_type_t match_type;
    referential_action_t on_update;
    referential_action_t on_delete;
    foreign_key_constraint(
            lexeme_t& ref_table,
            std::vector<lexeme_t>& referenced_cols,
            match_type_t match_type,
            referential_action_t on_update,
            referential_action_t on_delete) :
        constraint(CONSTRAINT_TYPE_FOREIGN_KEY),
        referenced_table(ref_table),
        referenced_columns(std::move(referenced_cols)),
        match_type(match_type),
        on_update(on_update),
        on_delete(on_delete)
    {}
} foreign_key_constraint_t;

} // namespace sqltoast

#endif /* SQLTOAST_CONSTRAINT_H */
