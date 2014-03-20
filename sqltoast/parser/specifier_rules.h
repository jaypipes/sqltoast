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
 * @file Parser rules for various specifiers in the SQL system
 */

#ifndef SQLTOAST_PARSER_SPECIFIER_RULES_H
#define SQLTOAST_PARSER_SPECIFIER_RULES_H

#include "sqltoast/visibility.h"
#include "sqltoast/parser/specifier_attributes.h"
#include "sqltoast/parser/identifier_rules.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <string>

namespace sqltoast
{
namespace parser
{
using boost::spirit::ascii::space_type;
using boost::spirit::ascii::no_case;
using boost::spirit::omit;

/**
 * Parses a character set specifier of the form:
 *
 * [[DEFAULT] (CHARACTER SET | CHARSET) identifier]
 *
 * @note 
 *
 * Returns an empty string when no character set specifier
 * was found
 */
boost::spirit::qi::rule<std::string::iterator, charset_specifier_attr(), space_type >
charset_specifier_rule =
omit[-no_case["default"] >> ((no_case["character"] >> no_case["set"]) | no_case["charset"])] >> non_qualified_identifier_rule;

/**
 * Parses a collation specifier of the form:
 *
 * [COLLATE identifier]
 *
 * @note 
 *
 * Returns an empty string when no collation specifier
 * was found
 */
boost::spirit::qi::rule<std::string::iterator, collation_specifier_attr(), space_type >
collation_specifier_rule =
omit[no_case["collate"]] >> non_qualified_identifier_rule;

} // namespace parser
} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SPECIFIER_RULES_H */
