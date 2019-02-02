cmake_minimum_required(VERSION 2.8.9)

project(compiler)

file(GLOB SOURCES
        "*.h"
        "*.c"
        "symbol/*.c"
        "symbol/*.h"
        )

add_executable(symbol main.c ${SOURCES})