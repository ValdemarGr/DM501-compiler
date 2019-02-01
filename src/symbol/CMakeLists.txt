cmake_minimum_required(VERSION 2.8.9)

project(symbol_module)

file(GLOB SOURCES
        "*.h"
        "*.c"
        )

add_executable(symbol main.c ${SOURCES})


