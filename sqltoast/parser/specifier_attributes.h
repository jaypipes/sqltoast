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
 * @file Parser attributes for SQL specifiers
 */

#ifndef SQLTOAST_PARSER_SPECIFIER_ATTRIBUTES_H
#define SQLTOAST_PARSER_SPECIFIER_ATTRIBUTES_H

#include "sqltoast/visibility.h"

#include <string>

namespace sqltoast
{
namespace parser
{
/* 
 * Specifiers are generally optional phrases in a SQL statement and
 * produce a string of the thing they are specifying
 */
typedef std::string charset_specifier_attr;
typedef std::string collation_specifier_attr;
} // namespace parser
} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SPECIFIER_ATTRIBUTES_H */
