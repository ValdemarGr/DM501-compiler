cd ../build-files
rm compiler
make compiler
./compiler -x -mem=2000 < ./../kimtests/$1.src > ./../asmsnippets/run.s
cd ./../asmsnippets
gcc -g -no-pie -m64 -o out run.s
objcopy --only-keep-debug out out.debug
./out
