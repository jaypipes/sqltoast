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
 * @file Common structures, enums, and defines
 */

#ifndef SQLTOAST_COMMON_H
#define SQLTOAST_COMMON_H

#include <boost/variant.hpp>
#include <string>

namespace sqltoast
{

/**
 * An empty string
 */
static std::string const EMPTY_STRING = "";

/**
 * Possible return codes from parsing, tokenizing, etc
 */
enum result_code
{
  SUCCESS = 0, ///< nothing wrong
  SYNTAX_ERROR = 1 ///< General syntax error found
};

/**
 * The different classes of SQL statements
 */
enum sql_statement_class
{
  DDL = 1, ///< Data definition statements such as CREATE TABLE
  DML = 2 ///< Data modification statements such as SELECT, INSERT, etc
};

/**
 * Types of columns in tables
 */
enum sql_column_type
{
  COLUMN_TYPE_INTEGER = 1,
  COLUMN_TYPE_BIG_INTEGER,
  COLUMN_TYPE_SMALL_INTEGER,
  COLUMN_TYPE_DOUBLE,
  COLUMN_TYPE_FLOAT,
  COLUMN_TYPE_DECIMAL,
  COLUMN_TYPE_DATE,
  COLUMN_TYPE_TIME,
  COLUMN_TYPE_TIMESTAMP,
  COLUMN_TYPE_VARCHAR,
  COLUMN_TYPE_CHAR,
  COLUMN_TYPE_TEXT,
  COLUMN_TYPE_BLOB
};

/**
 * Types of referential triggers
 */
enum sql_referential_trigger
{
  REF_TRIGGER_ON_UPDATE = 1,
  REF_TRIGGER_ON_DELETE
};

/**
 * Types of referential actions
 */
enum sql_referential_action
{
  REF_ACTION_NONE = 0,
  REF_ACTION_CASCADE,
  REF_ACTION_SET_DEFAULT,
  REF_ACTION_SET_NULL
};

/**
 * The resulting value of a literal expression
 */
typedef boost::variant<uint64_t,
                       int64_t,
                       uint32_t,
                       int32_t,
                       uint8_t,
                       int8_t,
                       double,
                       float,
                       bool,
                       std::string> literal_value_type;

} // namespace sqltoast

#endif /* SQLTOAST_COMMON_H */
