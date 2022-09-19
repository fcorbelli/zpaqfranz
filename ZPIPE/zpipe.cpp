/*  zpipe streaming file compressor v2.01

(C) 2010, Dell Inc.
    Written by Matt Mahoney, matmahoney@yahoo.com, Oct. 14, 2010.

    LICENSE

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    Visit <http://www.gnu.org/copyleft/gpl.html>.

To compress or decompress:   zpipe -option < input > output

Options are:
  -1 compress fastest (min.cfg)
  -2 compress average (mid.cfg)
  -3 compress smallest (max.cfg)
  -d decompress

Compressed output is in ZPAQ format as one segment within one
block. The segment has no filename, commment, or checksum. It is readable
by other ZPAQ compatible decompressors. -2 is equivalent to:

  zpaq nicmid.cfg output input

Decompression will accept ZPAQ compressed files from any source,
including embedded in other data, such as self extracting archives.
If the input archive contains more than one file, then all of the
output is concatenated. It does not verify checksums.

To compi
le:
g++ -O3 -s zpipe.cpp libzpaq.cpp -o zpipe


g++ -O2 -march=pentiumpro -fomit-frame-pointer -s zpipe.cpp libzpaq.cpp -o zpipe
To turn off assertion checking (faster), compile with -DNDEBUG

*/

#include <stdio.h>
#include <stdlib.h>
#ifndef unix
#include <fcntl.h> // for setmode(), requires g++
#endif

// libzpaq requires error() and concrete derivations of Reader and Writer
namespace libzpaq {
  void error(const char* msg) {
    fprintf(stderr, "zpipe error: %s\n", msg);
    exit(1);
  }
}
#include "libzpaq.h"

struct File: public libzpaq::Reader, public libzpaq::Writer {
  size_t posizione;
  FILE* f;
  File(FILE* f_): posizione(0),f(f_) {}
  int get() {
	  
	  posizione++;
	  return getc(f);
	  }
  void put(int c) {putc(c, f);}
};

// Print help message and exit
void usage() {
  fprintf(stderr, 
    "zpipe 2.01 file compressor %s\n"
    "(C) 2010, Dell Inc.\n"
    "Licensed under GPL v3. See http://www.gnu.org/copyleft/gpl.html\n"
    "\n"
    "Usage: zpipe -option < input > output\n"
    "Options are:\n"
    "  -1   compress fastest\n"
    "  -2   compress average\n"
    "  -3   compress smallest\n"
    "  -d   decompress\n", __DATE__);
  exit(1);
}
inline char *  migliaia(uint64_t n)
{
	static char retbuf[30];
	char *p = &retbuf[sizeof(retbuf)-1];
	unsigned int i = 0;
	*p = '\0';
	do 
	{
		if(i%3 == 0 && i != 0)
			*--p = '.';
		*--p = '0' + n % 10;
		n /= 10;
		i++;
		} while(n != 0);
	return p;
}
inline char *  migliaia2(uint64_t n)
{
	static char retbuf[30];
	char *p = &retbuf[sizeof(retbuf)-1];
	unsigned int i = 0;
	*p = '\0';
	do 
	{
		if(i%3 == 0 && i != 0)
			*--p = '.';
		*--p = '0' + n % 10;
		n /= 10;
		i++;
		} while(n != 0);
	return p;
}


struct Myresource: public libzpaq::Reader, public libzpaq::Writer {
	unsigned char*	thememory;
	size_t			memorysize;
	FILE* f;
	size_t			currentbyte;
	size_t			written;
	Myresource(unsigned char* i_memory,size_t i_memorysize,FILE* f_): thememory(i_memory),memorysize(i_memorysize),f(f_),currentbyte(0),written(0) {}
  
	int get()
	{
		int risultato=getc(f);
		printf("dato %08d  %d\n",currentbyte,risultato);
		currentbyte++;
		return risultato;
		
	}
	void put(int c) 
	{
		if(f)
		{
			putc(c, f);
			written++;
			printf("written %08d %d\n",written,c);
		}
	}
};


int main(int argc, char** argv) {

	FILE* infile=fopen("c:\\zpaqfranz\\sfx\\zsfx.zpaq","rb");
	FILE* outfile=fopen("z:\\pipato.exe","wb");

	unsigned char* zsfx_exe[110074];
	size_t zsfx_exe_len=110074;
	/*
	for (int i=0;i<110074;i++)
	{
		int k=getc(infile);
		if (k!=EOF)
			zsfx_exe[i]=12;//(unsigned char)k;
	}
	*/
	Myresource in	(( unsigned char *)zsfx_exe,zsfx_exe_len,infile);
	Myresource out	(( unsigned char *)zsfx_exe,zsfx_exe_len,outfile);
/*
	File in(infile);
	File out(outfile);
*/
	libzpaq::decompress(&in, &out);
	fclose(infile);
	fclose(outfile);
  return 0;
}
