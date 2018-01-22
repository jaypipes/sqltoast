/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_SYMBOL_H
#define SQLTOAST_PARSER_SYMBOL_H

#include <map>
#include <string>

namespace sqltoast {

// A symbol is similar to a token type but represents the specific meaning of
// the underlying lexeme. A token type represents the category of lexeme that
// is tokenized. A symbol indicates the content of the lexeme.
//
// For example, the string "CREATE DATABASE id" will be tokenized into a vector
// of three tokens:
//   * token_t { TOKEN_TYPE_KEYWORD ... }
//   * token_t { TOKEN_TYPE_KEYWORD ... }
//   * token_t { TOKEN_TYPE_IDENTIFIER ... }
//
// While tokenizing, we want to be able to indicate that the first two tokens
// are not just of type TOKEN_TYPE_KEYWORD, but contain the keywords "CREATE"
// and "DATABASE". However, of course, we don't want to store the strings
// "CREATE" or "DATABASE". So, instead, we'll mark that the token represents
// the SYMBOL_CREATE or SYMBOL_DATABASE symbols...
typedef enum symbol {
    // Special characters
    SYMBOL_SEMICOLON,
    SYMBOL_COMMA,
    SYMBOL_LPAREN,
    SYMBOL_RPAREN,

    // Reserved keywords
    SYMBOL_AUTHORIZATION,
    SYMBOL_CHARACTER,
    SYMBOL_CREATE,
    SYMBOL_DEFAULT,

    // Non-reserved keywords
    SYMBOL_SCHEMA,
    SYMBOL_SET,

    // Other symbols
    SYMBOL_IDENTIFIER,
    SYMBOL_COMMENT
} symbol_t;

struct symbol_map {
    typedef std::map<symbol_t, std::string> symbol_map_t;
    static symbol_map_t m;
    // Given a token_type, returns the string representation of the token_type
    static const std::string& to_string(symbol_t sym) {
        return symbol_map::m[sym];
    }
};

symbol_map::symbol_map_t _init_symbol_map();

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SYMBOL_H */
