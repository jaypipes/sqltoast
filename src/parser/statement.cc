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

static const size_t NUM_CREATE_STATEMENT_PARSERS = 2;
static const parse_func_t create_statement_parsers[2] = {
    &parse_create_table,
    &parse_create_schema
};
static const size_t NUM_DROP_STATEMENT_PARSERS = 1;
static const parse_func_t drop_statement_parsers[1] = {
    &parse_drop_schema
};
static const size_t NUM_SELECT_STATEMENT_PARSERS = 1;
static const parse_func_t select_statement_parsers[1] = {
    &parse_select
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
        case SYMBOL_CREATE:
        {
            num_parsers = NUM_CREATE_STATEMENT_PARSERS;
            parsers = create_statement_parsers;
            break;
        }
        case SYMBOL_DROP:
        {
            num_parsers = NUM_DROP_STATEMENT_PARSERS;
            parsers = drop_statement_parsers;
            break;
        }
        case SYMBOL_SELECT:
        {
            num_parsers = NUM_SELECT_STATEMENT_PARSERS;
            parsers = select_statement_parsers;
            break;
        }
        default:
            break;
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
