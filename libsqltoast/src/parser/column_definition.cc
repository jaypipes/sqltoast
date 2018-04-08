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

// <column definition> ::=
//      <column name>
//      { <data type> | <domain name> }
//      [ <default clause> ]
//      [ <column constraint definition> ... ]
//      [ <collate clause> ]
bool parse_column_definition(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<column_definition_t>& out) {
    lexer_t& lex = ctx.lexer;
    lexeme_t column_name;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<data_type_descriptor_t> data_type;
    std::unique_ptr<default_descriptor_t> default_descriptor;
    std::unique_ptr<constraint_t> constraint;
    std::vector<std::unique_ptr<constraint_t>> constraints;

    if (cur_sym != SYMBOL_IDENTIFIER)
        return false;
    column_name = cur_tok.lexeme;
    cur_tok = ctx.lexer.next();
    if (! parse_data_type_descriptor(ctx, cur_tok, data_type))
        return false;
    goto optional_default;
optional_default:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_DEFAULT) {
        cur_tok = lex.next();
        if (! parse_default_clause(ctx, cur_tok, default_descriptor))
            return false;
    }
    goto optional_constraints;
optional_constraints:
    cur_sym = lex.current_token.symbol;
    switch (cur_sym) {
        case SYMBOL_CONSTRAINT:
        case SYMBOL_NOT:
        case SYMBOL_UNIQUE:
        case SYMBOL_PRIMARY:
        case SYMBOL_REFERENCES:
        case SYMBOL_CHECK:
            if (! parse_column_constraint(ctx, cur_tok, constraint))
                return false;
            constraints.emplace_back(std::move(constraint));
            goto optional_constraints; // there may be >1 constraint...
        default:
            goto optional_collate;
    }
optional_collate:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COLLATE) {
        cur_tok = lex.next();
        if (! parse_collate_clause(ctx, cur_tok, *out))
            return false;
    }
    goto push_column_def;
push_column_def:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<column_definition_t>(
            column_name, data_type, default_descriptor, constraints);
    return true;
}

// <default clause> ::= DEFAULT <default option>
//
// <default option> ::=
//     <literal>
//     | <datetime value function>
//     | USER
//     | CURRENT_USER
//     | SESSION_USER
//     | SYSTEM_USER
//     | NULL
//
// <datetime value function> ::=
//     <current date value function>
//     | <current time value function>
//     | <current timestamp value function>
//
// <current date value function> ::= CURRENT_DATE
//
// <current time value function> ::=
//     CURRENT_TIME [ <left paren> <time precision> <right paren> ]
//
// <current timestamp value function> ::=
//     CURRENT_TIMESTAMP [ <left paren> <timestamp precision> <right paren> ]
bool parse_default_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<default_descriptor_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    default_type_t default_type;
    lexeme_t value;
    size_t prec;

    switch (cur_sym) {
        case SYMBOL_NULL:
            default_type = DEFAULT_TYPE_NULL;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_USER:
            default_type = DEFAULT_TYPE_USER;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_CURRENT_USER:
            default_type = DEFAULT_TYPE_CURRENT_USER;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_SESSION_USER:
            default_type = DEFAULT_TYPE_SESSION_USER;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_SYSTEM_USER:
            default_type = DEFAULT_TYPE_SYSTEM_USER;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_CURRENT_DATE:
            default_type = DEFAULT_TYPE_CURRENT_DATE;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto push_descriptor;
        case SYMBOL_CURRENT_TIME:
            default_type = DEFAULT_TYPE_CURRENT_TIME;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto optional_precision;
        case SYMBOL_CURRENT_TIMESTAMP:
            default_type = DEFAULT_TYPE_CURRENT_TIMESTAMP;
            value = cur_tok.lexeme;
            cur_tok = lex.next();
            goto optional_precision;
        case SYMBOL_MINUS:
        case SYMBOL_PLUS:
            default_type = DEFAULT_TYPE_LITERAL;
            value.start = cur_tok.lexeme.start;
            cur_tok = lex.next();
            goto expect_unsigned_numeric;
        default:
            if (cur_tok.is_literal()) {
                default_type = DEFAULT_TYPE_LITERAL;
                value = cur_tok.lexeme;
                cur_tok = lex.next();
                goto push_descriptor;
            }
            return false;
    }
expect_unsigned_numeric:
    // We get here after finding DEFAULT followed by a + or - sign, which
    // indicates the default value is a signed numeric. So we now need to parse
    // an unsigned numeric
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LITERAL_UNSIGNED_INTEGER &&
            cur_sym != SYMBOL_LITERAL_UNSIGNED_DECIMAL)
        goto err_expect_unsigned_numeric;
    value.end = cur_tok.lexeme.end;
    cur_tok = lex.next();
    goto push_descriptor;
err_expect_unsigned_numeric:
    expect_any_error(ctx, {SYMBOL_LITERAL_UNSIGNED_INTEGER,
            SYMBOL_LITERAL_UNSIGNED_DECIMAL});
    return false;
