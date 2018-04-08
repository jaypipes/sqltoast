/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast/sqltoast.h"

#include "parser/parse.h"
#include "parser/error.h"

namespace sqltoast {

// <alter table statement> ::=
//     ALTER TABLE <table name> <alter table action>
//
// <alter table action> ::=
//     <add column definition>
//     |     <alter column definition>
//     |     <drop column definition>
//     |     <add table constraint definition>
//     |     <drop table constraint definition>
//
// <add column definition> ::=
//     ADD [ COLUMN ] <column definition>
//
// <alter column definition> ::=
//     ALTER [ COLUMN ] <column name> <alter column action>
//
// <alter column action> ::=
//     <set column default clause> | <drop column default clause>
//
// <set column default clause> ::=
//     SET <default clause>
//
// <drop column default clause> ::=
//     DROP DEFAULT
//
// <drop column definition> ::=
//     DROP [ COLUMN ] <column name> <drop behaviour>
//
// <add table constraint definition> ::=
//     ADD <table constraint definition>
//
// <drop table constraint definition> ::=
//     DROP CONSTRAINT <constraint name> <drop behaviour>
bool parse_alter_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t table_name;
    lexeme_t column_name;
    lexeme_t constraint_name;
    drop_behaviour_t drop_behaviour = DROP_BEHAVIOUR_CASCADE;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<column_definition_t> column_def;
    std::unique_ptr<default_descriptor_t> default_descriptor;
    std::unique_ptr<constraint_t> constraint;
    std::unique_ptr<alter_table_action_t> action;

    cur_tok = lex.next(); // Consume the ALTER symbol
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_TABLE) {
        // rewind
        ctx.lexer.cursor = start;
        return false;
    }
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    table_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto expect_action;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_action:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ADD:
            cur_tok = lex.next();
            goto process_add_actions;
        case SYMBOL_DROP:
            cur_tok = lex.next();
            goto process_drop_actions;
        case SYMBOL_ALTER:
            cur_tok = lex.next();
            goto process_alter_action;
        default:
            goto err_expect_add_drop_or_alter;
    }
err_expect_add_drop_or_alter:
    expect_any_error(ctx, {SYMBOL_ADD, SYMBOL_DROP, SYMBOL_ALTER});
    return false;
process_add_actions:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_COLUMN:
            cur_tok = lex.next();
            goto process_add_column;
        case SYMBOL_IDENTIFIER:
            goto process_add_column;
        case SYMBOL_UNIQUE:
        case SYMBOL_PRIMARY:
        case SYMBOL_FOREIGN:
        case SYMBOL_CHECK:
        case SYMBOL_CONSTRAINT:
            goto process_add_constraint;
        default:
            goto err_expect_add_column_or_constraint;
    }
err_expect_add_column_or_constraint:
    {
        std::stringstream estr;
        estr << "Expected either an add column action or an add "
                "constraint action but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
process_add_column:
    if (ctx.opts.disable_statement_construction)
        goto statement_ending;
    if (! parse_column_definition(ctx, cur_tok, column_def))
        goto err_expect_column_definition;
    action = std::make_unique<add_column_action_t>(column_def);
    goto statement_ending;
err_expect_column_definition:
    {
        std::stringstream estr;
        estr << "Expected <column definition> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
process_add_constraint:
    if (ctx.opts.disable_statement_construction)
        goto statement_ending;
    if (! parse_constraint(ctx, cur_tok, constraint))
        goto err_expect_constraint;
    action = std::make_unique<add_constraint_action_t>(constraint);
    goto statement_ending;
err_expect_constraint:
    {
        std::stringstream estr;
        estr << "Expected <constraint definition> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
process_drop_actions:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_COLUMN:
            cur_tok = lex.next();
            goto process_drop_column;
        case SYMBOL_IDENTIFIER:
            goto process_drop_column;
        case SYMBOL_CONSTRAINT:
            cur_tok = lex.next();
            goto process_drop_constraint;
        default:
            goto err_expect_drop_column_or_constraint;
    }
err_expect_drop_column_or_constraint:
    {
        std::stringstream estr;
        estr << "Expected either a drop column action or a drop "
                "constraint action but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
process_drop_column:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COLUMN)
        cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    column_name = cur_tok.lexeme;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_CASCADE || cur_sym == SYMBOL_RESTRICT) {
        if (cur_sym == SYMBOL_RESTRICT)
            drop_behaviour = DROP_BEHAVIOUR_RESTRICT;
        cur_tok = lex.next();
    }
    if (ctx.opts.disable_statement_construction)
        goto statement_ending;
    action = std::make_unique<drop_column_action_t>(
            column_name, drop_behaviour);
    goto statement_ending;
process_drop_constraint:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    constraint_name = cur_tok.lexeme;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_CASCADE || cur_sym == SYMBOL_RESTRICT) {
        if (cur_sym == SYMBOL_RESTRICT)
            drop_behaviour = DROP_BEHAVIOUR_RESTRICT;
        cur_tok = lex.next();
    }
    if (ctx.opts.disable_statement_construction)
        goto statement_ending;
    action = std::make_unique<drop_constraint_action_t>(
            constraint_name, drop_behaviour);
    goto statement_ending;
process_alter_action:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COLUMN)
        cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    column_name = cur_tok.lexeme;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SET) {
        cur_tok = lex.next();
        goto process_alter_column_set_default_action;
    } else if (cur_sym == SYMBOL_DROP) {
        cur_tok = lex.next();
        goto process_alter_column_drop_default_action;
    } else
        goto err_expect_set_or_drop;
err_expect_set_or_drop:
    expect_any_error(ctx, {SYMBOL_SET, SYMBOL_DROP});
    return false;
process_alter_column_set_default_action:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_DEFAULT)
        goto err_expect_default;
    cur_tok = lex.next();
    if (! parse_default_clause(ctx, cur_tok, default_descriptor))
        goto err_expect_default_clause;
    if (ctx.opts.disable_statement_construction)
        goto statement_ending;
    action = std::make_unique<alter_column_action_t>(
            column_name, default_descriptor);
    goto statement_ending;
err_expect_default:
    expect_error(ctx, SYMBOL_DEFAULT);
    return false;
err_expect_default_clause:
    {
        std::stringstream estr;
        estr << "Expected <default clause> but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
process_alter_column_drop_default_action:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_DEFAULT)
        goto err_expect_default;
    cur_tok = lex.next();
    if (ctx.opts.disable_statement_construction)
        goto statement_ending;
    action = std::make_unique<alter_column_action_t>(column_name);
    goto statement_ending;
statement_ending:
    // We get here after successfully parsing the statement and now expect
    // either the end of parse content or a semicolon to indicate end of
    // statement.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS)
        goto push_statement;
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON});
    return false;
push_statement:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<alter_table_statement_t>(table_name, action);
    return true;
}

} // namespace sqltoast
