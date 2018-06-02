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

namespace sqltoast {

// <schema definition> ::=
//     CREATE SCHEMA <schema name clause>
//     [ <schema character set specification> ]
//     [ <schema element> ... ]
//
// <schema name clause> ::=
//     <schema name>
//     | AUTHORIZATION <schema authorization identifier>
//     | <schema name> AUTHORIZATION <schema authorization identifier>
//
// <schema authorization identifier> ::= <authorization identifier>
//
// <schema character set specification> ::=
//     DEFAULT CHARACTER SET <character set specification>
//
// <schema element> ::=
//     <domain definition>
//     | <table definition>
//     | <view definition>
//     | <grant statement>
//     | <assertion definition>
//     | <character set definition>
//     | <collation definition>
//     | <translation definition>
//
// So far, we only implement up to the <schema element> part of the grammar.
//
// TODO(jaypipes): Implement the <schema element> list
bool parse_create_schema(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t schema_name;
    lexeme_t authz_ident;
    lexeme_t default_charset;
    symbol_t cur_sym;

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
        schema_name = cur_tok.lexeme;
        cur_tok = lex.next();
        goto authz_or_statement_ending;
    }
    goto authorization_clause;
default_charset_clause:
{
    static const symbol_t exp_sym_seq[3] = {
        SYMBOL_CHARACTER,
        SYMBOL_SET,
        SYMBOL_IDENTIFIER
    };
    if (! expect_sequence(ctx, exp_sym_seq, 3))
        return false;
    default_charset = cur_tok.lexeme;
    cur_tok = lex.next();
    goto statement_ending;
}
authorization_clause:
    // The next non-comment token MUST be an identifier for the
    // AUTHORIZATION clause
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_IDENTIFIER) {
        authz_ident = cur_tok.lexeme;
        cur_tok = lex.next();
        goto default_charset_or_statement_ending;
    }
    goto err_expect_authz_identifier;
err_expect_authz_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
default_charset_or_statement_ending:
    // We get here after successfully parsing the <schema name clause>,
    // which must be followed by either a statement ending or a <default
    // character set clause>
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_SEMICOLON:
        case SYMBOL_EOS:
            goto push_statement;
        case SYMBOL_DEFAULT:
            cur_tok = lex.next();
            goto default_charset_clause;
        default:
            goto statement_ending;
    }
authz_or_statement_ending:
    // We get here if we already have the CREATE SCHEMA <identifier> and
    // now we are expecting either the end of the statement OR an
    // AUTHORIZATION clause
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_SEMICOLON:
        case SYMBOL_EOS:
            goto push_statement;
        case SYMBOL_AUTHORIZATION:
            cur_tok = lex.next();
            goto authorization_clause;
        case SYMBOL_DEFAULT:
            cur_tok = lex.next();
            goto default_charset_clause;
        default:
            expect_any_error(ctx,
                    {SYMBOL_EOS, SYMBOL_AUTHORIZATION,
                     SYMBOL_DEFAULT, SYMBOL_SEMICOLON});
            return false;
    }
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
    out = std::make_unique<create_schema_statement_t>(
            schema_name, authz_ident, default_charset);
    return true;
}

} // namespace sqltoast
