#!/bin/bash

cd ../build-files
rm compiler
make compiler
mv compiler ../Tests
cd ../Tests
