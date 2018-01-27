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
        tokens_t::iterator tok_it,
        std::vector<std::unique_ptr<column_definition_t>>& column_defs) {
    tokens_t::iterator tok_ident = ctx.tokens.end();
    symbol_t cur_sym = (*tok_it).symbol;

    goto expect_column_name;

    // BEGIN STATE MACHINE

    expect_column_name:
        // We start here. The first component of the column definition is the
        // identifier that indicates the column name.
        tok_it = ctx.skip_comments(tok_it);
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            tok_ident = tok_it++;
            goto push_column_def;
        }
        // Just return false, since callers could be looking for
        // a constraint definition
        return false;
        SQLTOAST_UNREACHABLE();
    push_column_def:
        {
            ctx.trim_to(tok_it);
            if (ctx.opts.disable_statement_construction)
                return true;
            identifier_t col_name((*tok_ident).start, (*tok_ident).end);
            auto cdef_p = std::make_unique<column_definition_t>(col_name);
            auto dtd_p = std::make_unique<character_string_t>();
            (*cdef_p).data_type = std::move(dtd_p);
            column_defs.emplace_back(std::move(cdef_p));
            return true;
        }
        SQLTOAST_UNREACHABLE();
}

} // namespace sqltoast
