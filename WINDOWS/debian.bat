copy zpaqfranz.cpp zpaqfranz-debian.cpp
sed -i "/DEBIANSTART/,/\/\/\/DEBIANEND/d"  zpaqfranz-debian.cpp
sed -i "s/\/\/\/char extract_test1/char extract_test1/g" zpaqfranz-debian.cpp

sed -i "s/(\" __DATE__ \")/(\" Debian \")/g" zpaqfranz-debian.cpp

