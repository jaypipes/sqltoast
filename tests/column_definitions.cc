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
 * @file Tests for parsing of column definitions
 */

#include "config.h"

//#define BOOST_SPIRIT_DEBUG

#include <sqltoast/parse.h>
#include <sqltoast/parser/column_definition.h>

#include <sqltoast/ast.h>
#include <sqltoast/nodes/column_definition.h>

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

using parser::column_definition_attr;
using parser::non_qualified_identifier_attr;
using parser::qualified_identifier_attr;
using parser::table_identifier_attr;
using parser::default_value_attr;
using parser::is_nullable_attr;
using parser::is_unique_attr;
using parser::is_primary_key_attr;
using parser::collation_specifier_attr;
using parser::optional_column_definition_attr;
using parser::parse_column_definition;
using parser::column_type_specifier_attr;
using parser::check_constraint_attr;
using parser::references_column_constraint_attr;
using parser::optional_referential_actions_attr;

TEST(test_parse_column_definitions, column_types_and_nullable_constraint)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       string, // The field identifier
                       sql_column_type // The SQL column type determined
                      > expected_result;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("id INT", SUCCESS, "id", COLUMN_TYPE_INTEGER));
  inputs.push_back(expected_result("id INTEGER", SUCCESS, "id", COLUMN_TYPE_INTEGER));
  inputs.push_back(expected_result("id BIGINT", SUCCESS, "id", COLUMN_TYPE_BIG_INTEGER));
  inputs.push_back(expected_result("id SMALLINT", SUCCESS, "id", COLUMN_TYPE_SMALL_INTEGER));
  inputs.push_back(expected_result("id FLOAT", SUCCESS, "id", COLUMN_TYPE_FLOAT));
  inputs.push_back(expected_result("id DOUBLE", SUCCESS, "id", COLUMN_TYPE_DOUBLE));
  inputs.push_back(expected_result("id DOUBLE PRECISION", SUCCESS, "id", COLUMN_TYPE_DOUBLE));
  inputs.push_back(expected_result("id DECIMAL", SUCCESS, "id", COLUMN_TYPE_DECIMAL));
  inputs.push_back(expected_result("id DATE", SUCCESS, "id", COLUMN_TYPE_DATE));
  inputs.push_back(expected_result("id TIME", SUCCESS, "id", COLUMN_TYPE_TIME));
  inputs.push_back(expected_result("id TIMESTAMP", SUCCESS, "id", COLUMN_TYPE_TIMESTAMP));
  inputs.push_back(expected_result("id REAL", SUCCESS, "id", COLUMN_TYPE_FLOAT));
  inputs.push_back(expected_result("id CHAR", SUCCESS, "id", COLUMN_TYPE_CHAR));
  inputs.push_back(expected_result("id CHARACTER", SUCCESS, "id", COLUMN_TYPE_CHAR));
  inputs.push_back(expected_result("id VARCHAR", SUCCESS, "id", COLUMN_TYPE_VARCHAR));
  inputs.push_back(expected_result("id CHARACTER VARYING", SUCCESS, "id", COLUMN_TYPE_VARCHAR));
  inputs.push_back(expected_result("id CHAR VARYING", SUCCESS, "id", COLUMN_TYPE_VARCHAR));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    string& name = tuples::get<2>(er);
    sql_column_type& ct = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;
    ASSERT_EQ(name, fill.name) << " for expression " << s;
    ASSERT_EQ(ct, fill.type) << " for expression " << s;
  }
}

TEST(test_parse_column_definitions, nullable_constraint)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Did nullable constraint appear?
                       is_nullable_attr // Is nullable?
                      > expected_result;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("id INT", SUCCESS, false, true));
  inputs.push_back(expected_result("id INT DEFAULT \"1\" NULL", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT DEFAULT \"1\" NOT NULL", SUCCESS, true, false));
  inputs.push_back(expected_result("id INT NOT NULL", SUCCESS, true, false));
  inputs.push_back(expected_result("id INT NULL", SUCCESS, true, true));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool nullable_appears = tuples::get<2>(er);
    bool is_nullable = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (nullable_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<is_nullable_attr> >(optionals)) << " for expression " << s;
      ASSERT_EQ(is_nullable, *boost::fusion::at_c<0>(optionals)) << " for expression " << s;
    }
  }
}

