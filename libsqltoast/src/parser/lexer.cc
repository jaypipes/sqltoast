/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "context.h"
#include "error.h"
#include "parser/comment.h"
#include "parser/identifier.h"
#include "parser/lexer.h"
#include "parser/literal.h"
#include "parser/keyword.h"
#include "parser/special.h"
#include "parser/token.h"

namespace sqltoast {

parse_position_t skip_simple_comments(parse_position_t cursor) {
    parse_position_t start = cursor;
    if (*cursor != '-')
        return start;

    cursor++;
    if (*cursor != '-') {
        return start;
    }

    // The comment content is from the cursor until we find a newline or EOS
    do {
        cursor++;
    } while (*cursor != '\0' && *cursor != '\n');
    return cursor;
}

parse_position_t skip(parse_position_t cur) {
    // Advance the lexer's cursor over any whitespace or simple comments
    while (std::isspace(*cur))
        cur++;
    cur = skip_simple_comments(cur);
    return cur;
}

static size_t NUM_TOKENIZERS = 5;
static tokenize_func_t tokenizers[5] = {
    &token_comment,
    &token_special,
    &token_keyword,
    &token_literal,
    &token_identifier
};

parse_position_t lexer_t::peek_from(parse_position_t cur, symbol_t* found) const {
    // Advance the lexer's cursor over any whitespace or simple comments
    *found = SYMBOL_EOS;
    cur = skip(cur);
    if (cur >= end)
        return cur;

    for (size_t x = 0; x < NUM_TOKENIZERS; x++) {
        auto tok_res = tokenizers[x](cur, end);
        if (tok_res.code == TOKEN_NOT_FOUND)
            continue;
        if (tok_res.code == TOKEN_FOUND) {
            *found = tok_res.token.symbol;
            cur = tok_res.token.lexeme.end;
            break;
        }
        // There was an error in tokenizing... return some error marker?
        break;
    }
    return cur;
}

symbol_t lexer_t::peek() const {
    parse_position_t cur = cursor;
    cur = skip(cur);
    if (cur >= end)
        return SYMBOL_EOS;

    for (size_t x = 0; x < NUM_TOKENIZERS; x++) {
        auto tok_res = tokenizers[x](cur, end);
        if (tok_res.code == TOKEN_NOT_FOUND)
            continue;
        if (tok_res.code == TOKEN_FOUND)
            return tok_res.token.symbol;
        // There was an error in tokenizing... return some error marker?
        return SYMBOL_EOS;
    }
    // No more tokens
    return SYMBOL_EOS;
}

token_t& lexer_t::next() {
    parse_position_t cur = cursor;
    cur = skip(cur);
    if (cur >= end) {
        current_token.symbol = SYMBOL_EOS;
        current_token.lexeme.start = end;
        current_token.lexeme.end = end;
        cursor = cur;
        return current_token;
    }

    for (size_t x = 0; x < NUM_TOKENIZERS; x++) {
        auto tok_res = tokenizers[x](cur, end);
        tokenize_result_code_t res_code = tok_res.code;
        if (res_code == TOKEN_NOT_FOUND)
            continue;
        // If there was an error in finding the next token, then tok_res.token
        // will contain SYMBOL_ERROR and the lexeme will point to the place
        // where the lexing error occurring.
        current_token = tok_res.token;
        cur = tok_res.token.lexeme.end;
        break;
    }
    cursor = cur;
    return current_token;
}

} // namespace sqltoast
