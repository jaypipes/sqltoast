/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "parser/parse.h"
#include "parser/error.h"
#include "parser/token.h"

namespace sqltoast {

//
// Parses a column definition, which follows this EBNF grammar for ANSI-92:
//
// <column definition> ::=
//      <column name>
//      { <data type> | <domain name> }
//      [ <default clause> ]
//      [ <column constraint definition> ... ]
//      [ <collate clause> ]
//
// TODO(jaypipes): Handle <data type> clause
// TODO(jaypipes): Handle <default type> clause
// TODO(jaypipes): Handle <column constraint definition> clause
// TODO(jaypipes): Handle <collate> clause
//

bool parse_column_definition(
        parse_context_t& ctx,
        token_t* cur_tok,
        std::vector<std::unique_ptr<column_definition_t>>& column_defs) {
    lexeme_t ident;
    symbol_t cur_sym = cur_tok->symbol;
    std::unique_ptr<column_definition_t> cdef_p;

    // BEGIN STATE MACHINE

    start:
        // We start here. The first component of the column definition is the
        // identifier that indicates the column name.
        if (cur_sym == SYMBOL_IDENTIFIER) {
            fill_lexeme(cur_tok, ident);
            cur_tok = next_token(ctx);
            goto create_column_def;
        }
        // Just return false, since callers could be looking for
        // a constraint definition
        return false;
        SQLTOAST_UNREACHABLE();
    create_column_def:
        {
            identifier_t col_name(ident);
            cdef_p = std::move(std::make_unique<column_definition_t>(col_name));
            if (! parse_data_type_descriptor(ctx, cur_tok, *cdef_p))
                return false;
            goto push_column_def;
        }
        SQLTOAST_UNREACHABLE();
    push_column_def:
        {
            if (ctx.opts.disable_statement_construction)
                return true;
            column_defs.emplace_back(std::move(cdef_p));
            return true;
        }
        SQLTOAST_UNREACHABLE();
}

} // namespace sqltoast