TEST(test_parse_column_definitions, default_clause)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // DEFAULT clause appears?
                       default_value_attr // The default value attribute
                      > expected_result;

  vector<expected_result> inputs;
  /* test with no DEFAULT clause */
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, default_value_attr()));
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, default_value_attr()));
  /* With a DEFAULT clause */
  inputs.push_back(expected_result("id INT NOT NULL DEFAULT \"1\"", SUCCESS, true, default_value_attr(string("1"))));
  inputs.push_back(expected_result("id INT NULL DEFAULT \"1\"", SUCCESS, true, default_value_attr(string("1"))));
  inputs.push_back(expected_result("id INT DEFAULT \"1\"", SUCCESS, true, default_value_attr(string("1"))));
  inputs.push_back(expected_result("id INT DEFAULT 1.982", SUCCESS, true, default_value_attr(double(1.982))));
  inputs.push_back(expected_result("id INT DEFAULT 1", SUCCESS, true, default_value_attr(int(1))));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool default_appears = tuples::get<2>(er);
    default_value_attr& default_value = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);
    ASSERT_EQ(r, result) << " for expression " << s;

    if (default_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<default_value_attr> >(optionals)) << " for expression " << s;
      default_value_attr& our_default_value = *boost::fusion::at_c<2>(optionals);
      ASSERT_EQ(default_value, our_default_value) << " for expression " << s;
    }
  }
}

TEST(test_parse_column_definitions, primary_key_constraint)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Did primary key constraint clause appear?
                       is_primary_key_attr // Is primary key?
                      > expected_result;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("id INT", SUCCESS, false, false));
  inputs.push_back(expected_result("id INT NOT NULL", SUCCESS, false, false));
  inputs.push_back(expected_result("id INT PRIMARY KEY NOT NULL", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT NOT NULL PRIMARY KEY", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT NULL DEFAULT \"1\" PRIMARY KEY", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT NULL DEFAULT \"1\"", SUCCESS, false, false));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool primary_key_appears = tuples::get<2>(er);
    is_primary_key_attr is_primary_key = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (primary_key_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<is_primary_key_attr> >(optionals)) << " for expression " << s;
      ASSERT_EQ(is_primary_key, *boost::fusion::at_c<1>(optionals)) << " for expression " << s;
    }
  }
}

TEST(test_parse_column_definitions, unique_constraint)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Did unique constraint clause appear?
                       is_unique_attr // Is unique?
                      > expected_result;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("id INT", SUCCESS, false, false));
  inputs.push_back(expected_result("id INT NOT NULL", SUCCESS, false, false));
  inputs.push_back(expected_result("id INT UNIQUE NOT NULL", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT NOT NULL UNIQUE", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT NULL DEFAULT \"1\" UNIQUE", SUCCESS, true, true));
  inputs.push_back(expected_result("id INT NULL DEFAULT \"1\"", SUCCESS, false, false));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool unique_appears = tuples::get<2>(er);
    is_unique_attr is_unique = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (unique_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<is_unique_attr> >(optionals)) << " for expression " << s;
      ASSERT_EQ(is_unique, *boost::fusion::at_c<5>(optionals)) << " for expression " << s;
    }
  }
}

TEST(test_parse_column_definitions, collate_clause)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // collate clause appears?
                       collation_specifier_attr // The collation attribute
                      > expected_result;

  vector<expected_result> inputs;
  /* test with no COLLATE clause */
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, collation_specifier_attr()));
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, collation_specifier_attr()));
  /* With a COLLATE clause */
  inputs.push_back(expected_result("id CHAR NOT NULL COLLATE \"utf8_bin\"", SUCCESS, true, collation_specifier_attr("utf8_bin")));
  inputs.push_back(expected_result("id CHAR NULL COLLATE utf8_bin", SUCCESS, true, collation_specifier_attr("utf8_bin")));
  inputs.push_back(expected_result("id CHAR COLLATE utf8_bin", SUCCESS, true, collation_specifier_attr("utf8_bin")));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool collation_appears = tuples::get<2>(er);
    collation_specifier_attr& collation_specifier = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);
    ASSERT_EQ(r, result) << " for expression " << s;

    if (collation_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<collation_specifier_attr> >(optionals)) << " for expression " << s;
      ASSERT_EQ(collation_specifier, *boost::fusion::at_c<4>(optionals)) << " for expression " << s;
    }
  }
}

