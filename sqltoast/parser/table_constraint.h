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
 * Grammar for parsing table constraints in DDL
 * statements
 */

#ifndef SQLTOAST_PARSER_TABLE_CONSTRAINT_H
#define SQLTOAST_PARSER_TABLE_CONSTRAINT_H

#include "sqltoast/visibility.h"
#include "sqltoast/common.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/unused.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>

#include "sqltoast/parser/common_rules.h"
#include "sqltoast/parser/identifier_rules.h"

#include <vector>

namespace sqltoast
{
namespace parser
{
using boost::fusion::unused_type;
using boost::spirit::_pass;
using boost::spirit::_val;

typedef std::string search_condition_attr;
typedef std::vector<non_qualified_identifier_attr> column_name_list_attr;

struct unique_constraint_definition_attr
{
  bool is_primary_key;
  column_name_list_attr columns;
};

struct references_specification_attr
{
  table_identifier_attr referenced_table;
  column_name_list_attr referenced_columns;
};

struct referential_constraint_definition_attr
{
  column_name_list_attr referencing_columns;
  references_specification_attr references_specification;
};

typedef std::string check_constraint_definition_attr;

/* A table constraint is one of the above structs... */
typedef boost::variant<unique_constraint_definition_attr,
                       referential_constraint_definition_attr,
                       check_constraint_definition_attr
                      > table_constraint_definition_attr;
                              

/**
 * Attribute of the table constraint
 * grammar.  This is what is produced by
 * a successful parse of a table constraint
 * clause
 */
struct table_constraint_attr
{
  boost::optional<non_qualified_identifier_attr> name;
  table_constraint_definition_attr definition;
};

/**
 * Parser for a list of column names, which are non-qualified
 */
boost::spirit::qi::rule<std::string::iterator, column_name_list_attr(), space_type>
column_name_list_rule =
non_qualified_identifier_rule % ',';

/**
 * Parser that returns whether the primary key keywords
 * were found
 */
boost::spirit::qi::rule<std::string::iterator, bool(), space_type>
unique_or_primary_key_rule =
omit[no_case["unique"]] [_val = false]
| omit[no_case["primary"] >> no_case["key"]] [_val = true]
| eps[_pass = false];

/**
 * Parses a unique constraint definition
 */
boost::spirit::qi::rule<std::string::iterator, unique_constraint_definition_attr(), space_type>
unique_constraint_definition_rule =
unique_or_primary_key_rule >> lit('(') >> column_name_list_rule >> lit(')');

/**
 * Parses a references specification
 */
boost::spirit::qi::rule<std::string::iterator, references_specification_attr(), space_type>
references_specification_rule = 
omit[no_case["references"]] >> table_identifier_rule >> lit('(') >> column_name_list_rule >> lit(')');

/**
 * Parses a FOREIGN KEY (x) clause into a column name list
 */
boost::spirit::qi::rule<std::string::iterator, column_name_list_attr(), space_type>
foreign_key_clause_rule =
omit[no_case["foreign"] >> no_case["key"]] >> lit('(') >> column_name_list_rule >> lit(')');
/**
 * Parses a referential constraint definition
 */
boost::spirit::qi::rule<std::string::iterator, referential_constraint_definition_attr(), space_type>
referential_constraint_definition_rule =
foreign_key_clause_rule >> references_specification_rule;

/**
 * Parses a search condition
 *
 * @todo
 *
 * Move to separate file..
 */
boost::spirit::qi::rule<std::string::iterator, search_condition_attr(), space_type>
search_condition_rule = quoted_string_rule;

/**
 * Parses a check constraint definition
 */
boost::spirit::qi::rule<std::string::iterator, check_constraint_definition_attr(), space_type>
check_constraint_definition_rule =
omit[no_case["check"]] >> lit('(') >> search_condition_rule >> lit(')');

/**
 * Parses a table constraint definition
 */
boost::spirit::qi::rule<std::string::iterator, table_constraint_definition_attr(), space_type>
table_constraint_definition_rule =
  unique_constraint_definition_rule
| referential_constraint_definition_rule
| check_constraint_definition_rule;

/**
 * Parser for a SQL table constraint, used in DDL
 * statements
 *
 * @detail
 *
 * A table constraint takes the following EBNF form:
 * 
 * table_constraint_definition : [constraint_name_definition] table_constraint
 *
 * constraint_name_definition : CONSTRAINT non_qualified_identifier
 *
 * table_constraint : unique_constraint_definition
 *                  | referential_constraint_definition
 *                  | check_constraint_definition
 *
 * unique_constraint_definition : unique_specification LPAREN column_name_list RPAREN
 *
 * unique_specification : UNIQUE | PRIMARY KEY
 *
 * referential_constraint_definition : FOREIGN KEY LPAREN column_name_list RPAREN references_specification
 *
 * references_specification : REFERENCES referenced_table_and_columns [referential_triggered_action]
 *
 * referenced_table_and_columns : table_identifier LPAREN column_name_list RPAREN
 *
 * referential_triggered_action : ...
 *
 * @todo
 *
 * Possibly support the check constraint time?
 */
template<class iterator_type, class space_type>
struct table_constraint_parser: boost::spirit::qi::grammar<iterator_type,
                                                            table_constraint_attr(),
                                                            space_type >
{
  table_constraint_parser() : table_constraint_parser::base_type(start)
  {
    using boost::spirit::eoi;

    start = -(omit[no_case["constraint"]] >> non_qualified_identifier_rule)
            >> table_constraint_definition_rule;

#ifdef BOOST_SPIRIT_DEBUG
    BOOST_SPIRIT_DEBUG_NODE(start);
    BOOST_SPIRIT_DEBUG_NODE(check_constraint_definition_rule);
    BOOST_SPIRIT_DEBUG_NODE(unique_constraint_definition_rule);
    BOOST_SPIRIT_DEBUG_NODE(table_constraint_definition_rule);
    BOOST_SPIRIT_DEBUG_NODE(foreign_key_clause_rule);
    BOOST_SPIRIT_DEBUG_NODE(references_specification_rule);
    BOOST_SPIRIT_DEBUG_NODE(referential_constraint_definition_rule);
#endif
  }

  boost::spirit::qi::rule<iterator_type, table_constraint_attr(), space_type > start;
};

/* Our table constraint parsers, created during library initialization time */
table_constraint_parser<std::string::iterator, boost::spirit::qi::ascii::space_type> basic_ascii_table_constraint_parser;

/**
 * Parses a string into a table constraint attribute
 *
 * @param[in] Start of string to parse
 * @param[in] End of string to parse
 * @param[out] Reference to output attribute
 */
template<class iterator_type>
SQLTOAST_API
result_code parse_table_constraint(iterator_type start,
                                   iterator_type end,
                                   table_constraint_attr& output)
{
  if (boost::spirit::qi::phrase_parse(start,
                                      end,
                                      basic_ascii_table_constraint_parser,
                                      boost::spirit::qi::ascii::space,
                                      output))
    return SUCCESS;
  return SYNTAX_ERROR;
}
} // namespace parser
} // namespace sqltoast

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::unique_constraint_definition_attr,
  (bool, is_primary_key)
  (sqltoast::parser::column_name_list_attr, columns)
)

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::references_specification_attr,
  (sqltoast::parser::table_identifier_attr, referenced_table)
  (sqltoast::parser::column_name_list_attr, referenced_columns)
)

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::referential_constraint_definition_attr,
  (sqltoast::parser::column_name_list_attr, referencing_columns)
  (sqltoast::parser::references_specification_attr, references_specification)
)

