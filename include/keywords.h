/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_KEYWORDS_H
#define SQLTOAST_KEYWORDS_H

#include <vector>
#include <string>

#include "parse.h"
#include "symbols.h"

namespace sqltoast {

typedef std::pair<symbol, std::string> kw_jump_table_entry_t;
typedef std::vector<kw_jump_table_entry_t> kw_jump_table_t;

struct kw_jump_tables {
    // There is a separate list of jump table entries for each lead character
    static kw_jump_table_t c;
    static kw_jump_table_t d;
};

kw_jump_table_t _init_kw_jump_table(char c);

// Moves the supplied parser context's cursor to the next keyword found in the
// context's input stream and sets the context's current symbol to the found
// keyword symbol. Returns whether a keyword was found.
bool keyword(parser_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_KEYWORDS_H */
