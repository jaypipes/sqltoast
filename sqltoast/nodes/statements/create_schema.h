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
 * Declares the class for all CREATE SCHEMA SQL statement nodes
 */

#ifndef SQLTOAST_NODES_STATEMENTS_CREATE_SCHEMA_H
#define SQLTOAST_NODES_STATEMENTS_CREATE_SCHEMA_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast.h"
#include "sqltoast/nodes/statement.h"
#include "sqltoast/nodes/identifier.h"

#include <boost/optional.hpp>

#include <string>

namespace sqltoast
{
namespace nodes
{
namespace statements
{
static std::string const CREATE_SCHEMA = "CREATE SCHEMA";
/**
 * Node representing a CREATE SCHEMA statement
 */
class SQLTOAST_API create_schema : public statement
{
public:
  /**
   * Simple constructor
   */
  create_schema() :
    statement(DDL),
    _schema_identifier(),
    _if_not_exists(false),
    _charset_name(),
    _collation_name()
  {}
  /**
   * Constructor taking a schema identifier as an argument
   * along with boolean values for the IF EXISTS 
   * phrase existence along with charset and collation
   * specifiers
   */
  create_schema(boost::shared_ptr<identifier> schema_identifier,
                bool if_not_exists,
                boost::optional<std::string> const& charset_name,
                boost::optional<std::string> const& collation_name) :
    statement(DDL),
    _schema_identifier(schema_identifier),
    _if_not_exists(if_not_exists),
    _charset_name(charset_name),
    _collation_name(collation_name)
  {
    schema_identifier->set_parent(this);
    add_child(schema_identifier.get());
  }
  virtual ~create_schema()
  {}
  /**
   * Returns a string describing the type of SQL statement
   * this node represents
   */
  virtual std::string const& get_statement_type(void) const;
  /**
   * Returns true if the IF NOT EXISTS clause was present
   */
  bool has_if_not_exists(void) const;
  /**
   * Returns the schema name to create
   */
  std::string const& get_schema_name(void) const;
  /**
   * Returns whether the charset name was specified
   */
  bool has_charset_name(void) const;
  /**
   * Returns whether the collation name was specified
   */
  bool has_collation_name(void) const;
  /**
   * Returns the charset name specified as an optional string
   */
  std::string const& get_charset_name(void) const;
  /**
   * Returns the collation name specified as an optional string
   */
  std::string const& get_collation_name(void) const;
private:
  boost::shared_ptr<identifier> _schema_identifier; ///< Identifier of the schema to create
  bool _if_not_exists; ///< Was the IF NOT EXISTS clause present?
  boost::optional<std::string> _charset_name; ///< Optional charset specified
  boost::optional<std::string> _collation_name; ///< Optional collation specified
};

inline std::string const& create_schema::get_statement_type(void) const
{
  return CREATE_SCHEMA;
}

inline bool create_schema::has_if_not_exists(void) const
{
  return _if_not_exists;
}

inline std::string const& create_schema::get_schema_name(void) const
{
  return _schema_identifier->get_name();
}

inline bool create_schema::has_charset_name(void) const
{
  return _charset_name;
}

inline bool create_schema::has_collation_name(void) const
{
  return _collation_name;
}

inline std::string const& create_schema::get_charset_name(void) const
{
  return *_charset_name;
}

inline std::string const& create_schema::get_collation_name(void) const
{
  return *_collation_name;
}

} // namespace statements
} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_STATEMENTS_CREATE_SCHEMA_H */
