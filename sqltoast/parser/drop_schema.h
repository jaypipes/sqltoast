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
 * @file Grammar for parsing DROP SCHEMA statements
 */

#ifndef SQLTOAST_PARSER_DROP_SCHEMA_H
#define SQLTOAST_PARSER_DROP_SCHEMA_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"
#include "sqltoast/parser/common_rules.h"
#include "sqltoast/parser/identifier_rules.h"
#include "sqltoast/ast.h"
#include "sqltoast/nodes/statements/drop_schema.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/unused.hpp>

#include <map>
#include <string>
#include <utility>

namespace sqltoast
{
namespace parser
{
using boost::fusion::unused_type;

struct drop_schema_parser_attr
{
  bool if_exists;  /* if exists? */
  non_qualified_identifier_attr schema_identifier; /* schema identifier */
};

/**
 * Parser for DROP SCHEMA statements
 *
 * @detail
 *
 * Class template for a parser with a synthesized
 * attribute of an ast-node of type statements::drop_schema.
 *
 * A DROP SCHEMA statement can take the following EBNF form:
 * 
 * drop_schema : DROP schema_or_database if_exists identifier
 *
 * schema_or_database : SCHEMA 
 *                    | DATABASE
 *
 * if_exists : IF EXISTS
 *               | empty
 */
template<class iterator_type, class space_type>
struct drop_schema_parser: boost::spirit::qi::grammar<iterator_type,
                                                        drop_schema_parser_attr(),
                                                        space_type >
{
  drop_schema_parser() : drop_schema_parser::base_type(start)
  {
    using boost::spirit::qi::char_;
    using boost::spirit::qi::alpha;
    using boost::spirit::qi::alnum;
    using boost::spirit::ascii::no_case;
    using boost::spirit::eps;
    using boost::spirit::eoi;
    using boost::spirit::omit;
    using boost::spirit::qi::_val;
    using boost::spirit::lexeme;

    start = omit[no_case["drop"]]
            >> schema_or_database 
            >> if_exists 
            >> non_qualified_identifier_rule
            >> eoi;
    schema_or_database = omit[no_case["schema"] | no_case["database"]];
  }

  boost::spirit::qi::rule<iterator_type, drop_schema_parser_attr(), space_type > start;
  boost::spirit::qi::rule<iterator_type, unused_type, space_type > schema_or_database;
};

/* Our drop schema parsers, dropd during library initialization time */
drop_schema_parser<std::string::iterator, boost::spirit::qi::ascii::space_type> basic_ascii_drop_schema_parser;

/**
 * Parses a string for the DROP SCHEMA statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output attribute
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_drop_schema(iterator_type start,
                                iterator_type end,
                                drop_schema_parser_attr& output)
{
  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_drop_schema_parser,
                                      boost::spirit::qi::ascii::space,
                                      output))
    return SUCCESS;
  return SYNTAX_ERROR;
}

/**
 * Parses a string for the DROP SCHEMA statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output AST
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_drop_schema(iterator_type start,
                                iterator_type end,
                                ast& output)
{
  drop_schema_parser_attr attr;

  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_drop_schema_parser,
                                      boost::spirit::qi::ascii::space,
                                      attr))
  {
    boost::shared_ptr<nodes::identifier> ptr_identifier = 
      boost::shared_ptr<nodes::identifier>(
        new nodes::identifier(attr.schema_identifier));
    boost::shared_ptr<nodes::statement> ptr_statement = 
      boost::shared_ptr<nodes::statement>(
        new nodes::statements::drop_schema(ptr_identifier,
                                           attr.if_exists));
    output.set_root(ptr_statement);
    return SUCCESS;
  }
  return SYNTAX_ERROR;
}

} // namespace parser
} // namespace sqltoast

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::drop_schema_parser_attr,
  (bool, if_exists)
  (sqltoast::parser::non_qualified_identifier_attr, schema_identifier)
)

#endif /* SQLTOAST_PARSER_DROP_SCHEMA_H */
