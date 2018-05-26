PROJECT(sqltoaster)

SET(PROJECT_VERSION 0.1)
SET(PROJECT_DESCRIPTION "A demonstration of the sqltoast library")

SET(SQLTOASTER_SOURCES
    main.cc
    printer.cc
    print/yaml/statement.cc
)

ADD_EXECUTABLE(sqltoaster ${SQLTOASTER_SOURCES})
INCLUDE_DIRECTORIES(../sqltoast/include)
TARGET_LINK_LIBRARIES(sqltoaster PRIVATE sqltoast)
