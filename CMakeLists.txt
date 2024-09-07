cmake_minimum_required(VERSION 3.10)

project(Compiler VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRCS
    src/compiler.cpp
    src/lexer.cpp
    src/parser.cpp
    src/token.cpp
)

add_executable(compiler.out ${SRCS})

target_compile_options(compiler.out PRIVATE -Wall -g)
