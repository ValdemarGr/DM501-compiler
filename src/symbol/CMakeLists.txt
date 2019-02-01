cmake_minimum_required(VERSION 2.8.9)

project(symbol_module)

add_executable(symbol main.c)

file(GLOB SOURCES "src/symbol/*.h")
file(GLOB SOURCES "src/symbol/*.c")
