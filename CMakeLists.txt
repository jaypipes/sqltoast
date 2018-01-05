CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(sqltoast)

SET(sqltoast_VERSION_MAJOR 0)
SET(sqltoast_VERSION_MINOR 1)

SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

# To build with debugging symbols and some extra tracing functionality, do:
#
# $ROOT_DIR/build $> cmake _DCMAKE_BUILD_TYPE=debug ..
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DSQLTOAST_DEBUG")

# Header file for configuration templated variables
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

# add the binary directory to the search path for include files
# so that we will find config.h
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}" "include")

ADD_EXECUTABLE(sqltoast
    src/ast.cc
    src/ast_node.cc
    src/create_database.cc
    src/example.cc
    src/identifier.cc
    src/keywords.cc
    src/parse.cc
    src/symbols.cc
)
