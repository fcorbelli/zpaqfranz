@echo off
del .\zsfx.exe
g++ -Os zsfx.cpp libzpaq.cpp   -o zsfx -static -fno-rtti -Wl,--gc-sections
strip zsfx.exe

