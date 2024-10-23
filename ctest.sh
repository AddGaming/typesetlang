#!/usr/bin/env bash

cd src
echo --------- GCC ------------- 
gcc tests.c -Wall -Wextra -o test.exe
echo --------------------- 
./test.exe 2> /dev/null # errors are exprected
rm test.exe
cd ..
