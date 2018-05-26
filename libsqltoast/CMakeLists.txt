PROJECT(libsqltoast)

SET(PROJECT_VERSION 0.1)
SET(PROJECT_DESCRIPTION "A recursive descent parser for SQL")
SET(SQLTOAST_VERSION_MAJOR 0)
SET(SQLTOAST_VERSION_MINOR 1)
SET(LIBSQLTOAST_SOURCES
    src/parser/column_definition.cc
    src/parser/data_type_descriptor.cc
    src/parser/comment.cc
    src/parser/constraint.cc
    src/parser/context.cc
    src/parser/error.cc
    src/parser/keyword.cc
    src/parser/identifier.cc
    src/parser/lexer.cc
    src/parser/literal.cc
    src/parser/parse.cc
    src/parser/predicate.cc
    src/parser/sequence.cc
    src/parser/statement.cc
    src/parser/statements/alter_table.cc
    src/parser/statements/create_schema.cc
    src/parser/statements/create_table.cc
    src/parser/statements/create_view.cc
    src/parser/statements/delete.cc
    src/parser/statements/drop_schema.cc
    src/parser/statements/drop_table.cc
    src/parser/statements/drop_view.cc
    src/parser/statements/grant.cc
    src/parser/statements/insert.cc
    src/parser/statements/rollback.cc
    src/parser/statements/select.cc
    src/parser/statements/commit.cc
    src/parser/statements/update.cc
    src/parser/symbol.cc
    src/parser/table_reference.cc
    src/parser/token.cc
    src/parser/value.cc
    src/parser/value_expression.cc
    src/print/column_definition.cc
    src/print/column_reference.cc
    src/print/constraint.cc
    src/print/data_type.cc
    src/print/identifier.cc
    src/print/predicate.cc
    src/print/statement.cc
    src/print/table_reference.cc
    src/print/value.cc
    src/print/value_expression.cc
)

# Header file for configuration templated variables
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/include/sqltoast/sqltoast.h.in"
    "${PROJECT_SOURCE_DIR}/include/sqltoast/sqltoast.h"
)

# Header files in src/ are private and not exported for public sqltoast
# interfaces
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

ADD_LIBRARY(sqltoast STATIC ${LIBSQLTOAST_SOURCES})
TARGET_INCLUDE_DIRECTORIES(sqltoast PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Set up a pkg-config file for libsqltoast from the template in the root
# directory and make sure we only interpolate @VARS not $VARS
CONFIGURE_FILE(sqltoast.pc.in sqltoast.pc @ONLY)

INSTALL(TARGETS sqltoast DESTINATION lib)
