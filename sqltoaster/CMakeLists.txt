PROJECT(sqltoaster)

SET(PROJECT_VERSION 0.1)
SET(PROJECT_DESCRIPTION "A demonstration of the sqltoast library")

SET(SQLTOASTER_SOURCES
    main.cc
    print/identifier.cc
    print/constraint.cc
    print/column_definition.cc
    print/column_reference.cc
    print/data_type.cc
    print/statement.cc
    print/predicate.cc
    print/table_reference.cc
    print/value.cc
    print/value_expression.cc
)

ADD_EXECUTABLE(sqltoaster ${SQLTOASTER_SOURCES})
INCLUDE_DIRECTORIES(../sqltoast/include)
TARGET_LINK_LIBRARIES(sqltoaster PRIVATE sqltoast)
