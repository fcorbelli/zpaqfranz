@echo off
del .\zpaqfranz.exe
g++ -O3 -DHWBLAKE3 blake3_windows_gnu.s zpaqfranz.cpp -o zpaqfranz -pthread -static
strip zpaqfranz.exe
