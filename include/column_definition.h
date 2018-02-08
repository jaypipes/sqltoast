/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_DEFINITION_H
#define SQLTOAST_COLUMN_DEFINITION_H

#include <memory>
#include <vector>
#include <ostream>

#include "data_type.h"
#include "identifier.h"

namespace sqltoast {

typedef enum default_type {
    DEFAULT_TYPE_LITERAL,
    DEFAULT_TYPE_USER,
    DEFAULT_TYPE_CURRENT_USER,
    DEFAULT_TYPE_SESSION_USER,
    DEFAULT_TYPE_SYSTEM_USER,
    DEFAULT_TYPE_CURRENT_DATE,
    DEFAULT_TYPE_CURRENT_TIME,
    DEFAULT_TYPE_CURRENT_TIMESTAMP,
    DEFAULT_TYPE_NULL
} default_type_t;

typedef struct default_descriptor {
    default_type_t type;
    // TODO(jaypipes): Maybe make this a std::unique_ptr<> to some concrete
    // struct representing a literal OR a datetime function type?
    lexeme_t lexeme;
    size_t precision;
    default_descriptor(default_type_t type, lexeme_t lexeme, size_t prec) :
        type(type),
        lexeme(lexeme),
        precision(prec)
    {}
} default_descriptor_t;

std::ostream& operator<< (std::ostream& out, const default_descriptor_t& column_def);

typedef enum column_constraint_type {
    COLUMN_CONSTRAINT_TYPE_NOT_NULL,
    COLUMN_CONSTRAINT_TYPE_UNIQUE,
    COLUMN_CONSTRAINT_TYPE_PRIMARY_KEY,
    COLUMN_CONSTRAINT_TYPE_REFERENCES,
    COLUMN_CONSTRAINT_TYPE_CHECK
} column_constraint_type_t;

typedef struct column_constraint {
    column_constraint_type_t type;
    std::unique_ptr<identifier_t> name;
    column_constraint(column_constraint_type_t type) :
        type(type)
    {}
} column_constraint_t;

std::ostream& operator<< (std::ostream& out, const column_constraint_t& constraint);

typedef enum references_match_type {
    REFERENCES_MATCH_TYPE_NONE,
    REFERENCES_MATCH_TYPE_FULL,
    REFERENCES_MATCH_TYPE_PARTIAL
} references_match_type_t;

typedef enum referential_action {
    REFERENTIAL_ACTION_NONE,
    REFERENTIAL_ACTION_CASCADE,
    REFERENTIAL_ACTION_SET_NULL,
    REFERENTIAL_ACTION_SET_DEFAULT
} referential_action_t;

typedef struct references_constraint : column_constraint_t {
    identifier_t table_name;
    references_match_type_t match_type;
    referential_action_t on_update;
    referential_action_t on_delete;
    std::vector<identifier_t> column_names;
    references_constraint(
            identifier_t& table_name,
            references_match_type_t match_type,
            referential_action_t on_update,
            referential_action_t on_delete) :
        column_constraint(COLUMN_CONSTRAINT_TYPE_REFERENCES),
        table_name(table_name),
        match_type(match_type),
        on_update(on_update),
        on_delete(on_delete)
    {}
} references_constraint_t;

std::ostream& operator<< (std::ostream& out, const references_constraint_t& constraint);

typedef struct column_definition {
    identifier_t id;
    std::unique_ptr<data_type_descriptor_t> data_type;
    std::unique_ptr<default_descriptor_t> default_descriptor;
    std::unique_ptr<identifier_t> collate;
    std::vector<std::unique_ptr<column_constraint_t>> constraints;
    column_definition(identifier_t& id) :
        id(id)
    {}
} column_definition_t;

std::ostream& operator<< (std::ostream& out, const column_definition_t& column_def);

} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_DEFINITION_H */
