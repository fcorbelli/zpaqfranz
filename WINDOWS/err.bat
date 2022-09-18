@echo off  
del .\zpaqfranz.exe
del z:\1.zpaq
del z:\commit.zpaq
del z:\normale.zpaq
g++ -DANCIENT zpaqfranz.cpp -o zpaqfranz -Wunused-parameter -Wall -Wextra -pedantic  >1.txt 2>&1
