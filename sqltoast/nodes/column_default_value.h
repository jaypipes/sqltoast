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

#ifndef SQLTOAST_NODES_COLUMN_DEFAULT_VALUE_H
#define SQLTOAST_NODES_COLUMN_DEFAULT_VALUE_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast_node.h"
#include "sqltoast/nodes/literal.h"
#include "sqltoast/nodes/function.h"

#include <boost/shared_ptr.hpp>
#include <boost/fusion/container/vector.hpp>

#include <string>

namespace sqltoast
{
namespace nodes
{
/**
 * A class representing a column definition's default value
 */
class SQLTOAST_API column_default_value : public ast_node
{
public:
  enum default_type
  {
    DEFAULT_TYPE_LITERAL,
    DEFAULT_TYPE_FUNCTION,
    DEFAULT_TYPE_NULL
  };
  /**
   * Constructor
   */
  column_default_value(default_type type) :
    ast_node(),
    _type(type)
  {}
  virtual ~column_default_value()
  {}
  /**
   * Returns the default value's type
   */
  default_type get_type(void) const;
private:
  default_type _type; ///< The type of default value we have
  boost::fusion::vector<
      boost::shared_ptr<literal>, // ast_node for literal values
      boost::shared_ptr<function>
    > _value; ///< The default value of the column
};

inline column_default_value::default_type column_default_value::get_type(void) const
{
  return _type;
}

} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_COLUMN_DEFAULT_VALUE_H */
