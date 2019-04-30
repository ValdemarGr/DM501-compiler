cd ../build-files
rm compiler
make compiler
./compiler < ./../kitty/simple.kitty > ./../asmsnippets/run.s
cd ./../asmsnippets
gcc -g -no-pie -m64 -o out run.s
./out
