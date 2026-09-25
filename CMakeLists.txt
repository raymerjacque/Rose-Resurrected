cmake_minimum_required(VERSION 3.12)
project(RoseResurrected CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

add_compile_options(
    -Wall
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-result
    -Wno-format-overflow
    -Wno-write-strings
    -Wreturn-type
)

# Output binaries directly to Server.Run.2 where 3DData, conf, and assets live
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/VS_Projects/Server.Run.2")

# Find Dependencies
find_package(Threads REQUIRED)

find_path(MYSQL_INCLUDE_DIR
    NAMES mysql.h
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES mariadb mysql
)

find_library(MYSQL_LIBRARY
    NAMES mariadb mysqlclient libmysql
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
)

if(NOT MYSQL_INCLUDE_DIR OR NOT MYSQL_LIBRARY)
    message(FATAL_ERROR "MySQL/MariaDB client development files not found! Please install libmariadb-dev or default-libmysqlclient-dev.")
endif()

message(STATUS "Found MySQL include: ${MYSQL_INCLUDE_DIR}")
message(STATUS "Found MySQL library: ${MYSQL_LIBRARY}")

# Common source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/VS_Projects/src")
set(COMMON_DIR "${SRC_DIR}/Common")

set(COMMON_SOURCES
    ${COMMON_DIR}/Compat.cpp
    ${COMMON_DIR}/SocketBase.cpp
    ${COMMON_DIR}/SocketClient.cpp
    ${COMMON_DIR}/SocketServer.cpp
    ${COMMON_DIR}/config.cpp
    ${COMMON_DIR}/log.cpp
    ${COMMON_DIR}/rosecrypt.cpp
    ${COMMON_DIR}/database/database.cpp
)

set(COMMON_INCLUDES
    ${COMMON_DIR}
    ${COMMON_DIR}/database
    ${MYSQL_INCLUDE_DIR}
)

# -------------------------------------------------------------
# 1. Login Server Target
# -------------------------------------------------------------
set(LOGIN_DIR "${SRC_DIR}/Login_Server")
file(GLOB LOGIN_SOURCES "${LOGIN_DIR}/*.cpp")

add_executable(loginserver
    ${COMMON_SOURCES}
    ${LOGIN_SOURCES}
)

target_include_directories(loginserver PRIVATE
    ${COMMON_INCLUDES}
    ${LOGIN_DIR}
)

target_link_libraries(loginserver PRIVATE
    Threads::Threads
    ${MYSQL_LIBRARY}
    m
)

# -------------------------------------------------------------
# 2. Character Server Target
# -------------------------------------------------------------
set(CHAR_DIR "${SRC_DIR}/Char_Server")
file(GLOB CHAR_SOURCES "${CHAR_DIR}/*.cpp")

add_executable(charserver
    ${COMMON_SOURCES}
    ${CHAR_SOURCES}
)

target_include_directories(charserver PRIVATE
    ${COMMON_INCLUDES}
    ${CHAR_DIR}
)

target_link_libraries(charserver PRIVATE
    Threads::Threads
    ${MYSQL_LIBRARY}
    m
)

# -------------------------------------------------------------
# 3. World Server Target
# -------------------------------------------------------------
set(WORLD_DIR "${SRC_DIR}/World_Server")
file(GLOB WORLD_SOURCES "${WORLD_DIR}/*.cpp")
file(GLOB AIP_SOURCES "${WORLD_DIR}/aip/*.cpp")
file(GLOB QUEST_SOURCES "${WORLD_DIR}/quest/*.cpp")

add_executable(worldserver
    ${COMMON_SOURCES}
    ${WORLD_SOURCES}
    ${AIP_SOURCES}
    ${QUEST_SOURCES}
)

target_include_directories(worldserver PRIVATE
    ${COMMON_INCLUDES}
    ${WORLD_DIR}
    ${WORLD_DIR}/aip
    ${WORLD_DIR}/quest
)

target_link_libraries(worldserver PRIVATE
    Threads::Threads
    ${MYSQL_LIBRARY}
    m
)
