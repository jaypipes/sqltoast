/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>

#include "error.h"

namespace sqltoast {

// TODO(jaypipes): Limit the amount of output to something like 200 characters
// before the syntax error position...
void create_syntax_error_marker(parse_context_t& ctx, std::stringstream& es) {
    lexer_t& lex = ctx.lexer;
    parse_position_t err_pos = lex.current_token.lexeme.start;
    std::string original(lex.start, lex.end);
    std::string location(original);
    auto start_pos = err_pos - lex.start;
    for (auto cur = location.begin(); cur != location.end(); cur++) {
        auto loc_pos = cur - location.begin() + 1;
        if (loc_pos < start_pos) {
            *cur = ' ';
        } else {
            *cur = '^';
        }
    }

    es << original << std::endl << location;
    ctx.result.error.assign(es.str());
    ctx.result.code = PARSE_SYNTAX_ERROR;
}

void expect_error(parse_context_t& ctx, symbol_t expected) {
    std::stringstream es;
    es << "Expected to find " << expected
        << " but found " << ctx.lexer.current_token << std::endl;
    create_syntax_error_marker(ctx, es);
}

void expect_any_error(parse_context_t& ctx, std::initializer_list<symbol_t> expected) {
    std::stringstream es;
    es << "Expected to find one of (";
    size_t num_expected = expected.size();
    size_t x = 0;
    for (auto exp_sym : expected) {
        es << exp_sym;
        if ((x + 1) < num_expected)
            es << "|";
        x++;
    }
    es << ") but found " << ctx.lexer.current_token << std::endl;
    create_syntax_error_marker(ctx, es);
}

} // namespace sqltoast
