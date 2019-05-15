#!/bin/bash

cd ../build-files
rm compiler
make compiler
mv compiler ../kimtests
cd ../kimtests
