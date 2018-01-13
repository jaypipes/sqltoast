/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_SYMBOLS_H
#define SQLTOAST_SYMBOLS_H

#include <map>
#include <string>

namespace sqltoast {

enum symbol {
    // keywords
    CREATE,
    DATABASE,

    // terminals
    DOUBLE_SLASH, // //
    EOS,  // end of the parsed string (\0)
    LPAREN, // (
    NEWLINE, // \n
    RPAREN, // )
    SEMICOLON, // ;
    SLASH, // /
    SOS,  // start of the parsed string

    // non-terminals
    IDENTIFIER, // any identifier/name of a DB object

    // an invalid symbol
    UNKNOWN
};

typedef std::map<symbol, std::string> symbol_map_t;
struct symbol_map {
    static symbol_map_t m;
};

// Given a symbol, returns the string representation of the symbol
const std::string& string_from_symbol(symbol s);

symbol_map_t _init_symbol_map();

} // namespace sqltoast

#endif /* SQLTOAST_SYMBOLS_H */
