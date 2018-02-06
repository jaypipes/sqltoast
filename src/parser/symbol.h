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
    SYMBOL_NONE,
    SYMBOL_ERROR,

    SYMBOL_SOS, // Start of the input stream
    SYMBOL_EOS, // End of the input stream

    // Punctuators
    SYMBOL_SEMICOLON,
    SYMBOL_COMMA,
    SYMBOL_LPAREN,
    SYMBOL_RPAREN,

    // Reserved keywords
    SYMBOL_AUTHORIZATION,
    SYMBOL_BIT,
    SYMBOL_CASCADE,
    SYMBOL_CHAR,
    SYMBOL_CHARACTER,
    SYMBOL_CREATE,
    SYMBOL_CURRENT_USER,
    SYMBOL_DATE,
    SYMBOL_DAY,
    SYMBOL_DEC,
    SYMBOL_DECIMAL,
    SYMBOL_DEFAULT,
    SYMBOL_DOUBLE,
    SYMBOL_DROP,
    SYMBOL_FLOAT,
    SYMBOL_GLOBAL,
    SYMBOL_HOUR,
    SYMBOL_INT,
    SYMBOL_INTEGER,
    SYMBOL_INTERVAL,
    SYMBOL_LOCAL,
    SYMBOL_MINUTE,
    SYMBOL_MONTH,
    SYMBOL_NATIONAL,
    SYMBOL_NCHAR,
    SYMBOL_NUMERIC,
    SYMBOL_NULL,
    SYMBOL_PRECISION,
    SYMBOL_REAL,
    SYMBOL_RESTRICT,
    SYMBOL_SCHEMA,
    SYMBOL_SECOND,
    SYMBOL_SET,
    SYMBOL_SMALLINT,
    SYMBOL_SESSION_USER,
    SYMBOL_SYSTEM_USER,
    SYMBOL_TABLE,
    SYMBOL_TEMPORARY,
    SYMBOL_TIME,
    SYMBOL_TIMESTAMP,
    SYMBOL_USER,
    SYMBOL_VARCHAR,
    SYMBOL_VARYING,
    SYMBOL_WITH,
    SYMBOL_YEAR,
    SYMBOL_ZONE,

    // Non-reserved keywords

    // Other symbols
    SYMBOL_IDENTIFIER,
    SYMBOL_COMMENT,

    // Literals
    SYMBOL_LITERAL_UNSIGNED_INTEGER,
    SYMBOL_LITERAL_SIGNED_INTEGER,
    SYMBOL_LITERAL_UNSIGNED_DECIMAL,
    SYMBOL_LITERAL_SIGNED_DECIMAL,
    SYMBOL_LITERAL_APPROXIMATE_NUMBER
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
