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
 * Base class for nodes that represent an SQL function that
 * may produce a value
 */

#ifndef SQLTOAST_NODES_FUNCTION_H
#define SQLTOAST_NODES_FUNCTION_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast_node.h"

#include <sstream>
#include <iomanip>
#include <string>

namespace sqltoast
{
namespace nodes
{
/**
 * The base class for the nodes representing an SQL function
 *
 * @detail
 *
 * When a SQL statement such as:
 * @code
 * SELECT LENGTH("help"), CAST(1 AS DOUBLE);
 * @endcode
 *
 * is parsed, two function node will be in the AST:
 *
 *  - A node representing the function producing the length
 *    of the "help" string literal node
 *  - A node representing the function casting the
 *    integer literal node of 1 to a double literal node
 */
class SQLTOAST_API function : public ast_node
{
public:
  /**
   * General constructor accepting a name of the function
   */
  function(std::string const& name) :
    ast_node(),
    _name(name)
  {}
  virtual ~function()
  {}
  /**
   * Returns the name of the function
   */
  std::string const& get_name(void) const;
private:
  std::string _name; ///< The name of the function
};

inline std::string const& function::get_name(void) const
{
  return _name;
}

} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_FUNCTION_H */
