del zpipe.exe
del z:\pipato.exe
g++ -O3 -s zpipe.cpp libzpaq.cpp -o zpipe
rem zpipe -d <c:\zpaqfranz\sfx\zsfx2.zpaq >z:\pipato.exe
zpipe
rem zpipe >z:\pipato.exe
fc c:\zpaqfranz\sfx\zsfx.exe z:\pipato.exe /b
