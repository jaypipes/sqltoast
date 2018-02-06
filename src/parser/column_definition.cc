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
// TODO(jaypipes): Handle <default > clause
// TODO(jaypipes): Handle <column constraint definition> clause
// TODO(jaypipes): Handle <collate> clause
//

bool parse_column_definition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::vector<std::unique_ptr<column_definition_t>>& column_defs) {
    lexer_t& lex = ctx.lexer;
    lexeme_t ident;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<column_definition_t> cdef_p;

    // BEGIN STATE MACHINE

    // We start here. The first component of the column definition is the
    // identifier that indicates the column name.
    if (cur_sym == SYMBOL_IDENTIFIER) {
        fill_lexeme(cur_tok, ident);
        cur_tok = ctx.lexer.next();
        goto create_column_def;
    }
    // Just return false, since callers could be looking for
    // a constraint definition
    return false;

create_column_def:
    {
        identifier_t col_name(ident);
        cdef_p = std::move(std::make_unique<column_definition_t>(col_name));
        if (! parse_data_type_descriptor(ctx, cur_tok, *cdef_p))
            return false;
        goto optional_default;
    }
    SQLTOAST_UNREACHABLE();
optional_default:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_DEFAULT) {
        cur_tok = lex.next();
        if (! parse_default_clause(ctx, cur_tok, *cdef_p))
            return false;
    }
    goto push_column_def;
push_column_def:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        column_defs.emplace_back(std::move(cdef_p));
        return true;
    }
    SQLTOAST_UNREACHABLE();
}

//  <default clause> ::= DEFAULT <default option>
//
//  <default option> ::=
//      <literal>
//      | <datetime value function>
//      | USER
//      | CURRENT_USER
//      | SESSION_USER
//      | SYSTEM_USER
//      | NULL
bool parse_default_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    default_type_t default_type;

    // BEGIN STATE MACHINE

    switch (cur_sym) {
        case SYMBOL_NULL:
            default_type = DEFAULT_TYPE_NULL;
            goto push_descriptor;
        case SYMBOL_USER:
            default_type = DEFAULT_TYPE_USER;
            goto push_descriptor;
        case SYMBOL_CURRENT_USER:
            default_type = DEFAULT_TYPE_CURRENT_USER;
            goto push_descriptor;
        case SYMBOL_SESSION_USER:
            default_type = DEFAULT_TYPE_SESSION_USER;
            goto push_descriptor;
        case SYMBOL_SYSTEM_USER:
            default_type = DEFAULT_TYPE_SYSTEM_USER;
            goto push_descriptor;
        default:
            if (cur_tok.is_literal()) {
                default_type = DEFAULT_TYPE_LITERAL;
                goto push_descriptor;
            }
            return false;
    }
push_descriptor:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        std::unique_ptr<default_descriptor_t> dd_p;
        dd_p = std::move(std::make_unique<default_descriptor_t>(default_type, cur_tok.lexeme));
        column_def.default_descriptor = std::move(dd_p);
        // Advance the next token here now that we've consumed the lexeme from
        // the current token above
        cur_tok = lex.next();
        return true;
    }
}

} // namespace sqltoast
