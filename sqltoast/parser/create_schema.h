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
 * @file Grammar for parsing CREATE SCHEMA statements
 */

#ifndef SQLTOAST_PARSER_CREATE_SCHEMA_H
#define SQLTOAST_PARSER_CREATE_SCHEMA_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"
#include "sqltoast/parser/identifier_attributes.h"
#include "sqltoast/parser/specifier_attributes.h"
#include "sqltoast/ast.h"
#include "sqltoast/nodes/statements/create_schema.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/unused.hpp>

#include <string>

namespace sqltoast
{
namespace parser
{
using boost::fusion::unused_type;
using boost::spirit::qi::rule;
using boost::spirit::qi::grammar;
using boost::spirit::qi::ascii::space_type;

struct create_schema_attr
{
  bool if_not_exists;  /* if not exists? */
  non_qualified_identifier_attr schema_identifier; /* schema identifier */
  boost::optional<charset_specifier_attr> charset_specifier; /* optional charset name */
  boost::optional<collation_specifier_attr> collation_specifier; /* optional collation specifier */
};

/**
 * Parser for CREATE SCHEMA statements
 *
 * @detail
 *
 * Class template for a parser with a synthesized
 * attribute of an ast-node of type statements::create_schema.
 *
 * A CREATE SCHEMA statement can take the following EBNF form:
 * 
 * create_schema : CREATE schema_or_database if_not_exists identifier charset collation
 *
 * schema_or_database : SCHEMA 
 *                    | DATABASE
 *
 * if_not_exists : IF NOT EXISTS
 *               | empty
 *
 * collation : COLLATE identifier
 *           | empty
 *
 * charset : DEFAULT CHARACTER SET identifier
 *         | CHARACTER SET identifier
 *         | empty
 */
struct ascii_create_schema_parser: grammar<std::string::iterator,
                                           create_schema_attr(),
                                           space_type>
{
  ascii_create_schema_parser();

  rule<std::string::iterator, create_schema_attr(), space_type> start;
  rule<std::string::iterator, unused_type, space_type > schema_or_database;
};

/* Our create schema parsers, created during library initialization time */
extern SQLTOAST_API ascii_create_schema_parser ascii_create_schema_parser;

/**
 * Parses a string for the CREATE SCHEMA statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output attribute
 */
SQLTOAST_API
result_code parse_create_schema(std::string::iterator start,
                                std::string::iterator end,
                                create_schema_attr& output);

/**
 * Parses a string for the CREATE SCHEMA statement
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output AST
 */
SQLTOAST_API
result_code parse_create_schema(std::string::iterator start,
                                std::string::iterator end,
                                ast& output);

} // namespace parser
} // namespace sqltoast

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::create_schema_attr,
  (bool, if_not_exists)
  (sqltoast::parser::non_qualified_identifier_attr, schema_identifier)
  (boost::optional<sqltoast::parser::charset_specifier_attr>, charset_specifier)
  (boost::optional<sqltoast::parser::collation_specifier_attr>, collation_specifier)
)

#endif /* SQLTOAST_PARSER_CREATE_SCHEMA_H */
