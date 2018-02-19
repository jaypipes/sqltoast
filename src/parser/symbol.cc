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
    m[SYMBOL_COMMA] = std::string("','");
    m[SYMBOL_EXCLAMATION] = std::string("'!'");
    m[SYMBOL_EQUAL] = std::string("'='");
    m[SYMBOL_GREATER_THAN] = std::string("'>'");
    m[SYMBOL_LESS_THAN] = std::string("'<'");
    m[SYMBOL_LPAREN] = std::string("'()'");
    m[SYMBOL_QUESTION_MARK] = std::string("'?'");
    m[SYMBOL_RPAREN] = std::string("')'");
    m[SYMBOL_SEMICOLON] = std::string("';'");

    // Reserved keywords
    m[SYMBOL_ACTION] = std::string("ACTION");
    m[SYMBOL_ALL] = std::string("ALL");
    m[SYMBOL_ALL] = std::string("AND");
    m[SYMBOL_AS] = std::string("AS");
    m[SYMBOL_AUTHORIZATION] = std::string("AUTHORIZATION");
    m[SYMBOL_AVG] = std::string("AVG");
    m[SYMBOL_BETWEEN] = std::string("BETWEEN");
    m[SYMBOL_BIT] = std::string("BIT");
    m[SYMBOL_BY] = std::string("BY");
    m[SYMBOL_CASCADE] = std::string("CASCADE");
    m[SYMBOL_CHAR] = std::string("CHAR");
    m[SYMBOL_CHARACTER] = std::string("CHARACTER");
    m[SYMBOL_CHECK] = std::string("CHECK");
    m[SYMBOL_COLLATE] = std::string("COLLATE");
    m[SYMBOL_CONSTRAINT] = std::string("CONSTRAINT");
    m[SYMBOL_COUNT] = std::string("COUNT");
    m[SYMBOL_CREATE] = std::string("CREATE");
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
    m[SYMBOL_DOUBLE] = std::string("DOUBLE");
    m[SYMBOL_EXISTS] = std::string("EXISTS");
    m[SYMBOL_FLOAT] = std::string("FLOAT");
    m[SYMBOL_FOREIGN] = std::string("FOREIGN");
    m[SYMBOL_FROM] = std::string("FROM");
    m[SYMBOL_FULL] = std::string("FULL");
    m[SYMBOL_GLOBAL] = std::string("GLOBAL");
    m[SYMBOL_GROUP] = std::string("GROUP");
    m[SYMBOL_HAVING] = std::string("HAVING");
    m[SYMBOL_HOUR] = std::string("HOUR");
    m[SYMBOL_IN] = std::string("IN");
    m[SYMBOL_INT] = std::string("INT");
    m[SYMBOL_INTEGER] = std::string("INTEGER");
    m[SYMBOL_INTERVAL] = std::string("INTERVAL");
    m[SYMBOL_IS] = std::string("IS");
    m[SYMBOL_LOCAL] = std::string("LOCAL");
    m[SYMBOL_LIKE] = std::string("LIKE");
    m[SYMBOL_KEY] = std::string("KEY");
    m[SYMBOL_MATCH] = std::string("MATCH");
    m[SYMBOL_MAX] = std::string("MAX");
    m[SYMBOL_MIN] = std::string("MIN");
    m[SYMBOL_MINUTE] = std::string("MINUTE");
    m[SYMBOL_MONTH] = std::string("MONTH");
    m[SYMBOL_NATIONAL] = std::string("NATIONAL");
    m[SYMBOL_NCHAR] = std::string("NCHAR");
    m[SYMBOL_NO] = std::string("NO");
    m[SYMBOL_NOT] = std::string("NOT");
    m[SYMBOL_NUMERIC] = std::string("NUMERIC");
    m[SYMBOL_NULL] = std::string("NULL");
    m[SYMBOL_ON] = std::string("ON");
    m[SYMBOL_OR] = std::string("OR");
    m[SYMBOL_PARTIAL] = std::string("PARTIAL");
    m[SYMBOL_PRECISION] = std::string("PRECISION");
    m[SYMBOL_PRIMARY] = std::string("PRIMARY");
    m[SYMBOL_REAL] = std::string("REAL");
    m[SYMBOL_REFERENCES] = std::string("REFERENCES");
    m[SYMBOL_RESTRICT] = std::string("RESTRICT");
    m[SYMBOL_SCHEMA] = std::string("SCHEMA");
    m[SYMBOL_SECOND] = std::string("SECOND");
    m[SYMBOL_SELECT] = std::string("SELECT");
    m[SYMBOL_SESSION_USER] = std::string("SESSION_USER");
    m[SYMBOL_SET] = std::string("SET");
    m[SYMBOL_SMALLINT] = std::string("SMALLINT");
    m[SYMBOL_SUM] = std::string("SUM");
    m[SYMBOL_SYSTEM_USER] = std::string("SYSTEM_USER");
    m[SYMBOL_TABLE] = std::string("TABLE");
    m[SYMBOL_TEMPORARY] = std::string("TEMPORARY");
    m[SYMBOL_TIME] = std::string("TIME");
    m[SYMBOL_TIMESTAMP] = std::string("TIMESTAMP");
    m[SYMBOL_UNIQUE] = std::string("UNIQUE");
    m[SYMBOL_UPDATE] = std::string("UPDATE");
    m[SYMBOL_USER] = std::string("USER");
    m[SYMBOL_VALUE] = std::string("VALUE");
    m[SYMBOL_VALUES] = std::string("VALUES");
    m[SYMBOL_VARCHAR] = std::string("VARCHAR");
    m[SYMBOL_VARYING] = std::string("VARYING");
    m[SYMBOL_WHERE] = std::string("WHERE");
    m[SYMBOL_WITH] = std::string("WITH");
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

} // namespace sqltoast
