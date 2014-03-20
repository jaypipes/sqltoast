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
 * @file Base class for statement nodes, which are
 * the root nodes on generated ASTs
 */

#ifndef SQLTOAST_NODES_STATEMENT_H
#define SQLTOAST_NODES_STATEMENT_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast_node.h"

#include <string>

namespace sqltoast
{
namespace nodes
{
/**
 * The base class for the root nodes on an AST.  Since all SQL
 * is distinct SQL statements, all generated ASTs will have a
 * SQL statement node at their root.
 */
class SQLTOAST_API statement : public ast_node
{
public:
  statement(sql_statement_class statement_class) :
    ast_node(),
    _statement_class(statement_class)
  {}
  virtual ~statement()
  {}
  /**
   * Returns a string describing the type of SQL statement
   * this node represents
   */
  virtual std::string const& get_statement_type(void) const = 0;
  /**
   * Returns the class of the SQL statement
   * this node represents
   */
  sql_statement_class get_statement_class(void) const;
private:
  sql_statement_class _statement_class; ///< The class of SQL statement represented
};

inline sql_statement_class statement::get_statement_class(void) const
{
  return _statement_class;
}

} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_STATEMENT_H */