TEST(test_parse_column_definitions, column_type_specifiers)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // specifier appears?
                       column_type_specifier_attr // The type specifier argument
                      > expected_result;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("id CHAR NULL", SUCCESS, false, column_type_specifier_attr(0)));
  inputs.push_back(expected_result("id CHAR(1) NULL", SUCCESS, true, column_type_specifier_attr(1)));
  inputs.push_back(expected_result("id CHAR(-1) NULL", SUCCESS, false, column_type_specifier_attr(1)));
  inputs.push_back(expected_result("id VARCHAR(1000) NULL", SUCCESS, true, column_type_specifier_attr(1000)));
  inputs.push_back(expected_result("id DECIMAL(10,2) NULL", SUCCESS, true, column_type_specifier_attr(pair<uint32_t, uint32_t>(10,2))));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool column_type_appears = tuples::get<2>(er);
    column_type_specifier_attr& column_type_specifier = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);
    ASSERT_EQ(r, result) << " for expression " << s;

    if (column_type_appears)
    {
      ASSERT_TRUE(fill.type_specifier);
      column_type_specifier_attr& our_column_type_specifier = *fill.type_specifier;
      if (uint32_t* val = boost::get<uint32_t>(&our_column_type_specifier))
      {
        uint32_t* cmp_val = boost::get<uint32_t>(&column_type_specifier);
        ASSERT_EQ(*cmp_val, *val) << " for expression " << s;
      }
      if (std::pair<uint32_t, uint32_t>* val = boost::get<std::pair<uint32_t, uint32_t> >(&our_column_type_specifier))
      {
        std::pair<uint32_t, uint32_t>* cmp_val = boost::get<std::pair<uint32_t,uint32_t> >(&column_type_specifier);
        ASSERT_EQ(cmp_val->first, val->first) << " for expression " << s;
        ASSERT_EQ(cmp_val->second, val->second) << " for expression " << s;
      }
    }
  }
}

TEST(test_parse_column_definitions, check_constraints)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // check constraint clause appears?
                       check_constraint_attr // The check constraint attribute
                      > expected_result;

  vector<expected_result> inputs;
  /* test with no CHECK clause */
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, check_constraint_attr()));
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, check_constraint_attr()));
  /* With a CHECK clause */
  inputs.push_back(expected_result("id CHAR NOT NULL CHECK (\"id > 'a'\")", SUCCESS, true, check_constraint_attr("id > 'a'")));
  inputs.push_back(expected_result("id CHAR CHECK (\"id > 'a'\") NOT NULL", SUCCESS, true, check_constraint_attr("id > 'a'")));
  inputs.push_back(expected_result("id CHAR CHECK (\"id > 'a'\") DEFAULT \"\"", SUCCESS, true, check_constraint_attr("id > 'a'")));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool check_appears = tuples::get<2>(er);
    check_constraint_attr& check_constraint = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);
    ASSERT_EQ(r, result) << " for expression " << s;

    if (check_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<check_constraint_attr> >(optionals)) << " for expression " << s;
      ASSERT_EQ(check_constraint, *boost::fusion::at_c<6>(optionals)) << " for expression " << s;
    }
  }
}

