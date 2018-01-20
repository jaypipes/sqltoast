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

typedef enum token_type {
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_LITERAL,
    TOKEN_TYPE_PUNCTUATOR,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_COMMENT
} token_type_t;

typedef struct token {
    token_type_t type;
    symbol_t symbol;
    parse_position_t start;
    parse_position_t end;
    token(
        token_type_t type,
        symbol_t symbol,
        parse_position_t start,
        parse_position_t end) :
        type(type), symbol(symbol), start(start), end(end)
    {}
} token_t;

std::ostream& operator<< (std::ostream& out, const token_t& token);

struct token_type_map {
    typedef std::map<token_type_t, std::string> tt_map_t;
    static tt_map_t m;
    // Given a token_type, returns the string representation of the token_type
    static const std::string& to_string(token_type_t type) {
        return token_type_map::m[type];
    }
};

token_type_map::tt_map_t _init_token_type_map();

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_TOKEN_H */
