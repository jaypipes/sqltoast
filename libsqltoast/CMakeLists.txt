CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

# To build with debugging symbols and some extra tracing functionality, do:
#
# $ROOT_DIR/build $> cmake _DCMAKE_BUILD_TYPE=debug ..
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DSQLTOAST_DEBUG")

# ============================================================================
# start of library build instructions
# ============================================================================

PROJECT(libsqltoast)

SET(PROJECT_VERSION 0.1)
SET(PROJECT_DESCRIPTION "convert SQL to an abstract syntax tree")
SET(SQLTOAST_VERSION_MAJOR 0)
SET(SQLTOAST_VERSION_MINOR 1)

# Header file for configuration templated variables
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/include/sqltoast.h.in"
    "${PROJECT_BINARY_DIR}/sqltoast.h"
)

# add the binary directory to the search path for include files
# so that we will find config.h
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}" "include")

ADD_LIBRARY(sqltoast SHARED
    src/ast.cc
    src/parser/create_database.cc
    src/internal/identifier.cc
    src/internal/keywords.cc
    src/parse.cc
    src/internal/symbols.cc
)

# Set up a pkg-config file for libsqltoast from the template in the root
# directory and make sure we only interpolate @VARS not $VARS
CONFIGURE_FILE(sqltoast.pc.in sqltoast.pc @ONLY)

INSTALL(TARGETS sqltoast DESTINATION lib)
INSTALL(FILES
    "${PROJECT_BINARY_DIR}/sqltoast.h"
    DESTINATION include)

# ============================================================================
# end of library build instructions
# ============================================================================
