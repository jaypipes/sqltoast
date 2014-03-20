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
 * @file Grammar for parsing key-value lists
 */

#ifndef SQLTOAST_PARSER_KEY_VALUE_LIST_H
#define SQLTOAST_PARSER_KEY_VALUE_LIST_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <map>
#include <string>
#include <utility>

namespace sqltoast
{
namespace parser
{
using boost::spirit::ascii::space_type;

typedef std::pair<std::string, std::string> key_value_list_item;
typedef std::map<std::string, std::string> key_value_list;

/**
 * Class template for a parser with a synthesized
 * attribute of a key value map
 */
template<class iterator_type>
struct key_value_list_parser : boost::spirit::qi::grammar<iterator_type, key_value_list(), space_type >
{
  key_value_list_parser() : key_value_list_parser::base_type(start)
  {
    using boost::spirit::qi::char_;
    using boost::spirit::qi::alpha;
    using boost::spirit::qi::alnum;

    start = item % ','; /* Zero or more items make a key-value list */
    item = key >> ':' >> value; /* An item is a key and a value, separated by a colon */
    key = alpha >> *alnum; /* A key is an identifier beginning with a letter */
    value = ('"' >> *(~char_('"')) >> '"') /* A value can be any number or letter combination or string */
            | *alnum;
  }

  boost::spirit::qi::rule<iterator_type, key_value_list(), space_type > start;
  boost::spirit::qi::rule<iterator_type, key_value_list_item(), space_type > item;
  boost::spirit::qi::rule<iterator_type, std::string(), space_type > key;
  boost::spirit::qi::rule<iterator_type, std::string(), space_type > value;
};

/**
 * Parses a string into a key-value map
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output map
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_key_value_list(iterator_type start,
                                 iterator_type end,
                                 key_value_list& output)
{
  parser::key_value_list_parser<iterator_type> p;

  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      p,
                                      boost::spirit::qi::ascii::space,
                                      output))
    return SUCCESS;
  return SYNTAX_ERROR;
}

} // namespace parser
} // namespace sqltoast

#endif /* SQLTOAST_PARSER_KEY_VALUE_LIST_H */
