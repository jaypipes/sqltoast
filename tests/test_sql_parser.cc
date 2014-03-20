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
 * @file Tests for parsing of various SQL statements
 */

#include "config.h"

//#define BOOST_SPIRIT_DEBUG

#include <sqltoast/parse.h>
#include <sqltoast/parser/key_value_list.h>
#include <sqltoast/parser/create_schema.h>
#include <sqltoast/parser/drop_schema.h>
#include <sqltoast/parser/create_table.h>

#include <sqltoast/ast.h>
#include <sqltoast/nodes/statements/create_schema.h>
#include <sqltoast/nodes/statements/drop_schema.h>
#include <sqltoast/nodes/statements/create_table.h>

#include <gtest/gtest.h>

#include <boost/variant.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_c.hpp>

#include <string>
#include <exception>

using namespace sqltoast;
using namespace std;

TEST(test_parse_key_value_list, key_values_1)
{
  string s("key1:value1,key2:value2");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::key_value_list fill;
  
  result_code result = parser::parse_key_value_list(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(2, fill.size());
}

TEST(test_parse_key_value_list, key_values_2)
{
  string s("key1:value1, key2:value2");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::key_value_list fill;
  
  result_code result = parser::parse_key_value_list(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(2, fill.size());
}

TEST(test_parse_key_value_list, key_values_3)
{
  string s("key1:value1,\nkey2:value2");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::key_value_list fill;
  
  result_code result = parser::parse_key_value_list(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(2, fill.size());
}

TEST(test_parse_key_value_list, key_values_4)
{
  /* Should only be one key-value pair...  */
  string s("key1:value1key2:value2");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::key_value_list fill;
  
  result_code result = parser::parse_key_value_list(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(1, fill.size());

  vector<pair<string, string> > expected_results;
  expected_results.push_back(make_pair("key1", "value1key2"));

  /* Check each expected value appears exactly once */
  for (vector<pair<string, string> >::iterator e_iter = expected_results.begin();
       e_iter != expected_results.end();
       ++e_iter)
  {
    pair<string, string> e_item = *e_iter;
    ASSERT_EQ(1, fill.count(e_item.first));
    ASSERT_EQ(e_item.second, fill[e_item.first]);
  }
}

TEST(test_parse_create_schema, create_schema_1)
{
  string s("CREATE SCHEMA test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_FALSE(fill.charset_specifier);
}

TEST(test_parse_create_schema, create_schema_2)
{
  string s("CREATE SCHEMA 1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_schema, create_schema_3)
{
  string s("CREATE SCHEMA \"test1\"");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_create_schema, create_schema_4)
{
  string s("CREATE SCHEMA IF NOT EXISTS test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(true, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_create_schema, create_schema_5)
{
  string s("CREATE SCHEMA IF NOT EXISTS \"test1\"");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(true, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_create_schema, create_schema_6)
{
  string s("CREATE SCHEMA IF EXISTS test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_schema, create_schema_7)
{
  string s("CREATE DATABASE test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_create_schema, create_schema_8)
{
  string s("CREATE DATABASE \"test1\"");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_create_schema, create_schema_9)
{
  string s("CREATE SCHEMA test1 DEFAULT CHARACTER SET utf8");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_EQ("utf8", *fill.charset_specifier);
  ASSERT_FALSE(fill.collation_specifier);
}

TEST(test_parse_create_schema, create_schema_10)
{
  string s("CREATE SCHEMA test1 CHARACTER SET utf8");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_EQ("utf8", *fill.charset_specifier);
}

TEST(test_parse_create_schema, create_schema_11)
{
  string s("CREATE SCHEMA test1 DEFAULT CHARACTER SET \"utf8\"");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_EQ("utf8", *fill.charset_specifier);
}

TEST(test_parse_create_schema, create_schema_12)
{
  /* A trailing semicolon is NOT parsed by the create_schema_parser */
  string s("CREATE SCHEMA test1;");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_schema, create_schema_13)
{
  string s("CREATE SCHEMA test1 COLLATE utf8bin");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_FALSE(fill.charset_specifier);
  ASSERT_EQ("utf8bin", *fill.collation_specifier);
}

TEST(test_parse_create_schema, create_schema_14)
{
  string s("CREATE SCHEMA test_foo");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test_foo", fill.schema_identifier);
}

TEST(test_parse_create_schema, create_schema_15)
{
  string s("CREATE SCHEMA test1 COLLATE utf8_bin");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_FALSE(fill.charset_specifier);
  ASSERT_EQ("utf8_bin", *fill.collation_specifier);
}

TEST(test_parse_create_schema, create_schema_16)
{
  string s("CREATE SCHEMA test1 CHARACTER SET utf8 COLLATE utf8_bin");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
  ASSERT_EQ("utf8", *fill.charset_specifier);
  ASSERT_EQ("utf8_bin", *fill.collation_specifier);
}

TEST(test_parse_create_schema, create_schema_17)
{
  /* Missing CHARACTER keyword. Should fail */
  string s("CREATE SCHEMA test1 SET utf8 COLLATE utf8_bin");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_schema, create_schema_18)
{
  string s("CREATE SCHEMA test1 CHARACTER SET utf8 COLLATE utf8_bin");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::create_schema &cs = static_cast<nodes::statements::create_schema&>(fill.get_root());

  ASSERT_EQ(false, cs.has_if_not_exists());
  ASSERT_EQ("test1", cs.get_schema_name());
  ASSERT_EQ("utf8", cs.get_charset_name());
  ASSERT_EQ("utf8_bin", cs.get_collation_name());
}

TEST(test_parse_create_schema, create_schema_19)
{
  string s("CREATE SCHEMA IF NOT EXISTS test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::create_schema &cs = static_cast<nodes::statements::create_schema&>(fill.get_root());

  ASSERT_EQ(true, cs.has_if_not_exists());
  ASSERT_EQ("test1", cs.get_schema_name());
  ASSERT_FALSE(cs.has_charset_name());
  ASSERT_FALSE(cs.has_collation_name());
}

TEST(test_parse_create_schema, create_schema_20)
{
  /* Backticks aren't allowed as enclosures in ANSI-SQL 92 */
  /** @todo Add ext/mysql parser */
  string s("CREATE SCHEMA `test1`");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_schema, create_schema_21)
{
  string s("CREATE SCHEMA \"test spaces\"");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_schema_attr fill;
  
  result_code result = parser::parse_create_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ("test spaces", fill.schema_identifier);
}

TEST(test_parse_drop_schema, drop_schema_1)
{
  string s("DROP SCHEMA test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::drop_schema_parser_attr fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_drop_schema, drop_schema_2)
{
  string s("DROP SCHEMA 1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::drop_schema_parser_attr fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_drop_schema, drop_schema_3)
{
  string s("DROP SCHEMA \"test1\"");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::drop_schema_parser_attr fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_drop_schema, drop_schema_4)
{
  string s("DROP SCHEMA IF EXISTS test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::drop_schema_parser_attr fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(true, fill.if_exists);
  ASSERT_EQ("test1", fill.schema_identifier);
}

TEST(test_parse_drop_schema, drop_schema_5)
{
  string s("DROP SCHEMA IF NOT EXISTS test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::drop_schema_parser_attr fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_drop_schema, drop_schema_6)
{
  string s("DROP SCHEMA IF EXISTS test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::drop_schema &ds = static_cast<nodes::statements::drop_schema&>(fill.get_root());

  ASSERT_EQ(true, ds.has_if_exists());
  ASSERT_EQ("test1", ds.get_schema_name());
}

TEST(test_parse_drop_schema, drop_schema_7)
{
  string s("DROP SCHEMA test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::drop_schema &ds = static_cast<nodes::statements::drop_schema&>(fill.get_root());

  ASSERT_EQ(false, ds.has_if_exists());
  ASSERT_EQ("test1", ds.get_schema_name());
}

TEST(test_parse_drop_schema, drop_schema_8)
{
  /* No qualified identifiers! Should fail. */
  string s("DROP SCHEMA test.test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::drop_schema_parser_attr fill;
  
  result_code result = parser::parse_drop_schema(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_1)
{
  string s("CREATE TABLE test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(false, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(1, fill.table_elements.size());
}

TEST(test_parse_create_table, create_table_2)
{
  string s("CREATE TABLE IF NOT EXISTS test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(true, fill.if_not_exists);
  ASSERT_EQ(false, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(1, fill.table_elements.size());
}

TEST(test_parse_create_table, create_table_3)
{
  string s("CREATE TEMPORARY TABLE IF NOT EXISTS test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(true, fill.if_not_exists);
  ASSERT_EQ(true, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(1, fill.table_elements.size());
}

TEST(test_parse_create_table, create_table_4)
{
  string s("CREATE TEMPORARY TABLE test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(true, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(1, fill.table_elements.size());
}

TEST(test_parse_create_table, create_table_5)
{
  string s("CREATE TABLE test.test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(false, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::qualified_identifier_attr* val = boost::get<parser::qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test", val->first);
  ASSERT_EQ("test1", val->second);
  ASSERT_EQ(1, fill.table_elements.size());
}

TEST(test_parse_create_table, create_table_6)
{
  /* Missing TABLE keyword. Should fail. */
  string s("CREATE test.test1");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_7)
{
  /* Identifier has spaces with no encapsulation. Should fail. */
  string s("CREATE TABLE test.test foo");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_8)
{
  /* Missing at least one table element. Should fail. */
  string s("CREATE TABLE test.test1 ()");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_9)
{
  string s("CREATE TABLE test1 (field1 int, field2 double)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(false, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(2, fill.table_elements.size());

  ASSERT_EQ("field1", get_column_definition(fill.table_elements, 0).name);
  ASSERT_EQ("field2", get_column_definition(fill.table_elements, 1).name);
}

TEST(test_parse_create_table, create_table_10)
{
  /* Test no column types. Should fail */
  string s("CREATE TABLE test1(field1,field2)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_11)
{
  /* Extraneous comma at end of table element list. Should fail */
  string s("CREATE TABLE test1 (field1, field2,)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_12)
{
  /* Unknown column type. Should fail */
  string s("CREATE TABLE test1 (field1 DUBBLE)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_13)
{
  string s("CREATE TABLE test1 (field1 int, field2 double)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(false, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(2, fill.table_elements.size());
  ASSERT_EQ("field1", get_column_definition(fill.table_elements, 0).name);
  ASSERT_EQ("field2", get_column_definition(fill.table_elements, 1).name);
  ASSERT_EQ(COLUMN_TYPE_INTEGER, get_column_definition(fill.table_elements, 0).type);
  ASSERT_EQ(COLUMN_TYPE_DOUBLE, get_column_definition(fill.table_elements, 1).type);
}

TEST(test_parse_create_table, create_table_14)
{
  string s("CREATE TABLE IF NOT EXISTS test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::create_table &ct = static_cast<nodes::statements::create_table&>(fill.get_root());

  ASSERT_EQ(false, ct.has_temporary());
  ASSERT_EQ(true, ct.has_if_not_exists());
  ASSERT_EQ("test1", ct.get_table_name());
  ASSERT_EQ("", ct.get_schema_name());
}

TEST(test_parse_create_table, create_table_15)
{
  string s("CREATE TEMPORARY TABLE IF NOT EXISTS test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::create_table &ct = static_cast<nodes::statements::create_table&>(fill.get_root());

  ASSERT_EQ(true, ct.has_temporary());
  ASSERT_EQ(true, ct.has_if_not_exists());
  ASSERT_EQ("test1", ct.get_table_name());
  ASSERT_EQ("", ct.get_schema_name());
}

TEST(test_parse_create_table, create_table_16)
{
  string s("CREATE TABLE test1 (field1 INT)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  ast fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);

  nodes::statements::create_table &ct = static_cast<nodes::statements::create_table&>(fill.get_root());

  ASSERT_EQ(false, ct.has_temporary());
  ASSERT_EQ(false, ct.has_if_not_exists());
  ASSERT_EQ("test1", ct.get_table_name());
  ASSERT_EQ("", ct.get_schema_name());
}

TEST(test_parse_create_table, create_table_18)
{
  /* Column identifiers must be non-qualified always. Should fail */
  string s("CREATE TABLE test1 (test1.field1 DOUBLE)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SYNTAX_ERROR, result);
}

TEST(test_parse_create_table, create_table_19)
{
  string s("CREATE TEMPORARY TABLE test1 (field1 INT NOT NULL)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(true, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(1, fill.table_elements.size());

  ASSERT_EQ("field1", get_column_definition(fill.table_elements, 0).name);
  ASSERT_EQ(COLUMN_TYPE_INTEGER, get_column_definition(fill.table_elements, 0).type);

  boost::optional<parser::optional_column_definition_attr> optionals_wrapper = get_column_definition(fill.table_elements, 0).optionals;
  ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
  parser::optional_column_definition_attr optionals = *optionals_wrapper;
  ASSERT_TRUE(boost::fusion::has_key<boost::optional<parser::is_nullable_attr> >(optionals)) << " for expression " << s;
  ASSERT_EQ(false, *boost::fusion::at_c<0>(optionals)) << " for expression " << s;
}

TEST(test_parse_create_table, create_table_20)
{
  string s("CREATE TABLE test1 (field1 INT NULL)");
  string::iterator start = s.begin();
  string::iterator end = s.end();

  parser::create_table_attr fill;
  
  result_code result = parser::parse_create_table(start, end, fill);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(false, fill.if_not_exists);
  ASSERT_EQ(false, fill.temporary);

  parser::table_identifier_attr table_ident_attr = fill.table_identifier;
  parser::non_qualified_identifier_attr* val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);

  ASSERT_FALSE(val == NULL);
  ASSERT_EQ("test1", *val);
  ASSERT_EQ(1, fill.table_elements.size());

  ASSERT_EQ("field1", get_column_definition(fill.table_elements, 0).name);
  ASSERT_EQ(COLUMN_TYPE_INTEGER, get_column_definition(fill.table_elements, 0).type);

  boost::optional<parser::optional_column_definition_attr> optionals_wrapper = get_column_definition(fill.table_elements, 0).optionals;
  ASSERT_TRUE(optionals_wrapper) << " for expression " << s;
  parser::optional_column_definition_attr optionals = *optionals_wrapper;
  ASSERT_TRUE(boost::fusion::has_key<boost::optional<parser::is_nullable_attr> >(optionals)) << " for expression " << s;
  ASSERT_EQ(true, *boost::fusion::at_c<0>(optionals)) << " for expression " << s;
}

TEST(test_parse_create_table, create_table_21)
{
  /* Test exact numeric column definitions */
  string s1("CREATE TABLE test1 (field1 INT)");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::create_table_attr fill1;
  
  result_code result1 = parser::parse_create_table(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);
  ASSERT_EQ(1, fill1.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_INTEGER, get_column_definition(fill1.table_elements, 0).type);

  string s2("CREATE TABLE test1 (field1 INTEGER)");

  string::iterator start2 = s2.begin();
  string::iterator end2 = s2.end();

  parser::create_table_attr fill2;
  
  result_code result2 = parser::parse_create_table(start2, end2, fill2);
  ASSERT_EQ(SUCCESS, result2);
  ASSERT_EQ(1, fill2.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_INTEGER, get_column_definition(fill2.table_elements, 0).type);

  string s3("CREATE TABLE test1 (field1 BIGINT)");

  string::iterator start3 = s3.begin();
  string::iterator end3 = s3.end();

  parser::create_table_attr fill3;
  
  result_code result3 = parser::parse_create_table(start3, end3, fill3);
  ASSERT_EQ(SUCCESS, result3);
  ASSERT_EQ(1, fill3.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_BIG_INTEGER, get_column_definition(fill3.table_elements, 0).type);

  string s4("CREATE TABLE test1 (field1 SMALLINT)");

  string::iterator start4 = s4.begin();
  string::iterator end4 = s4.end();

  parser::create_table_attr fill4;
  
  result_code result4 = parser::parse_create_table(start4, end4, fill4);
  ASSERT_EQ(SUCCESS, result4);
  ASSERT_EQ(1, fill4.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_SMALL_INTEGER, get_column_definition(fill4.table_elements, 0).type);
}

TEST(test_parse_create_table, create_table_22)
{
  /* Test approximate numeric column definitions */
  string s1("CREATE TABLE test1 (field1 DOUBLE)");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::create_table_attr fill1;
  
  result_code result1 = parser::parse_create_table(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);
  ASSERT_EQ(1, fill1.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_DOUBLE, get_column_definition(fill1.table_elements, 0).type);

  string s2("CREATE TABLE test1 (field1 DOUBLE PRECISION)");

  string::iterator start2 = s2.begin();
  string::iterator end2 = s2.end();

  parser::create_table_attr fill2;
  
  result_code result2 = parser::parse_create_table(start2, end2, fill2);
  ASSERT_EQ(SUCCESS, result2);
  ASSERT_EQ(1, fill2.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_DOUBLE, get_column_definition(fill2.table_elements, 0).type);
}

TEST(test_parse_create_table, create_table_23)
{
  /* Test fixed character column definitions */
  string s1("CREATE TABLE test1 (field1 CHAR)");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::create_table_attr fill1;
  
  result_code result1 = parser::parse_create_table(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);
  ASSERT_EQ(1, fill1.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_CHAR, get_column_definition(fill1.table_elements, 0).type);

  string s2("CREATE TABLE test1 (field1 CHARACTER)");

  string::iterator start2 = s2.begin();
  string::iterator end2 = s2.end();

  parser::create_table_attr fill2;
  
  result_code result2 = parser::parse_create_table(start2, end2, fill2);
  ASSERT_EQ(SUCCESS, result2);
  ASSERT_EQ(1, fill2.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_CHAR, get_column_definition(fill2.table_elements, 0).type);
}

TEST(test_parse_create_table, create_table_24)
{
  /* Test varying character column definitions */
  string s1("CREATE TABLE test1 (field1 VARCHAR)");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::create_table_attr fill1;
  
  result_code result1 = parser::parse_create_table(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);
  ASSERT_EQ(1, fill1.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_VARCHAR, get_column_definition(fill1.table_elements, 0).type);

  string s2("CREATE TABLE test1 (field1 CHARACTER VARYING)");

  string::iterator start2 = s2.begin();
  string::iterator end2 = s2.end();

  parser::create_table_attr fill2;
  
  result_code result2 = parser::parse_create_table(start2, end2, fill2);
  ASSERT_EQ(SUCCESS, result2);
  ASSERT_EQ(1, fill2.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_VARCHAR, get_column_definition(fill2.table_elements, 0).type);

  string s3("CREATE TABLE test1 (field1 CHAR VARYING)");

  string::iterator start3 = s3.begin();
  string::iterator end3 = s3.end();

  parser::create_table_attr fill3;
  
  result_code result3 = parser::parse_create_table(start3, end3, fill3);
  ASSERT_EQ(SUCCESS, result3);
  ASSERT_EQ(1, fill3.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_VARCHAR, get_column_definition(fill3.table_elements, 0).type);
}

TEST(test_parse_create_table, create_table_25)
{
  /* Test temporal column definitions */
  string s1("CREATE TABLE test1 (field1 DATE)");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::create_table_attr fill1;
  
  result_code result1 = parser::parse_create_table(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);
  ASSERT_EQ(1, fill1.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_DATE, get_column_definition(fill1.table_elements, 0).type);

  string s2("CREATE TABLE test1 (field1 TIME)");

  string::iterator start2 = s2.begin();
  string::iterator end2 = s2.end();

  parser::create_table_attr fill2;
  
  result_code result2 = parser::parse_create_table(start2, end2, fill2);
  ASSERT_EQ(SUCCESS, result2);
  ASSERT_EQ(1, fill2.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_TIME, get_column_definition(fill2.table_elements, 0).type);

  string s3("CREATE TABLE test1 (field1 TIMESTAMP)");

  string::iterator start3 = s3.begin();
  string::iterator end3 = s3.end();

  parser::create_table_attr fill3;
  
  result_code result3 = parser::parse_create_table(start3, end3, fill3);
  ASSERT_EQ(SUCCESS, result3);
  ASSERT_EQ(1, fill3.table_elements.size());

  ASSERT_EQ(COLUMN_TYPE_TIMESTAMP, get_column_definition(fill3.table_elements, 0).type);
}

TEST(test_parse_statement, statement_1)
{
  string s1("CREATE TABLE test1 (field1 DOUBLE)");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::sql_statement_attr fill1;
  
  result_code result1 = parser::parse_sql_statement(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);

  parser::create_table_attr* val = boost::get<parser::create_table_attr>(&fill1);

  ASSERT_FALSE(val == NULL);

  parser::create_table_attr& attr = *val;

  parser::table_identifier_attr table_ident_attr = attr.table_identifier;
  parser::non_qualified_identifier_attr* ident_val = boost::get<parser::non_qualified_identifier_attr>(&table_ident_attr);
  ASSERT_FALSE(ident_val == NULL);
  ASSERT_EQ("test1", *ident_val);
}

TEST(test_parse_statement, statement_2)
{
  string s1("CREATE SCHEMA test");

  string::iterator start1 = s1.begin();
  string::iterator end1 = s1.end();

  parser::sql_statement_attr fill1;
  
  result_code result1 = parser::parse_sql_statement(start1, end1, fill1);
  ASSERT_EQ(SUCCESS, result1);

  parser::create_schema_attr* val = boost::get<parser::create_schema_attr>(&fill1);

  ASSERT_FALSE(val == NULL);

  parser::create_schema_attr& attr = *val;

  parser::non_qualified_identifier_attr schema_ident_attr = attr.schema_identifier;
  ASSERT_EQ("test", schema_ident_attr);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
