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
// TODO(jaypipes): Handle <column constraint definition> clause

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
    goto optional_collate;
optional_collate:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COLLATE) {
        cur_tok = lex.next();
        if (! parse_collate_clause(ctx, cur_tok, *cdef_p))
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
//
//  <datetime value function> ::=
//      <current date value function>
//      | <current time value function>
//      | <current timestamp value function>
//
//  <current date value function> ::= CURRENT_DATE
//
//  <current time value function> ::=
//      CURRENT_TIME [ <left paren> <time precision> <right paren> ]
//
//  <current timestamp value function> ::=
//      CURRENT_TIMESTAMP [ <left paren> <timestamp precision> <right paren> ]
bool parse_default_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    default_type_t default_type;
    size_t prec;

    // BEGIN STATE MACHINE

    switch (cur_sym) {
        case SYMBOL_NULL:
            default_type = DEFAULT_TYPE_NULL;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_USER:
            default_type = DEFAULT_TYPE_USER;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_CURRENT_USER:
            default_type = DEFAULT_TYPE_CURRENT_USER;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_SESSION_USER:
            default_type = DEFAULT_TYPE_SESSION_USER;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_SYSTEM_USER:
            default_type = DEFAULT_TYPE_SYSTEM_USER;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_CURRENT_DATE:
            default_type = DEFAULT_TYPE_CURRENT_DATE;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_CURRENT_TIME:
            default_type = DEFAULT_TYPE_CURRENT_TIME;
            cur_tok = lex.next();
            goto optional_precision;
        case SYMBOL_CURRENT_TIMESTAMP:
            default_type = DEFAULT_TYPE_CURRENT_TIMESTAMP;
            cur_tok = lex.next();
            goto optional_precision;
        default:
            if (cur_tok.is_literal()) {
                default_type = DEFAULT_TYPE_LITERAL;
                cur_tok = lex.next();
                goto push_descriptor;
            }
            return false;
    }
optional_precision:
    // We get here after getting either a CURRENT_TIME or CURRENT_TIMESTAMP
    // symbol. This can be followed by an optional LPAREN <precision> RPAREN.
    // Since the length specifier is an identical structure, we use that...
    if (! parse_length_specifier(ctx, cur_tok, &prec))
        return false;
    goto push_descriptor;
push_descriptor:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        std::unique_ptr<default_descriptor_t> dd_p;
        dd_p = std::move(std::make_unique<default_descriptor_t>(default_type, cur_tok.lexeme, prec));
        column_def.default_descriptor = std::move(dd_p);
        return true;
    }
}

//  <collate clause> ::= COLLATE <qualified identifier>
bool parse_collate_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t collate_ident;

    // We get here after getting the COLLATE symbol while processing the column
    // definition. We expect an identifier (the collation) at this point.
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;

    collate_ident = cur_tok.lexeme;
    cur_tok = lex.next();
    goto push_descriptor;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_descriptor:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        column_def.collate = std::move(std::make_unique<identifier_t>(collate_ident));
        return true;
    }
}

} // namespace sqltoast
