#!/usr/bin/env bash

compiler=./cmake-build-debug/src/compiler
output_directory_asm=./build-kitty
output_directory_bin=./build-kitty

mkdir -p ${output_directory_asm}
mkdir -p ${output_directory_bin}

function compile_kitty_file {
    local filepath="$1"
    local filename=$(basename -- "$filepath")
    local filename="${filename%.*}"
    local subdirectory="$2"

    local output_file="$output_directory_asm/$subdirectory/$filename.s"
    mkdir -p "$output_directory_asm/$subdirectory"

    error=$(${compiler} ${filepath} 2<&1 > ${output_file})
    status=$?

    if [[ ${status} -eq 139 ]]; then
        error="Segmentation fault (core dumped)"
    fi

    if [[ ${status} -ne 0 ]]; then
        echo -ne "\e[1m\e[31m"
        echo -e "$error"
        echo -ne "\e[0m"
        echo
        return 1
    fi


    (>&2 echo -ne "\e[2m")
    (>&2 echo -e "$error")
    (>&2 echo -ne "\e[0m")
    echo "$output_file"
    return 0
}

function compile_asm {
    local filepath="$1"
    local filename=$(basename -- "$filepath")
    local filename="${filename%.*}"
    local subdirectory="$2"

    local output_file="$output_directory_bin/$subdirectory/$filename"
    mkdir -p "$output_directory_bin/$subdirectory"

    error=$(gcc -g -no-pie -m64 -o ${output_file} ${filepath})
    status=$?

    if [[ ${status} -ne 0 ]]; then
        echo error
        return 1
    fi

    echo "$output_file"
    return 0
}

full_filepath="$1"
directory="$(dirname ${full_filepath//.\/})"

asm_file=$(compile_kitty_file "$full_filepath" "$directory")
ret="$?"
if [[ ${ret} -eq 0 ]]; then
    echo "Compiled to assembly. $asm_file"
    binary=$(compile_asm "$asm_file" "$directory")
    ret="$?"

    if [[ ${ret} -eq 0 ]]; then
        echo "Compiled to binary. $binary"
        echo "---"
        ${binary}
        ret=$?
        if [[ ${ret} -ne 0 ]]; then
            echo -ne "\e[1m\e[31m"
            echo -e "Failed to run program see output above"
            echo "$binary"
            echo -ne "\e[0m"
        fi
    else
        echo "Failed to compile assembly to binary"
        echo -e "$asm_file"
    fi
else
    echo "Failed to compile Kitty to assembly"
    echo -e "$asm_file"
fi