TEST(test_parse_column_definitions, references)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // references clause appears?
                       references_column_constraint_attr // The references attribute
                      > expected_result;

  /* Some examples used below.. */
  references_column_constraint_attr e1, e2, e3, e4, e5;
  e1.table_identifier = "t1";
  e1.field_identifier = "id";
  e2.table_identifier = qualified_identifier_attr("test", "t1");
  e2.field_identifier = "id";
  e3.table_identifier = "t1";
  e3.field_identifier = "id";
  e3.optional_actions = optional_referential_actions_attr(REF_ACTION_CASCADE, boost::optional<sql_referential_action>());
  e4.table_identifier = "t1";
  e4.field_identifier = "id";
  e4.optional_actions = optional_referential_actions_attr(boost::optional<sql_referential_action>(), REF_ACTION_SET_DEFAULT);
  e5.table_identifier = "t1";
  e5.field_identifier = "id";
  e5.optional_actions = optional_referential_actions_attr(REF_ACTION_CASCADE, REF_ACTION_SET_NULL);

  vector<expected_result> inputs;
  /* test with no REFERENCES clause */
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, references_column_constraint_attr()));
  inputs.push_back(expected_result("id INT NULL", SUCCESS, false, references_column_constraint_attr()));
  /* With various REFERENCES clauses */
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES t1 (id)", SUCCESS, true, e1));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES \"t1\" (\"id\")", SUCCESS, true, e1));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES test.t1 (id)", SUCCESS, true, e2));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES test.t1 (\"id\")", SUCCESS, true, e2));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES test.t1 (\"id\")", SUCCESS, true, e2));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES t1 (id) ON UPDATE CASCADE", SUCCESS, true, e3));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES t1 (id) ON DELETE SET DEFAULT", SUCCESS, true, e4));
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES t1 (id) ON UPDATE CASCADE ON DELETE SET NULL", SUCCESS, true, e5));
  /* Poor references clause formation */
  inputs.push_back(expected_result("id CHAR NOT NULL REFERENCES t1 (id", SUCCESS, false, e1));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool references_appears = tuples::get<2>(er);
    references_column_constraint_attr& references = tuples::get<3>(er);
    table_identifier_attr& table_identifier = references.table_identifier;
    non_qualified_identifier_attr& field_identifier = references.field_identifier;

    string::iterator start = s.begin();
    string::iterator end = s.end();

    column_definition_attr fill;
    
    result_code result = parse_column_definition(start, end, fill);
    ASSERT_EQ(r, result) << " for expression " << s;

    if (references_appears)
    {
      boost::optional<optional_column_definition_attr> optionals_wrapper = fill.optionals;
      ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
      optional_column_definition_attr& optionals = *optionals_wrapper;
      ASSERT_TRUE(boost::fusion::has_key<boost::optional<references_column_constraint_attr> >(optionals)) << " for expression " << s;

      references_column_constraint_attr& our_references = *boost::fusion::at_c<3>(optionals);
      table_identifier_attr& our_table_identifier = our_references.table_identifier;
      non_qualified_identifier_attr& our_field_identifier = our_references.field_identifier;
      
      if (non_qualified_identifier_attr* val = boost::get<non_qualified_identifier_attr>(&our_table_identifier))
      {
        non_qualified_identifier_attr* cmp_val = boost::get<non_qualified_identifier_attr>(&table_identifier);
        ASSERT_EQ(*cmp_val, *val) << " for expression " << s;
      }
      else if (qualified_identifier_attr* val = boost::get<qualified_identifier_attr>(&our_table_identifier))
      {
        qualified_identifier_attr* cmp_val = boost::get<qualified_identifier_attr>(&table_identifier);
        ASSERT_EQ(cmp_val->first, val->first) << " for expression " << s;
        ASSERT_EQ(cmp_val->second, val->second) << " for expression " << s;
      }
      ASSERT_EQ(field_identifier, our_field_identifier) << " for expression " << s;

      if (references.optional_actions)
      {
        ASSERT_TRUE(our_references.optional_actions) << " for expression " << s;
        optional_referential_actions_attr& optional_actions = *references.optional_actions;
        optional_referential_actions_attr& our_optional_actions = *our_references.optional_actions;

        if (boost::fusion::at_c<0>(optional_actions))
        {
          ASSERT_TRUE(boost::fusion::at_c<0>(our_optional_actions)) << " for expression " << s;
          ASSERT_EQ(*boost::fusion::at_c<0>(optional_actions), *boost::fusion::at_c<0>(our_optional_actions)) << " for expression " << s;
        }
        else
        {
          ASSERT_FALSE(boost::fusion::at_c<0>(our_optional_actions)) << " for expression " << s;
        }
        if (boost::fusion::at_c<1>(optional_actions))
        {
          ASSERT_TRUE(boost::fusion::at_c<1>(our_optional_actions)) << " for expression " << s;
          ASSERT_EQ(*boost::fusion::at_c<1>(optional_actions), *boost::fusion::at_c<1>(our_optional_actions)) << " for expression " << s;
        }
        else
        {
          ASSERT_FALSE(boost::fusion::at_c<1>(our_optional_actions)) << " for expression " << s;
        }
      }
    }
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
