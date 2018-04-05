CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

# To build with debugging symbols and some extra tracing functionality, do:
#
# $ROOT_DIR/build $> cmake -DCMAKE_BUILD_TYPE=debug ..
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DSQLTOAST_DEBUG")

# To build a speed-optimized binary, do:
#
# $ROOT_DIR/build $> cmake -DCMAKE_BUILD_TYPE=fast ..
SET(CMAKE_CXX_FLAGS_FAST "-O2 -falign-functions=1 -falign-jumps=1 -falign-loops=1 -falign-labels=1")

# ============================================================================
# start of library build instructions
# ============================================================================

PROJECT(libsqltoast)

SET(PROJECT_VERSION 0.1)
SET(PROJECT_DESCRIPTION "convert SQL to Abstract Syntax Tree")
SET(SQLTOAST_VERSION_MAJOR 0)
SET(SQLTOAST_VERSION_MINOR 1)
SET(LIBSQLTOAST_SOURCES
    src/identifier.cc
    src/constraint.cc
    src/column_definition.cc
    src/column_reference.cc
    src/data_type.cc
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
    src/parser/statements/delete.cc
    src/parser/statements/drop_schema.cc
    src/parser/statements/drop_table.cc
    src/parser/statements/insert.cc
    src/parser/statements/select.cc
    src/parser/statements/commit.cc
    src/parser/statements/update.cc
    src/parser/symbol.cc
    src/parser/table_reference.cc
    src/parser/token.cc
    src/parser/value.cc
    src/parser/value_expression.cc
    src/statement.cc
    src/predicate.cc
    src/table_reference.cc
    src/value.cc
    src/value_expression.cc
)

# Header file for configuration templated variables
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/include/sqltoast.h.in"
    "${PROJECT_SOURCE_DIR}/include/sqltoast.h"
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

# ============================================================================
# end of library build instructions
# ============================================================================

SET(SQLTOASTER_SOURCES
    src/sqltoaster.cc
)

ADD_EXECUTABLE(sqltoaster ${SQLTOASTER_SOURCES})

TARGET_LINK_LIBRARIES(sqltoaster PRIVATE sqltoast)
