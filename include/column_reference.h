/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_COLUMN_REFERENCE_H
#define SQLTOAST_COLUMN_REFERENCE_H

namespace sqltoast {

typedef struct derived_column {
    lexeme_t alias;
    std::unique_ptr<value_expression_t> value;
    // Specialized constructor for the "asterisk" projection
    derived_column(lexeme_t& asterisk) :
        value(std::move(std::make_unique<value_expression_t>(
                        VALUE_EXPRESSION_TYPE_LITERAL, asterisk)))
    {}
    derived_column(std::unique_ptr<value_expression_t>& ve) :
        value(std::move(ve))
    {}
    inline bool has_alias() const {
        return alias.start != parse_position_t(0);
    }
} derived_column_t;

std::ostream& operator<< (std::ostream& out, const derived_column_t& dc);

typedef struct grouping_column_reference {
    lexeme_t column;
    lexeme_t collation;
    grouping_column_reference(lexeme_t& column) :
        column(column)
    {}
    inline bool has_collation() const {
        return collation.start != parse_position_t(0);
    }
} grouping_column_reference_t;

std::ostream& operator<< (std::ostream& out, const grouping_column_reference_t& dc);

} // namespace sqltoast

#endif /* SQLTOAST_COLUMN_REFERENCE_H */
