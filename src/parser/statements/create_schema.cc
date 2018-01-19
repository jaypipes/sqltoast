/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "create_schema.h"
#include "parser/error.h"
#include "parser/token.h"

namespace sqltoast {

//
// The CREATE SCHEMA statement follows this EBNF form for ANSI-2003 SQL:
//
// CREATE SCHEMA <schema_name_clause> [ <schema_character_set_or_path> ]
//
// <schema_name_clause> :=
//   <identifier>
//   | AUTHORIZATION <schema_authorization_identifier>
//   | <identifier> AUTHORIZATION <schema_authorization_identifier>
//
// <schema_character_set_or_path> :=
//   DEFAULT CHARACTER SET <charset_spec>
//
bool parse_create_schema(parse_context_t& ctx) {
    if (ctx.tokens.size() < 2)
        return false;

    // BEGIN STATE MACHINE

    std::vector<token_t>::iterator tok_it = ctx.tokens.begin();
    symbol_t exp_sym = SYMBOL_CREATE;
    symbol_t cur_sym = (*tok_it).symbol;
    goto next_token;

    if (0) {
    identifier_or_authorization_clause:
        // We get here after successfully finding CREATE followed by SCHEMA. We
        // now need to find either an identifier or the schema authorization
        // clause
        cur_sym = (*tok_it).symbol;
        tok_it++;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            goto statement_ending;
        }
        goto next_token;
        SQLTOAST_UNREACHABLE();
    statement_ending:
        // We get here when we're expecting the next symbol to be statement
        // ending, so either a semicolon or EOS
        tok_it++;
        cur_sym = (*tok_it).symbol;
        if (tok_it == ctx.tokens.end() || cur_sym == SYMBOL_SEMICOLON) {
            ctx.result.code = PARSE_SUCCESS;
            return true;
        }
        {
            parse_position_t err_pos = (*tok_it).start;
            std::stringstream estr;
            estr << "Expected EOS or SEMICOLON but found "
                 << symbol_map::to_string(cur_sym);
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    eos:
        if (exp_sym == SYMBOL_CREATE || exp_sym == SYMBOL_SCHEMA) {
            // Reached the end of the token stack and never found the
            // CREATE SCHEMA so just return false
            return false;
        }
        {
            // Reached the end of the token stream after already finding the
            // CREATE and SCHEMA symbols. Return a syntax error.
            parse_position_t err_pos = (*tok_it).start;
            std::stringstream estr;
            estr << "Expected <schema_name_clause> but found EOS";
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    next_token:
        if (tok_it == ctx.tokens.end()) {
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
            case SYMBOL_COMMENT:
                goto next_token;
            default:
                return false;
        }
        SQLTOAST_UNREACHABLE();
    }
    SQLTOAST_UNREACHABLE();
}

} // namespace sqltoast
