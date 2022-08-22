#!/bin/bash
set -e
export OMP_NUM_THREADS=8
gcc -g3 -O0 build/build.c -o build/build
./build/build
