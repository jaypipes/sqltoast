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

#include "sqltoast/ast_node.h"
#include "sqltoast/nodes/literal.h"

#include <string>
#include <algorithm>

using namespace std;

namespace sqltoast
{

ast_node::ast_node(ast_node const& other) :
  _parent(other._parent),
  _children()
{
  _children.resize(other._children.size());
  copy(other._children.begin(), other._children.end(), _children.begin());
}

ast_node& ast_node::operator=(ast_node const& other)
{
  if (&other == this)
    return *this;
  _parent = other._parent;
  _children.resize(other._children.size());
  copy(other._children.begin(), other._children.end(), _children.begin());
  return *this;
}

nodes::literal& ast_node::get_child_as_value(size_t index)
{
  return *(static_cast<nodes::literal*>(_children[index]));
}

} // namespace sqltoast
