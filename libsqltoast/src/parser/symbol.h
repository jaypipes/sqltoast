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
    SYMBOL_ASTERISK,
    SYMBOL_COLON,
    SYMBOL_CONCATENATION, // This is the || double-char symbol
    SYMBOL_COMMA,
    SYMBOL_EQUAL,
    SYMBOL_EXCLAMATION,
    SYMBOL_GREATER_THAN,
    SYMBOL_LESS_THAN,
    SYMBOL_LPAREN,
    SYMBOL_MINUS,
    SYMBOL_NOT_EQUAL, // This is the <> double-char symbol
    SYMBOL_PLUS,
    SYMBOL_QUESTION_MARK,
    SYMBOL_RPAREN,
    SYMBOL_SEMICOLON,
    SYMBOL_SOLIDUS,
    SYMBOL_VERTICAL_BAR,

    // Reserved keywords
    SYMBOL_ACTION,
    SYMBOL_ADD,
    SYMBOL_ALL,
    SYMBOL_ALTER,
    SYMBOL_AND,
    SYMBOL_ANY,
    SYMBOL_AS,
    SYMBOL_AT,
    SYMBOL_AUTHORIZATION,
    SYMBOL_AVG,
    SYMBOL_BETWEEN,
    SYMBOL_BIT,
    SYMBOL_BIT_LENGTH,
    SYMBOL_BOTH,
    SYMBOL_BY,
    SYMBOL_CASCADE,
    SYMBOL_CASCADED,
    SYMBOL_CASE,
    SYMBOL_CHAR,
    SYMBOL_CHARACTER,
    SYMBOL_CHAR_LENGTH,
    SYMBOL_CHARACTER_LENGTH,
    SYMBOL_CHECK,
    SYMBOL_COALESCE,
    SYMBOL_COLLATE,
    SYMBOL_COLLATION,
    SYMBOL_COLUMN,
    SYMBOL_COMMIT,
    SYMBOL_CONSTRAINT,
    SYMBOL_CONVERT,
    SYMBOL_COUNT,
    SYMBOL_CREATE,
    SYMBOL_CROSS,
    SYMBOL_CURRENT_DATE,
    SYMBOL_CURRENT_TIME,
    SYMBOL_CURRENT_TIMESTAMP,
    SYMBOL_CURRENT_USER,
    SYMBOL_DATE,
    SYMBOL_DAY,
    SYMBOL_DEC,
    SYMBOL_DECIMAL,
    SYMBOL_DEFAULT,
    SYMBOL_DELETE,
    SYMBOL_DISTINCT,
    SYMBOL_DOMAIN,
    SYMBOL_DOUBLE,
    SYMBOL_DROP,
    SYMBOL_ELSE,
    SYMBOL_END,
    SYMBOL_ESCAPE,
    SYMBOL_EXISTS,
    SYMBOL_EXTRACT,
    SYMBOL_FLOAT,
    SYMBOL_FOR,
    SYMBOL_FOREIGN,
    SYMBOL_FROM,
    SYMBOL_FULL,
    SYMBOL_GLOBAL,
    SYMBOL_GRANT,
    SYMBOL_GROUP,
    SYMBOL_HAVING,
    SYMBOL_HOUR,
    SYMBOL_IN,
    SYMBOL_INNER,
    SYMBOL_INSERT,
    SYMBOL_INT,
    SYMBOL_INTO,
    SYMBOL_INTEGER,
    SYMBOL_INTERVAL,
    SYMBOL_IS,
    SYMBOL_JOIN,
    SYMBOL_KEY,
    SYMBOL_LEADING,
    SYMBOL_LEFT,
    SYMBOL_LOCAL,
    SYMBOL_LOWER,
    SYMBOL_LIKE,
    SYMBOL_MATCH,
    SYMBOL_MAX,
    SYMBOL_MIN,
    SYMBOL_MINUTE,
    SYMBOL_MONTH,
    SYMBOL_NATIONAL,
    SYMBOL_NATURAL,
    SYMBOL_NCHAR,
    SYMBOL_NO,
    SYMBOL_NOT,
    SYMBOL_NUMERIC,
    SYMBOL_NULL,
    SYMBOL_NULLIF,
    SYMBOL_OCTET_LENGTH,
    SYMBOL_ON,
    SYMBOL_OPTION,
    SYMBOL_OR,
    SYMBOL_OVERLAPS,
    SYMBOL_OUTER,
    SYMBOL_PARTIAL,
    SYMBOL_POSITION,
    SYMBOL_PRECISION,
    SYMBOL_PRIMARY,
    SYMBOL_PRIVILEGES,
    SYMBOL_PUBLIC,
    SYMBOL_REAL,
    SYMBOL_REFERENCES,
    SYMBOL_RESTRICT,
    SYMBOL_RIGHT,
    SYMBOL_ROLLBACK,
    SYMBOL_SCHEMA,
    SYMBOL_SECOND,
    SYMBOL_SELECT,
    SYMBOL_SET,
    SYMBOL_SMALLINT,
    SYMBOL_SESSION_USER,
    SYMBOL_SOME,
    SYMBOL_SUBSTRING,
    SYMBOL_SUM,
    SYMBOL_SYSTEM_USER,
    SYMBOL_TABLE,
    SYMBOL_TEMPORARY,
    SYMBOL_THEN,
    SYMBOL_TIME,
    SYMBOL_TIMESTAMP,
    SYMBOL_TO,
    SYMBOL_TRAILING,
    SYMBOL_TRANSLATE,
    SYMBOL_TRANSLATION,
    SYMBOL_TRIM,
    SYMBOL_UNION,
    SYMBOL_UNIQUE,
    SYMBOL_UPDATE,
    SYMBOL_UPPER,
    SYMBOL_USAGE,
    SYMBOL_USER,
    SYMBOL_USING,
    SYMBOL_VALUE,
    SYMBOL_VALUES,
    SYMBOL_VARCHAR,
    SYMBOL_VARYING,
    SYMBOL_VIEW,
    SYMBOL_WHEN,
    SYMBOL_WHERE,
    SYMBOL_WITH,
    SYMBOL_WORK,
    SYMBOL_YEAR,
    SYMBOL_ZONE,

    // Non-reserved keywords

    // Other symbols
    SYMBOL_IDENTIFIER,
    SYMBOL_COMMENT,

    // Literals
    SYMBOL_LITERAL_APPROXIMATE_NUMBER,
    SYMBOL_LITERAL_BIT_STRING,
    SYMBOL_LITERAL_CHARACTER_STRING,
    SYMBOL_LITERAL_HEX_STRING,
    SYMBOL_LITERAL_NATIONAL_CHARACTER_STRING,
    SYMBOL_LITERAL_SIGNED_DECIMAL,
    SYMBOL_LITERAL_SIGNED_INTEGER,
    SYMBOL_LITERAL_UNSIGNED_DECIMAL,
    SYMBOL_LITERAL_UNSIGNED_INTEGER
} symbol_t;

