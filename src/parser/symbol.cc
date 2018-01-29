/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "symbol.h"

namespace sqltoast {

symbol_map::symbol_map_t  _init_symbol_map() {
    symbol_map::symbol_map_t m;

    // Special characters
    m[SYMBOL_SEMICOLON] = std::string("SEMICOLON");
    m[SYMBOL_COMMA] = std::string("COMMA");
    m[SYMBOL_LPAREN] = std::string("LPAREN");
    m[SYMBOL_RPAREN] = std::string("RPAREN");

    // Reserved keywords
    m[SYMBOL_AUTHORIZATION] = std::string("AUTHORIZATION");
    m[SYMBOL_CASCADE] = std::string("CASCADE");
    m[SYMBOL_CHAR] = std::string("CHAR");
    m[SYMBOL_CHARACTER] = std::string("CHARACTER");
    m[SYMBOL_CREATE] = std::string("CREATE");
    m[SYMBOL_DEFAULT] = std::string("DEFAULT");
    m[SYMBOL_DROP] = std::string("DROP");
    m[SYMBOL_GLOBAL] = std::string("GLOBAL");
    m[SYMBOL_LOCAL] = std::string("LOCAL");
    m[SYMBOL_RESTRICT] = std::string("RESTRICT");
    m[SYMBOL_SCHEMA] = std::string("SCHEMA");
    m[SYMBOL_SET] = std::string("SET");
    m[SYMBOL_TABLE] = std::string("TABLE");
    m[SYMBOL_TEMPORARY] = std::string("TEMPORARY");
    m[SYMBOL_VARCHAR] = std::string("VARCHAR");

    // Non-Reserved keywordsr

    // Other symbols
    m[SYMBOL_IDENTIFIER] = std::string("<< identifier >>");
    m[SYMBOL_COMMENT] = std::string("<< comment >>");
    m[SYMBOL_LITERAL_UNSIGNED_INTEGER] = std::string("<< unsigned integer >>");
    m[SYMBOL_LITERAL_SIGNED_INTEGER] = std::string("<< signed integer >>");
    m[SYMBOL_LITERAL_UNSIGNED_DECIMAL] = std::string("<< unsigned decimal >>");
    m[SYMBOL_LITERAL_SIGNED_DECIMAL] = std::string("<< signed decimal >>");
    m[SYMBOL_LITERAL_APPROXIMATE_NUMBER] = std::string("<< approximate number >>");

    return m;
}

symbol_map::symbol_map_t symbol_map::m = _init_symbol_map();

} // namespace sqltoast
