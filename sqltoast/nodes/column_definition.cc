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

#include "sqltoast/nodes/column_definition.h"

using namespace std;
using namespace boost;

namespace sqltoast
{
namespace nodes
{

column_default_value const& column_definition::get_default_value(void) const
{
  return *_default_value;
}

bool column_definition::has_default_value(void) const
{
  return _default_value;
}

} // namespace nodes
} // namespace sqltoast
