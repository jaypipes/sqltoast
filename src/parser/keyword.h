/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_KEYWORD_H
#define SQLTOAST_PARSER_KEYWORD_H

#include "parser/lexer.h"
#include "parser/symbol.h"

namespace sqltoast {

typedef struct kw_jump_table_entry {
    symbol_t symbol;
    const std::string kw_str;
    kw_jump_table_entry(symbol_t sym, const char *kw_str) :
        symbol(sym), kw_str(kw_str)
    {}
} kw_jump_table_entry_t;
typedef std::vector<kw_jump_table_entry_t> kw_jump_table_t;

struct kw_jump_tables {
    // There is a separate list of jump table entries for each lead character
    static kw_jump_table_t a;
    static kw_jump_table_t b;
    static kw_jump_table_t c;
    static kw_jump_table_t d;
    static kw_jump_table_t e;
    static kw_jump_table_t f;
    static kw_jump_table_t g;
    static kw_jump_table_t h;
    static kw_jump_table_t i;
    static kw_jump_table_t j;
    static kw_jump_table_t k;
    static kw_jump_table_t l;
    static kw_jump_table_t m;
    static kw_jump_table_t n;
    static kw_jump_table_t o;
    static kw_jump_table_t p;
    static kw_jump_table_t r;
    static kw_jump_table_t s;
    static kw_jump_table_t t;
    static kw_jump_table_t u;
    static kw_jump_table_t v;
    static kw_jump_table_t w;
    static kw_jump_table_t y;
    static kw_jump_table_t z;
};

kw_jump_table_t _init_kw_jump_table(const char c);

// Moves the supplied parse context's cursor to the next keyword found in the
// context's input stream and sets the context's current symbol to the found
// keyword symbol. Returns whether a keyword was found.
tokenize_result_t token_keyword(
        parse_position_t cursor,
        const parse_position_t end);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_KEYWORD_H */
