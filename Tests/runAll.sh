#!/bin/bash

cd ../build-files
rm compiler
make compiler
mv compiler ../Tests
cd ../Tests
for f in *.src; do
    echo "Processing $f"
    ./check.py ./compiler ${f::-4}
done
