/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "token.h"

namespace sqltoast {

token_type_map::tt_map_t  _init_token_type_map() {
    token_type_map::tt_map_t m;

    m[TOKEN_TYPE_KEYWORD] = std::string("<keyword>");
    m[TOKEN_TYPE_LITERAL] = std::string("<literal>");
    m[TOKEN_TYPE_PUNCTUATOR] = std::string("<punctuator>");
    m[TOKEN_TYPE_OPERATOR] = std::string("<operator>");
    m[TOKEN_TYPE_IDENTIFIER] = std::string("<identifier>");
    m[TOKEN_TYPE_COMMENT] = std::string("<comment>");

    return m;
}

token_type_map::tt_map_t token_type_map::m = _init_token_type_map();

} // namespace sqltoast
