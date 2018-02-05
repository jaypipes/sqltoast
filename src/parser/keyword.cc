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
        case 'b':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_BIT, SYMBOL_BIT, "BIT"));
            return t;
        case 'c':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CREATE, SYMBOL_CREATE, "CREATE"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CHAR, SYMBOL_CHAR, "CHAR"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CHARACTER, SYMBOL_CHARACTER, "CHARACTER"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_CASCADE, SYMBOL_CASCADE, "CASCADE"));
            return t;
        case 'd':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DATE, SYMBOL_DATE, "DATE"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DEC, SYMBOL_DEC, "DEC"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DECIMAL, SYMBOL_DECIMAL, "DECIMAL"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DEFAULT, SYMBOL_DEFAULT, "DEFAULT"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DROP, SYMBOL_DROP, "DROP"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_DOUBLE, SYMBOL_DOUBLE, "DOUBLE"));
            return t;
        case 'f':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_FLOAT, SYMBOL_FLOAT, "FLOAT"));
            return t;
        case 'g':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_GLOBAL, SYMBOL_GLOBAL, "GLOBAL"));
            return t;
        case 'i':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_INT, SYMBOL_INT, "INT"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_INTEGER, SYMBOL_INTEGER, "INTEGER"));
            return t;
        case 'l':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_LOCAL, SYMBOL_LOCAL, "LOCAL"));
            return t;
        case 'n':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_NATIONAL, SYMBOL_NATIONAL, "NATIONAL"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_NCHAR, SYMBOL_NCHAR, "NCHAR"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_NUMERIC, SYMBOL_NUMERIC, "NUMERIC"));
            return t;
        case 'p':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_PRECISION, SYMBOL_PRECISION, "PRECISION"));
            return t;
        case 'r':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_REAL, SYMBOL_REAL, "REAL"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_RESTRICT, SYMBOL_RESTRICT, "RESTRICT"));
            return t;
        case 's':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SCHEMA, SYMBOL_SCHEMA, "SCHEMA"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_SET, SYMBOL_SET, "SET"));
            return t;
        case 't':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_TABLE, SYMBOL_TABLE, "TABLE"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_TIME, SYMBOL_TIME, "TIME"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_TIMESTAMP, SYMBOL_TIMESTAMP, "TIMESTAMP"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_TEMPORARY, SYMBOL_TEMPORARY, "TEMPORARY"));
            return t;
        case 'v':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_VARCHAR, SYMBOL_VARCHAR, "VARCHAR"));
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_VARYING, SYMBOL_VARYING, "VARYING"));
            return t;
        case 'w':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_WITH, SYMBOL_WITH, "WITH"));
            return t;
        case 'z':
            t.emplace_back(kw_jump_table_entry_t(KEYWORD_ZONE, SYMBOL_ZONE, "ZONE"));
            return t;
    }
    return t;
}

kw_jump_table_t kw_jump_tables::a = _init_kw_jump_table('a');
kw_jump_table_t kw_jump_tables::b = _init_kw_jump_table('b');
kw_jump_table_t kw_jump_tables::c = _init_kw_jump_table('c');
kw_jump_table_t kw_jump_tables::d = _init_kw_jump_table('d');
kw_jump_table_t kw_jump_tables::f = _init_kw_jump_table('f');
kw_jump_table_t kw_jump_tables::g = _init_kw_jump_table('g');
kw_jump_table_t kw_jump_tables::i = _init_kw_jump_table('i');
kw_jump_table_t kw_jump_tables::l = _init_kw_jump_table('l');
kw_jump_table_t kw_jump_tables::n = _init_kw_jump_table('n');
kw_jump_table_t kw_jump_tables::p = _init_kw_jump_table('p');
kw_jump_table_t kw_jump_tables::r = _init_kw_jump_table('r');
kw_jump_table_t kw_jump_tables::s = _init_kw_jump_table('s');
kw_jump_table_t kw_jump_tables::t = _init_kw_jump_table('t');
kw_jump_table_t kw_jump_tables::v = _init_kw_jump_table('v');
kw_jump_table_t kw_jump_tables::w = _init_kw_jump_table('w');
kw_jump_table_t kw_jump_tables::z = _init_kw_jump_table('z');

tokenize_result_t token_keyword(parse_position_t cursor) {
    kw_jump_table_t* jump_tbl;
    switch (*cursor) {
        case 'a':
        case 'A':
            jump_tbl = &kw_jump_tables::a;
            break;
        case 'b':
        case 'B':
            jump_tbl = &kw_jump_tables::b;
            break;
        case 'c':
        case 'C':
            jump_tbl = &kw_jump_tables::c;
            break;
        case 'd':
        case 'D':
            jump_tbl = &kw_jump_tables::d;
            break;
        case 'f':
        case 'F':
            jump_tbl = &kw_jump_tables::f;
            break;
        case 'g':
        case 'G':
            jump_tbl = &kw_jump_tables::g;
            break;
        case 'i':
        case 'I':
            jump_tbl = &kw_jump_tables::i;
            break;
        case 'l':
        case 'L':
            jump_tbl = &kw_jump_tables::l;
            break;
        case 'n':
        case 'N':
            jump_tbl = &kw_jump_tables::n;
            break;
        case 'p':
        case 'P':
            jump_tbl = &kw_jump_tables::p;
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
        case 'w':
        case 'W':
            jump_tbl = &kw_jump_tables::w;
            break;
        case 'z':
        case 'Z':
            jump_tbl = &kw_jump_tables::z;
            break;
        default:
            return tokenize_result_t(TOKEN_NOT_FOUND);
    }

    parse_position_t start = cursor;
    // Find the next delimiter character...
    while (std::isalnum(*cursor))
        cursor++;

    const std::string lexeme(start, cursor);
    size_t lexeme_len = lexeme.size();
    for (auto entry : *jump_tbl) {
        size_t entry_len = entry.kw_str.size();
        if (lexeme_len != entry_len)
            continue;
        if (ci_find_substr(lexeme, entry.kw_str) == 0) {
            return tokenize_result_t(entry.symbol, start, cursor);
        }
    }
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

} // namespace sqltoast
