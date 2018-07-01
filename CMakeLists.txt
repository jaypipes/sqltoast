CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
PROJECT(sqltoast)

# String comparisons in cmake are case-sensitive and unfortunately, CamelCase
# is used for many examples/tutorials, so people do things like cmake ..
# -DCMAKE_BUILD_TYPE=Debug. We want to only have to compare lower-cased build
# types, so we have a variable here that lower-cases the build type and stores
# it in a cache variable for use by the other cmakes.
STRING(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)

SET(
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake_modules"
)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
INCLUDE(GNUInstallDirs)

ADD_SUBDIRECTORY(libsqltoast)
ADD_SUBDIRECTORY(sqltoaster)
