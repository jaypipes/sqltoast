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
 * @file Tests for sqltoast's literal value nodes
 */

#include "config.h"

#include <sqltoast/ast.h>
#include <sqltoast/nodes/literal.h>
#include <gtest/gtest.h>

#include <string>
#include <exception>

using namespace sqltoast;
using namespace std;

TEST(test_literals, variadic_value)
{
  nodes::literal node1(1);
  nodes::literal node2(double(2.0));
  nodes::literal node3;

  EXPECT_EQ("1", node1.as_string());
  EXPECT_EQ("2.0000", node2.as_string());
  EXPECT_EQ("2.000000000", node2.as_string(10));
  EXPECT_TRUE(node3.is_null());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
