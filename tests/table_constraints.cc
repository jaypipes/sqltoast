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
 * @file Tests for parsing of table constraints
 */

#include "config.h"

//#define BOOST_SPIRIT_DEBUG

#include <sqltoast/parser/table_constraint.h>

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

using parser::table_constraint_attr;
using parser::table_constraint_definition_attr;
using parser::unique_constraint_definition_attr;
using parser::references_specification_attr;
using parser::referential_constraint_definition_attr;
using parser::check_constraint_definition_attr;
using parser::non_qualified_identifier_attr;
using parser::column_name_list_attr;
using parser::parse_table_constraint;
using parser::non_qualified_identifier_attr;
using parser::qualified_identifier_attr;
using parser::search_condition_attr;

TEST(test_parse_table_constraints, optional_contraint_name)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Has a constraint name?
                       non_qualified_identifier_attr // The name of the constraint
                      > expected_result;

  non_qualified_identifier_attr e1("cst_formal");

  vector<expected_result> inputs;
  inputs.push_back(expected_result("CONSTRAINT cst_formal UNIQUE (id)", SUCCESS, true, e1));
  inputs.push_back(expected_result("UNIQUE (id)", SUCCESS, false, e1));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool name_appears = tuples::get<2>(er);
    non_qualified_identifier_attr& name = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    table_constraint_attr fill;
    
    result_code result = parse_table_constraint(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (name_appears)
    {
      ASSERT_TRUE(fill.name) << " for expression " << s;
      ASSERT_EQ(name, *fill.name) << " for expression " << s;
    }
  }
}

TEST(test_parse_table_constraints, unique_and_primary_keys)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Unique constraint appears?
                       unique_constraint_definition_attr // The attribute that should be produced
                      > expected_result;

  column_name_list_attr c1;
  c1.push_back("f1");

  column_name_list_attr c2;
  c2.push_back("f1");
  c2.push_back("f2");

  unique_constraint_definition_attr e1, e2;
  e1.is_primary_key = false;
  e1.columns = c1;
  e2.is_primary_key = true;
  e2.columns = c2;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("CONSTRAINT cst_formal UNIQUE (f1)", SUCCESS, true, e1));
  inputs.push_back(expected_result("UNIQUE (f1)", SUCCESS, true, e1));
  inputs.push_back(expected_result("PRIMARY KEY (f1, f2)", SUCCESS, true, e2));
  inputs.push_back(expected_result("CONSTRAINT pk PRIMARY KEY (f1, f2)", SUCCESS, true, e2));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool unique_appears = tuples::get<2>(er);
    unique_constraint_definition_attr& constraint = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    table_constraint_attr fill;
    
    result_code result = parse_table_constraint(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (unique_appears)
    {
      unique_constraint_definition_attr* our_constraint = boost::get<unique_constraint_definition_attr>(&fill.definition);
      ASSERT_TRUE(our_constraint);
      ASSERT_EQ(constraint.is_primary_key, our_constraint->is_primary_key) << " for expression " << s;
      ASSERT_EQ(constraint.columns.size(), our_constraint->columns.size()) << " for expression " << s;
      size_t x = 0;
      for (column_name_list_attr::iterator iter = constraint.columns.begin();
           iter != constraint.columns.end();
           ++iter, ++x)
      {
        ASSERT_EQ(constraint.columns[x], our_constraint->columns[x]) << " for expression " << s;
      }
    }
  }
}

