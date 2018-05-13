/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

bool parse_grant(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t on;
    lexeme_t to;
    bool with_grant_option = false;
    std::vector<std::unique_ptr<grant_action_t>> privileges;
    symbol_t cur_sym;
    cur_tok = lex.next(); // Consumer the GRANT symbol...

    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ALL) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_PRIVILEGES)
            goto err_expect_privileges;
        cur_tok = lex.next();
        goto expect_on;
    }
    goto process_privilege;
err_expect_privileges:
    expect_error(ctx, SYMBOL_PRIVILEGES);
    return false;
process_privilege:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_SELECT:
            cur_tok = lex.next();
            privileges.emplace_back(std::make_unique<grant_action_t>(GRANT_ACTION_TYPE_SELECT));
            break;
        case SYMBOL_DELETE:
            cur_tok = lex.next();
            privileges.emplace_back(std::make_unique<grant_action_t>(GRANT_ACTION_TYPE_DELETE));
            break;
        case SYMBOL_USAGE:
            cur_tok = lex.next();
            privileges.emplace_back(std::make_unique<grant_action_t>(GRANT_ACTION_TYPE_USAGE));
            break;
        default:
            goto err_expect_any_action;
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_privilege;
    }
    goto expect_on;
err_expect_any_action:
    expect_any_error(ctx,
        {
            SYMBOL_SELECT, SYMBOL_DELETE, SYMBOL_INSERT,
            SYMBOL_UPDATE, SYMBOL_REFERENCES, SYMBOL_USAGE
        }
    );
    return false;
expect_on:
    // We get here after successfully parsing the privileges list or the ALL
    // PRIVILEGES phrase. We now expect to find the ON <object name> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_ON)
        goto err_expect_on;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    on = cur_tok.lexeme;
    cur_tok = lex.next();
    goto expect_to;
err_expect_on:
    expect_error(ctx, SYMBOL_ON);
    return false;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_to:
    // We get here after successfully parsing the ON <object name> clause. We
    // now expect to find the TO <grantee> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_TO)
        goto err_expect_to;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER && cur_sym != SYMBOL_PUBLIC)
        goto err_expect_identifier_or_public;
    if (cur_sym != SYMBOL_PUBLIC)
        to = cur_tok.lexeme;
    cur_tok = lex.next();
    goto optional_with_clause;
err_expect_to:
    expect_error(ctx, SYMBOL_TO);
    return false;
err_expect_identifier_or_public:
    expect_any_error(ctx, {SYMBOL_IDENTIFIER, SYMBOL_PUBLIC});
    return false;
optional_with_clause:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_WITH) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_GRANT)
            goto err_expect_grant;
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_OPTION)
            goto err_expect_option;
        cur_tok = lex.next();
        with_grant_option = true;
    }
    goto statement_ending;
err_expect_grant:
    expect_error(ctx, SYMBOL_GRANT);
    return false;
err_expect_option:
    expect_error(ctx, SYMBOL_OPTION);
    return false;
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
    out = std::make_unique<grant_statement_t>(on, to, with_grant_option, privileges);
    return true;
}

} // namespace sqltoast
