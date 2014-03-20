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
 * @file Tests for parsing of identifiers
 */

#include "config.h"

//#define BOOST_SPIRIT_DEBUG

#include <sqltoast/common.h>
#include <sqltoast/parser/identifier_rules.h>

#include <gtest/gtest.h>

#include <boost/variant.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/tuple/tuple.hpp>

#include <string>
#include <utility>
#include <exception>

using namespace sqltoast;
using namespace std;
using namespace boost;

using parser::non_qualified_identifier_attr;
using parser::non_qualified_identifier_rule;
using parser::qualified_identifier_attr;
using parser::qualified_identifier_rule;

template <typename P, typename T>
result_code test_phrase_parser(string::iterator& start,
                               string::iterator& end,
                               P const& p,
                               T& attr)
{
  using boost::spirit::qi::phrase_parse;
  using boost::spirit::qi::ascii::space;
  
  if (phrase_parse(start, end, p, space, attr) && (start == end))
    return SUCCESS;
  else
    return SYNTAX_ERROR;
}

TEST(test_parse_identifiers, non_qualified_identifiers)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       non_qualified_identifier_attr // The name of the constraint
                      > expected_result;

  non_qualified_identifier_attr e1("t1");
  non_qualified_identifier_attr e2("test spaces t1");
  non_qualified_identifier_attr e3("test_underscore_name");

  vector<expected_result> inputs;
  inputs.push_back(expected_result("t1", SUCCESS, e1));
  inputs.push_back(expected_result("\"test spaces t1\"", SUCCESS, e2));
  inputs.push_back(expected_result("test_underscore_name", SUCCESS, e3));
  inputs.push_back(expected_result("test_bad name", SYNTAX_ERROR, e1));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    non_qualified_identifier_attr& name = tuples::get<2>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    non_qualified_identifier_attr fill;
    
    result_code result = test_phrase_parser(start, end, non_qualified_identifier_rule, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (r == SUCCESS)
    {
      ASSERT_EQ(name, fill) << " for expression " << s;
    }
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

