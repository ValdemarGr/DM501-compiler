#!/usr/bin/env bash
cd ../build-files
rm compiler || true
make compiler
./compiler -mem=1000 < ./../kitty/$1.kitty > ./../asmsnippets/run.s
cd ./../asmsnippets
gcc -g -no-pie -m64 -o out run.s
objcopy --only-keep-debug out out.debug
./out
