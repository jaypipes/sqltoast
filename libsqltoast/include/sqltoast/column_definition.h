/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_DEFINITION_H
#define SQLTOAST_COLUMN_DEFINITION_H

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
    lexeme_t lexeme;
    size_t precision;
    default_descriptor(default_type_t type, lexeme_t lexeme, size_t prec) :
        type(type),
        lexeme(lexeme),
        precision(prec)
    {}
} default_descriptor_t;

typedef struct column_definition {
    lexeme_t name;
    std::unique_ptr<data_type_descriptor_t> data_type;
    std::unique_ptr<default_descriptor_t> default_descriptor;
    std::vector<std::unique_ptr<constraint_t>> constraints;
    lexeme_t collate;
    column_definition(
            lexeme_t& name,
            std::unique_ptr<data_type_descriptor_t>& data_type,
            std::unique_ptr<default_descriptor_t>& default_descriptor,
            std::vector<std::unique_ptr<constraint_t>>& constraints) :
        name(name),
        data_type(std::move(data_type)),
        default_descriptor(std::move(default_descriptor)),
        constraints(std::move(constraints))
    {}
} column_definition_t;

} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_DEFINITION_H */
