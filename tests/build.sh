#!/bin/bash

cmake CMakeLists.txt
make all
rm Makefile
rm CMakeCache.txt
rm cmake_install.cmake
rm CMakeFiles -r
./tests

