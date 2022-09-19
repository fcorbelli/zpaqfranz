CXX=g++
CXXFLAGS=-O3 

all: zpaqfranz

zpaqfranz: 
#	$(CXX) $(CXXFLAGS) -s -DHWBLAKE3 -DHWSHA1 blake3_windows_gnu.S zpaqfranz.cpp sha1ugo.obj -o $@ -pthread -static 
	$(CXX) $(CXXFLAGS) -s -DHWBLAKE3 blake3_windows_gnu.S zpaqfranz.cpp -o $@ -pthread -static 
        
clean:	
	rm -f zpaqfranz
