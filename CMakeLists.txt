set(PROJECT_NAME "rack")

project(${PROJECT_NAME})
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wconversion -Wpedantic -fsanitize=undefined")

include_directories(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/include")

set(SOURCES
        "${CMAKE_SOURCE_DIR}/src/main.cpp"
        "${CMAKE_SOURCE_DIR}/src/Lexer.cpp"
        "${CMAKE_SOURCE_DIR}/src/Utility.cpp"
        "${CMAKE_SOURCE_DIR}/src/Compiler.cpp"
        )

add_executable(${PROJECT_NAME} ${SOURCES})
