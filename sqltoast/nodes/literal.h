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
 * Base class for value nodes, which simply store
 * a value that can be represented in different ways
 */

#ifndef SQLTOAST_NODES_LITERAL_H
#define SQLTOAST_NODES_LITERAL_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include "sqltoast/ast_node.h"

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include <sstream>
#include <iomanip>
#include <string>

namespace sqltoast
{
namespace nodes
{
/**
 * The base class for the literal value nodes in the AST
 *
 * @detail
 *
 * Value nodes represent terminal nodes (leaf nodes)
 * on an AST and are a simple, producer of a constant
 * value.
 *
 * When a SQL statement such as:
 * @code
 * SELECT "hello", field1 + 100 FROM t1 WHERE id > 13;
 * @endcode
 *
 * is parsed, three value nodes will be in the AST:
 *
 *  - A node representing the "hello" string value
 *  - A node representing the 100 integer value
 *  - A node representing the 13 integer value
 */
class SQLTOAST_API literal : public ast_node
{
public:
  typedef literal_value_type value_type;
  /**
   * General constructor accepting a variant with
   * any supported value type
   */
  literal(value_type const& anything) :
    ast_node(),
    _value(anything),
    _is_null(false)
  {}
  /**
   * Constructor for a NULL node
   */
  explicit literal() :
    ast_node(),
    _value(),
    _is_null(true)
  {}
  /**
   * Copy constructor
   */
  literal(literal const& other) :
    ast_node(),
    _value(other._value),
    _is_null(other._is_null)
  {}
  /**
   * Copy assignment
   */
  literal& operator=(literal const& other)
  {
    if (&other == this)
      return *this;
    _value = other._value;
  }
  virtual ~literal()
  {}
  /**
   * Returns a string representation of the value.
   *
   * Throws bad_lexical_cast if unable to convert
   * the value to a string.
   *
   * @param[in] Optional precision for double-to-string conversions (defaults to 5)
   */
  virtual std::string const as_string(std::streamsize precision = 5) const;
  /**
   * Returns true if this node has a value of NULL
   */
  bool is_null(void) const;
  /**
   * Specialization for value nodes, since they are always
   * leaf nodes
   */
  bool is_leaf(void) const;
  /**
   * If stored value if of type T, returns that value. Otherwise,
   * throws boost::bad_any_cast exception.
   *
   * @note
   *
   * This works because boost::variant<> is designed to be
   * compatible with boost::any_cast<> and boost::lexical_cast<>
   *
   * @todo
   *
   * Rewrite using spirit directives and parsers
   */
  template<class T>
  T const as(void) const
  {
    return boost::lexical_cast<T const>(_value);
  }
  /** @overload */
  template<class T>
  T as(void)
  {
    return boost::lexical_cast<T>(_value);
  }
private:
  value_type _value; ///< The variadic value of the node
  bool _is_null; ///< Is the value a NULL?
};

/**
 * Specialized value node representing a NULL value
 */
class SQLTOAST_API null_literal : public literal
{
public:
  /** Only constructor for a null node */
  explicit null_literal() :
    literal()
  {}
  /**
   * Returns a string representation of NULL
   */
  std::string const as_string(std::streamsize precision = 5) const;
};

inline std::string const literal::as_string(std::streamsize precision) const
{
  if (typeid(double) == _value.type())
  {
    /* Ensure that 2.0 comes out as "2.00000" and not "2" */
    std::stringstream ss;
    ss.precision(precision);
    ss << std::setiosflags(std::ios_base::showpoint) << _value;
    return ss.str();
  }
    
  return boost::lexical_cast<std::string>(_value);
}

inline bool literal::is_null(void) const
{
  return _is_null;
}

inline std::string const null_literal::as_string(std::streamsize unused) const
{
  (void) unused;
    
  return "NULL";
}

inline bool literal::is_leaf(void) const
{
  return true;
}

} // namespace nodes
} // namespace sqltoast

#endif /* SQLTOAST_NODES_LITERAL_H */
