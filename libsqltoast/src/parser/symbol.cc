/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "symbol.h"

namespace sqltoast {

symbol_map::symbol_map_t  _init_symbol_map() {
    symbol_map::symbol_map_t m;

    m[SYMBOL_SOS] = std::string("SOS");
    m[SYMBOL_EOS] = std::string("EOS");

    // Punctuators
    m[SYMBOL_ASTERISK] = std::string("'*'");
    m[SYMBOL_COLON] = std::string("':'");
    m[SYMBOL_CONCATENATION] = std::string("'||'");
    m[SYMBOL_COMMA] = std::string("','");
    m[SYMBOL_EXCLAMATION] = std::string("'!'");
    m[SYMBOL_EQUAL] = std::string("'='");
    m[SYMBOL_GREATER_THAN] = std::string("'>'");
    m[SYMBOL_LESS_THAN] = std::string("'<'");
    m[SYMBOL_LPAREN] = std::string("'('");
    m[SYMBOL_MINUS] = std::string("'-'");
    m[SYMBOL_NOT_EQUAL] = std::string("'<>'");
    m[SYMBOL_PLUS] = std::string("'+'");
    m[SYMBOL_QUESTION_MARK] = std::string("'?'");
    m[SYMBOL_RPAREN] = std::string("')'");
    m[SYMBOL_SEMICOLON] = std::string("';'");
    m[SYMBOL_SOLIDUS] = std::string("'/'");
    m[SYMBOL_VERTICAL_BAR] = std::string("'|'");

    // Reserved keywords
    m[SYMBOL_ACTION] = std::string("ACTION");
    m[SYMBOL_ADD] = std::string("ADD");
    m[SYMBOL_ALL] = std::string("ALL");
    m[SYMBOL_ALTER] = std::string("ALTER");
    m[SYMBOL_AND] = std::string("AND");
    m[SYMBOL_ANY] = std::string("ANY");
    m[SYMBOL_AS] = std::string("AS");
    m[SYMBOL_AT] = std::string("AT");
    m[SYMBOL_AUTHORIZATION] = std::string("AUTHORIZATION");
    m[SYMBOL_AVG] = std::string("AVG");
    m[SYMBOL_BETWEEN] = std::string("BETWEEN");
    m[SYMBOL_BIT] = std::string("BIT");
    m[SYMBOL_BIT_LENGTH] = std::string("BIT_LENGTH");
    m[SYMBOL_BOTH] = std::string("BOTH");
    m[SYMBOL_BY] = std::string("BY");
    m[SYMBOL_CASCADE] = std::string("CASCADE");
    m[SYMBOL_CASCADED] = std::string("CASCADED");
    m[SYMBOL_CASE] = std::string("CASE");
    m[SYMBOL_CHAR] = std::string("CHAR");
    m[SYMBOL_CHARACTER] = std::string("CHARACTER");
    m[SYMBOL_CHAR_LENGTH] = std::string("CHAR_LENGTH");
    m[SYMBOL_CHARACTER_LENGTH] = std::string("CHARACTER_LENGTH");
    m[SYMBOL_CHECK] = std::string("CHECK");
    m[SYMBOL_COALESCE] = std::string("COALESCE");
    m[SYMBOL_COLLATE] = std::string("COLLATE");
    m[SYMBOL_COLLATION] = std::string("COLLATION");
    m[SYMBOL_COLUMN] = std::string("COLUMN");
    m[SYMBOL_COMMIT] = std::string("COMMIT");
    m[SYMBOL_CONSTRAINT] = std::string("CONSTRAINT");
    m[SYMBOL_CONVERT] = std::string("CONVERT");
    m[SYMBOL_COUNT] = std::string("COUNT");
    m[SYMBOL_CREATE] = std::string("CREATE");
    m[SYMBOL_CROSS] = std::string("CROSS");
    m[SYMBOL_CURRENT_DATE] = std::string("CURRENT_DATE");
    m[SYMBOL_CURRENT_TIME] = std::string("CURRENT_TIME");
    m[SYMBOL_CURRENT_TIMESTAMP] = std::string("CURRENT_TIMESTAMP");
    m[SYMBOL_CURRENT_USER] = std::string("CURRENT_USER");
    m[SYMBOL_DATE] = std::string("DATE");
    m[SYMBOL_DAY] = std::string("DAY");
    m[SYMBOL_DEC] = std::string("DEC");
    m[SYMBOL_DECIMAL] = std::string("DECIMAL");
    m[SYMBOL_DEFAULT] = std::string("DEFAULT");
    m[SYMBOL_DELETE] = std::string("DELETE");
    m[SYMBOL_DISTINCT] = std::string("DISTINCT");
    m[SYMBOL_DROP] = std::string("DROP");
    m[SYMBOL_DOMAIN] = std::string("DOMAIN");
    m[SYMBOL_DOUBLE] = std::string("DOUBLE");
    m[SYMBOL_END] = std::string("END");
    m[SYMBOL_ESCAPE] = std::string("ESCAPE");
    m[SYMBOL_ELSE] = std::string("ELSE");
    m[SYMBOL_EXISTS] = std::string("EXISTS");
    m[SYMBOL_EXTRACT] = std::string("EXTRACT");
    m[SYMBOL_FLOAT] = std::string("FLOAT");
    m[SYMBOL_FOR] = std::string("FOR");
    m[SYMBOL_FOREIGN] = std::string("FOREIGN");
    m[SYMBOL_FROM] = std::string("FROM");
    m[SYMBOL_FULL] = std::string("FULL");
    m[SYMBOL_GLOBAL] = std::string("GLOBAL");
    m[SYMBOL_GRANT] = std::string("GRANT");
    m[SYMBOL_GROUP] = std::string("GROUP");
    m[SYMBOL_HAVING] = std::string("HAVING");
    m[SYMBOL_HOUR] = std::string("HOUR");
    m[SYMBOL_IN] = std::string("IN");
    m[SYMBOL_INNER] = std::string("INNER");
    m[SYMBOL_INSERT] = std::string("INSERT");
    m[SYMBOL_INT] = std::string("INT");
    m[SYMBOL_INTO] = std::string("INTO");
    m[SYMBOL_INTEGER] = std::string("INTEGER");
    m[SYMBOL_INTERVAL] = std::string("INTERVAL");
    m[SYMBOL_IS] = std::string("IS");
    m[SYMBOL_JOIN] = std::string("JOIN");
    m[SYMBOL_LEADING] = std::string("LEADING");
    m[SYMBOL_LEFT] = std::string("LEFT");
    m[SYMBOL_LOCAL] = std::string("LOCAL");
    m[SYMBOL_LIKE] = std::string("LIKE");
    m[SYMBOL_KEY] = std::string("KEY");
    m[SYMBOL_MATCH] = std::string("MATCH");
    m[SYMBOL_MAX] = std::string("MAX");
    m[SYMBOL_MIN] = std::string("MIN");
    m[SYMBOL_MINUTE] = std::string("MINUTE");
    m[SYMBOL_MONTH] = std::string("MONTH");
    m[SYMBOL_NATIONAL] = std::string("NATIONAL");
    m[SYMBOL_NATURAL] = std::string("NATURAL");
    m[SYMBOL_NCHAR] = std::string("NCHAR");
    m[SYMBOL_NO] = std::string("NO");
    m[SYMBOL_NOT] = std::string("NOT");
    m[SYMBOL_NUMERIC] = std::string("NUMERIC");
    m[SYMBOL_NULL] = std::string("NULL");
    m[SYMBOL_NULLIF] = std::string("NULLIF");
    m[SYMBOL_OCTET_LENGTH] = std::string("OCTET_LENGTH");
    m[SYMBOL_ON] = std::string("ON");
    m[SYMBOL_OPTION] = std::string("OPTION");
    m[SYMBOL_OR] = std::string("OR");
    m[SYMBOL_OVERLAPS] = std::string("OVERLAPS");
    m[SYMBOL_OUTER] = std::string("OUTER");
    m[SYMBOL_PARTIAL] = std::string("PARTIAL");
    m[SYMBOL_POSITION] = std::string("POSITION");
    m[SYMBOL_PRECISION] = std::string("PRECISION");
    m[SYMBOL_PRIMARY] = std::string("PRIMARY");
    m[SYMBOL_PRIVILEGES] = std::string("PRIVILEGES");
    m[SYMBOL_PUBLIC] = std::string("PUBLIC");
    m[SYMBOL_REAL] = std::string("REAL");
    m[SYMBOL_REFERENCES] = std::string("REFERENCES");
    m[SYMBOL_RESTRICT] = std::string("RESTRICT");
    m[SYMBOL_RIGHT] = std::string("RIGHT");
    m[SYMBOL_ROLLBACK] = std::string("ROLLBACK");
    m[SYMBOL_SCHEMA] = std::string("SCHEMA");
    m[SYMBOL_SECOND] = std::string("SECOND");
    m[SYMBOL_SELECT] = std::string("SELECT");
    m[SYMBOL_SESSION_USER] = std::string("SESSION_USER");
    m[SYMBOL_SET] = std::string("SET");
    m[SYMBOL_SMALLINT] = std::string("SMALLINT");
    m[SYMBOL_SOME] = std::string("SOME");
    m[SYMBOL_SUBSTRING] = std::string("SUBSTRING");
    m[SYMBOL_SUM] = std::string("SUM");
    m[SYMBOL_SYSTEM_USER] = std::string("SYSTEM_USER");
    m[SYMBOL_TABLE] = std::string("TABLE");
    m[SYMBOL_TEMPORARY] = std::string("TEMPORARY");
    m[SYMBOL_THEN] = std::string("THEN");
    m[SYMBOL_TIME] = std::string("TIME");
    m[SYMBOL_TIMESTAMP] = std::string("TIMESTAMP");
    m[SYMBOL_TO] = std::string("TO");
    m[SYMBOL_TRAILING] = std::string("TRAILING");
    m[SYMBOL_TRANSLATE] = std::string("TRANSLATE");
    m[SYMBOL_TRANSLATION] = std::string("TRANSLATION");
    m[SYMBOL_TRIM] = std::string("TRIM");
    m[SYMBOL_UNION] = std::string("UNION");
    m[SYMBOL_UNIQUE] = std::string("UNIQUE");
    m[SYMBOL_UPDATE] = std::string("UPDATE");
    m[SYMBOL_UPPER] = std::string("UPPER");
    m[SYMBOL_USAGE] = std::string("USAGE");
    m[SYMBOL_USER] = std::string("USER");
    m[SYMBOL_USING] = std::string("USING");
    m[SYMBOL_VALUE] = std::string("VALUE");
    m[SYMBOL_VALUES] = std::string("VALUES");
    m[SYMBOL_VARCHAR] = std::string("VARCHAR");
    m[SYMBOL_VARYING] = std::string("VARYING");
    m[SYMBOL_VIEW] = std::string("VIEW");
    m[SYMBOL_WHEN] = std::string("WHEN");
    m[SYMBOL_WHERE] = std::string("WHERE");
    m[SYMBOL_WITH] = std::string("WITH");
    m[SYMBOL_WORK] = std::string("WORK");
    m[SYMBOL_YEAR] = std::string("YEAR");
    m[SYMBOL_ZONE] = std::string("ZONE");

    // Non-Reserved keywordsr

    // Other symbols
    m[SYMBOL_COMMENT] = std::string("<< comment >>");
    m[SYMBOL_IDENTIFIER] = std::string("<< identifier >>");

    m[SYMBOL_LITERAL_APPROXIMATE_NUMBER] = std::string("<< approximate number >>");
    m[SYMBOL_LITERAL_BIT_STRING] = std::string("<< bit string literal >>");
    m[SYMBOL_LITERAL_CHARACTER_STRING] = std::string("<< string literal >>");
    m[SYMBOL_LITERAL_HEX_STRING] = std::string("<< hex string literal >>");
    m[SYMBOL_LITERAL_NATIONAL_CHARACTER_STRING] = std::string("<< utf8 string literal >>");
    m[SYMBOL_LITERAL_SIGNED_DECIMAL] = std::string("<< signed decimal >>");
    m[SYMBOL_LITERAL_SIGNED_INTEGER] = std::string("<< signed integer >>");
    m[SYMBOL_LITERAL_UNSIGNED_DECIMAL] = std::string("<< unsigned decimal >>");
    m[SYMBOL_LITERAL_UNSIGNED_INTEGER] = std::string("<< unsigned integer >>");

    return m;
}

symbol_map::symbol_map_t symbol_map::m = _init_symbol_map();

std::ostream& operator<< (std::ostream& out, const symbol_t& sym) {
    out << symbol_map::m[sym];
    return out;
}

} // namespace sqltoast
