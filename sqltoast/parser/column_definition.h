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
 * Grammar for parsing column definitions in DDL
 * statements
 */

#ifndef SQLTOAST_PARSER_COLUMN_DEFINITION_H
#define SQLTOAST_PARSER_COLUMN_DEFINITION_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/unused.hpp>
#include <boost/optional.hpp>

#include "sqltoast/parser/common_rules.h"
#include "sqltoast/parser/identifier_rules.h"
#include "sqltoast/parser/specifier_rules.h"

namespace sqltoast
{
namespace parser
{
using boost::fusion::unused_type;
using boost::spirit::qi::uint_;

/* A default value is optional in a column definition */
typedef boost::variant<std::string, // quoted string value
                       literal_value_attr
                      > default_value_attr;

/* The actions associated with referential constraints */
typedef boost::fusion::vector<boost::optional<sql_referential_action>, // ON UPDATE
                              boost::optional<sql_referential_action> // ON DELETE 
                             > optional_referential_actions_attr;

/* 
 * A references clause is optional in a column definition and
 * contains the table and field identifiers of the referred-to
 * column.
 */
struct references_column_constraint_attr
{
  table_identifier_attr table_identifier;
  non_qualified_identifier_attr field_identifier;
  boost::optional<optional_referential_actions_attr> optional_actions;
};

/* A check constraint clause is optional on a column definition */
typedef std::string check_constraint_attr;

/* The following are optional clauses that result in a boolean
 * value if found or not found for a column definition */
typedef bool is_nullable_attr;
typedef bool is_primary_key_attr;
typedef bool is_unique_attr;

/* Optional pieces of the column definition */
typedef boost::fusion::vector<boost::optional<is_nullable_attr>,
                              boost::optional<is_primary_key_attr>,
                              boost::optional<default_value_attr>,
                              boost::optional<references_column_constraint_attr>,
                              boost::optional<collation_specifier_attr>,
                              boost::optional<is_unique_attr>,
                              boost::optional<check_constraint_attr>
                             > optional_column_definition_attr;

/* 
 * A column type can have a specifier of an integer or pair
 * of integers 
 */
typedef boost::variant<uint32_t,
                       std::pair<uint32_t, uint32_t>
                      > column_type_specifier_attr;

/**
 * Attribute of the column definition
 * grammar.  This is what is produced by
 * a successful parse of a column definition
 * clause
 */
struct column_definition_attr
{
  non_qualified_identifier_attr name;
  sql_column_type type;
  boost::optional<column_type_specifier_attr> type_specifier;
  boost::optional<optional_column_definition_attr> optionals;
};

/**
 * Parses the SET DEFAULT, SET NULL, DELETE, or CASCADE clause of the
 * references constraint.
 */
boost::spirit::qi::rule<std::string::iterator, sql_referential_action(), space_type>
referential_action_rule =
omit[no_case["cascade"]] [_val = REF_ACTION_CASCADE]
| omit[no_case["set"] >> no_case["null"]] [_val = REF_ACTION_SET_NULL]
| omit[no_case["set"] >> no_case["default"]] [_val = REF_ACTION_SET_DEFAULT]
| omit[no_case["no"] >> no_case["action"]] [_val = REF_ACTION_NONE];

/**
 * Parses the ON UPDATE or ON DELETE clause of a column
 * references constraint.
 */
boost::spirit::qi::rule<std::string::iterator, sql_referential_action(), space_type>
on_update_action_rule = 
omit[no_case["on"] >> no_case["update"]] >> referential_action_rule;

boost::spirit::qi::rule<std::string::iterator, sql_referential_action(), space_type>
on_delete_action_rule =
omit[no_case["on"] >> no_case["delete"]] >> referential_action_rule;

/**
 * Parses the REFERENCES part of a column
 * definition which may only contain a table and single
 * column identifier, which is unlike a full table constraint's
 * REFERENCES clause...
 */
boost::spirit::qi::rule<std::string::iterator, references_column_constraint_attr(), space_type >
references_column_constraint_rule = 
omit[no_case["references"]]
>> table_identifier_rule
>> lit('(') >> non_qualified_identifier_rule >> lit(')')
>> -(on_update_action_rule ^ on_delete_action_rule);

/**
 * Parses the DEFAULT clause of a column
 * definition.
 */
boost::spirit::qi::rule<std::string::iterator, default_value_attr(), space_type >
default_value_rule = 
omit[no_case["default"]] >> 
(
  quoted_string_rule
| literal_value_rule
);

/**
 * Parses the CHECK constraint part of a column
 * definition.
 */
boost::spirit::qi::rule<std::string::iterator, check_constraint_attr(), space_type >
check_constraint_rule = 
omit[no_case["check"]] >> lit('(') >> quoted_string_rule >> lit(')');

/**
 * Parses the optional nullable column constraint clause in
 * a column definition and returns a bool representing whether
 * the column is nullable
 */
boost::spirit::qi::rule<std::string::iterator, bool(), space_type >
is_nullable_column_constraint_rule =
  omit[no_case["not"] >> no_case["null"]] [_val = false]
| omit[no_case["null"]] [_val = true];

/**
 * Parses the optional primary key constraint clause in
 * a column definition and returns a bool representing whether
 * the column is a primary key
 */
boost::spirit::qi::rule<std::string::iterator, bool(), space_type >
is_primary_key_column_constraint_rule =
omit[no_case["primary"] >> no_case["key"]] [_val = true];

/**
 * Parses the optional unique constraint clause in
 * a column definition and returns a bool representing whether
 * the column is unique
 */
boost::spirit::qi::rule<std::string::iterator, bool(), space_type >
is_unique_column_constraint_rule =
omit[no_case["unique"]] [_val = true];

/* Exact numeric types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
exact_numeric_column_types_rule = 
omit[(no_case["numeric"] | no_case["decimal"] | no_case["dec"])] [_val = COLUMN_TYPE_DECIMAL]
| omit[no_case["integer"] | no_case["int"]] [_val = COLUMN_TYPE_INTEGER]
| omit[no_case["bigint"]] [_val = COLUMN_TYPE_BIG_INTEGER]
| omit[no_case["smallint"]] [_val = COLUMN_TYPE_SMALL_INTEGER];

/* Approximate numeric types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
approximate_numeric_column_types_rule = 
  omit[no_case["double"] >> -no_case["precision"]] [_val = COLUMN_TYPE_DOUBLE]
| omit[no_case["float"]] [_val = COLUMN_TYPE_FLOAT]
| omit[no_case["real"]] [_val = COLUMN_TYPE_FLOAT];

/* Varying character types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
varying_character_column_types_rule =
omit[no_case["varchar"]] [_val = COLUMN_TYPE_VARCHAR]
| omit[(no_case["character"] | no_case["char"]) >> no_case["varying"]] [_val = COLUMN_TYPE_VARCHAR];

/* Fixed character types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
fixed_character_column_types_rule = 
omit[(no_case["character"] | no_case["char"])] [_val = COLUMN_TYPE_CHAR];

/* Character types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
character_column_types_rule = 
varying_character_column_types_rule
| fixed_character_column_types_rule;

/* Numeric types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
numeric_column_types_rule = 
exact_numeric_column_types_rule
| approximate_numeric_column_types_rule;

/* Temporal types */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
temporal_column_types_rule = 
omit[no_case["date"]] [_val = COLUMN_TYPE_DATE]
| omit[no_case["timestamp"]] [_val = COLUMN_TYPE_TIMESTAMP]
| omit[no_case["time"]] [_val = COLUMN_TYPE_TIME];

/**
 * Parser for the column type.  Unfortunately, because the
 * SQL syntax allows for multiple words to describe a column
 * type (e.g. CHAR VARYING), we use a rule and not a simple
 * symbol table here
 *
 * Returns as its attribute the sql_column_type determined, or
 * fails if no match.
 */
boost::spirit::qi::rule<std::string::iterator, sql_column_type(), space_type >
column_type_rule = 
character_column_types_rule
| numeric_column_types_rule
| temporal_column_types_rule;

/**
 * Parser for the paired column type specifier, which is a
 * pair of integers
 */
boost::spirit::qi::rule<std::string::iterator, std::pair<uint32_t, uint32_t>(), space_type >
paired_column_type_specifier_rule = 
lit('(') >> uint_ >> lit(',') >> uint_ >> lit(')');

/**
 * Parser for the simple column type specifier, which is an unsigned integer
 */
boost::spirit::qi::rule<std::string::iterator, uint32_t(), space_type >
simple_column_type_specifier_rule = 
lit('(') >> uint_ >> lit(')');

/**
 * Parser for column type specifiers
 */
boost::spirit::qi::rule<std::string::iterator, column_type_specifier_attr(), space_type >
column_type_specifier_rule = 
simple_column_type_specifier_rule | paired_column_type_specifier_rule;

/**
 * Parser for a SQL column definition, used in DDL
 * statements
 *
 * @detail
 *
 * A column definition takes the following EBNF form:
 * 
 * column_definition : identifier column_data_type [ default_clause ] [ column_constraint_definition ... ] [ collate_clause ] 
 */
template<class iterator_type, class space_type>
struct column_definition_parser: boost::spirit::qi::grammar<iterator_type,
                                                            column_definition_attr(),
                                                            space_type >
{
  column_definition_parser() : column_definition_parser::base_type(start)
  {
    using boost::spirit::eoi;

    start = non_qualified_identifier_rule
            >> column_type_rule
            >> -column_type_specifier_rule
            >> -(
                   is_nullable_column_constraint_rule
                 ^ is_primary_key_column_constraint_rule
                 ^ default_value_rule
                 ^ references_column_constraint_rule
                 ^ collation_specifier_rule
                 ^ is_unique_column_constraint_rule
                 ^ check_constraint_rule
                );

#ifdef BOOST_SPIRIT_DEBUG
    BOOST_SPIRIT_DEBUG_NODE(start);
    BOOST_SPIRIT_DEBUG_NODE(non_qualified_identifier_rule);
    BOOST_SPIRIT_DEBUG_NODE(exact_numeric_column_types_rule);
    BOOST_SPIRIT_DEBUG_NODE(approximate_numeric_column_types_rule);
    BOOST_SPIRIT_DEBUG_NODE(numeric_column_types_rule);
    BOOST_SPIRIT_DEBUG_NODE(fixed_character_column_types_rule);
    BOOST_SPIRIT_DEBUG_NODE(varying_character_column_types_rule);
    BOOST_SPIRIT_DEBUG_NODE(character_column_types_rule);
    BOOST_SPIRIT_DEBUG_NODE(column_type_rule);
    BOOST_SPIRIT_DEBUG_NODE(column_type_specifier_rule);
    BOOST_SPIRIT_DEBUG_NODE(default_value_rule);
    BOOST_SPIRIT_DEBUG_NODE(references_column_constraint_rule);
    BOOST_SPIRIT_DEBUG_NODE(is_nullable_column_constraint_rule);
    BOOST_SPIRIT_DEBUG_NODE(is_primary_key_column_constraint_rule);
    BOOST_SPIRIT_DEBUG_NODE(check_constraint_rule);
#endif
  }

  boost::spirit::qi::rule<iterator_type, column_definition_attr(), space_type > start;
};

/* Our column definition parsers, created during library initialization time */
column_definition_parser<std::string::iterator, boost::spirit::qi::ascii::space_type> basic_ascii_column_definition_parser;

/**
 * Parses a string into a column definition attribute
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output attribute
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_column_definition(iterator_type start,
                                    iterator_type end,
                                    column_definition_attr& output)
{
  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_column_definition_parser,
                                      boost::spirit::qi::ascii::space,
                                      output))
    return SUCCESS;
  return SYNTAX_ERROR;
}

} // namespace parser
} // namespace sqltoast

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::references_column_constraint_attr,
  (sqltoast::parser::table_identifier_attr, table_identifier)
  (sqltoast::parser::non_qualified_identifier_attr, field_identifier)
  (boost::optional<sqltoast::parser::optional_referential_actions_attr>, optional_actions)
)

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::column_definition_attr,
  (sqltoast::parser::non_qualified_identifier_attr, name)
  (sqltoast::sql_column_type, type)
  (boost::optional<sqltoast::parser::column_type_specifier_attr>, type_specifier)
  (boost::optional<sqltoast::parser::optional_column_definition_attr>, optionals)
)

#ifdef BOOST_SPIRIT_DEBUG
#include <iostream>
#include <iomanip>
#include <boost/fusion/sequence/intrinsic/empty.hpp>
#include <boost/fusion/include/empty.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/tuple/tuple.hpp>

namespace std
{
  using boost::fusion::unused_type;
  using boost::tuples;
  using sqltoast::parser::column_definition_attr;
  using sqltoast::parser::optional_column_definition_attr;
  using sqltoast::parser::collation_specifier_attr;
  using sqltoast::parser::is_nullable_attr;
  using sqltoast::parser::is_primary_key_attr;
  using sqltoast::parser::is_unique_attr;
  using sqltoast::parser::default_value_attr;
  using sqltoast::parser::references_column_constraint_attr;
  using sqltoast::parser::optional_referential_actions_attr;
  using sqltoast::parser::column_type_specifier_attr;
  using sqltoast::sql_referential_action;

  std::ostream& operator<<(std::ostream& stream, default_value_attr& value)
  {
    if (std::string* val = boost::get<std::string>(&value))
    {
      stream << "\"" << *val << "\"";
    }
    if (uint64_t* val = boost::get<uint64_t>(&value))
    {
      stream << *val;
    }
    if (double* val = boost::get<double>(&value))
    {
      /* Ensure that 2.0 comes out as "2.00000" and not "2" */
      stream.precision(7);
      stream << std::setiosflags(std::ios_base::showpoint) << *val;
    }
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, references_column_constraint_attr& value)
  {
    stream << " references: " << value.table_identifier << "." << value.field_identifier;
    if (value.optional_actions) // optional wrapper on actions
    {
      /* We had some sort of trigger action... */
      optional_referential_actions_attr& actions = *value.optional_actions;
      for (int x = 0; x < 2; ++x)
      {
        if (boost::fusion::at_c<x>(actions).is_initialized())
        {
          sql_referential_action& action = *boost::fusion::at_c<x>(actions);
          if (x == 0)
            stream << " on update ";
          else
            stream << " on delete ";
          switch (action)
          {
          case REF_ACTION_NONE:
            stream << "no action";
            break;
          case REF_ACTION_SET_DEFAULT:
            stream << "set default";
            break;
          case REF_ACTION_SET_NULL:
            stream << "set null";
            break;
          case REF_ACTION_CASCADE:
            stream << "cascade";
            break;
          }
        }
      }
    }
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, column_type_specifier_attr& value)
  {
    if (uint32_t* val = boost::get<uint32_t>(&value))
    {
      stream << *val;
    }
    if (std::pair<uint32_t, uint32_t>* val = boost::get<std::pair<uint32_t, uint32_t> >(&value))
    {
      stream << val->first << "," << val->second;
    }
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, column_definition_attr& value)
  {
    stream << "column_definition (name: " << value.name << " type: " << value.type;
    if (value.type_specifier)
      stream << " type specifier: " << *value.type_specifier;

    /* Process all the optional elements of the column definition... */
    if (value.optionals)
    {
      optional_column_definition_attr& optionals = *value.optionals;
      if (boost::fusion::at_c<0>(optionals).is_initialized())
        stream << " nullable: " << std::boolalpha << *boost::fusion::at_c<0>(optionals);
      if (boost::fusion::at_c<1>(optionals).is_initialized())
        stream << " primary key: " << std::boolalpha << *boost::fusion::at_c<1>(optionals);
      if (boost::fusion::at_c<2>(optionals).is_initialized())
        stream << " default: " << *boost::fusion::at_c<2>(optionals);
      if (boost::fusion::at_c<3>(optionals).is_initialized())
        stream << " references: " << *boost::fusion::at_c<3>(optionals);
      if (boost::fusion::at_c<4>(optionals).is_initialized())
        stream << " collation: " << *boost::fusion::at_c<4>(optionals);
      if (boost::fusion::at_c<5>(optionals).is_initialized())
        stream << " unique: " << std::boolalpha << *boost::fusion::at_c<5>(optionals);
      if (boost::fusion::at_c<6>(optionals).is_initialized())
        stream << " check: " << *boost::fusion::at_c<6>(optionals);
    }
    stream << ')';
    return stream;
  }
}
#endif

#endif /* SQLTOAST_PARSER_COLUMN_DEFINITION_H */
