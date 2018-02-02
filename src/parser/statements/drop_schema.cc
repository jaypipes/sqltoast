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
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t ident;
    token_t& cur_tok = lex.current_token;
    symbol_t cur_sym;
    statements::drop_behaviour_t behaviour = statements::DROP_BEHAVIOUR_CASCADE;

    // BEGIN STATE MACHINE

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ERROR:
            return false;
        case SYMBOL_SCHEMA:
            cur_tok = lex.next();
            goto expect_identifier;
        default:
            // rewind
            ctx.lexer.cursor = start;
            return false;
    }

    expect_identifier:
        // We get here after successfully finding DROP followed by SCHEMA. We
        // now need to find the schema identifier
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            fill_lexeme(cur_tok, ident);
            cur_tok = lex.next();
            goto drop_behaviour_or_statement_ending;
        }
        goto err_expect_identifier;
        SQLTOAST_UNREACHABLE();
    err_expect_identifier:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
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
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_CASCADE || cur_sym == SYMBOL_RESTRICT) {
            if (cur_sym == SYMBOL_RESTRICT) {
                behaviour = statements::DROP_BEHAVIOUR_RESTRICT;
            }
            cur_tok = lex.next();
        }
        goto statement_ending;
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
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected EOS or SEMICOLON but found "
                 << symbol_map::to_string(cur_sym) << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    push_statement:
        {
            if (ctx.opts.disable_statement_construction)
                return true;
            identifier_t schema_ident(ident);
            auto stmt_p = std::make_unique<statements::drop_schema_t>(schema_ident, behaviour);
            ctx.result.statements.emplace_back(std::move(stmt_p));
            return true;
        }
}

} // namespace sqltoast
