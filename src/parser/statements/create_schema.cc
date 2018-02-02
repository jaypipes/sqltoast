/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "create_schema.h"
#include "parser/error.h"
#include "parser/sequence.h"
#include "parser/token.h"
#include "statements/create_schema.h"

namespace sqltoast {

//
// The CREATE SCHEMA statement follows this EBNF form for the following SQL
// dialects:
//
// * SQL_DIALECT_ANSI_1992
// * SQL_DIALECT_ANSI_1999
// * SQL_DIALECT_ANSI_2003
//
//  <schema definition> ::=
//      CREATE SCHEMA <schema name clause>
//      [ <schema character set specification> ]
//      [ <schema element> ... ]
//
//  <schema name clause> ::=
//      <schema name>
//      | AUTHORIZATION <schema authorization identifier>
//      | <schema name> AUTHORIZATION <schema authorization identifier>
//
//  <schema authorization identifier> ::= <authorization identifier>
//
//  <schema character set specification> ::=
//      DEFAULT CHARACTER SET <character set specification>
//
//  <schema element> ::=
//      <domain definition>
//      | <table definition>
//      | <view definition>
//      | <grant statement>
//      | <assertion definition>
//      | <character set definition>
//      | <collation definition>
//      | <translation definition>
//
// So far, we only implement up to the <schema element> part of the grammar.
//
// TODO(jaypipes): Implement the <schema element> list
//

bool require_default_charset_clause(parse_context_t& ctx) {
    // Current token is pointing at SYMBOL_DEFAULT, so parse the <default
    // character set clause> element or set a syntax error
    static const symbol_t exp_sym_seq[3] = {
        SYMBOL_CHARACTER,
        SYMBOL_SET,
        SYMBOL_IDENTIFIER
    };
    return expect_sequence(ctx, exp_sym_seq, 3);
}

bool parse_create_schema(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    token_t& cur_tok = lex.current_token;
    lexeme_t ident;
    bool found_authz = false;
    lexeme_t authz_ident;
    bool found_default_charset = false;
    lexeme_t default_charset_ident;
    symbol_t cur_sym;

    // BEGIN STATE MACHINE

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ERROR:
            return false;
        case SYMBOL_SCHEMA:
            goto identifier_or_authorization_clause;
        default:
            // rewind
            ctx.lexer.cursor = start;
            return false;
    }

    identifier_or_authorization_clause:
        // We get here after successfully finding CREATE followed by SCHEMA. We
        // now need to find either an identifier or the schema authorization
        // clause
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            fill_lexeme(cur_tok, ident);
            cur_tok = lex.next();
            goto authz_or_statement_ending;
        }
        goto authorization_clause;
        SQLTOAST_UNREACHABLE();
    default_charset_clause:
        if (require_default_charset_clause(ctx)) {
            found_default_charset = true;
            fill_lexeme(cur_tok, default_charset_ident);
            cur_tok = lex.next();
            goto statement_ending;
        }
        return false;
        SQLTOAST_UNREACHABLE();
    authorization_clause:
        // The next non-comment token MUST be an identifier for the
        // AUTHORIZATION clause
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            found_authz = true;
            fill_lexeme(cur_tok, authz_ident);
            cur_tok = lex.next();
            goto default_charset_or_statement_ending;
        }
        goto err_expect_authz_identifier;
        SQLTOAST_UNREACHABLE();
    err_expect_authz_identifier:
        {
            std::stringstream estr;
            estr << "Expected <identifier> after AUTHORIZATION keyword but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    default_charset_or_statement_ending:
        // We get here after successfully parsing the <schema name clause>,
        // which must be followed by either a statement ending or a <default
        // character set clause>
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_EOS)
            goto push_statement;
        if (cur_sym == SYMBOL_DEFAULT)
            goto default_charset_clause;
        goto statement_ending;
    authz_or_statement_ending:
        // We get here if we already have the CREATE SCHEMA <identifier> and
        // now we are expecting either the end of the statement OR an
        // AUTHORIZATION clause
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS) {
            // skip-consume the semicolon token
            cur_tok = lex.next();
            goto push_statement;
        } else if (cur_sym == SYMBOL_AUTHORIZATION) {
            cur_tok = lex.next();
            goto authorization_clause;
        } else if (cur_sym == SYMBOL_DEFAULT) {
            goto default_charset_clause;
        }
        {
            std::stringstream estr;
            estr << "Expected EOS, SEMICOLON, <default character set clause> "
                 << " or <schema_authorization_clause> but found "
                 << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    statement_ending:
        // We get here if we have already successfully processed the CREATE
        // SCHEMA statement and are expecting EOS or SEMICOLON as the next
        // non-comment token
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS) {
            // skip-consume the semicolon token
            cur_tok = lex.next();
            goto push_statement;
        }
        {
            std::stringstream estr;
            estr << "Expected EOS or SEMICOLON but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    push_statement:
        {
            if (ctx.opts.disable_statement_construction)
                return true;
            identifier_t schema_ident(ident);
            auto stmt_p = std::make_unique<statements::create_schema_t>(schema_ident);
            if (found_authz) {
                auto p_authz = std::make_unique<identifier_t>(authz_ident);
                (*stmt_p).authorization_identifier = std::move(p_authz);
            }
            if (found_default_charset) {
                auto p_def_charset = std::make_unique<identifier_t>(default_charset_ident);
                (*stmt_p).default_charset = std::move(p_def_charset);
            }
            ctx.result.statements.emplace_back(std::move(stmt_p));
            return true;
        }
}

} // namespace sqltoast
