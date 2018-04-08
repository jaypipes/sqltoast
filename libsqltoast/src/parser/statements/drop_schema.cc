/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast/sqltoast.h"

#include "parser/error.h"
#include "parser/parse.h"
#include "parser/sequence.h"
#include "parser/token.h"

namespace sqltoast {

// <drop schema statement> ::= DROP SCHEMA <schema name> <drop behaviour>
//
// <schema name> [::= [ <catalog name> <period> ] <unqualified schema name>
//
// <drop behaviour> ::= CASCADE | RESTRICT
bool parse_drop_schema(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t schema_name;
    symbol_t cur_sym;
    drop_behaviour_t behaviour = DROP_BEHAVIOUR_CASCADE;

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
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
        schema_name = cur_tok.lexeme;
        cur_tok = lex.next();
        goto drop_behaviour_or_statement_ending;
    }
    goto err_expect_identifier;
    SQLTOAST_UNREACHABLE();
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
drop_behaviour_or_statement_ending:
    // We get here after successfully parsing the <schema name> element,
    // which must be followed by either a statement ending or a <drop
    // behaviour clause>
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_CASCADE || cur_sym == SYMBOL_RESTRICT) {
        if (cur_sym == SYMBOL_RESTRICT)
            behaviour = DROP_BEHAVIOUR_RESTRICT;
        cur_tok = lex.next();
    }
    goto statement_ending;
statement_ending:
    // We get here after successfully parsing the statement and now expect
    // either the end of parse content or a semicolon to indicate end of
    // statement.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS)
        goto push_statement;
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON});
    return false;
push_statement:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<drop_schema_statement_t>(
            schema_name, behaviour);
    return true;
}

} // namespace sqltoast
