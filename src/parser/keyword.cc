/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/compare.h"
#include "parser/keyword.h"

namespace sqltoast {

kw_jump_table_t _init_kw_jump_table(char lead_char) {
    kw_jump_table_t t;

    switch (lead_char) {
        case 'a':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_AUTHORIZATION, SYMBOL_AUTHORIZATION, "AUTHORIZATION"));
            return t;
        case 'c':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CREATE, SYMBOL_CREATE, "CREATE"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CHAR, SYMBOL_CHAR, "CHAR"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CHARACTER, SYMBOL_CHARACTER, "CHARACTER"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CASCADE, SYMBOL_CASCADE, "CASCADE"));
            return t;
        case 'd':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DEFAULT, SYMBOL_DEFAULT, "DEFAULT"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DROP, SYMBOL_DROP, "DROP"));
            return t;
        case 'g':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_GLOBAL, SYMBOL_GLOBAL, "GLOBAL"));
            return t;
        case 'l':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_LOCAL, SYMBOL_LOCAL, "LOCAL"));
            return t;
        case 'r':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_RESTRICT, SYMBOL_RESTRICT, "RESTRICT"));
            return t;
        case 's':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SCHEMA, SYMBOL_SCHEMA, "SCHEMA"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SET, SYMBOL_SET, "SET"));
            return t;
        case 't':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_TABLE, SYMBOL_TABLE, "TABLE"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_TEMPORARY, SYMBOL_TEMPORARY, "TEMPORARY"));
            return t;
        case 'v':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_VARCHAR, SYMBOL_VARCHAR, "VARCHAR"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_VARYING, SYMBOL_VARYING, "VARYING"));
            return t;
    }
    return t;
}

kw_jump_table_t kw_jump_tables::a = _init_kw_jump_table('a');
kw_jump_table_t kw_jump_tables::c = _init_kw_jump_table('c');
kw_jump_table_t kw_jump_tables::d = _init_kw_jump_table('d');
kw_jump_table_t kw_jump_tables::g = _init_kw_jump_table('g');
kw_jump_table_t kw_jump_tables::l = _init_kw_jump_table('l');
kw_jump_table_t kw_jump_tables::r = _init_kw_jump_table('r');
kw_jump_table_t kw_jump_tables::s = _init_kw_jump_table('s');
kw_jump_table_t kw_jump_tables::t = _init_kw_jump_table('t');
kw_jump_table_t kw_jump_tables::v = _init_kw_jump_table('v');

tokenize_result_t token_keyword(parse_position_t cursor) {
    kw_jump_table_t* jump_tbl;
    switch (*cursor) {
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
        case 'g':
        case 'G':
            jump_tbl = &kw_jump_tables::g;
            break;
        case 'l':
        case 'L':
            jump_tbl = &kw_jump_tables::l;
            break;
        case 'r':
        case 'R':
            jump_tbl = &kw_jump_tables::r;
            break;
        case 's':
        case 'S':
            jump_tbl = &kw_jump_tables::s;
            break;
        case 't':
        case 'T':
            jump_tbl = &kw_jump_tables::t;
            break;
        case 'v':
        case 'V':
            jump_tbl = &kw_jump_tables::v;
            break;
        default:
            return tokenize_result_t(TOKEN_NOT_FOUND);
    }

    parse_position_t start = cursor;
    // Find the next space or delimiter character...
    while ((*cursor >= 'a' && *cursor <= 'z') ||
            (*cursor >= 'A' && *cursor <= 'Z'))
        cursor++;

    const std::string lexeme(start, parse_position_t(cursor));
    size_t lexeme_len = lexeme.size();
    for (auto entry : *jump_tbl) {
        size_t entry_len = entry.kw_str.size();
        if (lexeme_len != entry_len)
            continue;
        if (ci_find_substr(lexeme, entry.kw_str) == 0) {
            return tokenize_result_t(entry.symbol, start, start + entry_len);
        }
    }
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

} // namespace sqltoast
