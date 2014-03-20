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
 * @file Parser rules for various identifiers in the SQL system
 */

#ifndef SQLTOAST_PARSER_IDENTIFIER_RULES_H
#define SQLTOAST_PARSER_IDENTIFIER_RULES_H

#include "sqltoast/visibility.h"
#include "sqltoast/parser/identifier_attributes.h"
#include "sqltoast/parser/common_rules.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>

#include <string>
#include <utility>

namespace sqltoast
{
namespace parser
{
using boost::spirit::ascii::space_type;
using boost::spirit::qi::char_;
using boost::spirit::qi::alpha;
using boost::spirit::qi::alnum;
using boost::spirit::ascii::no_case;
using boost::spirit::eps;
using boost::spirit::omit;
using boost::spirit::lit;
using boost::spirit::qi::locals;
using boost::spirit::qi::_val;
using boost::spirit::qi::_1;
using boost::spirit::qi::_a;
using boost::spirit::lexeme;

/**
 * A parser rule for a simple identifier
 *
 * A simple identifier is a character sequence beginning
 * with a letter and having zero or more alphanumeric characters
 * or the underscore:
 *
 *  - [a-Z][a-Z0-9_]*
 *
 * The rule has an attribute (return value) of a string
 * containing the identifier with any enclosing quotes
 * removed.
 */
basic_string_phrase simple_identifier_rule = lexeme[(alpha >> *(alnum | char_('_')))];

/**
 * A parser rule for an ANSI-SQL quoted identifier.  An
 * ANSI-SQL quoted identifier is alphanumeric characters
 * enclosed in double quotes.
 *
 * The rule has an attribute (return value) of a string
 * containing the identifier with any enclosing quotes
 * removed.
 */
basic_string_phrase quoted_identifier_rule = 
  lit('"') 
>> lexeme[*(~char_('"'))]
>> lit('"');

/**
 * A parser rule for an extended quoted identifier.  An extended
 * quoted identifier has any of the following forms:
 *
 *  - ".*"
 *  - `.*`
 *
 * The rule has an attribute (return value) of a string
 * containing the identifier with any enclosing quotes
 * removed.
 *
 * @note
 *
 * The single quote is not allowed for identifiers
 */
boost::spirit::qi::rule<std::string::iterator,
                        std::string(),
                        space_type,
                        locals<char> > extended_quoted_identifier_rule = 
  omit[char_("\"`") [_a = _1]] // begin with a ", an ', or a `. Store that character in _a
>> lexeme[*(char_ - char_(_a))] // everything until the _a character
>> omit[char_(_a)]; // the stored character

/**
 * A parser rule for a non-qualified identifier
 *
 * A non-qualified identifier can be either a quoted or simple identifier, and
 * does not have a period delimiter demarking qualification
 *
 * The rule has an attribute (return value) of a string
 * containing the identifier with any enclosing quotes
 * removed.
 */
boost::spirit::qi::rule<std::string::iterator, non_qualified_identifier_attr(), space_type>
non_qualified_identifier_rule =
  simple_identifier_rule
| quoted_identifier_rule;

/**
 * A parser for a qualified identifier.  A qualified identifier
 * is a qualified name for an object in the system.  It can
 * have any of the following forms:
 *
 *  - identifier '.' identifier
 *
 * The rule has an attribute (return value) of a pair of strings
 * containing the qualifying identifier and the object identifier 
 * with any enclosing quotes removed.
 */
boost::spirit::qi::rule<std::string::iterator, qualified_identifier_attr(), space_type>
qualified_identifier_rule = 
   non_qualified_identifier_rule
>> lit('.')
>> non_qualified_identifier_rule;

/**
 * A rule for table identifiers, which can be qualified, fully-qualified,
 * or non-qualified.
 */
boost::spirit::qi::rule<std::string::iterator, table_identifier_attr(), space_type>
table_identifier_rule = qualified_identifier_rule | non_qualified_identifier_rule;

} // namespace parser
} // namespace sqltoast

#ifdef BOOST_SPIRIT_DEBUG

#include <iostream>
#include <iomanip>
#include <boost/tuple/tuple.hpp>

namespace std
{
  using boost::fusion::unused_type;
  using sqltoast::parser::table_identifier_attr;
  using sqltoast::parser::non_qualified_identifier_attr;
  using sqltoast::parser::qualified_identifier_attr;

  std::ostream& operator<<(std::ostream& stream, table_identifier_attr& value)
  {
    if (non_qualified_identifier_attr* val = boost::get<non_qualified_identifier_attr>(&value))
    {
      stream << *val;
    }
    if (qualified_identifier_attr* val = boost::get<qualified_identifier_attr>(&value))
    {
      stream << val->first << "." << val->second;
    }
    return stream;
  }
}
#endif

#endif /* SQLTOAST_PARSER_IDENTIFIER_RULES_H */
