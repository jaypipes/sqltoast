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
 * @file Class representing a node in an abstract syntax tree
 *
 * @detail
 *
 * An abstract syntax tree is a representation of a parsed expression
 * that represents each part of the expression as a node on a tree.
 *
 * For example, let's take the SQL statement:
 * @code
 * SELECT field1 = 0 AS result FROM t1;
 * @endcode
 *
 * An abstract syntax tree that represented the above SQL statement
 * might look like this:
 *
 *                             (select statement)
 *                                 /      \
 *                                /        \
 *                        (table list)    (select list)
 *                              /            \
 *                             /              \
 *                          (table)         (bool expression)
 *
 * Each of the parenthesized phrases represents a node in the AST,
 * with the "root" node being the whole "statement"
 *
 * All SQL statements can be represented by a tree, and you can
 * find all parts of the SQL expression by walking the AST from the
 * root node through its children.
 */

#ifndef SQLTOAST_AST_NODE_H
#define SQLTOAST_AST_NODE_H

#include "sqltoast/visibility.h"

#include <vector>
#include <cinttypes>

namespace sqltoast
{
namespace nodes {class literal;}

/**
 * Generic base class for nodes in an abstract syntax
 * tree.
 */
class SQLTOAST_API ast_node
{
public:
  /** Base constructor */
  ast_node() :
    _parent(NULL),
    _children()
  {}
  /** Base destructor */
  virtual ~ast_node()
  {}
  /** Copy constructor */
  ast_node(ast_node const& other);
  /** Copy assignment operator */
  ast_node& operator=(ast_node const& other);
  /**
   * Returns a reference to this node's collection
   * of child nodes.
   */
  std::vector<ast_node*>const& get_children(void) const;
  std::vector<ast_node*>& get_children(void);
  /**
   * Returns a pointer to the node's parent, or NULL
   * if this is the root node
   */
  ast_node const* get_parent(void) const;
  ast_node* get_parent(void);
  /**
   * Sets the parent node for this node.  Returns
   * a reference to this node.
   */
  ast_node& set_parent(ast_node* parent);
  /**
   * Returns true if the node is a leaf node
   */
  virtual bool is_leaf(void) const;
  /**
   * Adds a child node to this node
   *
   * @param[in] Child to add
   */
  virtual void add_child(ast_node* child);
  /**
   * Convenience method for returning a child at
   * a specified index as a literal.
   *
   * @param[in] Index at which to grab child
   */
  nodes::literal& get_child_as_value(size_t index);
private:
  ast_node* _parent; ///< Node that is this node's parent, or NULL if none
  std::vector<ast_node*> _children; ///< Child nodes of this node
};

inline std::vector<ast_node*>& ast_node::get_children(void)
{
  return _children;
}

inline std::vector<ast_node*> const& ast_node::get_children(void) const
{
  return _children;
}

inline ast_node* ast_node::get_parent(void)
{
  return _parent;
}

inline ast_node const* ast_node::get_parent(void) const
{
  return _parent;
}

inline ast_node& ast_node::set_parent(ast_node* parent)
{
  _parent = parent;
  return *this;
}

inline bool ast_node::is_leaf(void) const
{
  return _children.empty();
}

inline void ast_node::add_child(ast_node* child)
{
  _children.push_back(child);
}

} // namespace sqltoast

#endif /* SQLTOAST_AST_NODE_H */