std::ostream& operator<< (std::ostream& out, const symbol_t& sym);

inline bool is_value_expression_terminator(const symbol_t& sym) {
    switch (sym) {
        case SYMBOL_AND:
        case SYMBOL_COMMA:
        case SYMBOL_ELSE:
        case SYMBOL_ESCAPE:
        case SYMBOL_END:
        case SYMBOL_EOS:
        case SYMBOL_EQUAL:
        case SYMBOL_FOR:
        case SYMBOL_FROM:
        case SYMBOL_GREATER_THAN:
        case SYMBOL_GROUP:
        case SYMBOL_HAVING:
        case SYMBOL_IN:
        case SYMBOL_LESS_THAN:
        case SYMBOL_LIKE:
        case SYMBOL_LPAREN:
        case SYMBOL_MATCH:
        case SYMBOL_NOT:
        case SYMBOL_NOT_EQUAL:
        case SYMBOL_OR:
        case SYMBOL_RPAREN:
        case SYMBOL_SEMICOLON:
        case SYMBOL_THEN:
        case SYMBOL_USING:
        case SYMBOL_WHEN:
        case SYMBOL_WHERE:
            return true;
        default:
            return false;
    }
}

struct symbol_map {
    typedef std::map<symbol_t, std::string> symbol_map_t;
    static symbol_map_t m;
};

symbol_map::symbol_map_t _init_symbol_map();

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SYMBOL_H */
