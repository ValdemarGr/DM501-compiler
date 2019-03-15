#!/bin/bash

./cmake-build-debug/src/Compiler \
	-t \
	$(pwd)/kitty/decl_test.kitty \
	$(pwd)/kitty/functions_only.kitty \
	$(pwd)/kitty/lots_of_nests.kitty \
	$(pwd)/kitty/lambda.kitty \
	$(pwd)/kitty/val.kitty \
	$(pwd)/kitty/declarative_programming.kitty \
	$(pwd)/kitty/class.kitty \
	$(pwd)/kitty/inheritance.kitty \
	$(pwd)/kitty/list_monad.kitty
