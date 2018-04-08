/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "parser/token.h"

namespace sqltoast {

std::ostream& operator<< (std::ostream& out, const token_t& token) {
    if (token.is_keyword()) {
        out << "keyword[" << token.symbol << "]";
        return out;
    }
    if (token.is_literal()){
        // TODO(jaypipes): Add typing of literal...
        size_t tok_len = token.lexeme.size();
        if (tok_len < 20) {
            out << "literal[" << std::string(token.lexeme.start, token.lexeme.end) << "]";
        } else {
            out << "literal[length: " << tok_len << "]";
        }
        return out;
    }
    if (token.is_identifier()){
        size_t tok_len = token.lexeme.size();
        if (tok_len < 20) {
            out << "identifier[" << std::string(token.lexeme.start, token.lexeme.end) << "]";
        } else {
            out << "identifier[length: " << tok_len << "]";
        }
        return out;
    }
    if (token.symbol == SYMBOL_COMMENT) {
        out << "comment[length: " << token.lexeme.size() << "]";
        return out;
    } else {
        out << "symbol[" << token.symbol << "]";
        return out;
    }
    return out;
}

} // namespace sqltoast
