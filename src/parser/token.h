/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_TOKEN_H
#define SQLTOAST_PARSER_TOKEN_H

#include <ostream>
#include <map>
#include <string>
#include <vector>

#include "symbol.h"

namespace sqltoast {

typedef std::vector<char>::const_iterator parse_position_t;

typedef struct token {
    symbol_t symbol;
    parse_position_t start;
    parse_position_t end;
    token(
        symbol_t symbol,
        parse_position_t start,
        parse_position_t end) :
        symbol(symbol), start(start), end(end)
    {}
    inline bool is_literal() const {
        return (symbol >= SYMBOL_LITERAL_UNSIGNED_INTEGER && symbol <= SYMBOL_LITERAL_APPROXIMATE_NUMBER);
    }
    inline bool is_punctuator() const {
        return (symbol >= SYMBOL_SEMICOLON && symbol <= SYMBOL_RPAREN);
    }
    inline bool is_keyword() const {
        return (symbol >= SYMBOL_AUTHORIZATION && symbol <= SYMBOL_VARYING);
    }
    inline bool is_identifier() const {
        return (symbol == SYMBOL_IDENTIFIER);
    }
} token_t;

std::ostream& operator<< (std::ostream& out, const token_t& token);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_TOKEN_H */