optional_precision:
    // We get here after getting either a CURRENT_TIME or CURRENT_TIMESTAMP
    // symbol. This can be followed by an optional LPAREN <precision> RPAREN.
    // Since the length specifier is an identical structure, we use that...
    if (! parse_length_specifier(ctx, cur_tok, &prec))
        return false;
    goto push_descriptor;
push_descriptor:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<default_descriptor_t>(
            default_type, value, prec);
    return true;
}

// <column constraint definition> ::=
//     [ <constraint name definition> ]
//     <column constraint> [ <constraint attributes> ]
//
// <constraint name definition> ::= CONSTRAINT <constraint name>
//
// <constraint name> ::= <qualified name>
//
// <column constraint> ::=
//     NOT NULL
//     | <unique specification>
//     | <references specification>
//     | <check constraint definition>
//
// <unique specification> ::= UNIQUE | PRIMARY KEY
//
// <check constraint definition> ::=
//     CHECK <left paren> <search condition> <right paren>
bool parse_column_constraint(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t constraint_name;
    std::vector<lexeme_t> columns;

    // We get here after getting one of the symbols that precede a constraint
    // definition, which include the CONSTRAINT, NOT, UNIQUE, PRIMARY,
    // REFERENCES or CHECK keywords.
    if (cur_sym == SYMBOL_CONSTRAINT) {
        cur_tok = lex.next();
        goto process_name;
    }
    goto process_constraint_type;
process_name:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    constraint_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto process_constraint_type;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
process_constraint_type:
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_NOT:
            cur_tok = lex.next();
            goto process_not_null;
        case SYMBOL_UNIQUE:
            cur_tok = lex.next();
            out = std::make_unique<unique_constraint_t>(false);
            goto push_constraint;
        case SYMBOL_PRIMARY:
            cur_tok = lex.next();
            goto process_primary_key;
        case SYMBOL_REFERENCES:
            cur_tok = lex.next();
            if (! parse_references_specification(ctx, cur_tok, out))
                return false;
            goto push_constraint;
        case SYMBOL_CHECK:
            // TODO
            return false;
        default:
            // should not get here...
            return false;
    }
process_not_null:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_NULL)
        goto err_expect_null;
    out = std::make_unique<not_null_constraint_t>();
    cur_tok = lex.next();
    return true;
err_expect_null:
    expect_error(ctx, SYMBOL_NULL);
    return false;
process_primary_key:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_KEY)
        goto err_expect_key;
    out = std::make_unique<unique_constraint_t>(true);
    cur_tok = lex.next();
    goto push_constraint;
err_expect_key:
    expect_error(ctx, SYMBOL_KEY);
    return false;
push_constraint:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (constraint_name)
        out->name = constraint_name;
    return true;
}

// <references specification>    ::=
//     REFERENCES <referenced table and columns>
//     [ MATCH <match type> ] [ <referential triggered action> ]
//
// <referenced table and columns> ::=
//     <table name> [ <left paren> <reference column list> <right paren> ]
//
// <table name> ::= <qualified name> | <qualified local table name>
//
// <reference column list> ::= <column name list>
//
// <column name list> ::= <column name> [ { <comma> <column name> }... ]
//
// <referential triggered action> ::=
//     <update rule> [ <delete rule> ]
//     | <delete rule> [ <update rule> ]
//
// <update rule> ::= ON UPDATE <referential action>
//
// <delete rule> ::= ON DELETE <referential action>
bool parse_references_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<constraint_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t ref_table;
    std::vector<lexeme_t> referenced_cols;
    bool found_on_update = false;
    bool found_on_delete = false;
    match_type_t match_type = MATCH_TYPE_NONE;
    referential_action_t on_update = REFERENTIAL_ACTION_NONE;
    referential_action_t on_delete = REFERENTIAL_ACTION_NONE;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    ref_table = cur_tok.lexeme;
    cur_tok = lex.next();
    goto optional_column_names;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
optional_column_names:
    // We get here after processing the <table_name> which is followed by an
    // optional parens-enclosed, comma-delimited list of column names
    // (identifiers)
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        if (! parse_identifier_list(ctx, cur_tok, referenced_cols))
            return false;
        goto optional_match_type;
    }
    goto push_constraint;
optional_match_type:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_MATCH) {
        cur_tok = lex.next();
        if (! parse_match_type(ctx, cur_tok, &match_type))
            return false;
        cur_tok = lex.current_token;
    }
    goto optional_trigger;
optional_trigger:
    // We get here after successfully processing the <table name>,
    // <column_name_list> and <match_type> clauses. We now look for an optional
    // referential trigger clause
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ON) {
        cur_tok = lex.next();
        goto process_trigger;
    }
    goto push_constraint;
