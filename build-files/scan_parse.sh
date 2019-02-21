#!/bin/bash

cmake ../CMakeLists.txt 
make -C .. PrettyPrint
rm ../CMakeCache.txt
rm ../cmake_install.cmake
rm ../CTestTestfile.cmake
rm ../CMakeFiles -r
mv ../src/scan_parse/PrettyPrint .
rm ../Makefile