TEST(test_parse_table_constraints, referential)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Unique constraint appears?
                       referential_constraint_definition_attr // The attribute that should be produced
                      > expected_result;

  column_name_list_attr c1, c2, c3, c4;
  c1.push_back("f1");
  c2.push_back("f1");
  c2.push_back("f2");
  c3.push_back("rf1");
  c4.push_back("rf1");
  c4.push_back("rf2");

  references_specification_attr r1, r2;

  r1.referenced_table = non_qualified_identifier_attr("t1");
  r1.referenced_columns = c3;
  r2.referenced_table = qualified_identifier_attr("test", "t1");
  r2.referenced_columns = c4;

  referential_constraint_definition_attr e1, e2;
  e1.referencing_columns = c1;
  e1.references_specification = r1;
  e2.referencing_columns = c2;
  e2.references_specification = r2;

  vector<expected_result> inputs;
  inputs.push_back(expected_result("CONSTRAINT cst_formal FOREIGN KEY (f1) REFERENCES t1 (rf1)", SUCCESS, true, e1));
  inputs.push_back(expected_result("FOREIGN KEY (f1) REFERENCES t1 (rf1)", SUCCESS, true, e1));
  inputs.push_back(expected_result("FOREIGN KEY (f1, f2) REFERENCES test.t1 (rf1,rf2)", SUCCESS, true, e2));
  inputs.push_back(expected_result("CONSTRAINT fk FOREIGN KEY (f1, f2) REFERENCES test.t1 (rf1,rf2)", SUCCESS, true, e2));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool referential_appears = tuples::get<2>(er);
    referential_constraint_definition_attr& constraint = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    table_constraint_attr fill;
    
    result_code result = parse_table_constraint(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (referential_appears)
    {
      referential_constraint_definition_attr* our_constraint = boost::get<referential_constraint_definition_attr>(&fill.definition);
      ASSERT_TRUE(our_constraint) << " for expression " << s;

      /* Check the referencing columns are the same */
      ASSERT_EQ(constraint.referencing_columns.size(), our_constraint->referencing_columns.size()) << " for expression " << s;
      size_t x = 0;
      for (column_name_list_attr::iterator iter = constraint.referencing_columns.begin();
           iter != constraint.referencing_columns.end();
           ++iter, ++x)
      {
        ASSERT_EQ(*iter, our_constraint->referencing_columns[x]) << " for expression " << s;
      }
      references_specification_attr& references_spec = constraint.references_specification;
      references_specification_attr& our_references_spec = our_constraint->references_specification;
      
      /* Check the table identifiers are the same */
      if (non_qualified_identifier_attr* table_ident = boost::get<non_qualified_identifier_attr>(&references_spec.referenced_table))
      {
        non_qualified_identifier_attr* our_table_ident = boost::get<non_qualified_identifier_attr>(&our_references_spec.referenced_table);

        ASSERT_TRUE(our_table_ident) << " for expression " << s;
        ASSERT_EQ(*table_ident, *our_table_ident) << " for expression " << s;
      }
      else
      {
        qualified_identifier_attr* table_ident = boost::get<qualified_identifier_attr>(&references_spec.referenced_table);
        qualified_identifier_attr* our_table_ident = boost::get<qualified_identifier_attr>(&our_references_spec.referenced_table);

        ASSERT_TRUE(table_ident) << " for expression " << s;
        ASSERT_TRUE(our_table_ident) << " for expression " << s;
        ASSERT_EQ(table_ident->first, our_table_ident->first) << " for expression " << s;
        ASSERT_EQ(table_ident->second, our_table_ident->second) << " for expression " << s;
      }

      /* Check the referenced columns are the same */
      x = 0;
      ASSERT_EQ(references_spec.referenced_columns.size(), our_references_spec.referenced_columns.size()) << " for expression " << s;
      for (column_name_list_attr::iterator iter = references_spec.referenced_columns.begin();
           iter != references_spec.referenced_columns.end();
           ++iter, ++x)
      {
        ASSERT_EQ(*iter, our_references_spec.referenced_columns[x]) << " for expression " << s;
      }
    }
  }
}

TEST(test_parse_table_constraints, check_constraints)
{
  typedef boost::tuple<string, // SQL string phrase to parse
                       result_code, // expected return result
                       bool, // Has a check constraint?
                       search_condition_attr // The search condition of the check constraint
                      > expected_result;

  search_condition_attr e1("f1 > 100");
  search_condition_attr e2("f1 > 100 AND f2 < 10000");

  vector<expected_result> inputs;
  inputs.push_back(expected_result("CONSTRAINT cst_formal CHECK (\"f1 > 100\")", SUCCESS, true, e1));
  inputs.push_back(expected_result("CHECK (\"f1 > 100 AND f2 < 10000\")", SUCCESS, true, e2));

  for (vector<expected_result>::iterator it = inputs.begin();
       it != inputs.end();
       ++it)
  {
    expected_result& er = *it;
    string& s = tuples::get<0>(er);
    result_code& r = tuples::get<1>(er);
    bool check_appears = tuples::get<2>(er);
    search_condition_attr& search = tuples::get<3>(er);

    string::iterator start = s.begin();
    string::iterator end = s.end();

    table_constraint_attr fill;
    
    result_code result = parse_table_constraint(start, end, fill);

    ASSERT_EQ(r, result) << " for expression " << s;

    if (check_appears)
    {
      check_constraint_definition_attr* our_constraint = boost::get<check_constraint_definition_attr>(&fill.definition);
      ASSERT_TRUE(our_constraint) << " for expression " << s;

      ASSERT_EQ(search, *our_constraint) << " for expression " << s;
    }
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
