/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "drop_schema.h"
#include "parser/error.h"
#include "parser/sequence.h"
#include "parser/token.h"
#include "statements/drop_schema.h"

namespace sqltoast {

//
// The DROP SCHEMA statement follows this EBNF form for the following SQL
// dialects:
//
// * SQL_DIALECT_ANSI_1992
// * SQL_DIALECT_ANSI_1999
// * SQL_DIALECT_ANSI_2003
//
//   <drop schema statement> ::= DROP SCHEMA <schema name> <drop behaviour>
//
//   <schema name> [::= [ <catalog name> <period> ] <unqualified schema name>
//
//   <drop behaviour> ::= CASCADE | RESTRICT
//

bool parse_drop_schema(parse_context_t& ctx) {
    tokens_t::iterator tok_it = ctx.tokens.begin();
    tokens_t::iterator tok_ident = ctx.tokens.end();
    symbol_t exp_sym = SYMBOL_DROP;
    symbol_t cur_sym = (*tok_it).symbol;
    statements::drop_behaviour_t behaviour = statements::DROP_BEHAVIOUR_CASCADE;

    goto next_token;

    // BEGIN STATE MACHINE

    identifier:
        // We get here after successfully finding DROP followed by SCHEMA. We
        // now need to find the schema identifier
        if (ctx.at_end(tok_it)) {
            goto eos;
        }
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            tok_ident = tok_it++;
            goto drop_behaviour_or_statement_ending;
        }
        goto err_expect_identifier;
        SQLTOAST_UNREACHABLE();
    err_expect_identifier:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            tok_it++;
            std::stringstream estr;
            cur_sym = (*tok_it).symbol;
            estr << "Expected <identifier> after DROP SCHEMA but found "
                 << symbol_map::to_string(cur_sym);
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    drop_behaviour_or_statement_ending:
        // We get here after successfully parsing the <schema name> element,
        // which must be followed by either a statement ending or a <drop
        // behaviour clause>
        if (ctx.at_end(tok_it)) {
            goto push_statement;
        }

        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_CASCADE || cur_sym == SYMBOL_RESTRICT) {
            if (cur_sym == SYMBOL_RESTRICT) {
                behaviour = statements::DROP_BEHAVIOUR_RESTRICT;
            }
            tok_it++;
        }
        goto statement_ending;
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
            auto stmt_p = std::make_unique<statements::drop_schema_t>(schema_ident, behaviour);
            ctx.result.statements.emplace_back(std::move(stmt_p));
            return true;
        }
    eos:
        if (exp_sym == SYMBOL_DROP || exp_sym == SYMBOL_SCHEMA) {
            // Reached the end of the token stack and never found the
            // DROP SCHEMA so just return false
            return false;
        }
        {
            // Reached the end of the token stream after already finding the
            // DROP and SCHEMA symbols. Return a syntax error.
            parse_position_t err_pos = (*tok_it).lexeme.start;
            std::stringstream estr;
            estr << "Expected <schema_name> but found EOS" << std::endl;
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
            case SYMBOL_DROP:
                if (exp_sym == SYMBOL_DROP) {
                    exp_sym = SYMBOL_SCHEMA;
                }
                goto next_token;
            case SYMBOL_SCHEMA:
                if (exp_sym == SYMBOL_SCHEMA) {
                    exp_sym = SYMBOL_IDENTIFIER;
                    goto identifier;
                }
                goto next_token;
            default:
                return false;
        }
        SQLTOAST_UNREACHABLE();
}

} // namespace sqltoast
