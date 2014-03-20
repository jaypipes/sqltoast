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
 * @file 
 *
 * Declares the class for all SELECT SQL statement nodes
 */

#ifndef SQLTOAST_NODES_STATEMENTS_SELECT_H
#define SQLTOAST_NODES_STATEMENTS_SELECT_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/nodes/statement.h"

#include <string>

namespace sqltoast
{
namespace nodes
{
namespace statements
{
static std::string const SELECT = "SELECT";
/**
 * Node representing an entire SELECT statement
 */
class SQLTOAST_API select : public statement
{
public:
  /**
   * Simple constructor
   */
  select() :
    statement(DML)
  {}
  virtual ~select()
  {}
  /**
   * Returns a string describing the type of SQL statement
   * this node represents
   */
  virtual std::string const& get_statement_type(void) const;
private:
};

std::string const& select::get_statement_type(void) const
{
  return SELECT;
}

} // namespace statements
} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_STATEMENTS_SELECT_H */
