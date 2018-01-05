/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "symbols.h"

namespace sqltoast {

#ifdef SQLTOAST_DEBUG

// Given a symbol, returns the string representation of the symbol
const std::string& string_from_symbol(symbol s) {
    return symbol_map::m[s];
}

symbol_map_t _init_symbol_map() {
    symbol_map_t m;
    // keywords
    m[CREATE] = std::string("CREATE");
    m[DATABASE] = std::string("DATABASE");

    // terminals
    m[EOS] = std::string("<< EOS >>"); // end of the parsed string (\0)
    m[DOUBLE_SLASH] = std::string("DOUBLE_SLASH"); // /
    m[LPAREN] = std::string("LPAREN"); // (
    m[NEWLINE] = std::string("NEWLINE"); // \n
    m[RPAREN] = std::string("RPAREN"); // )
    m[SEMICOLON] = std::string("SEMICOLON"); // ;
    m[SLASH] = std::string("SLASH"); // /
    m[SOS] = std::string("<< SOS >>"); // start of the parsed string

    // non-terminals
    m[IDENTIFIER] = std::string("<identifier>"); // start of the parsed string

    // an invalid symbol
    m[UNKNOWN] = std::string("UNKNOWN");
    return m;
}

symbol_map_t symbol_map::m = _init_symbol_map();

#endif // SQLTOAST_DEBUG

} // namespace sqltoast
