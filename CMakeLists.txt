cmake_minimum_required(VERSION 3.10)

project(Compiler VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRCS
    compiler/compiler.cpp
    compiler/enums.cpp
    compiler/lexer.cpp
    compiler/parser.cpp
)

add_executable(compiler.out ${SRCS})

target_compile_options(compiler.out PRIVATE -Wall -g)
