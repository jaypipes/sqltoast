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
    tokens_t::iterator tok_it = ctx.tokens.begin();
    tokens_t::iterator tok_ident = ctx.tokens.end();
    symbol_t exp_sym = SYMBOL_CREATE;
    symbol_t cur_sym = (*tok_it).symbol;
    statements::table_type_t table_type = statements::TABLE_TYPE_NORMAL;
    std::vector<std::unique_ptr<column_definition_t>> column_defs;

    goto next_token;

    // BEGIN STATE MACHINE

    table_kw_or_table_type:
        // We get here after successfully finding the CREATE symbol. We can
        // either match the table keyword or the table type clause
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_GLOBAL) {
            table_type = statements::TABLE_TYPE_TEMPORARY_GLOBAL;
            tok_it++;
            goto expect_temporary;
        } else if (cur_sym == SYMBOL_LOCAL) {
            table_type = statements::TABLE_TYPE_TEMPORARY_LOCAL;
            tok_it++;
            goto expect_temporary;
        } else if (cur_sym == SYMBOL_TEMPORARY) {
            table_type = statements::TABLE_TYPE_TEMPORARY_GLOBAL;
            tok_it++;
        }
        exp_sym = SYMBOL_TABLE;
        goto next_token;
        SQLTOAST_UNREACHABLE();
    expect_temporary:
        // We get here if we successfully matched CREATE followed by either the
        // GLOBAL or LOCAL symbol. If this is the case, we expect to find the
        // TEMPORARY keyword followed by the TABLE keyword.
        if (ctx.at_end(tok_it))
            goto err_expect_temporary;
        cur_sym = (*tok_it).symbol;
        if (cur_sym != SYMBOL_TEMPORARY)
            goto err_expect_temporary;
        tok_it++;
        if (ctx.at_end(tok_it))
            goto err_expect_table;
        cur_sym = (*tok_it).symbol;
        if (cur_sym != SYMBOL_TABLE)
            goto err_expect_table;
        tok_it++;
        goto expect_table_name;
        SQLTOAST_UNREACHABLE();
    err_expect_temporary:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            std::stringstream estr;
            if (ctx.at_end(tok_it)) {
                estr << "Expected TEMPORARY after CREATE {GLOBAL | LOCAL} but found EOS";
            } else {
                cur_sym = (*tok_it).symbol;
                estr << "Expected TEMPORARY after CREATE {GLOBAL | LOCAL} but found "
                     << symbol_map::to_string(cur_sym);
            }
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    err_expect_table:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            std::stringstream estr;
            if (ctx.at_end(tok_it)) {
                estr << "Expected TABLE after CREATE {GLOBAL | LOCAL} TEMPORARY but found EOS";
            } else {
                cur_sym = (*tok_it).symbol;
                estr << "Expected TABLE after CREATE {GLOBAL | LOCAL} TEMPORARY but found "
                     << symbol_map::to_string(cur_sym);
            }
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_table_name:
        // We get here after successfully finding CREATE followed by the TABLE
        // symbol (after optionally processing the table type modifier). We now
        // need to find an identifier
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_IDENTIFIER) {
            tok_ident = tok_it++;
            goto expect_column_list_open;
        }
        goto err_expect_identifier;
        SQLTOAST_UNREACHABLE();
    err_expect_identifier:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            std::stringstream estr;
            if (ctx.at_end(tok_it)) {
                estr << "Expected <identifier> after CREATE TABLE but found EOS";
            } else {
                cur_sym = (*tok_it).symbol;
                estr << "Expected <identifier> after CREATE TABLE keyword but found "
                     << symbol_map::to_string(cur_sym);
            }
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_column_list_open:
        // We get here after successfully finding the CREATE ... TABLE <table name>
        // part of the statement. We now expect to find the <table element
        // list> clause
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_LPAREN) {
            tok_it++;
            goto expect_column_list_element;
        }
        goto err_expect_lparen;
        SQLTOAST_UNREACHABLE();
    err_expect_lparen:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            std::stringstream estr;
            if (ctx.at_end(tok_it)) {
                estr << "Expected opening '(' after CREATE TABLE <table name> but found EOS";
            } else {
                cur_sym = (*tok_it).symbol;
                estr << "Expected '(' after CREATE TABLE <table name> but found "
                     << symbol_map::to_string(cur_sym);
            }
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    expect_column_list_element:
        // We get here after finding the LPAREN opening the <table element
        // list> clause. Now we expect to find one or more column or constraint
        // definitions
        if (! parse_column_definition(ctx, tok_it, column_defs))
            return false;
        tok_it = ctx.tokens.begin();
        goto expect_column_list_close;
        SQLTOAST_UNREACHABLE();
    expect_column_list_close:
        // We get here after successfully parsing the <table element list>
        // column/constraint definitions and are now expecting the closing
        // RPAREN to indicate the end of the <table element list>
        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_RPAREN) {
            tok_it++;
            goto statement_ending;
        }
        goto err_expect_rparen;
        SQLTOAST_UNREACHABLE();
    err_expect_rparen:
        {
            parse_position_t err_pos = (*(tok_it)).lexeme.start;
            std::stringstream estr;
            if (ctx.at_end(tok_it)) {
                estr << "Expected closing ')' after CREATE TABLE <table name> but found EOS";
            } else {
                cur_sym = (*tok_it).symbol;
                estr << "Expected closing ')' after CREATE TABLE <table name> but found "
                     << symbol_map::to_string(cur_sym);
            }
            estr << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    statement_ending:
        // We get here if we have already successfully processed the CREATE
        // TABLE statement and are expecting EOS or SEMICOLON as the next
        // non-comment token
        if (ctx.at_end(tok_it)) {
            goto push_statement;
        }

        cur_sym = (*tok_it).symbol;
        if (cur_sym == SYMBOL_SEMICOLON) {
            // skip-consume the semicolon token
            tok_it++;
            goto push_statement;
        }
        {
            parse_position_t err_pos = (*tok_it).lexeme.start;
            std::stringstream estr;
            estr << "Expected EOS or SEMICOLON but found "
                 << symbol_map::to_string(cur_sym) << std::endl;
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    push_statement:
        {
            ctx.trim_to(tok_it);
            if (ctx.opts.disable_statement_construction)
                return true;
            identifier_t table_ident((*tok_ident).lexeme);
            auto stmt_p = std::make_unique<statements::create_table_t>(table_type, table_ident);
            (*stmt_p).column_definitions = std::move(column_defs);
            ctx.result.statements.emplace_back(std::move(stmt_p));
            return true;
        }
    eos:
        if (exp_sym == SYMBOL_CREATE || exp_sym == SYMBOL_TABLE) {
            // Reached the end of the token stack and never found the
            // CREATE TABLE so just return false
            return false;
        }
        {
            // Reached the end of the token stream after already finding the
            // CREATE and TABLE symbols. Return a syntax error.
            parse_position_t err_pos = (*tok_it).lexeme.start;
            std::stringstream estr;
            estr << "Expected <table_name_clause> but found EOS";
            create_syntax_error_marker(ctx, estr, err_pos);
            return false;
        }
        SQLTOAST_UNREACHABLE();
    next_token:
        if (ctx.at_end(tok_it)) {
            goto eos;
        }
        cur_sym = (*tok_it).symbol;
        tok_it++;
        switch (cur_sym) {
            case SYMBOL_CREATE:
                if (exp_sym == SYMBOL_CREATE) {
                    goto table_kw_or_table_type;
                }
                goto next_token;
            case SYMBOL_TABLE:
                if (exp_sym == SYMBOL_TABLE) {
                    goto expect_table_name;
                }
                goto next_token;
            default:
                return false;
        }
        SQLTOAST_UNREACHABLE();
}

} // namespace sqltoast