process_trigger:
    // A trigger can be either ON UPDATE or ON DELETE. By this point, we've
    // already consumed the ON symbol
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_UPDATE:
            if (found_on_update)
                goto err_already_found_on_update;
            found_on_update = true;
            cur_tok = lex.next();
            if (! parse_referential_action(ctx, cur_tok, &on_update))
                return false;
            goto push_constraint;
        case SYMBOL_DELETE:
            if (found_on_delete)
                goto err_already_found_on_delete;
            found_on_delete = true;
            cur_tok = lex.next();
            if (! parse_referential_action(ctx, cur_tok, &on_delete))
                return false;
            goto push_constraint;
        default:
            goto err_expect_update_or_delete;
    }
err_expect_update_or_delete:
    expect_any_error(ctx, {SYMBOL_UPDATE, SYMBOL_DELETE});
    return false;
err_already_found_on_update:
    {
        std::stringstream estr;
        estr << "Already found ON UPDATE constraint." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
err_already_found_on_delete:
    {
        std::stringstream estr;
        estr << "Already found ON DELETE constraint." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_constraint:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<foreign_key_constraint_t>(
            ref_table, referenced_cols, match_type, on_update, on_delete);
    return true;
}

// <column name list> ::= <column name> [ { <comma> <column name> }... ]
bool parse_identifier_list(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::vector<lexeme_t>& identifiers) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;

    // We get here after processing the LPAREN opening and now expect a
    // comma-delimited list of identifiers and a closing RPAREN symbol

process_column:
    // We have already consumed the LPAREN opening of a column list or a COMMA
    // and now expect a column name
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    identifiers.emplace_back(cur_tok.lexeme);
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_column;
    }
    goto expect_rparen;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_rparen:
    // We get here after processing an identifier and not finding a COMMA,
    // indicating another identifier should be expected. So, we now expect to
    // find a closing RPAREN
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    return true;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
}

// <match type> ::= FULL | PARTIAL
bool parse_match_type(
        parse_context_t& ctx,
        token_t& cur_tok,
        match_type* match_type) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;

    // We get here after finding the MATCH symbol and now expecting either FULL
    // or PARTIAL symbol
    switch (cur_sym) {
        case SYMBOL_FULL:
            *match_type = MATCH_TYPE_FULL;
            cur_tok = lex.next();
            return true;
        case SYMBOL_PARTIAL:
            *match_type = MATCH_TYPE_PARTIAL;
            cur_tok = lex.next();
            return true;
        default:
            goto err_expect_match_type;
    }
err_expect_match_type:
    expect_any_error(ctx, {SYMBOL_FULL, SYMBOL_PARTIAL});
    return false;
}

// <referential action> ::= CASCADE | SET NULL | SET DEFAULT | NO ACTION
//
// <delete rule> ::= ON DELETE <referential action>
bool parse_referential_action(
        parse_context_t& ctx,
        token_t& cur_tok,
        referential_action_t* action) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;

    // We get here if we've already successfully consume the ON followed by
    // either the UPDATE or DELETE symbols and now expect to find a trigger
    // action
    switch (cur_sym) {
        case SYMBOL_SET:
            // Either SET NULL or SET DEFAULT are allowed
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
            if (cur_sym == SYMBOL_NULL) {
                cur_tok = lex.next();
                *action = REFERENTIAL_ACTION_SET_NULL;
                return true;
            } else if (cur_sym == SYMBOL_DEFAULT) {
                cur_tok = lex.next();
                *action = REFERENTIAL_ACTION_SET_DEFAULT;
                return true;
            } else {
                goto err_expect_null_or_default;
            }
        case SYMBOL_CASCADE:
            cur_tok = lex.next();
            *action = REFERENTIAL_ACTION_CASCADE;
            return true;
        case SYMBOL_NO:
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
            if (cur_sym != SYMBOL_ACTION)
                goto err_expect_action;
            *action = REFERENTIAL_ACTION_NONE;
            return true;
        default:
            goto err_expect_action_trigger;
    }
err_expect_null_or_default:
    expect_any_error(ctx, {SYMBOL_NULL, SYMBOL_DEFAULT});
    return false;
err_expect_action:
    expect_error(ctx, SYMBOL_ACTION);
    return false;
err_expect_action_trigger:
    expect_any_error(ctx, {SYMBOL_SET, SYMBOL_NO, SYMBOL_CASCADE});
    return false;
}

//  <collate clause> ::= COLLATE <qualified identifier>
bool parse_collate_clause(
        parse_context_t& ctx,
        token_t& cur_tok,
        column_definition_t& column_def) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t collate_ident;

    // We get here after getting the COLLATE symbol while processing the column
    // definition. We expect an identifier (the collation) at this point.
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;

    collate_ident = cur_tok.lexeme;
    cur_tok = lex.next();
    goto push_descriptor;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_descriptor:
    {
        if (ctx.opts.disable_statement_construction)
            return true;
        column_def.collate = collate_ident;
        return true;
    }
}

} // namespace sqltoast
