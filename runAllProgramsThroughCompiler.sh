#!/bin/bash

compiler=./cmake-build-debug/src/compiler
output_directory=./build-kitty
directories=(kitty kimtests)

mkdir -p ${output_directory}

number_files=0
number_errors=0

function compile_file {
    local filepath="$1"
    local filename=$(basename -- "$filepath")
    local filename="${filename%.*}"
    local subdirectory="$2"

    local output_file="$output_directory/$subdirectory/$filename.s"
    error=$(${compiler} ${filepath} 2<&1 > ${output_file})
    status=$?

    if [[ ${status} -eq 139 ]]; then
        error="Segmentation fault (core dumped)"
    fi

    number_files=$((number_files + 1))

    if [[ -n "$error" ]]; then
        echo "Error in $filepath"
        echo -ne "\e[1m\e[31m"
        echo "$error"
        echo -ne "\e[0m"
        echo
        number_errors=$((number_errors + 1))
    fi
}

for dir in ${directories[*]}; do
    mkdir -p "$output_directory/$dir"
    for filename in ./${dir}/*.kitty ./${dir}/*.src; do
        if [[ -f "$filename" ]]; then
            compile_file ${filename} ${dir}
        fi
    done
done


echo "Compiled $number_files files. $number_errors errors"
