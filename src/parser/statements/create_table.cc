/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "column_definition.h"
#include "parser/parse.h"
#include "parser/statements/create_table.h"
#include "parser/error.h"
#include "parser/token.h"
#include "statements/create_table.h"

namespace sqltoast {

//
// The CREATE TABLE statement follows this EBNF form for the following SQL
// dialects:
//
// * SQL_DIALECT_ANSI_1992
//
//  <table definition> ::=
//      CREATE [{GLOBAL|LOCAL} TEMPORARY] TABLE <table name>
//      <table element list>
//      [ON COMMIT {DELETE|PRESERVE} ROWS]
//

bool parse_create_table(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    token_t& cur_tok = lex.current_token;
    lexeme_t ident;
    symbol_t cur_sym;
    statements::table_type_t table_type = statements::TABLE_TYPE_NORMAL;
    std::vector<std::unique_ptr<column_definition_t>> column_defs;

    // BEGIN STATE MACHINE

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ERROR:
            return false;
        case SYMBOL_TABLE:
            goto expect_table_name;
        case SYMBOL_GLOBAL:
        case SYMBOL_LOCAL:
        case SYMBOL_TEMPORARY:
            goto table_kw_or_table_type;
        default:
            // rewind
            ctx.lexer.cursor = start;
            return false;
    }

    table_kw_or_table_type:
        // We get here after successfully finding the CREATE symbol. We can
        // either match the table keyword or the table type clause
        if (cur_sym == SYMBOL_GLOBAL) {
            table_type = statements::TABLE_TYPE_TEMPORARY_GLOBAL;
            goto expect_temporary;
        } else if (cur_sym == SYMBOL_LOCAL) {
            table_type = statements::TABLE_TYPE_TEMPORARY_LOCAL;
            goto expect_temporary;
        } else if (cur_sym == SYMBOL_TEMPORARY) {
            table_type = statements::TABLE_TYPE_TEMPORARY_GLOBAL;
            goto expect_table;
        }
        SQLTOAST_UNREACHABLE();
    expect_temporary:
        // We get here if we successfully matched CREATE followed by either the
        // GLOBAL or LOCAL symbol. If this is the case, we expect to find the
        // TEMPORARY keyword followed by the TABLE keyword.
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_TEMPORARY)
            goto err_expect_temporary;
        goto expect_table;
    err_expect_temporary:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected TEMPORARY after CREATE {GLOBAL | LOCAL} but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_table:
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_TABLE)
            goto err_expect_table;
        goto expect_table_name;
        SQLTOAST_UNREACHABLE();
    err_expect_table:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected TABLE after CREATE {GLOBAL | LOCAL} TEMPORARY but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_table_name:
        // We get here after successfully finding CREATE followed by the TABLE
        // symbol (after optionally processing the table type modifier). We now
        // need to find an identifier
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            fill_lexeme(cur_tok, ident);
            goto expect_column_list_open;
        }
        goto err_expect_identifier;
        SQLTOAST_UNREACHABLE();
    err_expect_identifier:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected <identifier> after CREATE TABLE keyword but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_column_list_open:
        // We get here after successfully finding the CREATE ... TABLE <table name>
        // part of the statement. We now expect to find the <table element
        // list> clause
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_LPAREN) {
            goto expect_column_list_element;
        }
        goto err_expect_lparen;
        SQLTOAST_UNREACHABLE();
    err_expect_lparen:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected '(' after CREATE TABLE <table name> but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_column_list_element:
        // We get here after finding the LPAREN opening the <table element
        // list> clause. Now we expect to find one or more column or constraint
        // definitions
        cur_tok = lex.next();
        if (! parse_column_definition(ctx, cur_tok, column_defs))
            goto err_expect_column_definition;
        goto expect_column_list_close;
        SQLTOAST_UNREACHABLE();
    err_expect_column_definition:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected column definition but got " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_column_list_close:
        // We get here after successfully parsing the <table element list>
        // column/constraint definitions and are now expecting the closing
        // RPAREN to indicate the end of the <table element list>
        cur_tok = lex.current_token;
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_RPAREN) {
            cur_tok = lex.next();
            goto statement_ending;
        }
        goto err_expect_rparen;
        SQLTOAST_UNREACHABLE();
    err_expect_rparen:
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected closing ')' after CREATE TABLE <table name> but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    statement_ending:
        // We get here if we have already successfully processed the CREATE
        // TABLE statement and are expecting EOS or SEMICOLON as the next
        // non-comment token
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS) {
            // skip-consume the semicolon or EOS token
            cur_tok = lex.next();
            goto push_statement;
        }
        {
            parse_position_t err_pos = ctx.lexer.cursor;
            std::stringstream estr;
            estr << "Expected EOS or SEMICOLON but found " << cur_tok << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    push_statement:
        {
            if (ctx.opts.disable_statement_construction)
                return true;
            identifier_t table_ident(ident);
            auto stmt_p = std::make_unique<statements::create_table_t>(table_type, table_ident);
            (*stmt_p).column_definitions = std::move(column_defs);
            ctx.result.statements.emplace_back(std::move(stmt_p));
            return true;
        }
}

} // namespace sqltoast
