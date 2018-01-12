CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

# To build with debugging symbols and some extra tracing functionality, do:
#
# $ROOT_DIR/build $> cmake _DCMAKE_BUILD_TYPE=debug ..
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DSQLTOAST_DEBUG")

# Handle the libsqltoast library
ADD_SUBDIRECTORY(libsqltoast)
