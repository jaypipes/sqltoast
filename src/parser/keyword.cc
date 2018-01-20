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
        case 'c':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CREATE, SYMBOL_CREATE, "CREATE"));
            return t;
        case 's':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SCHEMA, SYMBOL_SCHEMA, "SCHEMA"));
            return t;
    }
    return t;
}

kw_jump_table_t kw_jump_tables::c = _init_kw_jump_table('c');
kw_jump_table_t kw_jump_tables::s = _init_kw_jump_table('s');

bool token_keyword(parse_context_t& ctx) {
    kw_jump_table_t jump_tbl;
    switch (*ctx.cursor) {
        case 'c':
        case 'C':
            jump_tbl = kw_jump_tables::c;
            break;
        case 's':
        case 'S':
            jump_tbl = kw_jump_tables::s;
            break;
        default:
            return false;
    }

    parse_cursor_t start = ctx.cursor;

    for (auto entry : jump_tbl) {
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
