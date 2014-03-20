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
 * @file Common rules used in multiple parsers and grammars
 */

#ifndef SQLTOAST_PARSER_COMMON_RULES_H
#define SQLTOAST_PARSER_COMMON_RULES_H

#include "sqltoast/visibility.h"
#include "sqltoast/parser/common_rules.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <boost/optional.hpp>

#include <string>
#include <utility>

namespace sqltoast
{
namespace parser
{
using boost::spirit::ascii::space_type;
using boost::spirit::qi::char_;
using boost::spirit::qi::int_;
using boost::spirit::qi::alpha;
using boost::spirit::qi::alnum;
using boost::spirit::ascii::no_case;
using boost::spirit::eps;
using boost::spirit::omit;
using boost::spirit::lit;
using boost::spirit::qi::_val;
using boost::spirit::qi::_1;
using boost::spirit::qi::_a;
using boost::spirit::lexeme;
using boost::spirit::qi::locals;

typedef std::pair<boost::optional<std::string>, std::string>
basic_string_pair_optional_first;

/**
 * A type of simple terminal rule that returns
 * a string.
 */
typedef boost::spirit::qi::rule<std::string::iterator, std::string(), space_type>
basic_string_phrase;

/**
 * A type of terminal rule that returns
 * a pair of strings.
 */
typedef boost::spirit::qi::rule<std::string::iterator, std::pair<std::string, std::string>(), space_type>
basic_string_pair_phrase;

typedef boost::spirit::qi::rule<std::string::iterator, basic_string_pair_optional_first(), space_type>
basic_string_pair_optional_first_phrase;

/**
 * A parser attribute for a literal value
 */
typedef boost::variant<double,
                       int
                      > literal_value_attr;

/**
 * We do a strict parsing for doubles to ensure that
 * coercion is not done implicitly.
 */
boost::spirit::qi::real_parser<double, boost::spirit::qi::strict_real_policies<double> > strict_double;

/**
 * A parser rule for a literal value
 */
boost::spirit::qi::rule<std::string::iterator,
                        literal_value_attr(),
                        space_type >
literal_value_rule =
strict_double | int_;


/**
 * A parser rule for a quoted string.
 *
 * The rule has an attribute (return value) of a string
 * containing the identifier with any enclosing quotes
 * removed.
 */
boost::spirit::qi::rule<std::string::iterator,
                        std::string(),
                        space_type,
                        locals<char> >
quoted_string_rule = 
omit[char_("\"'") [_a = _1]] // begin with a ", or an '. Store that character in _a
>> lexeme[*(char_ - char_(_a))] // everything until the _a character
>> omit[char_(_a)]; // the stored character

/**
 * A type of terminal rule that returns true
 * if a phrase is present, false otherwise
 */
typedef boost::spirit::qi::rule<std::string::iterator, bool(), space_type> optional_phrase_rule;

/**
 * A generic parser terminal for the TEMPORARY keyword
 *
 * The rule has an attribute of a boolean, which is true
 * if the keyword was found, false otherwise.
 */
optional_phrase_rule temporary =
  (omit[no_case["temporary"]] [_val = true])
| eps[_val = false];

/**
 * A generic parser terminal for the IF NOT EXISTS clause.
 *
 * The rule has an attribute of a boolean, which is true
 * if the clause was found, false otherwise.
 */
optional_phrase_rule if_not_exists =
  (omit[no_case["if"] >> no_case["not"] >> no_case["exists"]] [_val = true])
| eps[_val = false];

/**
 * A generic parser terminal for the IF EXISTS clause.
 *
 * The rule has an attribute of a boolean, which is true
 * if the clause was found, false otherwise.
 */
optional_phrase_rule if_exists =
  (omit[no_case["if"] >> no_case["exists"]] [_val = true])
| eps[_val = false];

} // namespace parser
} // namespace sqltoast

#endif /* SQLTOAST_PARSER_COMMON_RULES_H */
