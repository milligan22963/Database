cmake_minimum_required(VERSION 3.13)

project(Database)

# Debug specifics
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

# Release specifics
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

# Configuration specific defines
include(configuration/${AFM_CONFIG}.cmake)

set (CMAKE_CXX_STANDARD 17)

# Output for install
set (LIB_DIR ${CMAKE_BINARY_DIR}/staging/lib)
set (BIN_DIR ${CMAKE_BINARY_DIR}/staging/bin)

include_directories (
    .
    internal
)

set(SQLITE_SOURCE_LOCATION src/sqlite)
set(MARIA_SOURCE_LOCATION src/maria)
set(PGSQL_SOURCE_LOCATION src/pgsql)

set(SRC_FILES
    src/Column.cpp
    src/Database.cpp
    src/DatabaseFactory.cpp
    src/Row.cpp
    src/Table.cpp
    src/VariableData.cpp
    tools/src/tools.cpp
)

set(SQLITE_SRC_FILES
    ${SQLITE_SOURCE_LOCATION}/SQLiteColumn.cpp
    ${SQLITE_SOURCE_LOCATION}/SQLiteDB.cpp
    ${SQLITE_SOURCE_LOCATION}/SQLiteTable.cpp
    ${SQLITE_SOURCE_LOCATION}/SQLiteUtility.cpp
)

set(MARIA_SRC_FILES
    ${MARIA_SOURCE_LOCATION}/MariaColumn.cpp
    ${MARIA_SOURCE_LOCATION}/MariaDB.cpp
    ${MARIA_SOURCE_LOCATION}/MariaTable.cpp
    ${MARIA_SOURCE_LOCATION}/MariaUtility.cpp
)

set(PGSQL_SRC_FILES
    ${PGSQL_SOURCE_LOCATION}/PgSqlColumn.cpp
    ${PGSQL_SOURCE_LOCATION}/PgSqlDB.cpp
    ${PGSQL_SOURCE_LOCATION}/PgSqlTable.cpp
    ${PGSQL_SOURCE_LOCATION}/PgSqlUtility.cpp
)

set(TEST_FILES
    test/main.cpp
)

add_library(afmdatabase SHARED ${SRC_FILES} ${SQLITE_SRC_FILES} ${MARIA_SRC_FILES} ${PGSQL_SRC_FILES})
add_executable (TestApp ${TEST_FILES})

target_link_libraries(
    TestApp
    afmdatabase
    sqlite3
    mariadb
    pq
    pqxx
    m
    dl
    pthread
)

install (TARGETS TestApp DESTINATION ${BIN_DIR})