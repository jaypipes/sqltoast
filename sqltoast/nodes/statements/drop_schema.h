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
 * Declares the class for all DROP SCHEMA SQL statement nodes
 */

#ifndef SQLTOAST_NODES_STATEMENTS_DROP_SCHEMA_H
#define SQLTOAST_NODES_STATEMENTS_DROP_SCHEMA_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast.h"
#include "sqltoast/nodes/statement.h"
#include "sqltoast/nodes/identifier.h"

#include <string>

namespace sqltoast
{
namespace nodes
{
namespace statements
{
static std::string const DROP_SCHEMA = "DROP SCHEMA";
/**
 * Node representing a DROP SCHEMA statement
 */
class SQLTOAST_API drop_schema : public statement
{
public:
  /**
   * Simple constructor
   */
  drop_schema() :
    statement(DDL),
    _schema_identifier(),
    _if_exists(false)
  {}
  /**
   * Constructor taking a schema identifier as an argument
   * along with boolean values for the IF EXISTS 
   * phrase existence along with charset and collation
   * specifiers
   */
  drop_schema(boost::shared_ptr<identifier> schema_identifier,
              bool if_exists) :
    statement(DDL),
    _schema_identifier(schema_identifier),
    _if_exists(if_exists)
  {
    schema_identifier->set_parent(this);
    add_child(schema_identifier.get());
  }
  virtual ~drop_schema()
  {}
  /**
   * Returns a string describing the type of SQL statement
   * this node represents
   */
  virtual std::string const& get_statement_type(void) const;
  /**
   * Returns true if the IF NOT EXISTS clause was present
   */
  bool has_if_exists(void) const;
  /**
   * Returns the schema name to create
   */
  std::string const& get_schema_name(void) const;
private:
  boost::shared_ptr<identifier> _schema_identifier; ///< Identifier of the schema to drop
  bool _if_exists; ///< Was the IF EXISTS clause present?
};

inline std::string const& drop_schema::get_statement_type(void) const
{
  return DROP_SCHEMA;
}

inline bool drop_schema::has_if_exists(void) const
{
  return _if_exists;
}

inline std::string const& drop_schema::get_schema_name(void) const
{
  return _schema_identifier->get_name();
}

} // namespace statements
} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_STATEMENTS_DROP_SCHEMA_H */
