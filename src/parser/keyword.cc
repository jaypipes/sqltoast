/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "compare.h"
#include "keyword.h"
#include "token.h"
#include "symbol.h"

namespace sqltoast {

kw_jump_table_t _init_kw_jump_table(char lead_char) {
    kw_jump_table_t t;

    switch (lead_char) {
        case 'a':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_AUTHORIZATION, SYMBOL_AUTHORIZATION, "AUTHORIZATION"));
            return t;
        case 'c':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CREATE, SYMBOL_CREATE, "CREATE"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CHARACTER, SYMBOL_CHARACTER, "CHARACTER"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CASCADE, SYMBOL_CASCADE, "CASCADE"));
            return t;
        case 'd':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DEFAULT, SYMBOL_DEFAULT, "DEFAULT"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DROP, SYMBOL_DROP, "DROP"));
            return t;
        case 'r':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_RESTRICT, SYMBOL_RESTRICT, "RESTRICT"));
            return t;
        case 's':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SCHEMA, SYMBOL_SCHEMA, "SCHEMA"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SET, SYMBOL_SET, "SET"));
            return t;
    }
    return t;
}

kw_jump_table_t kw_jump_tables::a = _init_kw_jump_table('a');
kw_jump_table_t kw_jump_tables::c = _init_kw_jump_table('c');
kw_jump_table_t kw_jump_tables::d = _init_kw_jump_table('d');
kw_jump_table_t kw_jump_tables::r = _init_kw_jump_table('r');
kw_jump_table_t kw_jump_tables::s = _init_kw_jump_table('s');

bool token_keyword(parse_context_t& ctx) {
    kw_jump_table_t* jump_tbl;
    switch (*ctx.cursor) {
        case 'a':
        case 'A':
            jump_tbl = &kw_jump_tables::a;
            break;
        case 'c':
        case 'C':
            jump_tbl = &kw_jump_tables::c;
            break;
        case 'd':
        case 'D':
            jump_tbl = &kw_jump_tables::d;
            break;
        case 'r':
        case 'R':
            jump_tbl = &kw_jump_tables::r;
            break;
        case 's':
        case 'S':
            jump_tbl = &kw_jump_tables::s;
            break;
        default:
            return false;
    }

    parse_cursor_t start = ctx.cursor;

    for (auto entry : *jump_tbl) {
        const std::string to_end(parse_position_t(ctx.cursor), ctx.end_pos);
        if (ci_find_substr(to_end, entry.kw_str) != -1) {
            token_t tok(TOKEN_TYPE_KEYWORD, entry.symbol, start, parse_position_t(ctx.cursor));
            ctx.push_token(tok);
            ctx.cursor += entry.kw_str.size();
            return true;
        }
    }
    return false;
}

} // namespace sqltoast
