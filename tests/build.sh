#!/bin/bash

cmake CMakeLists.txt -B ./build
cd build
make all
./tests
cd ..
