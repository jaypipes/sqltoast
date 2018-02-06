/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_KEYWORD_H
#define SQLTOAST_PARSER_KEYWORD_H

#include <vector>
#include <string>

#include "parser/lexer.h"

namespace sqltoast {

typedef enum keyword {
    KEYWORD_AUTHORIZATION,
    KEYWORD_BIT,
    KEYWORD_CASCADE,
    KEYWORD_CHAR,
    KEYWORD_CHARACTER,
    KEYWORD_CREATE,
    KEYWORD_DATE,
    KEYWORD_DAY,
    KEYWORD_DEC,
    KEYWORD_DECIMAL,
    KEYWORD_DEFAULT,
    KEYWORD_DROP,
    KEYWORD_DOUBLE,
    KEYWORD_FLOAT,
    KEYWORD_GLOBAL,
    KEYWORD_HOUR,
    KEYWORD_INT,
    KEYWORD_INTEGER,
    KEYWORD_INTERVAL,
    KEYWORD_LOCAL,
    KEYWORD_NATIONAL,
    KEYWORD_MINUTE,
    KEYWORD_MONTH,
    KEYWORD_NCHAR,
    KEYWORD_NUMERIC,
    KEYWORD_PRECISION,
    KEYWORD_REAL,
    KEYWORD_RESTRICT,
    KEYWORD_SCHEMA,
    KEYWORD_SECOND,
    KEYWORD_SET,
    KEYWORD_SMALLINT,
    KEYWORD_TABLE,
    KEYWORD_TEMPORARY,
    KEYWORD_TIME,
    KEYWORD_TIMESTAMP,
    KEYWORD_VARCHAR,
    KEYWORD_VARYING,
    KEYWORD_WITH,
    KEYWORD_YEAR,
    KEYWORD_ZONE
} keyword_t;

typedef struct kw_jump_table_entry {
    keyword_t kw;
    symbol_t symbol;
    std::string kw_str;
    kw_jump_table_entry(keyword_t kw, symbol_t sym, const char *kw_str) :
        kw(kw), symbol(sym), kw_str(kw_str)
    {}
} kw_jump_table_entry_t;
typedef std::vector<kw_jump_table_entry_t> kw_jump_table_t;

struct kw_jump_tables {
    // There is a separate list of jump table entries for each lead character
    static kw_jump_table_t a;
    static kw_jump_table_t b;
    static kw_jump_table_t c;
    static kw_jump_table_t d;
    static kw_jump_table_t f;
    static kw_jump_table_t g;
    static kw_jump_table_t h;
    static kw_jump_table_t i;
    static kw_jump_table_t l;
    static kw_jump_table_t m;
    static kw_jump_table_t n;
    static kw_jump_table_t p;
    static kw_jump_table_t r;
    static kw_jump_table_t s;
    static kw_jump_table_t t;
    static kw_jump_table_t v;
    static kw_jump_table_t w;
    static kw_jump_table_t y;
    static kw_jump_table_t z;
};

kw_jump_table_t _init_kw_jump_table(char c);

// Moves the supplied parse context's cursor to the next keyword found in the
// context's input stream and sets the context's current symbol to the found
// keyword symbol. Returns whether a keyword was found.
tokenize_result_t token_keyword(parse_position_t cursor);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_KEYWORD_H */
