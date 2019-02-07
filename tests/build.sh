#!/bin/bash

cmake CMakeLists.txt
make all
rm CMakeCache.txt
rm cmake_install.cmake
rm CMakeFiles -r
./tests

