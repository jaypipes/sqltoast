/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "token.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const token_t& token) {
    if (token.is_keyword()) {
        out << "keyword[" << symbol_map::to_string(token.symbol) << "]";
        return out;
    }
    if (token.is_punctuator()) {
        out << symbol_map::to_string(token.symbol);
        return out;
    }
    if (token.is_literal()){
        // TODO(jaypipes): Add typing of literal...
        size_t len = (token.end - token.start);
        out << "literal[length: " << len << "]";
        return out;
    }
    if (token.is_identifier()){
        size_t len = (token.end - token.start);
        out << "identifier[length: " << len << "]";
        return out;
    }
    if (token.symbol == SYMBOL_COMMENT) {
        size_t len = (token.end - token.start);
        out << "comment[length: " << len << "]";
        return out;
    }
    return out;
}

} // namespace sqltoast
