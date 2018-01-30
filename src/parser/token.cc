/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "token.h"

namespace sqltoast {

token_type_map::tt_map_t  _init_token_type_map() {
    token_type_map::tt_map_t m;

    m[TOKEN_TYPE_LITERAL] = std::string("<literal>");
    m[TOKEN_TYPE_PUNCTUATOR] = std::string("<punctuator>");
    m[TOKEN_TYPE_IDENTIFIER] = std::string("<identifier>");
    m[TOKEN_TYPE_COMMENT] = std::string("<comment>");

    return m;
}

token_type_map::tt_map_t token_type_map::m = _init_token_type_map();

std::ostream& operator<< (std::ostream& out, const token_t& token) {
    if (token.is_keyword()) {
        out << "keyword[" << symbol_map::to_string(token.symbol) << "]";
        return out;
    }
    token_type_t tt = token.type;
    switch (tt) {
        case TOKEN_TYPE_PUNCTUATOR:
            out << symbol_map::to_string(token.symbol);
            break;
        case TOKEN_TYPE_IDENTIFIER:
            {
                size_t len = (token.end - token.start);
                out << "identifier[length: " << len << "]";
            }
            break;
        case TOKEN_TYPE_COMMENT:
            {
                size_t len = (token.end - token.start);
                out << "comment[length: " << len << "]";
            }
            break;
        case TOKEN_TYPE_LITERAL:
            {
                // TODO(jaypipes): Add typing of literal...
                size_t len = (token.end - token.start);
                out << "literal[length: " << len << "]";
            }
            break;
        default:
            out << "token[type: " << token_type_map::to_string(token.type)
                << " symbol: " << symbol_map::to_string(token.symbol) << "]";
    }
    return out;
}

} // namespace sqltoast
