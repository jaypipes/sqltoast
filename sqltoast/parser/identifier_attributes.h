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
 * @file All definitions of attributes used in identifier rules
 */

#ifndef SQLTOAST_PARSER_IDENTIFIER_ATTRIBUTES_H
#define SQLTOAST_PARSER_IDENTIFIER_ATTRIBUTES_H

#include "sqltoast/visibility.h"

#include <boost/fusion/include/std_pair.hpp>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>

#include <string>
#include <utility>

namespace sqltoast
{
namespace parser
{

/* 
 * Definitions of identifier attributes produced from identifier
 * parsers
 */
typedef std::string non_qualified_identifier_attr;
typedef std::pair<std::string, non_qualified_identifier_attr> qualified_identifier_attr;
typedef boost::tuple<std::string, std::string, std::string> fqdn_attr; // fully-qualified domain name
typedef boost::variant<qualified_identifier_attr, non_qualified_identifier_attr> table_identifier_attr;

} // namespace parser
} // namespace sqltoast

#endif /* SQLTOAST_PARSER_IDENTIFIER_ATTRIBUTES_H */
