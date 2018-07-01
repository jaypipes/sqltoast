PROJECT(libsqltoast VERSION 0.1 LANGUAGES CXX)

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
    src/parser/query.cc
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
    src/print/query.cc
    src/print/statement.cc
    src/print/table_reference.cc
    src/print/value.cc
    src/print/value_expression.cc
)

ADD_LIBRARY(sqltoast STATIC ${LIBSQLTOAST_SOURCES})
SET_TARGET_PROPERTIES(sqltoast PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
SET(LIBSQLTOAST_BUILD_OPTS, "-Wall --no-rtti")
TARGET_COMPILE_OPTIONS(sqltoast
    PUBLIC ${LIBSQLTOAST_BUILD_OPTS}
)

# To build with debugging symbols and some extra tracing functionality, do:
#
# $ROOT_DIR/build $> cmake -DCMAKE_BUILD_TYPE=debug ..
SET(LIBSQLTOAST_BUILD_DEBUG_OPTS, "-Werror -DSQLTOAST_DEBUG")
TARGET_COMPILE_OPTIONS(sqltoast
    PRIVATE "$<$<CONFIG:DEBUG>:${LIBSQLTOAST_BUILD_DEBUG_OPTS}>"
)

# To build a speed-optimized binary, do:
#
# $ROOT_DIR/build $> cmake -DCMAKE_BUILD_TYPE=release ..
SET(LIBSQLTOAST_BUILD_RELEASE_OPTS, "-O2 -falign-functions=1 -falign-jumps=1 -falign-loops=1 -falign-labels=1")
TARGET_COMPILE_OPTIONS(sqltoast
    PRIVATE "$<$<CONFIG:RELEASE>:${LIBSQLTOAST_BUILD_RELEASE_OPTS}>"
)

# Header file for configuration templated variables
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/include/sqltoast/sqltoast.h.in"
    "${PROJECT_SOURCE_DIR}/include/sqltoast/sqltoast.h"
)

# Header files in src/ are private and not exported for public sqltoast
# interfaces
TARGET_INCLUDE_DIRECTORIES(sqltoast
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src)

# Include libunwind when we're in debug mode
if (${BUILD_TYPE_LOWER} STREQUAL "debug")
    FIND_PACKAGE(libunwind REQUIRED)
    TARGET_LINK_LIBRARIES(sqltoast ${LIBUNWIND_LIBRARIES})
endif ()

# 'make install' to the correct locations (provided by GNUInstallDirs).
INSTALL(TARGETS sqltoast EXPORT sqltoast_config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
INSTALL(EXPORT sqltoast_config DESTINATION share/sqltoast/cmake)

# This makes the project importable from the build directory
EXPORT(TARGETS sqltoast FILE sqltoast_config.cmake)
