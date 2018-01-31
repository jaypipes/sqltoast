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
// So far, we only implement up to the <schema name clause> part of the grammar.
//
// TODO(jaypipes): Implement the <schema element> list
//

bool require_default_charset_clause(parse_context_t& ctx, tokens_t::iterator& cur_tok) {
    // Current token is pointing at SYMBOL_DEFAULT, so parse the <default
    // character set clause> element or set a syntax error
    static const symbol_t exp_sym_seq[3] = {
        SYMBOL_CHARACTER,
        SYMBOL_SET,
        SYMBOL_IDENTIFIER
    };
    return follows_sequence(ctx, cur_tok, exp_sym_seq, 3);
}

bool parse_create_schema(parse_context_t& ctx) {
    tokens_t::iterator tok_it = ctx.tokens.begin();
    tokens_t::iterator tok_ident = ctx.tokens.end();
    bool found_authz = false;
    tokens_t::iterator tok_authz_ident;
    bool found_default_charset = false;
    tokens_t::iterator tok_default_charset_ident;
    symbol_t exp_sym = SYMBOL_CREATE;
    symbol_t cur_sym = (*tok_it).symbol;

    goto next_token;

    // BEGIN STATE MACHINE

    identifier_or_authorization_clause:
        // We get here after successfully finding CREATE followed by SCHEMA. We
        // now need to find either an identifier or the schema authorization
        // clause
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            tok_ident = tok_it++;
            goto authz_or_statement_ending;
        }
        goto authorization_clause;
        SQLTOAST_UNREACHABLE();
    default_charset_clause:
        if (require_default_charset_clause(ctx, tok_it)) {
            found_default_charset = true;
            tok_default_charset_ident = tok_it++;
            goto statement_ending;
        }
        return false;
        SQLTOAST_UNREACHABLE();
    authorization_clause:
        // The next non-comment token MUST be an identifier for the
        // AUTHORIZATION clause
        if (ctx.at_end(tok_it)) {
            goto err_expect_authz_identifier;
        }
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            found_authz = true;
            tok_authz_ident = tok_it++;
            goto default_charset_or_statement_ending;
        }
        goto err_expect_authz_identifier;
        SQLTOAST_UNREACHABLE();
    err_expect_authz_identifier:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            tok_it++;
            std::stringstream estr;
            if (ctx.at_end(tok_it)) {
                estr << "Expected <identifier> after AUTHORIZATION keyword but found EOS";
            } else {
                cur_sym = (*tok_it).symbol;
                estr << "Expected <identifier> after AUTHORIZATION keyword but found "
                     << symbol_map::to_string(cur_sym);
            }
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    default_charset_or_statement_ending:
        // We get here after successfully parsing the <schema name clause>,
        // which must be followed by either a statement ending or a <default
        // character set clause>
        if (ctx.at_end(tok_it)) {
            goto push_statement;
        }

        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_DEFAULT) {
            goto default_charset_clause;
        }
        goto statement_ending;
    authz_or_statement_ending:
        // We get here if we already have the CREATE SCHEMA <identifier> and
        // now we are expecting either the end of the statement OR an
        // AUTHORIZATION clause
        if (ctx.at_end(tok_it)) {
            goto push_statement;
        }

        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_SEMICOLON) {
            // skip-consume the semicolon token
            tok_it++;
            goto push_statement;
        } else if (cur_sym == SYMBOL_AUTHORIZATION) {
            tok_it++;
            goto authorization_clause;
        } else if (cur_sym == SYMBOL_DEFAULT) {
            goto default_charset_clause;
        }
        {
            parse_position_t err_pos = (*tok_it).lexeme.start;
            std::stringstream estr;
            estr << "Expected EOS, SEMICOLON, <default character set clause> "
                 << " or <schema_authorization_clause> but found "
                 << symbol_map::to_string(cur_sym) << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    statement_ending:
        // We get here if we have already successfully processed the CREATE
        // SCHEMA statement and are expecting EOS or SEMICOLON as the next
        // non-comment token
        if (ctx.at_end(tok_it)) {
            goto push_statement;
        }

        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_SEMICOLON) {
            // skip-consume the semicolon token
            tok_it++;
            goto push_statement;
        }
        {
            parse_position_t err_pos = (*tok_it).lexeme.start;
            std::stringstream estr;
            estr << "Expected EOS or SEMICOLON but found "
                 << symbol_map::to_string(cur_sym) << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    push_statement:
        {
            ctx.trim_to(tok_it);
            if (ctx.opts.disable_statement_construction)
                return true;
            identifier_t schema_ident((*tok_ident).lexeme);
            auto stmt_p = std::make_unique<statements::create_schema_t>(schema_ident);
            if (found_authz) {
                auto p_authz = std::make_unique<identifier_t>((*tok_authz_ident).lexeme);
                (*stmt_p).authorization_identifier = std::move(p_authz);
            }
            if (found_default_charset) {
                auto p_def_charset = std::make_unique<identifier_t>((*tok_default_charset_ident).lexeme);
                (*stmt_p).default_charset = std::move(p_def_charset);
            }
            ctx.result.statements.emplace_back(std::move(stmt_p));
            return true;
        }
    eos:
        if (exp_sym == SYMBOL_CREATE || exp_sym == SYMBOL_SCHEMA) {
            // Reached the end of the token stack and never found the
            // CREATE SCHEMA so just return false
            return false;
        }
        {
            // Reached the end of the token stream after already finding the
            // CREATE and SCHEMA symbols. Return a syntax error.
            parse_position_t err_pos = (*tok_it).lexeme.start;
            std::stringstream estr;
            estr << "Expected <schema_name_clause> but found EOS";
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    next_token:
        if (ctx.at_end(tok_it)) {
            goto eos;
        }
        cur_sym = (*tok_it).symbol;
        tok_it++;
        switch (cur_sym) {
            case SYMBOL_CREATE:
                if (exp_sym == SYMBOL_CREATE) {
                    exp_sym = SYMBOL_SCHEMA;
                }
                goto next_token;
            case SYMBOL_SCHEMA:
                if (exp_sym == SYMBOL_SCHEMA) {
                    goto identifier_or_authorization_clause;
                }
                goto next_token;
            default:
                return false;
        }
        SQLTOAST_UNREACHABLE();
}

} // namespace sqltoast
