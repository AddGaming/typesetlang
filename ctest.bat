@echo off
cd src
echo --------- GCC ------------- 
gcc tests.c -Wall -Wextra -o test.exe
echo --------------------- 
test.exe 2> nul :: errors are exprected
rm test.exe
cd ..
