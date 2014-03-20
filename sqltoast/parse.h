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
 * @file SQL to AST methods
 */

#ifndef SQLTOAST_PARSE_H
#define SQLTOAST_PARSE_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"
#include "sqltoast/parser/create_schema.h"
#include "sqltoast/parser/create_table.h"
#include "sqltoast/ast.h"

#include <string>

namespace sqltoast
{
namespace parser
{
using boost::fusion::unused_type;

typedef boost::variant<create_schema_attr,
                       create_table_attr> sql_statement_attr;

template<class iterator_type, class space_type>
struct ansi_sql_parser: boost::spirit::qi::grammar<iterator_type,
                                                   sql_statement_attr(),
                                                   space_type >
{
  ansi_sql_parser() : ansi_sql_parser::base_type(start)
  {
    using boost::spirit::eoi;

    start = (
              ascii_create_schema_parser
            | basic_ascii_create_table_parser
            )
            >> eoi;
  }

  boost::spirit::qi::rule<iterator_type, sql_statement_attr(), space_type > start;
};

/* Our ANSI SQL parser, created during library initialization time */
ansi_sql_parser<std::string::iterator, boost::spirit::qi::ascii::space_type> basic_ascii_ansi_sql_parser;

/**
 * Parses a SQL statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output attribute
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_sql_statement(iterator_type start,
                                iterator_type end,
                                sql_statement_attr& output)
{
  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_ansi_sql_parser,
                                      boost::spirit::qi::ascii::space,
                                      output))
    return SUCCESS;
  return SYNTAX_ERROR;
}

/**
 * Parses a string for the CREATE SCHEMA statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output AST
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_sql_statement(iterator_type start,
                                iterator_type end,
                                ast& output)
{
  sql_statement_attr attr;

  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_ansi_sql_parser,
                                      boost::spirit::qi::ascii::space,
                                      attr))
  {
    return SUCCESS;
  }
  return SYNTAX_ERROR;
}

} // namespace parser

} // namespace sqltoast

#endif /* SQLTOAST_PARSE_H */
