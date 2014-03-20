/*
 * sqltoast
 *
 * Copyright (c) 2010 Jay Pipes <jaypipes@gmail.com>
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See the
 * COPYING file in the root project directory for full text.
 */

#include "config.h"

#include <iostream>

#include <sqltoast/ast.h>
#include <sqltoast/nodes/statements/select.h>

#include <boost/shared_ptr.hpp>

using namespace sqltoast;
using namespace boost;

int main(int, char**)
{
  shared_ptr<nodes::statement> sel_node(new nodes::statements::select);
  ast const ast(sel_node);
  std::cout << ast.get_root().get_children().size() << std::endl;
  return 0;
}
