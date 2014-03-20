/*
 * sqltoast
 *
 * Copyright (c) 2010 Jay Pipes <jaypipes@gmail.com>
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license. See the
 * COPYING file in the root project directory for full text.
 */

#include "config.h"

#include "sqltoast/nodes/statements/create_table.h"
#include "sqltoast/nodes/identifier.h"

#include <string>
#include <vector>

using namespace std;
using namespace boost;

namespace sqltoast
{
namespace nodes
{
namespace statements
{
create_table::create_table(boost::shared_ptr<identifier> table_identifier,
                           bool temporary,
                           bool if_not_exists) :
  statement(DDL),
  _table_identifier(table_identifier),
  _temporary(temporary),
  _if_not_exists(if_not_exists)
{
  table_identifier->set_parent(this);
  add_child(table_identifier.get());
}

std::string const& create_table::get_statement_type(void) const
{
  return CREATE_TABLE;
}

bool create_table::has_if_not_exists(void) const
{
  return _if_not_exists;
}

bool create_table::has_temporary(void) const
{
  return _temporary;
}

std::string const& create_table::get_table_name(void) const
{
  return _table_identifier->get_name();
}

std::string const& create_table::get_schema_name(void) const
{
  if (_table_identifier->is_qualified())
    return static_cast<qualified_identifier*>(_table_identifier.get())->get_qualifier();
  return EMPTY_STRING;
}

} // namespace statements
} // namespace nodes
} // namespace sqltoast
