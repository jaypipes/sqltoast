/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <vector>

#include "context.h"
#include "error.h"
#include "parser/comment.h"
#include "parser/identifier.h"
#include "parser/lexer.h"
#include "parser/literal.h"
#include "parser/keyword.h"
#include "parser/punctuator.h"
#include "parser/token.h"

namespace sqltoast {

void fill_lexeme(token_t* tok, lexeme_t& lexeme) {
    lexeme.start = tok->lexeme.start;
    lexeme.end = tok->lexeme.end;
}

void lexer_t::skip_simple_comments() {
    if (! peek_char('-'))
        return;

    cursor++;
    if (! peek_char('-')) {
        cursor--; // rewind
        return;
    }

    // The comment content is from the cursor until we find a newline or EOS
    do {
        cursor++;
    } while (cursor != end_pos && *cursor != '\n');
}

token_t* next_token(parse_context_t &ctx) {
    lexer_t& lex = ctx.lexer;
    // Advance the lexer's cursor over any whitespace or simple comments
    while (std::isspace(*lex.cursor))
        lex.cursor++;
    lex.skip_simple_comments();
    if (token_comment(ctx))
        return &lex.current_token;
    if (lex.error != ERR_NONE)
        return NULL;
    if (token_punctuator(ctx))
        return &lex.current_token;
    if (token_literal(ctx))
        return &lex.current_token;
    if (token_keyword(ctx))
        return &lex.current_token;
    if (token_identifier(ctx))
        return &lex.current_token;
    return NULL;
}

} // namespace sqltoast
