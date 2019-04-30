#!/bin/bash

./cmake-build-debug/src/Compiler -t $(pwd)/kitty/decl_test.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/functions_only.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/lots_of_nests.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/lambda.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/val.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/declarative_programming.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/class.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/inheritance.kitty
./cmake-build-debug/src/Compiler -t $(pwd)/kitty/list_monad.kitty
