@echo off
del .\zpaqfranz32.exe

rem attenzione a -flto

c:\mingw32\bin\g++ -m32 -Os  zsfx.cpp libzpaq.cpp -o zsfx32 -pthread -static -fno-rtti  -Wl,--gc-sections -flto
strip zsfx32.exe
