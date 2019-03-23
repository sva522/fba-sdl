#!/bin/bash

CC=g++
CFLAGS="-O0 -g -Wall"
EXE=lz4-test

if [ ! -z "$1" ]; then
    CC=mipsel-linux-g++
    CFLAGS="-O3 -s -Wall"
    EXE=lz4.dge
    rm -f *.o
fi

if [ ! -f lz4_all.o ]; then
    $CC $CFLAGS -o lz4_all.o -c lz4_all.c
fi
rm -f main.o
$CC $CFLAGS -o main.o -c main.cpp
$CC -o $EXE main.o lz4_all.o
rm -f main.o
