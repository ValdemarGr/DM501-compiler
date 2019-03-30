#!/bin/bash

cmake ../CMakeLists.txt 
make -C .. compiler
rm ../CMakeCache.txt
rm ../cmake_install.cmake
rm ../CTestTestfile.cmake
rm ../CMakeFiles -r
mv ../src/compiler .
rm ../Makefile
