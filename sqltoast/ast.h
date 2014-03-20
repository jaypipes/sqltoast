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
 * @file Main abstract syntax tree class declaration
 */

#ifndef SQLTOAST_AST_H
#define SQLTOAST_AST_H

#include "sqltoast/visibility.h"

#include "sqltoast/nodes/statement.h"

#include <list>
#include <cinttypes>

#include <boost/shared_ptr.hpp>

namespace sqltoast
{

/**
 * A class representing an abstract syntax tree
 * representation of an SQL statement
 */
class SQLTOAST_API ast
{
public:
  /** Constructor takes the root node as its argument */
  ast(boost::shared_ptr<nodes::statement>& root) :
    _root(root)
  {}
  /** 
   * Constructor taking no arguments.  This version of the
   * constructor is used when passing an AST to a tokenizer
   * or parser
   */
  ast() :
    _root()
  {}
  /** Simple destructor */
  ~ast()
  {}
  /**
   * Returns the root node, which is the statement
   * node for SQL ASTs
   */
  nodes::statement const& get_root(void) const;
  nodes::statement& get_root(void);
  /**
   * Returns true if the tree has a root node
   */
  bool has_root(void) const;
  /**
   * Sets the root node of the AST
   */
  void set_root(boost::shared_ptr<nodes::statement>& root);
private:
  /* Disallow these */
  ast(ast const& other);
  ast& operator=(ast const& other);
  /**
   * The AST's "root" node, which is always going to be
   * a subclass of ast_node of type statement
   */
  boost::shared_ptr<nodes::statement> _root;
};

inline nodes::statement const& ast::get_root(void) const
{
  return *_root;
}

inline nodes::statement& ast::get_root(void)
{
  return *_root;
}

inline bool ast::has_root(void) const
{
  return _root.get() != NULL;
}

inline void ast::set_root(boost::shared_ptr<nodes::statement>& root)
{
  _root = root;
}

} // namespace sqltoast

#endif /* SQLTOAST_AST_H */
