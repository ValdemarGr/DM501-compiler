cmake_minimum_required(VERSION 2.8.9)

project(tests)

file(GLOB SRC
        "*.c"
        "*.h"
        )

file(GLOB SYMBOL_TESTS
        "symbol/*.c"
        "symbol/*.h"
        )

file(GLOB SYMBOL_SRC
        "../src/symbol/*.c"
        "../src/symbol/*.h"
        )

add_executable(symbol symbol/main.c ${SYMBOL_TESTS} ${SYMBOL_SRC} ${SRC})