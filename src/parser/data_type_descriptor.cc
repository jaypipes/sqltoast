/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <stdlib.h>

#include <iostream>
#include <cctype>
#include <sstream>

#include "parser/parse.h"
#include "parser/error.h"
#include "parser/sequence.h"

namespace sqltoast {

//
// Parses the <data type> clause of a column definition, which follows this
// EBNF grammar for ANSI-92 SQL:
//
// <data type> ::=
//     <character string type> [ CHARACTER SET <character set specification> ]
//     | <national character string type>
//     | <bit string type>
//     | <numeric type>
//     | <datetime type>
//     | <interval type>
//
// <character string type> ::=
//     CHARACTER [ <left paren> <length> <right paren> ]
//     | CHAR [ <left paren> <length> <right paren> ]
//     | CHARACTER VARYING [ <left paren> <length> <right paren> ]
//     | CHAR VARYING [ <left paren> <length> <right paren> ]
//     | VARCHAR [ <left paren> <length> <right paren> ]
//
// <length> ::= <unsigned integer>
//
// <national character string type> ::=
//     NATIONAL CHARACTER [ <left paren> <length> <right paren> ]
//     | NATIONAL CHAR [ <left paren> <length> <right paren> ]
//     | NCHAR [ <left paren> <length> <right paren> ]
//     | NATIONAL CHARACTER VARYING [ <left paren> <length> <right paren> ]
//     | NATIONAL CHAR VARYING [ <left paren> <length> <right paren> ]
//     | NCHAR VARYING [ <left paren> <length> <right paren> ]
//
// <bit string type> ::=
//     BIT [ <left paren> <length> <right paren> ]
//     | BIT VARYING [ <left paren> <length> <right paren> ]
//
// <numeric type> ::=
//     <exact numeric type>
//     | <approximate numeric type>
//
// <exact numeric type> ::=
//     NUMERIC [ <left paren> <precision> [ <comma> <scale> ] <right paren> ]
//     | DECIMAL [ <left paren> <precision> [ <comma> <scale> ] <right paren> ]
//     | DEC [ <left paren> <precision> [ <comma> <scale> ] <right paren> ]
//     | INTEGER
//     | INT
//     | SMALLINT
//
// <precision> ::= <unsigned integer>
//
// <scale> ::= <unsigned integer>
//
// <approximate numeric type> ::=
//     FLOAT [ <left paren> <precision> <right paren> ]
//     | REAL
//     | DOUBLE PRECISION
//
// <datetime type> ::=
//     DATE
//     | TIME [ <left paren> <time precision> <right paren> ] [ WITH TIME ZONE ]
//     | TIMESTAMP [ <left paren> <timestamp precision> <right paren> ] [ WITH TIME ZONE ]
//
// <time precision> ::= <time fractional seconds precision>
//
// <time fractional seconds precision> ::= <unsigned integer>
//
// <timestamp precision> ::= <time fractional seconds precision>
//
// <interval type> ::= INTERVAL <interval qualifier>
//
// <interval qualifier> ::=
//     <start field> TO <end field>
//     | <single datetime field>
//
// <start field> ::=
//     <non-second datetime field> [ <left paren> <interval leading field precision> <right paren> ]
//
// <non-second datetime field> ::= YEAR | MONTH | DAY | HOUR | MINUTE
//
// <interval leading field precision> ::= <unsigned integer>
//
// <end field> ::=
//     <non-second datetime field>
//     | SECOND [ <left paren> <interval fractional seconds precision> <right paren> ]
//
// <interval fractional seconds precision> ::= <unsigned integer>
//
// <single datetime field> ::=
//     <non-second datetime field> [ <left paren> <interval leading field precision> <right paren> ]
//     | SECOND [ <left paren> <interval leading field precision> [ <comma> <left paren> <interval fractional seconds precision> ] <right paren> ]
//
// <domain name> ::= <qualified name>
//
// <qualified name> ::= [ <schema name> <period> ] <qualified identifier>
//
// <default clause> ::= DEFAULT <default option>
//
// <default option> ::=
//     <literal>
//     |     <datetime value function>
//     |     USER
//     |     CURRENT_USER
//     |     SESSION_USER
//     |     SYSTEM_USER
//     |     NULL
//

bool parse_data_type_descriptor(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def) {
    symbol_t cur_sym = cur_tok.symbol;

    // BEGIN STATE MACHINE

    // We start here. The first component of the column definition is the
    // identifier that indicates the column name.
    switch (cur_sym) {
        case SYMBOL_CHAR:
        case SYMBOL_CHARACTER:
        case SYMBOL_VARCHAR:
            if (! parse_character_string(ctx, cur_tok, column_def))
                return false;
            goto optional_character_set;
        default:
            goto err_expect_data_type;
    }
err_expect_data_type:
    {
        std::stringstream estr;
        estr << "Expected data type after <column name> but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
optional_character_set:
    // We get here after processing the optional length specifier. After
    // that specifier, there may be an optional CHARACTER SET <character
    // set specification> clause
    {
        symbol_t cur_sym = ctx.lexer.current_token.symbol;
        if (cur_sym == SYMBOL_EOS ||
                cur_sym == SYMBOL_COMMA ||
                cur_sym == SYMBOL_RPAREN)
            return true;
        if (cur_sym == SYMBOL_CHARACTER) {
            goto process_character_set;
        }
    }
    return true;
process_character_set:
    {
        symbol_t exp_sym_seq[3] = {
            SYMBOL_CHARACTER,
            SYMBOL_SET,
            SYMBOL_IDENTIFIER
        };
        if (! expect_sequence(ctx, exp_sym_seq, 3))
            return false;
        // tack the character set onto the char_string_t data type descriptor
        char_string_t* dtd = static_cast<char_string_t*>(column_def.data_type.get());
        return true;
    }
}

//
// <character string type> ::=
//     CHARACTER [ <left paren> <length> <right paren> ]
//     | CHAR [ <left paren> <length> <right paren> ]
//     | CHARACTER VARYING [ <left paren> <length> <right paren> ]
//     | CHAR VARYING [ <left paren> <length> <right paren> ]
//     | VARCHAR [ <left paren> <length> <right paren> ]
//
// <length> ::= <unsigned integer>
bool parse_character_string(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    data_type_t data_type = DATA_TYPE_CHAR;
    size_t char_len = 0;

    // BEGIN STATE MACHINE

    // We get here after the column name identifier has been found and
    // we've determined that either the CHAR, CHARACTER, or VARCHAR symbols
    // were next
    switch (cur_sym) {
        case SYMBOL_CHAR:
        case SYMBOL_CHARACTER:
            cur_tok = lex.next();
            goto optional_char_varying;
        case SYMBOL_VARCHAR:
            data_type = DATA_TYPE_VARCHAR;
            cur_tok = lex.next();
            goto optional_length;
        default:
            return false;
    }

optional_char_varying:
    // We get here if we got a CHAR or CHARACTER as the data type. This
    // might be followed by the VARYING symbol, in which case we will
    // process a VARCHAR. Otherwise, we'll process a CHAR type
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_VARYING) {
        data_type = DATA_TYPE_VARCHAR;
        cur_tok = lex.next();
    }
    goto optional_length;
optional_length:
    // We get here after determining the exact type of the character
    // string. The type will be followed by an optional length specifier
    // clause, which if an unsigned integer enclosed by parentheses.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        goto process_length;
    }
    goto push_descriptor;
process_length:
    // We get here if we've processed the opening parentheses of the
    // optional length modifier and now expect to find an unsigned integer
    // followed by a closing parentheses
    if (cur_tok.is_literal()) {
        // Make sure we can parse our literal token to an unsigned integer
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_LITERAL_UNSIGNED_INTEGER)
            goto err_expect_size_literal;
        const std::string char_len_str(cur_tok.lexeme.start, cur_tok.lexeme.end);
        char_len = atoi(char_len_str.data());
        cur_tok = lex.next();
        goto length_close;
    }
    goto err_expect_size_literal;
err_expect_size_literal:
    expect_error(ctx, SYMBOL_LITERAL_UNSIGNED_INTEGER);
    return false;
length_close:
    // We get here if we've processed the opening parentheses of the length
    // modifier and the unsigned integer size and now expect a closing
    // parentheses for the length modifier
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_RPAREN)
        goto push_descriptor;
    goto err_expect_length_rparen;
err_expect_length_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
push_descriptor:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        std::unique_ptr<data_type_descriptor_t> dtd_p;
        dtd_p = std::move(std::make_unique<char_string_t>(data_type, char_len));
        column_def.data_type = std::move(dtd_p);
        return true;
    }
}

} // namespace sqltoast