BOOST_FUSION_ADAPT_STRUCT(
  sqltoast::parser::table_constraint_attr,
  (boost::optional<sqltoast::parser::non_qualified_identifier_attr>, name)
  (sqltoast::parser::table_constraint_definition_attr, definition)
)

#ifdef BOOST_SPIRIT_DEBUG

#include <iostream>
#include <iomanip>
#include <boost/fusion/sequence/intrinsic/empty.hpp>
#include <boost/fusion/include/empty.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>

namespace std
{
  using boost::fusion::unused_type;
  using sqltoast::parser::table_constraint_attr;
  using sqltoast::parser::table_constraint_definition_attr;
  using sqltoast::parser::unique_constraint_definition_attr;
  using sqltoast::parser::referential_constraint_definition_attr;
  using sqltoast::parser::check_constraint_definition_attr;
  using sqltoast::parser::column_name_list_attr;
  using sqltoast::parser::references_specification_attr;

  std::ostream& operator<<(std::ostream& stream, column_name_list_attr& value)
  {
    for (column_name_list_attr::iterator iter = value.begin();
         iter != value.end();
         ++iter)
    {
      stream << *iter;
      if (iter != (value.end() - 1))
        stream << ", ";
    }
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, unique_constraint_definition_attr& value)
  {
    stream << " unique_constraint_def (is_primary_key: " << std::boolalpha << value.is_primary_key;
    stream << " columns: " << value.columns << ")";
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, references_specification_attr& value)
  {
    stream << " referenced_table: " << value.referenced_table;
    stream << " referenced_columns: " << value.referenced_columns;
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, referential_constraint_definition_attr& value)
  {
    stream << " ref_constraint_def (referencing columns: " << value.referencing_columns;
    stream << value.references_specification;
    stream << ")";
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, table_constraint_definition_attr& value)
  {
    if (unique_constraint_definition_attr* val = boost::get<unique_constraint_definition_attr>(&value))
      stream << *val;
    if (referential_constraint_definition_attr* val = boost::get<referential_constraint_definition_attr>(&value))
      stream << *val;
    if (check_constraint_definition_attr* val = boost::get<check_constraint_definition_attr>(&value))
      stream << " check_constraint_def (" << *val << ")";
    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, table_constraint_attr& value)
  {
    stream << "table_constraint (";
    if (value.name)
      stream << "name: " << *value.name;
    stream << value.definition;
    return stream;
  }
}
#endif

#endif /* SQLTOAST_PARSER_TABLE_CONSTRAINT_H */
