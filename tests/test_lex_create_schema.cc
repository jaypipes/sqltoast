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
 * @file Tests for tokenizing CREATE SCHEMA SQL statements
 */

#include "config.h"

#include <sqltoast/ast.h>
#include <sqltoast/tokenize.h>
#include <sqltoast/nodes/value_node.h>
#include <gtest/gtest.h>

#include <string>
#include <exception>
#include <list>

using namespace sqltoast;
using namespace std;

TEST(test_lex_create_schema, create_schema_1)
{
  string s("CREATE SCHEMA test");
  list<nodes::value_node> nodes;
  result_code result = tokenize(s, nodes);
  ASSERT_EQ(SUCCESS, result);
  ASSERT_EQ(3, nodes.size());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

