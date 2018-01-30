/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_CONTEXT_H
#define SQLTOAST_PARSER_CONTEXT_H

#include <deque>
#include <memory>
#include <vector>

#include "sqltoast.h"

#include "parser/lexer.h"
#include "parser/token.h"

namespace sqltoast {

typedef std::deque<token_t> tokens_t;

typedef struct parse_context {
    parse_result_t& result;
    parse_options_t& opts;
    lexer_t lexer;
    tokens_t tokens;
    parse_context(parse_result_t& result, parse_options_t& opts, parse_input_t& subject) :
        result(result),
        opts(opts),
        lexer(subject)
    {}
    inline void push_token(token_t& tok) {
        tokens.emplace_back(tok);
    }
    // NOTE(jaypipes): should we instead just have the context store a marker
    // for the last processed token instead of adjusting the token stack
    // itself?
    inline void trim_to(tokens_t::const_iterator pos) {
        if (pos == tokens.end()) {
            tokens.clear();
        } else {
            tokens.erase(tokens.begin(), pos);
        }
    }
    inline bool at_end(tokens_t::const_iterator tok_it) const {
        return tok_it == tokens.end();
    }
} parse_context_t;

// Attempts to find the next token in the parse context. If a token was found,
// returns true, else false. If true, the parse context's tokens stack will
// have had a token pushed onto it.
bool next_token(parse_context_t& ctx);

// Run through the subject string to parse, creating a stack of token_t's on
// the parse context.
void tokenize(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_CONTEXT_H */
