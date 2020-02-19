PROJECT(sqltoaster)

SET(PROJECT_VERSION 0.1)
SET(PROJECT_DESCRIPTION "A demonstration of the sqltoast library")

SET(SQLTOASTER_SOURCES
    main.cc
    node.cc
    printer.cc
    node/statement.cc
)

ADD_EXECUTABLE(sqltoaster ${SQLTOASTER_SOURCES})
SET_TARGET_PROPERTIES(sqltoaster PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
SET(SQLTOASTER_BUILD_OPTS, "-Wall --no-rtti")
TARGET_COMPILE_OPTIONS(sqltoaster
    PUBLIC ${SQLTOASTER_BUILD_OPTS}
)
TARGET_LINK_LIBRARIES(sqltoaster sqltoast)
