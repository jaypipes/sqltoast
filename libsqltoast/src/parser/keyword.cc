/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "parser/compare.h"
#include "parser/keyword.h"
#include "parser/symbol.h"

namespace sqltoast {

// NOTE(jaypipes): These keywords are inserted in order of FREQUENCY of
// appearance in SQL statements, not alphabetically.
kw_jump_table_t _init_kw_jump_table(char lead_char) {
    kw_jump_table_t t;

    switch (lead_char) {
        case 'a':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_AND, "AND"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_AS, "AS"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_AVG, "AVG"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ALTER, "ALTER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ALL, "ALL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ANY, "ANY"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ACTION, "ACTION"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_AT, "AT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ADD, "ADD"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_AUTHORIZATION, "AUTHORIZATION"));
            return t;
        case 'b':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_BETWEEN, "BETWEEN"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_BY, "BY"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_BIT, "BIT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_BIT_LENGTH, "BIT_LENGTH"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_BOTH, "BOTH"));
            return t;
        case 'c':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_COUNT, "COUNT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_COMMIT, "COMMIT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CREATE, "CREATE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CURRENT_DATE, "CURRENT_DATE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CURRENT_TIME, "CURRENT_TIME"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CURRENT_TIMESTAMP, "CURRENT_TIMESTAMP"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CURRENT_USER, "CURRENT_USER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_COALESCE, "COALESCE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CASE, "CASE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CROSS, "CROSS"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CONVERT, "CONVERT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CHAR_LENGTH, "CHAR_LENGTH"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CHARACTER_LENGTH, "CHARACTER_LENGTH"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CHAR, "CHAR"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CHARACTER, "CHARACTER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CASCADE, "CASCADE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CHECK, "CHECK"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_COLUMN, "COLUMN"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_COLLATE, "COLLATE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_COLLATION, "COLLATION"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CONSTRAINT, "CONSTRAINT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_CASCADED, "CASCADED"));
            return t;
        case 'd':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DELETE, "DELETE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DISTINCT, "DISTINCT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DATE, "DATE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DAY, "DAY"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DEC, "DEC"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DECIMAL, "DECIMAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DEFAULT, "DEFAULT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DROP, "DROP"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DOUBLE, "DOUBLE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_DOMAIN, "DOMAIN"));
            return t;
        case 'e':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_EXISTS, "EXISTS"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_END, "END"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ELSE, "ELSE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_EXTRACT, "EXTRACT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ESCAPE, "ESCAPE"));
            return t;
        case 'f':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_FROM, "FROM"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_FOR, "FOR"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_FLOAT, "FLOAT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_FULL, "FULL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_FOREIGN, "FOREIGN"));
            return t;
        case 'g':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_GROUP, "GROUP"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_GLOBAL, "GLOBAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_GRANT, "GRANT"));
            return t;
        case 'h':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_HAVING, "HAVING"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_HOUR, "HOUR"));
            return t;
        case 'i':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_IN, "IN"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_INSERT, "INSERT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_INTO, "INTO"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_IS, "IS"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_INNER, "INNER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_INTERVAL, "INTERVAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_INT, "INT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_INTEGER, "INTEGER"));
            return t;
        case 'j':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_JOIN, "JOIN"));
            return t;
        case 'k':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_KEY, "KEY"));
            return t;
        case 'l':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_LIKE, "LIKE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_LEFT, "LEFT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_LOWER, "LOWER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_LOCAL, "LOCAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_LEADING, "LEADING"));
            return t;
        case 'm':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_MAX, "MAX"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_MIN, "MIN"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_MATCH, "MATCH"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_MINUTE, "MINUTE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_MONTH, "MONTH"));
            return t;
        case 'n':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NOT, "NOT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NO, "NO"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NULL, "NULL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NULLIF, "NULLIF"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NATURAL, "NATURAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NATIONAL, "NATIONAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NCHAR, "NCHAR"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_NUMERIC, "NUMERIC"));
            return t;
        case 'o':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_OR, "OR"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ON, "ON"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_OUTER, "OUTER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_OCTET_LENGTH, "OCTET_LENGTH"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_OPTION, "OPTION"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_OVERLAPS, "OVERLAPS"));
            return t;
        case 'p':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_POSITION, "POSITION"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_PRECISION, "PRECISION"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_PRIMARY, "PRIMARY"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_PARTIAL, "PARTIAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_PRIVILEGES, "PRIVILEGES"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_PUBLIC, "PUBLIC"));
            return t;
        case 'r':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ROLLBACK, "ROLLBACK"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_RIGHT, "RIGHT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_REAL, "REAL"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_REFERENCES, "REFERENCES"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_RESTRICT, "RESTRICT"));
            return t;
        case 's':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SELECT, "SELECT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SET, "SET"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SUM, "SUM"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SCHEMA, "SCHEMA"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SECOND, "SECOND"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SUBSTRING, "SUBSTRING"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SMALLINT, "SMALLINT"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SESSION_USER, "SESSION_USER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SYSTEM_USER, "SYSTEM_USER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_SOME, "SOME"));
            return t;
        case 't':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TABLE, "TABLE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TIME, "TIME"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TIMESTAMP, "TIMESTAMP"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TEMPORARY, "TEMPORARY"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TRIM, "TRIM"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_THEN, "THEN"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TO, "TO"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TRAILING, "TRAILING"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TRANSLATE, "TRANSLATE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_TRANSLATION, "TRANSLATION"));
            return t;
        case 'u':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_UPDATE, "UPDATE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_UPPER, "UPPER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_USING, "USING"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_UNION, "UNION"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_USER, "USER"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_UNIQUE, "UNIQUE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_USAGE, "USAGE"));
            return t;
        case 'v':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_VALUES, "VALUES"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_VARCHAR, "VARCHAR"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_VARYING, "VARYING"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_VIEW, "VIEW"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_VALUE, "VALUE"));
            return t;
        case 'w':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_WHERE, "WHERE"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_WITH, "WITH"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_WHEN, "WHEN"));
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_WORK, "WORK"));
            return t;
        case 'y':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_YEAR, "YEAR"));
            return t;
        case 'z':
            t.emplace_back(kw_jump_table_entry_t(SYMBOL_ZONE, "ZONE"));
            return t;
    }
    return t;
}

