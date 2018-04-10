/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_IDENTIFIER_H
#define SQLTOAST_IDENTIFIER_H

namespace sqltoast {

typedef struct identifier {
    const std::string name;
    identifier(lexeme_t& lexeme) : name(lexeme.start, lexeme.end)
    {}
} identifier_t;

} // namespace sqltoast

#endif /* SQLTOAST_IDENTIFIER_H */
