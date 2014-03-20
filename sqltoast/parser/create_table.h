/*
 * sqltoast
 *
 * Copyright (c) 2010 Jay Pipes <jaypipes@gmail.com>
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See the
 * COPYING file in the root project directory for full text.
 */

/**
 * @file Grammar for parsing CREATE TABLE statements
 */

#ifndef SQLTOAST_PARSER_CREATE_TABLE_H
#define SQLTOAST_PARSER_CREATE_TABLE_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"
#include "sqltoast/parser/common_rules.h"
#include "sqltoast/parser/identifier_rules.h"
#include "sqltoast/parser/specifier_rules.h"
#include "sqltoast/parser/column_definition.h"
#include "sqltoast/parser/table_constraint.h"
#include "sqltoast/ast.h"
#include "sqltoast/nodes/statements/create_table.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/unused.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sqltoast
{
namespace parser
{
using boost::fusion::unused_type;

/* 
 * A table element in a create table statement
 * is either a column definition or a table
 * constraint
 */
typedef boost::variant<column_definition_attr,
                       table_constraint_attr
                      > table_element_attr;
typedef std::vector<table_element_attr> table_element_list_attr;

/**
 * Main structure that is parsed for a CREATE TABLE
 * SQL statement
 */
struct create_table_attr
{
  bool temporary; /* the temporary keyword appeared? */
  bool if_not_exists; /* if not exists? */
  table_identifier_attr table_identifier;
  table_element_list_attr table_elements;
};

/**
 * Parser for a table element in a CREATE TABLE statement,
 * which is either a table constraint or a column definition
 *
 * @see /sqltoast/parser/table_constraint.h
 * @see /sqltoast/parser/column_definition.h
 */
boost::spirit::qi::rule<std::string::iterator, table_element_attr(), space_type>
table_element_rule = basic_ascii_column_definition_parser | basic_ascii_table_constraint_parser;

/**
 * Parser for CREATE TABLE statements
 *
 * @detail
 *
 * Class template for a parser with a synthesized
 * attribute of an ast-node of type statements::create_table.
 *
 * A CREATE TABLE statement can take the following EBNF form:
 * 
 * create_table : CREATE temporary TABLE if_not_exists table_identifier table_element_list
 *
 * temporary : TEMPORARY
 *               | empty
 *
 * if_not_exists : IF NOT EXISTS
 *               | empty
 *
 * table_identifier : identifier '.' identifier
 *                  | identifier
 *
 * table_element_list : '(' table_element [ {',' table_element} ... ')'
 *
 * table_element : column_definition
 *               | table_constraint_definition
 *
 * column_definition : identifier column_data_type [ default_clause ] [ column_constraint_definition ... ] [ collate_clause ] 
 */
template<class iterator_type, class space_type>
struct create_table_parser: boost::spirit::qi::grammar<iterator_type,
                                                       create_table_attr(),
                                                       space_type >
{
  create_table_parser() : create_table_parser::base_type(start)
  {
    using boost::spirit::ascii::no_case;
    using boost::spirit::eoi;
    using boost::spirit::omit;

    table_element_list = table_element_rule % ',';

    start = omit[no_case["create"]]
            >> temporary
            >> omit[no_case["table"]]
            >> if_not_exists
            >> table_identifier_rule
            >> lit('(')
            >> table_element_list
            >> lit(')')
            >> eoi;
  }

  boost::spirit::qi::rule<iterator_type, create_table_attr(), space_type > start;
  boost::spirit::qi::rule<iterator_type, table_element_list_attr(), space_type > table_element_list;
};

/* Our create table parsers, created during library initialization time */
create_table_parser<std::string::iterator, boost::spirit::qi::ascii::space_type> basic_ascii_create_table_parser;

/**
 * Parses a string for the CREATE TABLE statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output attribute
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_create_table(iterator_type start,
                               iterator_type end,
                               create_table_attr& output)
{
  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_create_table_parser,
                                      boost::spirit::qi::ascii::space,
                                      output))
    return SUCCESS;
  return SYNTAX_ERROR;
}

/**
 * Parses a string for the CREATE TABLE statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output AST
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_create_table(iterator_type start,
                                iterator_type end,
                                ast& output)
{
  create_table_attr attr;

  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_create_table_parser,
                                      boost::spirit::qi::ascii::space,
                                      attr))
  {
    boost::shared_ptr<nodes::identifier> p_identifier;
    boost::shared_ptr<nodes::statement> ptr_statement;
    /*
     * Check out the table identifier and construct
     * the statement node based on whether a qualified
     * or non-qualified identifier was found.  We use the
     * get<> method of boost::variant to return a pointer
     * to the variant's element, or NULL, and check for a
     * NULL return
     */
    if (std::string* p_string = boost::get<std::string>(&attr.table_identifier))
    {
      /* Non-qualified identifier */
      p_identifier =
        boost::shared_ptr<nodes::identifier>(
          new nodes::identifier(*p_string));
    }
    else
    {
      /* Qualified identifier */
      qualified_identifier_attr* p_attr = boost::get<qualified_identifier_attr>(&attr.table_identifier);
      p_identifier =
        boost::shared_ptr<nodes::identifier>(
          new nodes::qualified_identifier(p_attr->first, p_attr->second));
    }
    ptr_statement = 
      boost::shared_ptr<nodes::statement>(
        new nodes::statements::create_table(p_identifier,
                                            attr.temporary,
                                            attr.if_not_exists));

    output.set_root(ptr_statement);
    return SUCCESS;
  }
  return SYNTAX_ERROR;
}

/**
 * Utility method that takes a supplied index and
 * accesses the table element at that index and assumes
 * it is a column definition.
 */
column_definition_attr& get_column_definition(table_element_list_attr& subject, size_t index)
{
  return *boost::get<column_definition_attr>(&subject.at(index));
}

/**
 * Utility method that takes a supplied index and
 * returns whether the element at that index is
 * a column definition.  If return is false, one
 * can assume the element is a table constraint
 */
bool is_column_definition(table_element_list_attr& subject, size_t index)
{
  return (boost::get<table_constraint_attr>(&subject.at(index)) == NULL);
}

/**
 * Utility method that takes a supplied index and
 * accesses the table element at that index and assumes
 * it is a table constraint definition.
 */
table_constraint_attr& get_table_constraint(table_element_list_attr& subject, size_t index)
{
  return *boost::get<table_constraint_attr>(&subject.at(index));
}

} // namespace parser
} // namespace sqltoast

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::create_table_attr,
  (bool, temporary)
  (bool, if_not_exists)
  (sqltoast::parser::table_identifier_attr, table_identifier)
  (sqltoast::parser::table_element_list_attr, table_elements)
)

#endif /* SQLTOAST_PARSER_CREATE_TABLE_H */
