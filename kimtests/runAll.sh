#!/bin/bash

cd ../build-files
rm compiler
make compiler
mv compiler ../kimtests
cd ../kimtests
for f in *.src; do
    echo "Processing $f"
    ./check.py ./compiler ${f::-4}
done