kw_jump_table_t kw_jump_tables::a = _init_kw_jump_table('a');
kw_jump_table_t kw_jump_tables::b = _init_kw_jump_table('b');
kw_jump_table_t kw_jump_tables::c = _init_kw_jump_table('c');
kw_jump_table_t kw_jump_tables::d = _init_kw_jump_table('d');
kw_jump_table_t kw_jump_tables::e = _init_kw_jump_table('e');
kw_jump_table_t kw_jump_tables::f = _init_kw_jump_table('f');
kw_jump_table_t kw_jump_tables::g = _init_kw_jump_table('g');
kw_jump_table_t kw_jump_tables::h = _init_kw_jump_table('h');
kw_jump_table_t kw_jump_tables::i = _init_kw_jump_table('i');
kw_jump_table_t kw_jump_tables::j = _init_kw_jump_table('j');
kw_jump_table_t kw_jump_tables::k = _init_kw_jump_table('k');
kw_jump_table_t kw_jump_tables::l = _init_kw_jump_table('l');
kw_jump_table_t kw_jump_tables::m = _init_kw_jump_table('m');
kw_jump_table_t kw_jump_tables::n = _init_kw_jump_table('n');
kw_jump_table_t kw_jump_tables::o = _init_kw_jump_table('o');
kw_jump_table_t kw_jump_tables::p = _init_kw_jump_table('p');
kw_jump_table_t kw_jump_tables::r = _init_kw_jump_table('r');
kw_jump_table_t kw_jump_tables::s = _init_kw_jump_table('s');
kw_jump_table_t kw_jump_tables::t = _init_kw_jump_table('t');
kw_jump_table_t kw_jump_tables::u = _init_kw_jump_table('u');
kw_jump_table_t kw_jump_tables::v = _init_kw_jump_table('v');
kw_jump_table_t kw_jump_tables::w = _init_kw_jump_table('w');
kw_jump_table_t kw_jump_tables::y = _init_kw_jump_table('y');
kw_jump_table_t kw_jump_tables::z = _init_kw_jump_table('z');

tokenize_result_t token_keyword(
        parse_position_t cursor,
        const parse_position_t end) {
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
        case 'e':
        case 'E':
            jump_tbl = &kw_jump_tables::e;
            break;
        case 'f':
        case 'F':
            jump_tbl = &kw_jump_tables::f;
            break;
        case 'g':
        case 'G':
            jump_tbl = &kw_jump_tables::g;
            break;
        case 'h':
        case 'H':
            jump_tbl = &kw_jump_tables::h;
            break;
        case 'i':
        case 'I':
            jump_tbl = &kw_jump_tables::i;
            break;
        case 'j':
        case 'J':
            jump_tbl = &kw_jump_tables::j;
            break;
        case 'k':
        case 'K':
            jump_tbl = &kw_jump_tables::k;
            break;
        case 'l':
        case 'L':
            jump_tbl = &kw_jump_tables::l;
            break;
        case 'm':
        case 'M':
            jump_tbl = &kw_jump_tables::m;
            break;
        case 'n':
        case 'N':
            jump_tbl = &kw_jump_tables::n;
            break;
        case 'o':
        case 'O':
            jump_tbl = &kw_jump_tables::o;
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
        case 'u':
        case 'U':
            jump_tbl = &kw_jump_tables::u;
            break;
        case 'v':
        case 'V':
            jump_tbl = &kw_jump_tables::v;
            break;
        case 'w':
        case 'W':
            jump_tbl = &kw_jump_tables::w;
            break;
        case 'y':
        case 'Y':
            jump_tbl = &kw_jump_tables::y;
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
    while (cursor != end && (std::isalnum(*cursor) || *cursor == '_'))
        cursor++;

    const std::string lexeme(start, cursor);
    const size_t lexeme_len = cursor - start;
    size_t entry_len;
    for (auto entry : *jump_tbl) {
        entry_len = entry.kw_str.size();
        if (lexeme_len != entry_len)
            continue;
        if (ci_find_substr(lexeme, entry.kw_str) == 0) {
            return tokenize_result_t(entry.symbol, start, cursor);
        }
    }
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

} // namespace sqltoast
