/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <cctype>

#include "internal/symbols.h"
#include "parse.h"
#include "parser/identifier.h"
#include "parser/keyword.h"
#include "parser/statements/create_database.h"

namespace sqltoast {

bool accept(parse_context_t& ctx, symbol s) {
    if (ctx.current_symbol == s) {
        return true;
    }
    return false;
}

bool expect(parse_context_t& ctx, symbol s) {
    if (accept(ctx, s))
        return true;

    char estr[200];
    sprintf(estr, "Expected symbol %s but found %s",
            string_from_symbol(s).data(),
            string_from_symbol(ctx.current_symbol).data());
    ctx.result.error.assign(estr);
    return false;
}

bool simple_terminal(parse_context_t& ctx) {
    // Search through the single-character terminal symbols first
    switch (*ctx.cursor++) {
        case '\n':
            ctx.current_symbol = NEWLINE;
            break;
        case '(':
            ctx.current_symbol = LPAREN;
            break;
        case ')':
            ctx.current_symbol = RPAREN;
            break;
        case ';':
            ctx.current_symbol = SEMICOLON;
            break;
        case '/':
            ctx.current_symbol = SLASH;
            // Peak at the next character and determine if it's a double slash
            if (ctx.end_pos != ctx.cursor) {
                if (*ctx.cursor == '/') {
                    ctx.current_symbol = DOUBLE_SLASH;
                    ctx.cursor++;
                }
            }
            break;
        case '\0':
            ctx.current_symbol = EOS;
            break;
        default:
            ctx.cursor--; // rewind... we didn't find a single-char terminal
            return false;
    }
    return true;
}

bool identifier(parse_context_t& ctx) {
    // Return true if the context's cursor is at the beginning of something
    // that can be considered an identifier.
    // Skip cursor to the next whitespace
    parse_position_t start = ctx.cursor;
    while (! std::isspace(*ctx.cursor) && *ctx.cursor != ';' && ctx.end_pos != ctx.cursor) {
        ctx.cursor++;
    }
    // if we went more than a single non-space character, that's an
    // identifier...
    bool res = (start != ctx.cursor);
    if (res)
        ctx.current_symbol = IDENTIFIER;
    return res;
}

void next_symbol(parse_context_t& ctx) {
    skip_ws(ctx);
    if (simple_terminal(ctx))
        return;

    if (keyword(ctx))
        return;

    if (parse_identifier(ctx))
        return;

    ctx.current_symbol = UNKNOWN;
    return;
}

bool comment(parse_context_t& ctx) {
    if (! accept(ctx, DOUBLE_SLASH))
        return false;

    std::string val;

    // A comment in SQL is a double slash followed by any number of characters
    // and a newline. Consecutive lines of comments should be concatenated into
    // the same comment string value.
    val = consume_to_next(ctx, NEWLINE);
    return true;
}

const std::string consume_to_next(parse_context_t& ctx, symbol s) {
    parse_position_t start = ctx.cursor;

    while (! accept(ctx, s) || accept(ctx, EOS)) {
        skip_ws(ctx);
        next_symbol(ctx);
    }
    return std::string(start, parse_position_t(ctx.cursor));
}

void skip_ws(parse_context_t& ctx) {
    while (std::isspace(*ctx.cursor))
        ctx.cursor++;
    return;
}

parse_result_t parse(parse_input_t& subject) {
    parse_result_t res;
    res.code = PARSE_SYNTAX_ERROR;
    parse_context_t ctx(res, subject);

    next_symbol(ctx);
    switch (ctx.current_symbol) {
        case CREATE:
            next_symbol(ctx);
            if (accept(ctx, DATABASE)) {
                if (parse_create_database(ctx)) {
                    res.code = PARSE_SUCCESS;
                }
            }
        case EOS:
            break;
        default:
            break;
    }

    return res;
}

} // namespace sqltoast
