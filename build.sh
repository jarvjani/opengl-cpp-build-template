#!/bin/bash
set -e # disable shell exception handling, stops execution on error. 
gcc -g3 -O0 build/build.c -o build/build 
./build/build
