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
INCLUDE_DIRECTORIES(../sqltoast/include)
TARGET_LINK_LIBRARIES(sqltoaster PRIVATE sqltoast)
