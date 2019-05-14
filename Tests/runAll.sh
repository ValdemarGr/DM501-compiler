#!/bin/bash

for f in *.src; do
    echo "Processing $f"
    ./check.py ../cmake-build-debug/src/compiler ${f::-4}
done
