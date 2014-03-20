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

#ifndef SQLTOAST_NODES_IDENTIFIER_H
#define SQLTOAST_NODES_IDENTIFIER_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast_node.h"

#include <string>

namespace sqltoast
{
namespace nodes
{
/**
 * The base class for all nodes that identify an object in the
 * system when it is in a SQL expression.
 *
 * For instance, the SQL expression:
 *
 * SELECT field1 FROM table1;
 *
 * Contains two identifier nodes, one for field1 and one for table1.
 *
 * Identifiers may either be qualified or non-qualified.  Qualified
 * identifiers exhibit the forms:
 *
 * A.B 
 * 
 * where A is the qualifier for B
 * 
 * or A.B.C 
 *
 * where A.B is the qualifier for C.  This form is known as a fully-qualified
 * domain name (FQDN) and is what the is_fqdn() method below is about.
 *
 * This base class implements a simple *non-qualified identifier*, with the
 * form:
 * 
 * A
 *
 * where A is a simple name of an object in the SQL system
 *
 * @note
 *
 * All identifier nodes are immutable once constructed.  They contain
 * no accessors for mutable data and contain no setter methods.
 *
 * @note
 *
 * During parsing, unless an object name is qualified, the AST produced
 * during parsing will contain identifier nodes that contain the non-qualified
 * names of the identified objects in the SQL statement.  During a process
 * called *name resolution*, those identifier nodes will be replaced with
 * qualified_identifier nodes as missing qualifiers are resolved.
 *
 * @see /sqltoast/helpers/name_resolution.cc
 */
class SQLTOAST_API identifier : public ast_node
{
public:
  /**
   * Constructor taking the non-qualified name of the
   * identified object
   */
  identifier(std::string const& name) :
    ast_node(),
    _is_qualified(false),
    _is_fqdn(false),
    _name(name)
  {}
  /**
   * Constructor taking the non-qualified name of the
   * identified object and a boolean value indicating
   * whether the identifier has a qualifier.
   *
   * @note
   *
   * This constructor should really only be used in
   * derived class constructors, since derived classes
   * implement qualified identifiers
   */
  identifier(std::string const& name, bool in_is_qualified, bool in_is_fqdn) :
    ast_node(),
    _is_qualified(in_is_qualified),
    _is_fqdn(in_is_fqdn),
    _name(name)
  {}
  virtual ~identifier()
  {}
  /**
   * Returns the non-qualified name of the identified object
   * this node represents
   */
  std::string const& get_name(void) const;
  /**
   * Returns whether the identifier has a qualifier of any sort
   */
  bool is_qualified(void) const;
  /**
   * Returns whether the identifier is a fully-qualified domain name.
   * If this returns true, then the name identifies an object in the
   * SQL system without the need of any "current schema" or "current
   * domain"
   */
  bool is_fqdn(void) const;
  /**
   * Specialization.  Identifiers are always leaf nodes.
   */
  virtual bool is_leaf(void) const;
private:
  bool _is_qualified; ///< Is there a qualifier on the identifier?
  bool _is_fqdn; ///< Is this a fully-qualified domain name?
  std::string _name; ///< The non-qualified name of the identifier
};

/**
 * Derived identifier class for a qualified identifier
 */
class qualified_identifier : public identifier
{
public:
  /**
   * Constructor taking the qualifier and the non-qualified name of the
   * identified object
   */
  qualified_identifier(std::string const& qualifier, std::string const& name) :
    identifier(name, true, false),
    _qualifier(qualifier)
  {}
  /**
   * Returns the qualifer name of the identified object
   * this node represents
   */
  std::string const& get_qualifier(void) const;
private:
  std::string _qualifier; ///< The qualifier for the identifier
};

inline bool identifier::is_leaf(void) const
{
  return true;
}

inline std::string const& identifier::get_name(void) const
{
  return _name;
}

inline bool identifier::is_qualified(void) const
{
  return _is_qualified;
}

inline bool identifier::is_fqdn(void) const
{
  return _is_fqdn;
}

inline std::string const& qualified_identifier::get_qualifier(void) const
{
  return _qualifier;
}

} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_IDENTIFIER_H */
