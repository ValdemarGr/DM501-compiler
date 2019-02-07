SRC_DIR := ../../src/symbol
TEST_DIR := ..
THIS_DIR := .
TEST_FILES := $(wildcard $(TEST_DIR)/*.c) 
THESE_FILES := $(wildcard $(THIS_DIR)/*.c) 
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
FLAGS := -std=c11 -Wall -Wextra -pedantic
CC := gcc

default_target: all

all:
	$(CC) $(FLAGS) -o tests $(SRC_FILES) $(TEST_FILES) $(THESE_FILES) 

