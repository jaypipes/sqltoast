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

#ifndef SQLTOAST_NODES_COLUMN_DEFINITION_H
#define SQLTOAST_NODES_COLUMN_DEFINITION_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast_node.h"
#include "sqltoast/nodes/identifier.h"
#include "sqltoast/nodes/column_default_value.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace sqltoast
{
namespace nodes
{
/**
 * The base class for all nodes that define a column in a table.
 */
class SQLTOAST_API column_definition : public ast_node
{
public:
  /**
   * Constructor taking the non-qualified identifier of the
   * column, its column type.
   */
  column_definition(boost::shared_ptr<identifier>& identifier,
                    sql_column_type type) :
    ast_node(),
    _identifier(identifier),
    _type(type),
    _default_value()
  {
    identifier->set_parent(this);
    add_child(identifier.get());
  }
  /**
   * Constructor taking the non-qualified identifier of the
   * column, its column type, and a default value.
   */
  column_definition(boost::shared_ptr<identifier>& identifier,
                    sql_column_type type,
                    column_default_value& default_value) :
    ast_node(),
    _identifier(identifier),
    _type(type),
    _default_value(default_value)
  {
    identifier->set_parent(this);
    default_value.set_parent(this);
    add_child(identifier.get());
    add_child(&(*_default_value)); // grab address of contained optional
  }
  virtual ~column_definition()
  {}
  /**
   * Returns the non-qualified name of the identified object
   * this node represents
   */
  std::string const& get_name(void) const;
  /**
   * Returns the column type
   */
  sql_column_type get_type(void) const;
  /**
   * Returns the column's default value
   */
  column_default_value const& get_default_value(void) const;
  /**
   * Returns whether the column definition
   * has a default value
   */
  bool has_default_value(void) const;
private:
  boost::shared_ptr<identifier> _identifier; ///< The non-qualified identifier of the column
  sql_column_type _type;
  boost::optional<column_default_value> _default_value; ///< The default value of the column
};

inline std::string const& column_definition::get_name(void) const
{
  return _identifier->get_name();
}

inline sql_column_type column_definition::get_type(void) const
{
  return _type;
}

} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_COLUMN_DEFINITION_H */
