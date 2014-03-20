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
 * @file Tests for manually constructing an AST
 */

#include "config.h"

#include <sqltoast/ast.h>
#include <sqltoast/ast_node.h>
#include <sqltoast/nodes/literal.h>
#include <sqltoast/nodes/statements/select.h>
#include <gtest/gtest.h>

#include <string>
#include <exception>

#include <boost/shared_ptr.hpp>

using namespace sqltoast;
using namespace std;
using namespace boost;

TEST(test_manually_construct_ast, simple_select_construction)
{
  using boost::shared_ptr;
  shared_ptr<nodes::statement> sel_node(new nodes::statements::select);
  nodes::literal v_node(1000);
  sel_node->add_child(&v_node);
  ast tree(sel_node);

  uint32_t const x_1000 = 1000;

  EXPECT_EQ("SELECT", tree.get_root().get_statement_type());
  EXPECT_EQ(DML, tree.get_root().get_statement_class());
  EXPECT_EQ(1, tree.get_root().get_children().size());
  EXPECT_EQ(x_1000, tree.get_root().get_child_as_value(0).as<uint32_t>());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
