/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "parser/error.h"
#include "parser/parse.h"
#include "parser/symbol.h"
#include "parser/token.h"

namespace sqltoast {

static const size_t NUM_ALTER_STATEMENT_PARSERS = 1;
static const parse_func_t alter_statement_parsers[1] = {
    &parse_alter_table
};
static const size_t NUM_CREATE_STATEMENT_PARSERS = 3;
static const parse_func_t create_statement_parsers[3] = {
    &parse_create_table,
    &parse_create_view,
    &parse_create_schema
};
static const size_t NUM_DROP_STATEMENT_PARSERS = 3;
static const parse_func_t drop_statement_parsers[3] = {
    &parse_drop_table,
    &parse_drop_view,
    &parse_drop_schema
};
static const size_t NUM_DELETE_STATEMENT_PARSERS = 1;
static const parse_func_t delete_statement_parsers[1] = {
    &parse_delete
};
static const size_t NUM_SELECT_STATEMENT_PARSERS = 1;
static const parse_func_t select_statement_parsers[1] = {
    &parse_select
};
static const size_t NUM_INSERT_STATEMENT_PARSERS = 1;
static const parse_func_t insert_statement_parsers[1] = {
    &parse_insert
};
static const size_t NUM_UPDATE_STATEMENT_PARSERS = 1;
static const parse_func_t update_statement_parsers[1] = {
    &parse_update
};
static const size_t NUM_TRX_STATEMENT_PARSERS = 2;
static const parse_func_t trx_statement_parsers[2] = {
    &parse_commit,
    &parse_rollback
};
static const size_t NUM_GRANT_STATEMENT_PARSERS = 1;
static const parse_func_t grant_statement_parsers[1] = {
    &parse_grant
};

void parse_statement(parse_context_t& ctx) {
    // Assumption: the current token will be a keyword
    lexer_t& lex = ctx.lexer;
    token_t& cur_tok = lex.current_token;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<statement_t> stmt_p;

    size_t num_parsers = 0;
    const parse_func_t* parsers;
    switch (cur_sym) {
        case SYMBOL_ALTER:
            num_parsers = NUM_ALTER_STATEMENT_PARSERS;
            parsers = alter_statement_parsers;
            break;
        case SYMBOL_CREATE:
            num_parsers = NUM_CREATE_STATEMENT_PARSERS;
            parsers = create_statement_parsers;
            break;
        case SYMBOL_DELETE:
            num_parsers = NUM_DELETE_STATEMENT_PARSERS;
            parsers = delete_statement_parsers;
            break;
        case SYMBOL_DROP:
            num_parsers = NUM_DROP_STATEMENT_PARSERS;
            parsers = drop_statement_parsers;
            break;
        case SYMBOL_SELECT:
            num_parsers = NUM_SELECT_STATEMENT_PARSERS;
            parsers = select_statement_parsers;
            break;
        case SYMBOL_INSERT:
            num_parsers = NUM_INSERT_STATEMENT_PARSERS;
            parsers = insert_statement_parsers;
            break;
        case SYMBOL_UPDATE:
            num_parsers = NUM_UPDATE_STATEMENT_PARSERS;
            parsers = update_statement_parsers;
            break;
        case SYMBOL_COMMIT:
        case SYMBOL_ROLLBACK:
            num_parsers = NUM_TRX_STATEMENT_PARSERS;
            parsers = trx_statement_parsers;
            break;
        case SYMBOL_GRANT:
            num_parsers = NUM_GRANT_STATEMENT_PARSERS;
            parsers = grant_statement_parsers;
            break;
        default:
        {
            std::stringstream estr;
            estr << "Failed to recognize any valid SQL statement." << std::endl;
            create_syntax_error_marker(ctx, estr);
            return;
        }
    }

    size_t x = 0;
    while (ctx.result.code == PARSE_OK && x < num_parsers) {
        if (parsers[x++](ctx, cur_tok, stmt_p))
            goto push_statement;
    }
    if (ctx.result.code == PARSE_SYNTAX_ERROR) {
        // Already have a nicely-formatted error, so just return
        return;
    } else {
        std::stringstream estr;
        estr << "Failed to recognize any valid SQL statement." << std::endl;
        create_syntax_error_marker(ctx, estr);
    }
push_statement:
    if (ctx.opts.disable_statement_construction)
        return;
    ctx.result.statements.emplace_back(std::move(stmt_p));
}

} // namespace sqltoast
