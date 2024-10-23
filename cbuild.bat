@echo off
cd src
echo --------- GCC ------------- 
gcc main.c -Wall -Wextra -o main.exe
cd ..
echo --------------------------- 
mv src/main.exe main.exe
