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
 * @file Implementation of various CREATE SCHEMA parsers
 */
#include "config.h"

#include "sqltoast/parser/common_rules.h"
#include "sqltoast/parser/identifier_rules.h"
#include "sqltoast/parser/specifier_rules.h"
#include "sqltoast/parser/create_schema.h"

#include "sqltoast/nodes/statements/create_schema.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;

namespace sqltoast
{
namespace parser
{

ascii_create_schema_parser::ascii_create_schema_parser() :
  ascii_create_schema_parser::base_type(start)
{
  using boost::spirit::ascii::no_case;
  using boost::spirit::eoi;
  using boost::spirit::omit;

  schema_or_database = omit[no_case["schema"] | no_case["database"]];
  start = omit[no_case["create"]]
          >> schema_or_database 
          >> if_not_exists 
          >> non_qualified_identifier_rule
          >> -charset_specifier_rule
          >> -collation_specifier_rule 
          >> eoi;
}

struct ascii_create_schema_parser ascii_create_schema_parser;

result_code parse_create_schema(string::iterator start,
                                string::iterator end,
                                create_schema_attr& output)
{
  using boost::spirit::qi::phrase_parse;
  using boost::spirit::qi::ascii::space;

  if (phrase_parse(start,
                   end,
                   ascii_create_schema_parser,
                   space,
                   output))
    return SUCCESS;
  return SYNTAX_ERROR;
}

result_code parse_create_schema(string::iterator start,
                                string::iterator end,
                                ast& output)
{
  using boost::spirit::qi::phrase_parse;
  using boost::spirit::qi::ascii::space;
  using boost::shared_ptr;

  create_schema_attr attr;

  if (phrase_parse(start,
                   end,
                   ascii_create_schema_parser,
                   space,
                   attr))
  {
    shared_ptr<nodes::identifier> ptr_identifier = 
      shared_ptr<nodes::identifier>(
        new nodes::identifier(attr.schema_identifier));
    shared_ptr<nodes::statement> ptr_statement = 
      shared_ptr<nodes::statement>(
        new nodes::statements::create_schema(ptr_identifier,
                                             attr.if_not_exists,
                                             attr.charset_specifier,
                                             attr.collation_specifier));
    output.set_root(ptr_statement);
    return SUCCESS;
  }
  return SYNTAX_ERROR;
}

} // namespace parser
} // namespace sqltoast
