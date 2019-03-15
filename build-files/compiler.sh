#!/bin/bash

cmake ../CMakeLists.txt 
make -C .. Compiler
rm ../CMakeCache.txt
rm ../cmake_install.cmake
rm ../CTestTestfile.cmake
rm ../CMakeFiles -r
mv ../src/Compiler .
rm ../Makefile
