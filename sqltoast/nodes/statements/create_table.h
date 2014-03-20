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
 * Declares the class for all CREATE TABLE SQL statement nodes
 */

#ifndef SQLTOAST_NODES_STATEMENTS_CREATE_TABLE_H
#define SQLTOAST_NODES_STATEMENTS_CREATE_TABLE_H

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
static std::string const CREATE_TABLE = "CREATE TABLE";
/**
 * Node representing a CREATE TABLE statement
 */
class SQLTOAST_API create_table : public statement
{
public:
  /**
   * Constructor taking a table identifier as an argument
   * along with boolean values for the IF NOT EXISTS and TEMPORARY
   * phrase existences
   */
  create_table(boost::shared_ptr<identifier> table_identifier,
               bool temporary,
               bool if_not_exists);
  virtual ~create_table()
  {}
  /**
   * Returns a string describing the type of SQL statement
   * this node represents
   */
  std::string const& get_statement_type(void) const;
  /**
   * Returns true if the IF NOT EXISTS clause was present
   */
  bool has_if_not_exists(void) const;
  /**
   * Returns true if the TEMPORARY keyword was present
   */
  bool has_temporary(void) const;
  /**
   * Returns the table name to create
   */
  std::string const& get_table_name(void) const;
  /**
   * Returns the schema name this table is in
   */
  std::string const& get_schema_name(void) const;
private:
  boost::shared_ptr<identifier> _table_identifier; ///< Identifier of the table to create
  bool _temporary; ///< Was the TEMPORARY keyword present?
  bool _if_not_exists; ///< Was the IF NOT EXISTS clause present?
};

} // namespace statements
} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_STATEMENTS_CREATE_TABLE_H */
