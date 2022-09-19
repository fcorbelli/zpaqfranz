
@echo off

del .\zpaqfranz32.exe

c:\mingw32\bin\g++ -m32 -O3  zpaqfranz.cpp -o zpaqfranz32 -pthread -static
strip zpaqfranz32.exe

