CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
SET(CMAKE_CXX_FLAGS "-std=c++14 -Wall -fno-rtti")

# To build with debugging symbols and some extra tracing functionality, do:
#
# $ROOT_DIR/build $> cmake -DCMAKE_BUILD_TYPE=debug ..
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -DSQLTOAST_DEBUG")

# To build a speed-optimized binary, do:
#
# $ROOT_DIR/build $> cmake -DCMAKE_BUILD_TYPE=fast ..
SET(CMAKE_CXX_FLAGS_FAST "-O2 -falign-functions=1 -falign-jumps=1 -falign-loops=1 -falign-labels=1")

ADD_SUBDIRECTORY(libsqltoast)
ADD_SUBDIRECTORY(sqltoaster)
