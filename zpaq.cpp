// zpaq.cpp - Journaling incremental deduplicating archiver

///////// EXPERIMENTAL BUILD
///////// The source is a mess, and without *nix ifdef
///////// Strongly in development

#define ZPAQ_VERSION "50.18-experimental"
#define FRANZOFFSET 50

/*
This is zpaqfranz, a patched  but (maybe) compatible fork of 
Matt Mahoney's ZPAQ version 7.15 (http://mattmahoney.net/dc/zpaq.html)

Provided as-is, with no warranty whatsoever, by
Franco Corbelli, franco@francocorbelli.com

NOTE1: -, not -- (into switch)
NOTE2: switches ARE case sensitive.   -maxsize <> -MAXSIZE

Launch magics

zpaqfranz help
zpaqfranz -h
zpaqfranz -help
  long (full) help
  
zpaqfranz -he
zpaqfranz --helpe
zpaqfranz -examples
  for some examples

zpaqfranz -diff 
  brief difference agains standard 7.15



============
Targets

Windows 64 (g++ 7.3.0 64 bit)
g++ -O3 zpaq.cpp libzpaq.cpp -o zpaqfranz -static 

Windows 32 (g++ 7.3.0 64 bit)
c:\mingw32\bin\g++  -O3 -m32 zpaq.cpp libzpaq.cpp -o zpaqfranz32 -static -pthread

FreeBSD (11.x) gcc 7
gcc7 -O3 -march=native -Dunix zpaq.cpp -static -lstdc++ libzpaq.cpp -pthread -o zpaq -static -lm

FreeBSD (12.1) gcc 9.3.0
g++ -O3 -march=native -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaqfranz -static-libstdc++ -static-libgcc

FreeBSD (11.4) gcc 10.2.0
g++ -O3 -march=native -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaqfranz -static-libstdc++ -static-libgcc -Wno-stringop-overflow

FreeBSD (11.3) clang 6.0.0
clang++ -march=native -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaqfranz -static

Debian Linux (10/11) gcc 8.3.0
g++ -O3 -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaqfranz -static

*/

/*
  This software is provided as-is, with no warranty.
  I, Matt Mahoney, release this software into
  the public domain.   This applies worldwide.
  In some countries this may not be legally possible; if so:
  I grant anyone the right to use this software for any purpose,
  without any conditions, unless such conditions are required by law.

zpaq is a journaling (append-only) archiver for incremental backups.
Files are added only when the last-modified date has changed. Both the old
and new versions are saved. You can extract from old versions of the
archive by specifying a date or version number. zpaq supports 5
compression levels, deduplication, AES-256 encryption, and multi-threading
using an open, self-describing format for backward and forward
compatibility in Windows and Linux. See zpaq.pod for usage.

TO COMPILE:

This program needs libzpaq from http://mattmahoney.net/zpaq/
Recommended compile for Windows with MinGW:

  g++ -O3 zpaq.cpp libzpaq.cpp -o zpaq

With Visual C++:

  cl /O2 /EHsc zpaq.cpp libzpaq.cpp advapi32.lib

For Linux:

  g++ -O3 -Dunix zpaq.cpp libzpaq.cpp -pthread -o zpaq

For BSD or OS/X

  g++ -O3 -Dunix -DBSD zpaq.cpp libzpaq.cpp -pthread -o zpaq

Possible options:

  -DDEBUG    Enable run time checks and help screen for undocumented options.
  -DNOJIT    Don't assume x86 with SSE2 for libzpaq. Slower (disables JIT).
  -Dunix     Not Windows. Sometimes automatic in Linux. Needed for Mac OS/X.
  -DBSD      For BSD or OS/X.
  -DPTHREAD  Use Pthreads instead of Windows threads. Requires pthreadGC2.dll
             or pthreadVC2.dll from http://sourceware.org/pthreads-win32/
  -Dunixtest To make -Dunix work in Windows with MinGW.
  -fopenmp   Parallel divsufsort (faster, implies -pthread, broken in MinGW).
  -pthread   Required in Linux, implied by -fopenmp.
  -O3 or /O2 Optimize (faster).
  -o         Name of output executable.
  /EHsc      Enable exception handing in VC++ (required).
  advapi32.lib  Required for libzpaq in VC++.

*/

/* unzpaq206.cpp - ZPAQ level 2 reference decoder version 2.06.

  This software is provided as-is, with no warranty.
  I, Matt Mahoney, release this software into
  the public domain.   This applies worldwide.
  In some countries this may not be legally possible; if so:
  I grant anyone the right to use this software for any purpose,
  without any conditions, unless such conditions are required by law.
*/

  
/*
Codes from other authors are used.
As far as I know of free use in opensource, 
otherwise I apologize.

Include mod by data man and reg2s patch from encode.su forum

xxhash64.h Copyright (c) 2016 Stephan Brumme. All rights reserved.
Crc32.h    Copyright (c) 2011-2019 Stephan Brumme. All rights reserved, slicing-by-16 contributed by Bulat Ziganshin
crc32c.c  C opyright (C) 2013 Mark Adler

*/


#define _FILE_OFFSET_BITS 64  // In Linux make sizeof(off_t) == 8
#ifndef UNICODE
#define UNICODE  // For Windows
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include "libzpaq.h"
/*
#include <atomic>
*/
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78
#define CRC32_TEST_BITWISE
#define CRC32_TEST_HALFBYTE
#define CRC32_TEST_TABLELESS

#ifndef DEBUG
#define NDEBUG 1
#endif
#include <assert.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#ifndef unix
#define unix 1
#endif
#endif
#ifdef unix
#define PTHREAD 1
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <utime.h>
#include <errno.h>
#ifdef BSD
#include <termios.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <libutil.h>
#include <sys/statvfs.h>
#endif

#else  // Assume Windows
#include <conio.h>
#include <windows.h>
#include <io.h>
#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>
using namespace std;

#endif

// For testing -Dunix in Windows
#ifdef unixtest
#define lstat(a,b) stat(a,b)
#define mkdir(a,b) mkdir(a)
#ifndef fseeko
#define fseeko(a,b,c) fseeko64(a,b,c)
#endif
#ifndef ftello
#define ftello(a) ftello64(a)
#endif
#endif


using std::string;
using std::vector;
using std::map;
using std::min;
using std::max;
using libzpaq::StringBuffer;

int unz(const char * archive,const char * key,bool all); // paranoid unzpaq 2.06

string g_exec_error;
string g_exec_ok;
string g_exec_text;




enum ealgoritmi		{ ALGO_SIZE,ALGO_SHA1,ALGO_CRC32C,ALGO_CRC32,ALGO_XXHASH,ALGO_SHA256,ALGO_LAST };
typedef std::map<int,std::string> algoritmi;
const algoritmi::value_type rawData[] = {
   algoritmi::value_type(ALGO_SIZE,"Size"),
   algoritmi::value_type(ALGO_SHA1,"SHA1"),
   algoritmi::value_type(ALGO_CRC32,"CRC32"),
   algoritmi::value_type(ALGO_CRC32C,"CRC-32C"),
   algoritmi::value_type(ALGO_XXHASH,"xxHASH64"),
};
const int numElems = sizeof rawData / sizeof rawData[0];
algoritmi myalgoritmi(rawData, rawData + numElems);


// Handle errors in libzpaq and elsewhere
void libzpaq::error(const char* msg) {
	g_exec_text=msg;
  if (strstr(msg, "ut of memory")) throw std::bad_alloc();
  throw std::runtime_error(msg);
}
using libzpaq::error;

// Portable thread types and functions for Windows and Linux. Use like this:
//
// // Create mutex for locking thread-unsafe code
// Mutex mutex;            // shared by all threads
// init_mutex(mutex);      // initialize in unlocked state
// Semaphore sem(n);       // n >= 0 is initial state
//
// // Declare a thread function
// ThreadReturn thread(void *arg) {  // arg points to in/out parameters
//   lock(mutex);          // wait if another thread has it first
//   release(mutex);       // allow another waiting thread to continue
//   sem.wait();           // wait until n>0, then --n
//   sem.signal();         // ++n to allow waiting threads to continue
//   return 0;             // must return 0 to exit thread
// }
//
// // Start a thread
// ThreadID tid;
// run(tid, thread, &arg); // runs in parallel
// join(tid);              // wait for thread to return
// destroy_mutex(mutex);   // deallocate resources used by mutex
// sem.destroy();          // deallocate resources used by semaphore

#ifdef PTHREAD
#include <pthread.h>
typedef void* ThreadReturn;                                // job return type
typedef pthread_t ThreadID;                                // job ID type
void run(ThreadID& tid, ThreadReturn(*f)(void*), void* arg)// start job
  {pthread_create(&tid, NULL, f, arg);}
void join(ThreadID tid) {pthread_join(tid, NULL);}         // wait for job
typedef pthread_mutex_t Mutex;                             // mutex type
void init_mutex(Mutex& m) {pthread_mutex_init(&m, 0);}     // init mutex
void lock(Mutex& m) {pthread_mutex_lock(&m);}              // wait for mutex
void release(Mutex& m) {pthread_mutex_unlock(&m);}         // release mutex
void destroy_mutex(Mutex& m) {pthread_mutex_destroy(&m);}  // destroy mutex

class Semaphore {
public:
  Semaphore() {sem=-1;}
  void init(int n) {
    assert(n>=0);
    assert(sem==-1);
    pthread_cond_init(&cv, 0);
    pthread_mutex_init(&mutex, 0);
    sem=n;
  }
  void destroy() {
    assert(sem>=0);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cv);
  }
  int wait() {
    assert(sem>=0);
    pthread_mutex_lock(&mutex);
    int r=0;
    if (sem==0) r=pthread_cond_wait(&cv, &mutex);
    assert(sem>0);
    --sem;
    pthread_mutex_unlock(&mutex);
    return r;
  }
  void signal() {
    assert(sem>=0);
    pthread_mutex_lock(&mutex);
    ++sem;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mutex);
  }
private:
  pthread_cond_t cv;  // to signal FINISHED
  pthread_mutex_t mutex; // protects cv
  int sem;  // semaphore count
};

#else  // Windows
typedef DWORD ThreadReturn;
typedef HANDLE ThreadID;
void run(ThreadID& tid, ThreadReturn(*f)(void*), void* arg) {
  tid=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f, arg, 0, NULL);
  if (tid==NULL) error("CreateThread failed");
}
void join(ThreadID& tid) {WaitForSingleObject(tid, INFINITE);}
typedef HANDLE Mutex;
void init_mutex(Mutex& m) {m=CreateMutex(NULL, FALSE, NULL);}
void lock(Mutex& m) {WaitForSingleObject(m, INFINITE);}
void release(Mutex& m) {ReleaseMutex(m);}
void destroy_mutex(Mutex& m) {CloseHandle(m);}

class Semaphore {
public:
  enum {MAXCOUNT=2000000000};
  Semaphore(): h(NULL) {}
  void init(int n) {assert(!h); h=CreateSemaphore(NULL, n, MAXCOUNT, NULL);}
  void destroy() {assert(h); CloseHandle(h);}
  int wait() {assert(h); return WaitForSingleObject(h, INFINITE);}
  void signal() {assert(h); ReleaseSemaphore(h, 1, NULL);}
private:
  HANDLE h;  // Windows semaphore
};

#endif



// Global variables
int64_t global_start=0;  	// set to mtime() at start of main()
bool flagnoeta;					// -noeta (do not show ETA, for batch file)
bool flagpakka;					// different output
bool flagvss;					// make VSS on Windows with admin rights
bool flagverbose;             	// show more
bool flagverify;			// read from filesystem
  
int64_t g_dimensione=0;
int64_t g_scritti=0;// note: not thread safe, but who care?
int64_t g_zerotime=0;

int64_t g_bytescanned=0;
int64_t g_filescanned=0;
int64_t g_worked=0;


/*
atomic_int64_t g_bytescanned;
atomic_int64_t g_filescanned;
*/

	
  
// In Windows, convert 16-bit wide string to UTF-8 and \ to /
#ifdef _WIN32
bool windows7_or_above=false; //windows version (for using FindFirstFileExW)

string wtou(const wchar_t* s) {
  assert(sizeof(wchar_t)==2);  // Not true in Linux
  assert((wchar_t)(-1)==65535);
  string r;
  if (!s) return r;
  for (; *s; ++s) {
    if (*s=='\\') r+='/';
    else if (*s<128) r+=*s;
    else if (*s<2048) r+=192+*s/64, r+=128+*s%64;
    else r+=224+*s/4096, r+=128+*s/64%64, r+=128+*s%64;
  }
  return r;
}

// In Windows, convert UTF-8 string to wide string ignoring
// invalid UTF-8 or >64K. Convert "/" to slash (default "\").
std::wstring utow(const char* ss, char slash='\\') {
  assert(sizeof(wchar_t)==2);
  assert((wchar_t)(-1)==65535);
  std::wstring r;
  if (!ss) return r;
  const unsigned char* s=(const unsigned char*)ss;
  for (; s && *s; ++s) {
    if (s[0]=='/') r+=slash;
    else if (s[0]<128) r+=s[0];
    else if (s[0]>=192 && s[0]<224 && s[1]>=128 && s[1]<192)
      r+=(s[0]-192)*64+s[1]-128, ++s;
    else if (s[0]>=224 && s[0]<240 && s[1]>=128 && s[1]<192
             && s[2]>=128 && s[2]<192)
      r+=(s[0]-224)*4096+(s[1]-128)*64+s[2]-128, s+=2;
  }
  return r;
}
#endif



/////////////	case insensitive compare

char* stristr( const char* str1, const char* str2 )
{
    const char* p1 = str1;
    const char* p2 = str2;
    const char* r = *p2 == 0 ? str1 : 0 ;

    while( *p1 != 0 && *p2 != 0 )
    {
        if( tolower( (unsigned char)*p1 ) == tolower( (unsigned char)*p2 ) )
        {
            if( r == 0 )
                r = p1 ;
            p2++ ;
        }
        else
        {
            p2 = str2 ;
            if( r != 0 )
                p1 = r + 1 ;

            if( tolower( (unsigned char)*p1 ) == tolower( (unsigned char)*p2 ) )
            {
                r = p1 ;
                p2++ ;
            }
            else
                r = 0 ;
        }

        p1++ ;
    }

    return *p2 == 0 ? (char*)r : 0 ;
}
bool isdirectory(string i_filename)
{
	if (i_filename.length()==0)
		return false;
	else
	return
		i_filename[i_filename.size()-1]=='/';
	
}
bool isextension(const char* i_filename,const char* i_ext)
{
	bool risultato=false;
	if (!i_filename)
		return false;
	if (!i_ext)
		return false;
	if (isdirectory(i_filename))
		return false;

	char * posizione=stristr(i_filename, i_ext);
	if (!posizione)
		return false;
	/*
	printf("Strlen filename %d\n",strlen(i_filename));
	printf("Strlen i_ext    %d\n",strlen(i_ext));
	printf("Delta  %d\n",posizione-i_filename);
	*/
	return (posizione-i_filename)+strlen(i_ext)==strlen(i_filename);
}
bool isxls(string i_filename)
{
	return isextension(i_filename.c_str(), ".xls");
}
bool isads(string i_filename)
{
	if (i_filename.length()==0)
		return false;
	else
		return strstr(i_filename.c_str(), ":$DATA")!=0;  
}
bool iszfs(string i_filename)
{
	if (i_filename.length()==0)
		return false;
	else
		return strstr(i_filename.c_str(), ".zfs")!=0;  
}


// Print a UTF-8 string to f (stdout, stderr) so it displays properly
void printUTF8(const char* s, FILE* f=stdout) {
  assert(f);
  assert(s);
#ifdef unix
  fprintf(f, "%s", s);
#else
  const HANDLE h=(HANDLE)_get_osfhandle(_fileno(f));
  DWORD ft=GetFileType(h);
  if (ft==FILE_TYPE_CHAR) {
    fflush(f);
    std::wstring w=utow(s, '/');  // Windows console: convert to UTF-16
    DWORD n=0;
    WriteConsole(h, w.c_str(), w.size(), &n, 0);
  }
  else  // stdout redirected to file
    fprintf(f, "%s", s);
#endif
}


// Return relative time in milliseconds
int64_t mtime() {
#ifdef unix
  timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec*1000LL+tv.tv_usec/1000;
#else
  int64_t t=GetTickCount();
  if (t<global_start) t+=0x100000000LL;
  return t;
#endif
}

// Convert 64 bit decimal YYYYMMDDHHMMSS to "YYYY-MM-DD HH:MM:SS"
// where -1 = unknown date, 0 = deleted.
string dateToString(int64_t date) {
  if (date<=0) return "                   ";
  string s="0000-00-00 00:00:00";
  static const int t[]={18,17,15,14,12,11,9,8,6,5,3,2,1,0};
  for (int i=0; i<14; ++i) s[t[i]]+=int(date%10), date/=10;
  return s;
}

// Convert attributes to a readable format
string attrToString(int64_t attrib) {
  string r="     ";
  if ((attrib&255)=='u') {
    r[0]="0pc3d5b7 9lBsDEF"[(attrib>>20)&15];
    for (int i=0; i<4; ++i)
      r[4-i]=(attrib>>(8+3*i))%8+'0';
  }
  else if ((attrib&255)=='w') {
    for (int i=0, j=0; i<32; ++i) {
      if ((attrib>>(i+8))&1) {
        char c="RHS DAdFTprCoIEivs89012345678901"[i];
        if (j<5) r[j]=c;
        else r+=c;
        ++j;
      }
    }
  }
  return r;
}

// Convert seconds since 0000 1/1/1970 to 64 bit decimal YYYYMMDDHHMMSS
// Valid from 1970 to 2099.
int64_t decimal_time(time_t tt) {
  if (tt==-1) tt=0;
  int64_t t=(sizeof(tt)==4) ? unsigned(tt) : tt;
  const int second=t%60;
  const int minute=t/60%60;
  const int hour=t/3600%24;
  t/=86400;  // days since Jan 1 1970
  const int term=t/1461;  // 4 year terms since 1970
  t%=1461;
  t+=(t>=59);  // insert Feb 29 on non leap years
  t+=(t>=425);
  t+=(t>=1157);
  const int year=term*4+t/366+1970;  // actual year
  t%=366;
  t+=(t>=60)*2;  // make Feb. 31 days
  t+=(t>=123);   // insert Apr 31
  t+=(t>=185);   // insert June 31
  t+=(t>=278);   // insert Sept 31
  t+=(t>=340);   // insert Nov 31
  const int month=t/31+1;
  const int day=t%31+1;
  return year*10000000000LL+month*100000000+day*1000000
         +hour*10000+minute*100+second;
}

// Convert decimal date to time_t - inverse of decimal_time()
time_t unix_time(int64_t date) {
  if (date<=0) return -1;
  static const int days[12]={0,31,59,90,120,151,181,212,243,273,304,334};
  const int year=date/10000000000LL%10000;
  const int month=(date/100000000%100-1)%12;
  const int day=date/1000000%100;
  const int hour=date/10000%100;
  const int min=date/100%100;
  const int sec=date%100;
  return (day-1+days[month]+(year%4==0 && month>1)+((year-1970)*1461+1)/4)
    *86400+hour*3600+min*60+sec;
}








/////////////////////////////// File //////////////////////////////////

// Windows/Linux compatible file type
#ifdef unix
typedef FILE* FP;
const FP FPNULL=NULL;
const char* const RB="rb";
const char* const WB="wb";
const char* const RBPLUS="rb+";
const char* const WBPLUS="wb+";

#else // Windows
typedef HANDLE FP;
const FP FPNULL=INVALID_HANDLE_VALUE;
typedef enum {RB, WB, RBPLUS, WBPLUS} MODE;  // fopen modes

// Few lines from https://gist.github.com/nu774/10381075, and few from milo1012 TC zpaq plugin.
std::wstring GetFullPathNameX(const std::wstring &path)
{
    DWORD length = GetFullPathNameW(path.c_str(), 0, 0, 0);
    std::vector<wchar_t> buffer(length);
    length = GetFullPathNameW(path.c_str(), buffer.size(), &buffer[0], 0);
    return std::wstring(&buffer[0], &buffer[length]);
}

// Code from https://gist.github.com/nu774/10381075 and Milo's ея zpaq plugin.
std::wstring make_long_path(const char* filename) {

  if (strlen(filename) > 2 && filename[0] == '/' && filename[1] == '/' && filename[2] == '?')
    return utow(filename);
  else {
    std::wstring ws =  L"\\\\?\\" + GetFullPathNameX(utow(filename));
    //wprintf(L"\nCONV: (%d) %ls\n\n", ws.length(), ws.c_str());
    return ws;
  }
}

	

// Open file. Only modes "rb", "wb", "rb+" and "wb+" are supported.
FP fopen(const char* filename, MODE mode) {
  assert(filename);

  DWORD access=0;
  if (mode!=WB) access=GENERIC_READ;
  if (mode!=RB) access|=GENERIC_WRITE;
  DWORD disp=OPEN_ALWAYS;  // wb or wb+
  if (mode==RB || mode==RBPLUS) disp=OPEN_EXISTING;
  DWORD share=FILE_SHARE_READ;
  if (mode==RB) share|=FILE_SHARE_WRITE|FILE_SHARE_DELETE;
  return CreateFile(utow(filename).c_str(), access, share,
                    NULL, disp, FILE_ATTRIBUTE_NORMAL, NULL);
}


// Close file
int fclose(FP fp) {
  return CloseHandle(fp) ? 0 : EOF;
}

// Read nobj objects of size size into ptr. Return number of objects read.
size_t fread(void* ptr, size_t size, size_t nobj, FP fp) {
  DWORD r=0;
  ReadFile(fp, ptr, size*nobj, &r, NULL);
  if (size>1) r/=size;
  return r;
}

// Write nobj objects of size size from ptr to fp. Return number written.
size_t fwrite(const void* ptr, size_t size, size_t nobj, FP fp) {
  DWORD r=0;
  WriteFile(fp, ptr, size*nobj, &r, NULL);
  if (size>1) r/=size;
  return r;
}

// Move file pointer by offset. origin is SEEK_SET (from start), SEEK_CUR,
// (from current position), or SEEK_END (from end).
int fseeko(FP fp, int64_t offset, int origin) {
  if (origin==SEEK_SET) origin=FILE_BEGIN;
  else if (origin==SEEK_CUR) origin=FILE_CURRENT;
  else if (origin==SEEK_END) origin=FILE_END;
  LONG h=uint64_t(offset)>>32;
  SetFilePointer(fp, offset&0xffffffffull, &h, origin);
  return GetLastError()!=NO_ERROR;
}

// Get file position
int64_t ftello(FP fp) {
  LONG h=0;
  DWORD r=SetFilePointer(fp, 0, &h, FILE_CURRENT);
  return r+(uint64_t(h)<<32);
}

#endif


/// We want to read the damn UTF8 files. 
/// In the case of Windows using the function for UTF16, after conversion

FILE* freadopen(const char* i_filename)
{
#ifdef _WIN32
	wstring widename=utow(i_filename);
	FILE* myfp=_wfopen(widename.c_str(), L"rb" );
#else
	FILE* myfp=fopen(i_filename, "rb" );
#endif
	if (myfp==NULL)
	{
		printf( "\nfreadopen cannot open:");
		printUTF8(i_filename);
		printf("\n");
	}
	return myfp;
}


/////////////////// calculate CRC32 

// //////////////////////////////////////////////////////////
// Crc32.h
// Copyright (c) 2011-2019 Stephan Brumme. All rights reserved.
// Slicing-by-16 contributed by Bulat Ziganshin
// Tableless bytewise CRC contributed by Hagai Gold
// see http://create.stephan-brumme.com/disclaimer.html
//

// if running on an embedded system, you might consider shrinking the
// big Crc32Lookup table by undefining these lines:
#define CRC32_USE_LOOKUP_TABLE_BYTE
#define CRC32_USE_LOOKUP_TABLE_SLICING_BY_4
#define CRC32_USE_LOOKUP_TABLE_SLICING_BY_8
#define CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
// - crc32_bitwise  doesn't need it at all
// - crc32_halfbyte has its own small lookup table
// - crc32_1byte_tableless and crc32_1byte_tableless2 don't need it at all
// - crc32_1byte    needs only Crc32Lookup[0]
// - crc32_4bytes   needs only Crc32Lookup[0..3]
// - crc32_8bytes   needs only Crc32Lookup[0..7]
// - crc32_4x8bytes needs only Crc32Lookup[0..7]
// - crc32_16bytes  needs all of Crc32Lookup
// using the aforementioned #defines the table is automatically fitted to your needs

// uint8_t, uint32_t, int32_t
#include <stdint.h>
// size_t
#include <cstddef>


/// merge two CRC32 such that result = crc32(dataB, lengthB, crc32(dataA, lengthA))
uint32_t crc32_combine (uint32_t crcA, uint32_t crcB, size_t lengthB);

/// compute CRC32 (bitwise algorithm)
uint32_t crc32_bitwise (const void* data, size_t length, uint32_t previousCrc32 = 0);
/// compute CRC32 (half-byte algoritm)
uint32_t crc32_halfbyte(const void* data, size_t length, uint32_t previousCrc32 = 0);

#ifdef CRC32_USE_LOOKUP_TABLE_BYTE
/// compute CRC32 (standard algorithm)
uint32_t crc32_1byte   (const void* data, size_t length, uint32_t previousCrc32 = 0);
#endif

/// compute CRC32 (byte algorithm) without lookup tables
uint32_t crc32_1byte_tableless (const void* data, size_t length, uint32_t previousCrc32 = 0);
/// compute CRC32 (byte algorithm) without lookup tables
uint32_t crc32_1byte_tableless2(const void* data, size_t length, uint32_t previousCrc32 = 0);

#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_4
/// compute CRC32 (Slicing-by-4 algorithm)
uint32_t crc32_4bytes  (const void* data, size_t length, uint32_t previousCrc32 = 0);
#endif

#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_8
/// compute CRC32 (Slicing-by-8 algorithm)
uint32_t crc32_8bytes  (const void* data, size_t length, uint32_t previousCrc32 = 0);
/// compute CRC32 (Slicing-by-8 algorithm), unroll inner loop 4 times
uint32_t crc32_4x8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0);
#endif

#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
/// compute CRC32 (Slicing-by-16 algorithm)
uint32_t crc32_16bytes (const void* data, size_t length, uint32_t previousCrc32 = 0);
/// compute CRC32 (Slicing-by-16 algorithm, prefetch upcoming data blocks)
uint32_t crc32_16bytes_prefetch(const void* data, size_t length, uint32_t previousCrc32 = 0, size_t prefetchAhead = 256);
#endif



// //////////////////////////////////////////////////////////
// Crc32.cpp
// Copyright (c) 2011-2019 Stephan Brumme. All rights reserved.
// Slicing-by-16 contributed by Bulat Ziganshin
// Tableless bytewise CRC contributed by Hagai Gold
// see http://create.stephan-brumme.com/disclaimer.html
//

// if running on an embedded system, you might consider shrinking the
// big Crc32Lookup table:
// - crc32_bitwise  doesn't need it at all
// - crc32_halfbyte has its own small lookup table
// - crc32_1byte    needs only Crc32Lookup[0]
// - crc32_4bytes   needs only Crc32Lookup[0..3]
// - crc32_8bytes   needs only Crc32Lookup[0..7]
// - crc32_4x8bytes needs only Crc32Lookup[0..7]
// - crc32_16bytes  needs all of Crc32Lookup



#ifndef __LITTLE_ENDIAN
  #define __LITTLE_ENDIAN 1234
#endif
#ifndef __BIG_ENDIAN
  #define __BIG_ENDIAN    4321
#endif

// define endianess and some integer data types
#if defined(_MSC_VER) || defined(__MINGW32__)
  // Windows always little endian
  #define __BYTE_ORDER __LITTLE_ENDIAN

  // intrinsics / prefetching
  #include <xmmintrin.h>
  #ifdef __MINGW32__
    #define PREFETCH(location) __builtin_prefetch(location)
  #else
    #define PREFETCH(location) _mm_prefetch(location, _MM_HINT_T0)
  #endif
#else
  // defines __BYTE_ORDER as __LITTLE_ENDIAN or __BIG_ENDIAN
  #include <sys/param.h>

  // intrinsics / prefetching
  #ifdef __GNUC__
    #define PREFETCH(location) __builtin_prefetch(location)
  #else
    // no prefetching
    #define PREFETCH(location) ;
  #endif
#endif

/*
// abort if byte order is undefined
#if !defined(__BYTE_ORDER)
#error undefined byte order, compile with -D__BYTE_ORDER=1234 (if little endian) or -D__BYTE_ORDER=4321 (big endian)
#endif
*/

#define __BYTE_ORDER __LITTLE_ENDIAN


namespace
{
  /// zlib's CRC32 polynomial
  const uint32_t Polynomial = 0xEDB88320;
//esx
  /// swap endianess
  static inline uint32_t swap(uint32_t x)
  {
  #if defined(__GNUC__) || defined(__clang__) && !defined(ESX)
    return __builtin_bswap32(x);
  #else
    return (x >> 24) |
          ((x >>  8) & 0x0000FF00) |
          ((x <<  8) & 0x00FF0000) |
           (x << 24);
  #endif
  }

  /// Slicing-By-16
  #ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
  const size_t MaxSlice = 16;
  #elif defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_8)
  const size_t MaxSlice = 8;
  #elif defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_4)
  const size_t MaxSlice = 4;
  #elif defined(CRC32_USE_LOOKUP_TABLE_BYTE)
  const size_t MaxSlice = 1;
  #else
    #define NO_LUT // don't need Crc32Lookup at all
  #endif

} // anonymous namespace

#ifndef NO_LUT
/// forward declaration, table is at the end of this file
extern const uint32_t Crc32Lookup[MaxSlice][256]; // extern is needed to keep compiler happy
#endif


/// compute CRC32 (bitwise algorithm)
uint32_t crc32_bitwise(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  while (length-- != 0)
  {
    crc ^= *current++;

    for (int j = 0; j < 8; j++)
    {
      // branch-free
      crc = (crc >> 1) ^ (-int32_t(crc & 1) & Polynomial);

      // branching, much slower:
      //if (crc & 1)
      //  crc = (crc >> 1) ^ Polynomial;
      //else
      //  crc =  crc >> 1;
    }
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (half-byte algoritm)
uint32_t crc32_halfbyte(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  /// look-up table for half-byte, same as crc32Lookup[0][16*i]
  static const uint32_t Crc32Lookup16[16] =
  {
    0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,
    0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C
  };

  while (length-- != 0)
  {
    crc = Crc32Lookup16[(crc ^  *current      ) & 0x0F] ^ (crc >> 4);
    crc = Crc32Lookup16[(crc ^ (*current >> 4)) & 0x0F] ^ (crc >> 4);
    current++;
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


#ifdef CRC32_USE_LOOKUP_TABLE_BYTE
/// compute CRC32 (standard algorithm)
uint32_t crc32_1byte(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  while (length-- != 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *current++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}
#endif


/// compute CRC32 (byte algorithm) without lookup tables
uint32_t crc32_1byte_tableless(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint8_t* current = (const uint8_t*) data;

  while (length-- != 0)
  {
    uint8_t s = uint8_t(crc) ^ *current++;

    // Hagai Gold made me aware of this table-less algorithm and send me code

    // polynomial 0xEDB88320 can be written in binary as 11101101101110001000001100100000b
    // reverse the bits (or just assume bit 0 is the first one)
    // and we have bits set at position 0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26
    // => those are the shift offsets:
    //crc = (crc >> 8) ^
    //       t ^
    //      (t >>  1) ^ (t >>  2) ^ (t >>  4) ^ (t >>  5) ^  // == y
    //      (t >>  7) ^ (t >>  8) ^ (t >> 10) ^ (t >> 11) ^  // == y >> 6
    //      (t >> 12) ^ (t >> 16) ^                          // == z
    //      (t >> 22) ^ (t >> 26) ^                          // == z >> 10
    //      (t >> 23);

    // the fastest I can come up with:
    uint32_t low = (s ^ (s << 6)) & 0xFF;
    uint32_t a   = (low * ((1 << 23) + (1 << 14) + (1 << 2)));
    crc = (crc >> 8) ^
          (low * ((1 << 24) + (1 << 16) + (1 << 8))) ^
           a ^
          (a >> 1) ^
          (low * ((1 << 20) + (1 << 12)           )) ^
          (low << 19) ^
          (low << 17) ^
          (low >>  2);

    // Hagai's code:
    /*uint32_t t = (s ^ (s << 6)) << 24;

    // some temporaries to optimize XOR
    uint32_t x = (t >> 1) ^ (t >> 2);
    uint32_t y = x ^ (x >> 3);
    uint32_t z = (t >> 12) ^ (t >> 16);

    crc = (crc >> 8) ^
           t ^ (t >> 23) ^
           y ^ (y >>  6) ^
           z ^ (z >> 10);*/
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (byte algorithm) without lookup tables
uint32_t crc32_1byte_tableless2(const void* data, size_t length, uint32_t previousCrc32)
{
  int32_t crc = ~previousCrc32; // note: signed integer, right shift distributes sign bit into lower bits
  const uint8_t* current = (const uint8_t*) data;

  while (length-- != 0)
  {
    crc = crc ^ *current++;

    uint32_t c = (((crc << 31) >> 31) & ((Polynomial >> 7)  ^ (Polynomial >> 1))) ^
                 (((crc << 30) >> 31) & ((Polynomial >> 6)  ^  Polynomial)) ^
                 (((crc << 29) >> 31) &  (Polynomial >> 5)) ^
                 (((crc << 28) >> 31) &  (Polynomial >> 4)) ^
                 (((crc << 27) >> 31) &  (Polynomial >> 3)) ^
                 (((crc << 26) >> 31) &  (Polynomial >> 2)) ^
                 (((crc << 25) >> 31) &  (Polynomial >> 1)) ^
                 (((crc << 24) >> 31) &   Polynomial);

    crc = ((uint32_t)crc >> 8) ^ c; // convert to unsigned integer before right shift
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_4
/// compute CRC32 (Slicing-by-4 algorithm)
uint32_t crc32_4bytes(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t  crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process four bytes at once (Slicing-by-4)
  while (length >= 4)
  {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc = Crc32Lookup[0][ one      & 0xFF] ^
          Crc32Lookup[1][(one>> 8) & 0xFF] ^
          Crc32Lookup[2][(one>>16) & 0xFF] ^
          Crc32Lookup[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc = Crc32Lookup[0][(one>>24) & 0xFF] ^
          Crc32Lookup[1][(one>>16) & 0xFF] ^
          Crc32Lookup[2][(one>> 8) & 0xFF] ^
          Crc32Lookup[3][ one      & 0xFF];
#endif

    length -= 4;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 3 bytes (standard algorithm)
  while (length-- != 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}
#endif


#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_8
/// compute CRC32 (Slicing-by-8 algorithm)
uint32_t crc32_8bytes(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 8)
  {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc = Crc32Lookup[0][ two      & 0xFF] ^
          Crc32Lookup[1][(two>> 8) & 0xFF] ^
          Crc32Lookup[2][(two>>16) & 0xFF] ^
          Crc32Lookup[3][(two>>24) & 0xFF] ^
          Crc32Lookup[4][ one      & 0xFF] ^
          Crc32Lookup[5][(one>> 8) & 0xFF] ^
          Crc32Lookup[6][(one>>16) & 0xFF] ^
          Crc32Lookup[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc = Crc32Lookup[0][(two>>24) & 0xFF] ^
          Crc32Lookup[1][(two>>16) & 0xFF] ^
          Crc32Lookup[2][(two>> 8) & 0xFF] ^
          Crc32Lookup[3][ two      & 0xFF] ^
          Crc32Lookup[4][(one>>24) & 0xFF] ^
          Crc32Lookup[5][(one>>16) & 0xFF] ^
          Crc32Lookup[6][(one>> 8) & 0xFF] ^
          Crc32Lookup[7][ one      & 0xFF];
#endif

    length -= 8;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- != 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-8 algorithm), unroll inner loop 4 times
uint32_t crc32_4x8bytes(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // enabling optimization (at least -O2) automatically unrolls the inner for-loop
  const size_t Unroll = 4;
  const size_t BytesAtOnce = 8 * Unroll;

  // process 4x eight bytes at once (Slicing-by-8)
  while (length >= BytesAtOnce)
  {
    for (size_t unrolling = 0; unrolling < Unroll; unrolling++)
    {
#if __BYTE_ORDER == __BIG_ENDIAN
      uint32_t one = *current++ ^ swap(crc);
      uint32_t two = *current++;
      crc = Crc32Lookup[0][ two      & 0xFF] ^
            Crc32Lookup[1][(two>> 8) & 0xFF] ^
            Crc32Lookup[2][(two>>16) & 0xFF] ^
            Crc32Lookup[3][(two>>24) & 0xFF] ^
            Crc32Lookup[4][ one      & 0xFF] ^
            Crc32Lookup[5][(one>> 8) & 0xFF] ^
            Crc32Lookup[6][(one>>16) & 0xFF] ^
            Crc32Lookup[7][(one>>24) & 0xFF];
#else
      uint32_t one = *current++ ^ crc;
      uint32_t two = *current++;
      crc = Crc32Lookup[0][(two>>24) & 0xFF] ^
            Crc32Lookup[1][(two>>16) & 0xFF] ^
            Crc32Lookup[2][(two>> 8) & 0xFF] ^
            Crc32Lookup[3][ two      & 0xFF] ^
            Crc32Lookup[4][(one>>24) & 0xFF] ^
            Crc32Lookup[5][(one>>16) & 0xFF] ^
            Crc32Lookup[6][(one>> 8) & 0xFF] ^
            Crc32Lookup[7][ one      & 0xFF];
#endif

    }

    length -= BytesAtOnce;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 31 bytes (standard algorithm)
  while (length-- != 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}
#endif // CRC32_USE_LOOKUP_TABLE_SLICING_BY_8


#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
/// compute CRC32 (Slicing-by-16 algorithm)
uint32_t crc32_16bytes(const void* data, size_t length, uint32_t previousCrc32)
{
  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // enabling optimization (at least -O2) automatically unrolls the inner for-loop
  const size_t Unroll = 4;
  const size_t BytesAtOnce = 16 * Unroll;

  while (length >= BytesAtOnce)
  {
    for (size_t unrolling = 0; unrolling < Unroll; unrolling++)
    {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one   = *current++ ^ swap(crc);
    uint32_t two   = *current++;
    uint32_t three = *current++;
    uint32_t four  = *current++;
    crc  = Crc32Lookup[ 0][ four         & 0xFF] ^
           Crc32Lookup[ 1][(four  >>  8) & 0xFF] ^
           Crc32Lookup[ 2][(four  >> 16) & 0xFF] ^
           Crc32Lookup[ 3][(four  >> 24) & 0xFF] ^
           Crc32Lookup[ 4][ three        & 0xFF] ^
           Crc32Lookup[ 5][(three >>  8) & 0xFF] ^
           Crc32Lookup[ 6][(three >> 16) & 0xFF] ^
           Crc32Lookup[ 7][(three >> 24) & 0xFF] ^
           Crc32Lookup[ 8][ two          & 0xFF] ^
           Crc32Lookup[ 9][(two   >>  8) & 0xFF] ^
           Crc32Lookup[10][(two   >> 16) & 0xFF] ^
           Crc32Lookup[11][(two   >> 24) & 0xFF] ^
           Crc32Lookup[12][ one          & 0xFF] ^
           Crc32Lookup[13][(one   >>  8) & 0xFF] ^
           Crc32Lookup[14][(one   >> 16) & 0xFF] ^
           Crc32Lookup[15][(one   >> 24) & 0xFF];
#else
    uint32_t one   = *current++ ^ crc;
    uint32_t two   = *current++;
    uint32_t three = *current++;
    uint32_t four  = *current++;
    crc  = Crc32Lookup[ 0][(four  >> 24) & 0xFF] ^
           Crc32Lookup[ 1][(four  >> 16) & 0xFF] ^
           Crc32Lookup[ 2][(four  >>  8) & 0xFF] ^
           Crc32Lookup[ 3][ four         & 0xFF] ^
           Crc32Lookup[ 4][(three >> 24) & 0xFF] ^
           Crc32Lookup[ 5][(three >> 16) & 0xFF] ^
           Crc32Lookup[ 6][(three >>  8) & 0xFF] ^
           Crc32Lookup[ 7][ three        & 0xFF] ^
           Crc32Lookup[ 8][(two   >> 24) & 0xFF] ^
           Crc32Lookup[ 9][(two   >> 16) & 0xFF] ^
           Crc32Lookup[10][(two   >>  8) & 0xFF] ^
           Crc32Lookup[11][ two          & 0xFF] ^
           Crc32Lookup[12][(one   >> 24) & 0xFF] ^
           Crc32Lookup[13][(one   >> 16) & 0xFF] ^
           Crc32Lookup[14][(one   >>  8) & 0xFF] ^
           Crc32Lookup[15][ one          & 0xFF];
#endif
    }

    length -= BytesAtOnce;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 63 bytes (standard algorithm)
  while (length-- != 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}


/// compute CRC32 (Slicing-by-16 algorithm, prefetch upcoming data blocks)
uint32_t crc32_16bytes_prefetch(const void* data, size_t length, uint32_t previousCrc32, size_t prefetchAhead)
{
  // CRC code is identical to crc32_16bytes (including unrolling), only added prefetching
  // 256 bytes look-ahead seems to be the sweet spot on Core i7 CPUs

  uint32_t crc = ~previousCrc32; // same as previousCrc32 ^ 0xFFFFFFFF
  const uint32_t* current = (const uint32_t*) data;

  // enabling optimization (at least -O2) automatically unrolls the for-loop
  const size_t Unroll = 4;
  const size_t BytesAtOnce = 16 * Unroll;

  while (length >= BytesAtOnce + prefetchAhead)
  {
    PREFETCH(((const char*) current) + prefetchAhead);

    for (size_t unrolling = 0; unrolling < Unroll; unrolling++)
    {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one   = *current++ ^ swap(crc);
    uint32_t two   = *current++;
    uint32_t three = *current++;
    uint32_t four  = *current++;
    crc  = Crc32Lookup[ 0][ four         & 0xFF] ^
           Crc32Lookup[ 1][(four  >>  8) & 0xFF] ^
           Crc32Lookup[ 2][(four  >> 16) & 0xFF] ^
           Crc32Lookup[ 3][(four  >> 24) & 0xFF] ^
           Crc32Lookup[ 4][ three        & 0xFF] ^
           Crc32Lookup[ 5][(three >>  8) & 0xFF] ^
           Crc32Lookup[ 6][(three >> 16) & 0xFF] ^
           Crc32Lookup[ 7][(three >> 24) & 0xFF] ^
           Crc32Lookup[ 8][ two          & 0xFF] ^
           Crc32Lookup[ 9][(two   >>  8) & 0xFF] ^
           Crc32Lookup[10][(two   >> 16) & 0xFF] ^
           Crc32Lookup[11][(two   >> 24) & 0xFF] ^
           Crc32Lookup[12][ one          & 0xFF] ^
           Crc32Lookup[13][(one   >>  8) & 0xFF] ^
           Crc32Lookup[14][(one   >> 16) & 0xFF] ^
           Crc32Lookup[15][(one   >> 24) & 0xFF];
#else
    uint32_t one   = *current++ ^ crc;
    uint32_t two   = *current++;
    uint32_t three = *current++;
    uint32_t four  = *current++;
    crc  = Crc32Lookup[ 0][(four  >> 24) & 0xFF] ^
           Crc32Lookup[ 1][(four  >> 16) & 0xFF] ^
           Crc32Lookup[ 2][(four  >>  8) & 0xFF] ^
           Crc32Lookup[ 3][ four         & 0xFF] ^
           Crc32Lookup[ 4][(three >> 24) & 0xFF] ^
           Crc32Lookup[ 5][(three >> 16) & 0xFF] ^
           Crc32Lookup[ 6][(three >>  8) & 0xFF] ^
           Crc32Lookup[ 7][ three        & 0xFF] ^
           Crc32Lookup[ 8][(two   >> 24) & 0xFF] ^
           Crc32Lookup[ 9][(two   >> 16) & 0xFF] ^
           Crc32Lookup[10][(two   >>  8) & 0xFF] ^
           Crc32Lookup[11][ two          & 0xFF] ^
           Crc32Lookup[12][(one   >> 24) & 0xFF] ^
           Crc32Lookup[13][(one   >> 16) & 0xFF] ^
           Crc32Lookup[14][(one   >>  8) & 0xFF] ^
           Crc32Lookup[15][ one          & 0xFF];
#endif
    }

    length -= BytesAtOnce;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 63 bytes (standard algorithm)
  while (length-- != 0)
    crc = (crc >> 8) ^ Crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc; // same as crc ^ 0xFFFFFFFF
}
#endif




/// merge two CRC32 such that result = crc32(dataB, lengthB, crc32(dataA, lengthA))
uint32_t crc32_combine(uint32_t crcA, uint32_t crcB, size_t lengthB)
{
  // based on Mark Adler's crc_combine from
  // https://github.com/madler/pigz/blob/master/pigz.c

  // main idea:
  // - if you have two equally-sized blocks A and B,
  //   then you can create a block C = A ^ B
  //   which has the property crc(C) = crc(A) ^ crc(B)
  // - if you append length(B) zeros to A and call it A' (think of it as AAAA000)
  //   and   prepend length(A) zeros to B and call it B' (think of it as 0000BBB)
  //   then exists a C' = A' ^ B'
  // - remember: if you XOR someting with zero, it remains unchanged: X ^ 0 = X
  // - that means C' = A concat B so that crc(A concat B) = crc(C') = crc(A') ^ crc(B')
  // - the trick is to compute crc(A') based on crc(A)
  //                       and crc(B') based on crc(B)
  // - since B' starts with many zeros, the crc of those initial zeros is still zero
  // - that means crc(B') = crc(B)
  // - unfortunately the trailing zeros of A' change the crc, so usually crc(A') != crc(A)
  // - the following code is a fast algorithm to compute crc(A')
  // - starting with crc(A) and appending length(B) zeros, needing just log2(length(B)) iterations
  // - the details are explained by the original author at
  //   https://stackoverflow.com/questions/23122312/crc-calculation-of-a-mostly-static-data-stream/23126768
  //
  // notes:
  // - I squeezed everything into one function to keep global namespace clean (original code two helper functions)
  // - most original comments are still in place, I added comments where these helper functions where made inline code
  // - performance-wise there isn't any differenze to the original zlib/pigz code

  // degenerated case
  if (lengthB == 0)
    return crcA;

  /// CRC32 => 32 bits
  const uint32_t CrcBits = 32;

  uint32_t odd [CrcBits]; // odd-power-of-two  zeros operator
  uint32_t even[CrcBits]; // even-power-of-two zeros operator

  // put operator for one zero bit in odd
  odd[0] = Polynomial;    // CRC-32 polynomial
  for (int i = 1; i < CrcBits; i++)
    odd[i] = 1 << (i - 1);

  // put operator for two zero bits in even
  // same as gf2_matrix_square(even, odd);
  for (int i = 0; i < CrcBits; i++)
  {
    uint32_t vec = odd[i];
    even[i] = 0;
    for (int j = 0; vec != 0; j++, vec >>= 1)
      if (vec & 1)
        even[i] ^= odd[j];
  }
  // put operator for four zero bits in odd
  // same as gf2_matrix_square(odd, even);
  for (int i = 0; i < CrcBits; i++)
  {
    uint32_t vec = even[i];
    odd[i] = 0;
    for (int j = 0; vec != 0; j++, vec >>= 1)
      if (vec & 1)
        odd[i] ^= even[j];
  }

  // the following loop becomes much shorter if I keep swapping even and odd
  uint32_t* a = even;
  uint32_t* b = odd;
  // apply secondLength zeros to firstCrc32
  for (; lengthB > 0; lengthB >>= 1)
  {
    // same as gf2_matrix_square(a, b);
    for (int i = 0; i < CrcBits; i++)
    {
      uint32_t vec = b[i];
      a[i] = 0;
      for (int j = 0; vec != 0; j++, vec >>= 1)
        if (vec & 1)
          a[i] ^= b[j];
    }

    // apply zeros operator for this bit
    if (lengthB & 1)
    {
      // same as firstCrc32 = gf2_matrix_times(a, firstCrc32);
      uint32_t sum = 0;
      for (int i = 0; crcA != 0; i++, crcA >>= 1)
        if (crcA & 1)
          sum ^= a[i];
      crcA = sum;
    }

    // switch even and odd
    uint32_t* t = a; a = b; b = t;
  }

  // return combined crc
  return crcA ^ crcB;
}


// //////////////////////////////////////////////////////////
// constants


#ifndef NO_LUT
/// look-up table, already declared above
const uint32_t Crc32Lookup[MaxSlice][256] =
{
  //// same algorithm as crc32_bitwise
  //for (int i = 0; i <= 0xFF; i++)
  //{
  //  uint32_t crc = i;
  //  for (int j = 0; j < 8; j++)
  //    crc = (crc >> 1) ^ ((crc & 1) * Polynomial);
  //  Crc32Lookup[0][i] = crc;
  //}
  //// ... and the following slicing-by-8 algorithm (from Intel):
  //// http://www.intel.com/technology/comms/perfnet/download/CRC_generators.pdf
  //// http://sourceforge.net/projects/slicing-by-8/
  //for (int slice = 1; slice < MaxSlice; slice++)
  //  Crc32Lookup[slice][i] = (Crc32Lookup[slice - 1][i] >> 8) ^ Crc32Lookup[0][Crc32Lookup[slice - 1][i] & 0xFF];
  {
    // note: the first number of every second row corresponds to the half-byte look-up table !
    0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
    0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
    0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
    0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
    0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
    0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
    0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
    0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
    0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
    0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
    0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
    0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
    0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
    0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
    0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
    0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
    0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
    0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
    0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
    0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
    0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
    0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
    0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
    0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
    0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
    0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
    0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
    0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
    0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
    0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
    0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
    0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
  }

#if defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_4) || defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_8) || defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_16)
  // beyond this point only relevant for Slicing-by-4, Slicing-by-8 and Slicing-by-16
  ,{
    0x00000000,0x191B3141,0x32366282,0x2B2D53C3,0x646CC504,0x7D77F445,0x565AA786,0x4F4196C7,
    0xC8D98A08,0xD1C2BB49,0xFAEFE88A,0xE3F4D9CB,0xACB54F0C,0xB5AE7E4D,0x9E832D8E,0x87981CCF,
    0x4AC21251,0x53D92310,0x78F470D3,0x61EF4192,0x2EAED755,0x37B5E614,0x1C98B5D7,0x05838496,
    0x821B9859,0x9B00A918,0xB02DFADB,0xA936CB9A,0xE6775D5D,0xFF6C6C1C,0xD4413FDF,0xCD5A0E9E,
    0x958424A2,0x8C9F15E3,0xA7B24620,0xBEA97761,0xF1E8E1A6,0xE8F3D0E7,0xC3DE8324,0xDAC5B265,
    0x5D5DAEAA,0x44469FEB,0x6F6BCC28,0x7670FD69,0x39316BAE,0x202A5AEF,0x0B07092C,0x121C386D,
    0xDF4636F3,0xC65D07B2,0xED705471,0xF46B6530,0xBB2AF3F7,0xA231C2B6,0x891C9175,0x9007A034,
    0x179FBCFB,0x0E848DBA,0x25A9DE79,0x3CB2EF38,0x73F379FF,0x6AE848BE,0x41C51B7D,0x58DE2A3C,
    0xF0794F05,0xE9627E44,0xC24F2D87,0xDB541CC6,0x94158A01,0x8D0EBB40,0xA623E883,0xBF38D9C2,
    0x38A0C50D,0x21BBF44C,0x0A96A78F,0x138D96CE,0x5CCC0009,0x45D73148,0x6EFA628B,0x77E153CA,
    0xBABB5D54,0xA3A06C15,0x888D3FD6,0x91960E97,0xDED79850,0xC7CCA911,0xECE1FAD2,0xF5FACB93,
    0x7262D75C,0x6B79E61D,0x4054B5DE,0x594F849F,0x160E1258,0x0F152319,0x243870DA,0x3D23419B,
    0x65FD6BA7,0x7CE65AE6,0x57CB0925,0x4ED03864,0x0191AEA3,0x188A9FE2,0x33A7CC21,0x2ABCFD60,
    0xAD24E1AF,0xB43FD0EE,0x9F12832D,0x8609B26C,0xC94824AB,0xD05315EA,0xFB7E4629,0xE2657768,
    0x2F3F79F6,0x362448B7,0x1D091B74,0x04122A35,0x4B53BCF2,0x52488DB3,0x7965DE70,0x607EEF31,
    0xE7E6F3FE,0xFEFDC2BF,0xD5D0917C,0xCCCBA03D,0x838A36FA,0x9A9107BB,0xB1BC5478,0xA8A76539,
    0x3B83984B,0x2298A90A,0x09B5FAC9,0x10AECB88,0x5FEF5D4F,0x46F46C0E,0x6DD93FCD,0x74C20E8C,
    0xF35A1243,0xEA412302,0xC16C70C1,0xD8774180,0x9736D747,0x8E2DE606,0xA500B5C5,0xBC1B8484,
    0x71418A1A,0x685ABB5B,0x4377E898,0x5A6CD9D9,0x152D4F1E,0x0C367E5F,0x271B2D9C,0x3E001CDD,
    0xB9980012,0xA0833153,0x8BAE6290,0x92B553D1,0xDDF4C516,0xC4EFF457,0xEFC2A794,0xF6D996D5,
    0xAE07BCE9,0xB71C8DA8,0x9C31DE6B,0x852AEF2A,0xCA6B79ED,0xD37048AC,0xF85D1B6F,0xE1462A2E,
    0x66DE36E1,0x7FC507A0,0x54E85463,0x4DF36522,0x02B2F3E5,0x1BA9C2A4,0x30849167,0x299FA026,
    0xE4C5AEB8,0xFDDE9FF9,0xD6F3CC3A,0xCFE8FD7B,0x80A96BBC,0x99B25AFD,0xB29F093E,0xAB84387F,
    0x2C1C24B0,0x350715F1,0x1E2A4632,0x07317773,0x4870E1B4,0x516BD0F5,0x7A468336,0x635DB277,
    0xCBFAD74E,0xD2E1E60F,0xF9CCB5CC,0xE0D7848D,0xAF96124A,0xB68D230B,0x9DA070C8,0x84BB4189,
    0x03235D46,0x1A386C07,0x31153FC4,0x280E0E85,0x674F9842,0x7E54A903,0x5579FAC0,0x4C62CB81,
    0x8138C51F,0x9823F45E,0xB30EA79D,0xAA1596DC,0xE554001B,0xFC4F315A,0xD7626299,0xCE7953D8,
    0x49E14F17,0x50FA7E56,0x7BD72D95,0x62CC1CD4,0x2D8D8A13,0x3496BB52,0x1FBBE891,0x06A0D9D0,
    0x5E7EF3EC,0x4765C2AD,0x6C48916E,0x7553A02F,0x3A1236E8,0x230907A9,0x0824546A,0x113F652B,
    0x96A779E4,0x8FBC48A5,0xA4911B66,0xBD8A2A27,0xF2CBBCE0,0xEBD08DA1,0xC0FDDE62,0xD9E6EF23,
    0x14BCE1BD,0x0DA7D0FC,0x268A833F,0x3F91B27E,0x70D024B9,0x69CB15F8,0x42E6463B,0x5BFD777A,
    0xDC656BB5,0xC57E5AF4,0xEE530937,0xF7483876,0xB809AEB1,0xA1129FF0,0x8A3FCC33,0x9324FD72,
  },

  {
    0x00000000,0x01C26A37,0x0384D46E,0x0246BE59,0x0709A8DC,0x06CBC2EB,0x048D7CB2,0x054F1685,
    0x0E1351B8,0x0FD13B8F,0x0D9785D6,0x0C55EFE1,0x091AF964,0x08D89353,0x0A9E2D0A,0x0B5C473D,
    0x1C26A370,0x1DE4C947,0x1FA2771E,0x1E601D29,0x1B2F0BAC,0x1AED619B,0x18ABDFC2,0x1969B5F5,
    0x1235F2C8,0x13F798FF,0x11B126A6,0x10734C91,0x153C5A14,0x14FE3023,0x16B88E7A,0x177AE44D,
    0x384D46E0,0x398F2CD7,0x3BC9928E,0x3A0BF8B9,0x3F44EE3C,0x3E86840B,0x3CC03A52,0x3D025065,
    0x365E1758,0x379C7D6F,0x35DAC336,0x3418A901,0x3157BF84,0x3095D5B3,0x32D36BEA,0x331101DD,
    0x246BE590,0x25A98FA7,0x27EF31FE,0x262D5BC9,0x23624D4C,0x22A0277B,0x20E69922,0x2124F315,
    0x2A78B428,0x2BBADE1F,0x29FC6046,0x283E0A71,0x2D711CF4,0x2CB376C3,0x2EF5C89A,0x2F37A2AD,
    0x709A8DC0,0x7158E7F7,0x731E59AE,0x72DC3399,0x7793251C,0x76514F2B,0x7417F172,0x75D59B45,
    0x7E89DC78,0x7F4BB64F,0x7D0D0816,0x7CCF6221,0x798074A4,0x78421E93,0x7A04A0CA,0x7BC6CAFD,
    0x6CBC2EB0,0x6D7E4487,0x6F38FADE,0x6EFA90E9,0x6BB5866C,0x6A77EC5B,0x68315202,0x69F33835,
    0x62AF7F08,0x636D153F,0x612BAB66,0x60E9C151,0x65A6D7D4,0x6464BDE3,0x662203BA,0x67E0698D,
    0x48D7CB20,0x4915A117,0x4B531F4E,0x4A917579,0x4FDE63FC,0x4E1C09CB,0x4C5AB792,0x4D98DDA5,
    0x46C49A98,0x4706F0AF,0x45404EF6,0x448224C1,0x41CD3244,0x400F5873,0x4249E62A,0x438B8C1D,
    0x54F16850,0x55330267,0x5775BC3E,0x56B7D609,0x53F8C08C,0x523AAABB,0x507C14E2,0x51BE7ED5,
    0x5AE239E8,0x5B2053DF,0x5966ED86,0x58A487B1,0x5DEB9134,0x5C29FB03,0x5E6F455A,0x5FAD2F6D,
    0xE1351B80,0xE0F771B7,0xE2B1CFEE,0xE373A5D9,0xE63CB35C,0xE7FED96B,0xE5B86732,0xE47A0D05,
    0xEF264A38,0xEEE4200F,0xECA29E56,0xED60F461,0xE82FE2E4,0xE9ED88D3,0xEBAB368A,0xEA695CBD,
    0xFD13B8F0,0xFCD1D2C7,0xFE976C9E,0xFF5506A9,0xFA1A102C,0xFBD87A1B,0xF99EC442,0xF85CAE75,
    0xF300E948,0xF2C2837F,0xF0843D26,0xF1465711,0xF4094194,0xF5CB2BA3,0xF78D95FA,0xF64FFFCD,
    0xD9785D60,0xD8BA3757,0xDAFC890E,0xDB3EE339,0xDE71F5BC,0xDFB39F8B,0xDDF521D2,0xDC374BE5,
    0xD76B0CD8,0xD6A966EF,0xD4EFD8B6,0xD52DB281,0xD062A404,0xD1A0CE33,0xD3E6706A,0xD2241A5D,
    0xC55EFE10,0xC49C9427,0xC6DA2A7E,0xC7184049,0xC25756CC,0xC3953CFB,0xC1D382A2,0xC011E895,
    0xCB4DAFA8,0xCA8FC59F,0xC8C97BC6,0xC90B11F1,0xCC440774,0xCD866D43,0xCFC0D31A,0xCE02B92D,
    0x91AF9640,0x906DFC77,0x922B422E,0x93E92819,0x96A63E9C,0x976454AB,0x9522EAF2,0x94E080C5,
    0x9FBCC7F8,0x9E7EADCF,0x9C381396,0x9DFA79A1,0x98B56F24,0x99770513,0x9B31BB4A,0x9AF3D17D,
    0x8D893530,0x8C4B5F07,0x8E0DE15E,0x8FCF8B69,0x8A809DEC,0x8B42F7DB,0x89044982,0x88C623B5,
    0x839A6488,0x82580EBF,0x801EB0E6,0x81DCDAD1,0x8493CC54,0x8551A663,0x8717183A,0x86D5720D,
    0xA9E2D0A0,0xA820BA97,0xAA6604CE,0xABA46EF9,0xAEEB787C,0xAF29124B,0xAD6FAC12,0xACADC625,
    0xA7F18118,0xA633EB2F,0xA4755576,0xA5B73F41,0xA0F829C4,0xA13A43F3,0xA37CFDAA,0xA2BE979D,
    0xB5C473D0,0xB40619E7,0xB640A7BE,0xB782CD89,0xB2CDDB0C,0xB30FB13B,0xB1490F62,0xB08B6555,
    0xBBD72268,0xBA15485F,0xB853F606,0xB9919C31,0xBCDE8AB4,0xBD1CE083,0xBF5A5EDA,0xBE9834ED,
  },

  {
    0x00000000,0xB8BC6765,0xAA09C88B,0x12B5AFEE,0x8F629757,0x37DEF032,0x256B5FDC,0x9DD738B9,
    0xC5B428EF,0x7D084F8A,0x6FBDE064,0xD7018701,0x4AD6BFB8,0xF26AD8DD,0xE0DF7733,0x58631056,
    0x5019579F,0xE8A530FA,0xFA109F14,0x42ACF871,0xDF7BC0C8,0x67C7A7AD,0x75720843,0xCDCE6F26,
    0x95AD7F70,0x2D111815,0x3FA4B7FB,0x8718D09E,0x1ACFE827,0xA2738F42,0xB0C620AC,0x087A47C9,
    0xA032AF3E,0x188EC85B,0x0A3B67B5,0xB28700D0,0x2F503869,0x97EC5F0C,0x8559F0E2,0x3DE59787,
    0x658687D1,0xDD3AE0B4,0xCF8F4F5A,0x7733283F,0xEAE41086,0x525877E3,0x40EDD80D,0xF851BF68,
    0xF02BF8A1,0x48979FC4,0x5A22302A,0xE29E574F,0x7F496FF6,0xC7F50893,0xD540A77D,0x6DFCC018,
    0x359FD04E,0x8D23B72B,0x9F9618C5,0x272A7FA0,0xBAFD4719,0x0241207C,0x10F48F92,0xA848E8F7,
    0x9B14583D,0x23A83F58,0x311D90B6,0x89A1F7D3,0x1476CF6A,0xACCAA80F,0xBE7F07E1,0x06C36084,
    0x5EA070D2,0xE61C17B7,0xF4A9B859,0x4C15DF3C,0xD1C2E785,0x697E80E0,0x7BCB2F0E,0xC377486B,
    0xCB0D0FA2,0x73B168C7,0x6104C729,0xD9B8A04C,0x446F98F5,0xFCD3FF90,0xEE66507E,0x56DA371B,
    0x0EB9274D,0xB6054028,0xA4B0EFC6,0x1C0C88A3,0x81DBB01A,0x3967D77F,0x2BD27891,0x936E1FF4,
    0x3B26F703,0x839A9066,0x912F3F88,0x299358ED,0xB4446054,0x0CF80731,0x1E4DA8DF,0xA6F1CFBA,
    0xFE92DFEC,0x462EB889,0x549B1767,0xEC277002,0x71F048BB,0xC94C2FDE,0xDBF98030,0x6345E755,
    0x6B3FA09C,0xD383C7F9,0xC1366817,0x798A0F72,0xE45D37CB,0x5CE150AE,0x4E54FF40,0xF6E89825,
    0xAE8B8873,0x1637EF16,0x048240F8,0xBC3E279D,0x21E91F24,0x99557841,0x8BE0D7AF,0x335CB0CA,
    0xED59B63B,0x55E5D15E,0x47507EB0,0xFFEC19D5,0x623B216C,0xDA874609,0xC832E9E7,0x708E8E82,
    0x28ED9ED4,0x9051F9B1,0x82E4565F,0x3A58313A,0xA78F0983,0x1F336EE6,0x0D86C108,0xB53AA66D,
    0xBD40E1A4,0x05FC86C1,0x1749292F,0xAFF54E4A,0x322276F3,0x8A9E1196,0x982BBE78,0x2097D91D,
    0x78F4C94B,0xC048AE2E,0xD2FD01C0,0x6A4166A5,0xF7965E1C,0x4F2A3979,0x5D9F9697,0xE523F1F2,
    0x4D6B1905,0xF5D77E60,0xE762D18E,0x5FDEB6EB,0xC2098E52,0x7AB5E937,0x680046D9,0xD0BC21BC,
    0x88DF31EA,0x3063568F,0x22D6F961,0x9A6A9E04,0x07BDA6BD,0xBF01C1D8,0xADB46E36,0x15080953,
    0x1D724E9A,0xA5CE29FF,0xB77B8611,0x0FC7E174,0x9210D9CD,0x2AACBEA8,0x38191146,0x80A57623,
    0xD8C66675,0x607A0110,0x72CFAEFE,0xCA73C99B,0x57A4F122,0xEF189647,0xFDAD39A9,0x45115ECC,
    0x764DEE06,0xCEF18963,0xDC44268D,0x64F841E8,0xF92F7951,0x41931E34,0x5326B1DA,0xEB9AD6BF,
    0xB3F9C6E9,0x0B45A18C,0x19F00E62,0xA14C6907,0x3C9B51BE,0x842736DB,0x96929935,0x2E2EFE50,
    0x2654B999,0x9EE8DEFC,0x8C5D7112,0x34E11677,0xA9362ECE,0x118A49AB,0x033FE645,0xBB838120,
    0xE3E09176,0x5B5CF613,0x49E959FD,0xF1553E98,0x6C820621,0xD43E6144,0xC68BCEAA,0x7E37A9CF,
    0xD67F4138,0x6EC3265D,0x7C7689B3,0xC4CAEED6,0x591DD66F,0xE1A1B10A,0xF3141EE4,0x4BA87981,
    0x13CB69D7,0xAB770EB2,0xB9C2A15C,0x017EC639,0x9CA9FE80,0x241599E5,0x36A0360B,0x8E1C516E,
    0x866616A7,0x3EDA71C2,0x2C6FDE2C,0x94D3B949,0x090481F0,0xB1B8E695,0xA30D497B,0x1BB12E1E,
    0x43D23E48,0xFB6E592D,0xE9DBF6C3,0x516791A6,0xCCB0A91F,0x740CCE7A,0x66B96194,0xDE0506F1,
  }
#endif // defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_4) || defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_8) || defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_16)
#if defined (CRC32_USE_LOOKUP_TABLE_SLICING_BY_8) || defined(CRC32_USE_LOOKUP_TABLE_SLICING_BY_16)
  // beyond this point only relevant for Slicing-by-8 and Slicing-by-16
  ,{
    0x00000000,0x3D6029B0,0x7AC05360,0x47A07AD0,0xF580A6C0,0xC8E08F70,0x8F40F5A0,0xB220DC10,
    0x30704BC1,0x0D106271,0x4AB018A1,0x77D03111,0xC5F0ED01,0xF890C4B1,0xBF30BE61,0x825097D1,
    0x60E09782,0x5D80BE32,0x1A20C4E2,0x2740ED52,0x95603142,0xA80018F2,0xEFA06222,0xD2C04B92,
    0x5090DC43,0x6DF0F5F3,0x2A508F23,0x1730A693,0xA5107A83,0x98705333,0xDFD029E3,0xE2B00053,
    0xC1C12F04,0xFCA106B4,0xBB017C64,0x866155D4,0x344189C4,0x0921A074,0x4E81DAA4,0x73E1F314,
    0xF1B164C5,0xCCD14D75,0x8B7137A5,0xB6111E15,0x0431C205,0x3951EBB5,0x7EF19165,0x4391B8D5,
    0xA121B886,0x9C419136,0xDBE1EBE6,0xE681C256,0x54A11E46,0x69C137F6,0x2E614D26,0x13016496,
    0x9151F347,0xAC31DAF7,0xEB91A027,0xD6F18997,0x64D15587,0x59B17C37,0x1E1106E7,0x23712F57,
    0x58F35849,0x659371F9,0x22330B29,0x1F532299,0xAD73FE89,0x9013D739,0xD7B3ADE9,0xEAD38459,
    0x68831388,0x55E33A38,0x124340E8,0x2F236958,0x9D03B548,0xA0639CF8,0xE7C3E628,0xDAA3CF98,
    0x3813CFCB,0x0573E67B,0x42D39CAB,0x7FB3B51B,0xCD93690B,0xF0F340BB,0xB7533A6B,0x8A3313DB,
    0x0863840A,0x3503ADBA,0x72A3D76A,0x4FC3FEDA,0xFDE322CA,0xC0830B7A,0x872371AA,0xBA43581A,
    0x9932774D,0xA4525EFD,0xE3F2242D,0xDE920D9D,0x6CB2D18D,0x51D2F83D,0x167282ED,0x2B12AB5D,
    0xA9423C8C,0x9422153C,0xD3826FEC,0xEEE2465C,0x5CC29A4C,0x61A2B3FC,0x2602C92C,0x1B62E09C,
    0xF9D2E0CF,0xC4B2C97F,0x8312B3AF,0xBE729A1F,0x0C52460F,0x31326FBF,0x7692156F,0x4BF23CDF,
    0xC9A2AB0E,0xF4C282BE,0xB362F86E,0x8E02D1DE,0x3C220DCE,0x0142247E,0x46E25EAE,0x7B82771E,
    0xB1E6B092,0x8C869922,0xCB26E3F2,0xF646CA42,0x44661652,0x79063FE2,0x3EA64532,0x03C66C82,
    0x8196FB53,0xBCF6D2E3,0xFB56A833,0xC6368183,0x74165D93,0x49767423,0x0ED60EF3,0x33B62743,
    0xD1062710,0xEC660EA0,0xABC67470,0x96A65DC0,0x248681D0,0x19E6A860,0x5E46D2B0,0x6326FB00,
    0xE1766CD1,0xDC164561,0x9BB63FB1,0xA6D61601,0x14F6CA11,0x2996E3A1,0x6E369971,0x5356B0C1,
    0x70279F96,0x4D47B626,0x0AE7CCF6,0x3787E546,0x85A73956,0xB8C710E6,0xFF676A36,0xC2074386,
    0x4057D457,0x7D37FDE7,0x3A978737,0x07F7AE87,0xB5D77297,0x88B75B27,0xCF1721F7,0xF2770847,
    0x10C70814,0x2DA721A4,0x6A075B74,0x576772C4,0xE547AED4,0xD8278764,0x9F87FDB4,0xA2E7D404,
    0x20B743D5,0x1DD76A65,0x5A7710B5,0x67173905,0xD537E515,0xE857CCA5,0xAFF7B675,0x92979FC5,
    0xE915E8DB,0xD475C16B,0x93D5BBBB,0xAEB5920B,0x1C954E1B,0x21F567AB,0x66551D7B,0x5B3534CB,
    0xD965A31A,0xE4058AAA,0xA3A5F07A,0x9EC5D9CA,0x2CE505DA,0x11852C6A,0x562556BA,0x6B457F0A,
    0x89F57F59,0xB49556E9,0xF3352C39,0xCE550589,0x7C75D999,0x4115F029,0x06B58AF9,0x3BD5A349,
    0xB9853498,0x84E51D28,0xC34567F8,0xFE254E48,0x4C059258,0x7165BBE8,0x36C5C138,0x0BA5E888,
    0x28D4C7DF,0x15B4EE6F,0x521494BF,0x6F74BD0F,0xDD54611F,0xE03448AF,0xA794327F,0x9AF41BCF,
    0x18A48C1E,0x25C4A5AE,0x6264DF7E,0x5F04F6CE,0xED242ADE,0xD044036E,0x97E479BE,0xAA84500E,
    0x4834505D,0x755479ED,0x32F4033D,0x0F942A8D,0xBDB4F69D,0x80D4DF2D,0xC774A5FD,0xFA148C4D,
    0x78441B9C,0x4524322C,0x028448FC,0x3FE4614C,0x8DC4BD5C,0xB0A494EC,0xF704EE3C,0xCA64C78C,
  },

  {
    0x00000000,0xCB5CD3A5,0x4DC8A10B,0x869472AE,0x9B914216,0x50CD91B3,0xD659E31D,0x1D0530B8,
    0xEC53826D,0x270F51C8,0xA19B2366,0x6AC7F0C3,0x77C2C07B,0xBC9E13DE,0x3A0A6170,0xF156B2D5,
    0x03D6029B,0xC88AD13E,0x4E1EA390,0x85427035,0x9847408D,0x531B9328,0xD58FE186,0x1ED33223,
    0xEF8580F6,0x24D95353,0xA24D21FD,0x6911F258,0x7414C2E0,0xBF481145,0x39DC63EB,0xF280B04E,
    0x07AC0536,0xCCF0D693,0x4A64A43D,0x81387798,0x9C3D4720,0x57619485,0xD1F5E62B,0x1AA9358E,
    0xEBFF875B,0x20A354FE,0xA6372650,0x6D6BF5F5,0x706EC54D,0xBB3216E8,0x3DA66446,0xF6FAB7E3,
    0x047A07AD,0xCF26D408,0x49B2A6A6,0x82EE7503,0x9FEB45BB,0x54B7961E,0xD223E4B0,0x197F3715,
    0xE82985C0,0x23755665,0xA5E124CB,0x6EBDF76E,0x73B8C7D6,0xB8E41473,0x3E7066DD,0xF52CB578,
    0x0F580A6C,0xC404D9C9,0x4290AB67,0x89CC78C2,0x94C9487A,0x5F959BDF,0xD901E971,0x125D3AD4,
    0xE30B8801,0x28575BA4,0xAEC3290A,0x659FFAAF,0x789ACA17,0xB3C619B2,0x35526B1C,0xFE0EB8B9,
    0x0C8E08F7,0xC7D2DB52,0x4146A9FC,0x8A1A7A59,0x971F4AE1,0x5C439944,0xDAD7EBEA,0x118B384F,
    0xE0DD8A9A,0x2B81593F,0xAD152B91,0x6649F834,0x7B4CC88C,0xB0101B29,0x36846987,0xFDD8BA22,
    0x08F40F5A,0xC3A8DCFF,0x453CAE51,0x8E607DF4,0x93654D4C,0x58399EE9,0xDEADEC47,0x15F13FE2,
    0xE4A78D37,0x2FFB5E92,0xA96F2C3C,0x6233FF99,0x7F36CF21,0xB46A1C84,0x32FE6E2A,0xF9A2BD8F,
    0x0B220DC1,0xC07EDE64,0x46EAACCA,0x8DB67F6F,0x90B34FD7,0x5BEF9C72,0xDD7BEEDC,0x16273D79,
    0xE7718FAC,0x2C2D5C09,0xAAB92EA7,0x61E5FD02,0x7CE0CDBA,0xB7BC1E1F,0x31286CB1,0xFA74BF14,
    0x1EB014D8,0xD5ECC77D,0x5378B5D3,0x98246676,0x852156CE,0x4E7D856B,0xC8E9F7C5,0x03B52460,
    0xF2E396B5,0x39BF4510,0xBF2B37BE,0x7477E41B,0x6972D4A3,0xA22E0706,0x24BA75A8,0xEFE6A60D,
    0x1D661643,0xD63AC5E6,0x50AEB748,0x9BF264ED,0x86F75455,0x4DAB87F0,0xCB3FF55E,0x006326FB,
    0xF135942E,0x3A69478B,0xBCFD3525,0x77A1E680,0x6AA4D638,0xA1F8059D,0x276C7733,0xEC30A496,
    0x191C11EE,0xD240C24B,0x54D4B0E5,0x9F886340,0x828D53F8,0x49D1805D,0xCF45F2F3,0x04192156,
    0xF54F9383,0x3E134026,0xB8873288,0x73DBE12D,0x6EDED195,0xA5820230,0x2316709E,0xE84AA33B,
    0x1ACA1375,0xD196C0D0,0x5702B27E,0x9C5E61DB,0x815B5163,0x4A0782C6,0xCC93F068,0x07CF23CD,
    0xF6999118,0x3DC542BD,0xBB513013,0x700DE3B6,0x6D08D30E,0xA65400AB,0x20C07205,0xEB9CA1A0,
    0x11E81EB4,0xDAB4CD11,0x5C20BFBF,0x977C6C1A,0x8A795CA2,0x41258F07,0xC7B1FDA9,0x0CED2E0C,
    0xFDBB9CD9,0x36E74F7C,0xB0733DD2,0x7B2FEE77,0x662ADECF,0xAD760D6A,0x2BE27FC4,0xE0BEAC61,
    0x123E1C2F,0xD962CF8A,0x5FF6BD24,0x94AA6E81,0x89AF5E39,0x42F38D9C,0xC467FF32,0x0F3B2C97,
    0xFE6D9E42,0x35314DE7,0xB3A53F49,0x78F9ECEC,0x65FCDC54,0xAEA00FF1,0x28347D5F,0xE368AEFA,
    0x16441B82,0xDD18C827,0x5B8CBA89,0x90D0692C,0x8DD55994,0x46898A31,0xC01DF89F,0x0B412B3A,
    0xFA1799EF,0x314B4A4A,0xB7DF38E4,0x7C83EB41,0x6186DBF9,0xAADA085C,0x2C4E7AF2,0xE712A957,
    0x15921919,0xDECECABC,0x585AB812,0x93066BB7,0x8E035B0F,0x455F88AA,0xC3CBFA04,0x089729A1,
    0xF9C19B74,0x329D48D1,0xB4093A7F,0x7F55E9DA,0x6250D962,0xA90C0AC7,0x2F987869,0xE4C4ABCC,
  },

  {
    0x00000000,0xA6770BB4,0x979F1129,0x31E81A9D,0xF44F2413,0x52382FA7,0x63D0353A,0xC5A73E8E,
    0x33EF4E67,0x959845D3,0xA4705F4E,0x020754FA,0xC7A06A74,0x61D761C0,0x503F7B5D,0xF64870E9,
    0x67DE9CCE,0xC1A9977A,0xF0418DE7,0x56368653,0x9391B8DD,0x35E6B369,0x040EA9F4,0xA279A240,
    0x5431D2A9,0xF246D91D,0xC3AEC380,0x65D9C834,0xA07EF6BA,0x0609FD0E,0x37E1E793,0x9196EC27,
    0xCFBD399C,0x69CA3228,0x582228B5,0xFE552301,0x3BF21D8F,0x9D85163B,0xAC6D0CA6,0x0A1A0712,
    0xFC5277FB,0x5A257C4F,0x6BCD66D2,0xCDBA6D66,0x081D53E8,0xAE6A585C,0x9F8242C1,0x39F54975,
    0xA863A552,0x0E14AEE6,0x3FFCB47B,0x998BBFCF,0x5C2C8141,0xFA5B8AF5,0xCBB39068,0x6DC49BDC,
    0x9B8CEB35,0x3DFBE081,0x0C13FA1C,0xAA64F1A8,0x6FC3CF26,0xC9B4C492,0xF85CDE0F,0x5E2BD5BB,
    0x440B7579,0xE27C7ECD,0xD3946450,0x75E36FE4,0xB044516A,0x16335ADE,0x27DB4043,0x81AC4BF7,
    0x77E43B1E,0xD19330AA,0xE07B2A37,0x460C2183,0x83AB1F0D,0x25DC14B9,0x14340E24,0xB2430590,
    0x23D5E9B7,0x85A2E203,0xB44AF89E,0x123DF32A,0xD79ACDA4,0x71EDC610,0x4005DC8D,0xE672D739,
    0x103AA7D0,0xB64DAC64,0x87A5B6F9,0x21D2BD4D,0xE47583C3,0x42028877,0x73EA92EA,0xD59D995E,
    0x8BB64CE5,0x2DC14751,0x1C295DCC,0xBA5E5678,0x7FF968F6,0xD98E6342,0xE86679DF,0x4E11726B,
    0xB8590282,0x1E2E0936,0x2FC613AB,0x89B1181F,0x4C162691,0xEA612D25,0xDB8937B8,0x7DFE3C0C,
    0xEC68D02B,0x4A1FDB9F,0x7BF7C102,0xDD80CAB6,0x1827F438,0xBE50FF8C,0x8FB8E511,0x29CFEEA5,
    0xDF879E4C,0x79F095F8,0x48188F65,0xEE6F84D1,0x2BC8BA5F,0x8DBFB1EB,0xBC57AB76,0x1A20A0C2,
    0x8816EAF2,0x2E61E146,0x1F89FBDB,0xB9FEF06F,0x7C59CEE1,0xDA2EC555,0xEBC6DFC8,0x4DB1D47C,
    0xBBF9A495,0x1D8EAF21,0x2C66B5BC,0x8A11BE08,0x4FB68086,0xE9C18B32,0xD82991AF,0x7E5E9A1B,
    0xEFC8763C,0x49BF7D88,0x78576715,0xDE206CA1,0x1B87522F,0xBDF0599B,0x8C184306,0x2A6F48B2,
    0xDC27385B,0x7A5033EF,0x4BB82972,0xEDCF22C6,0x28681C48,0x8E1F17FC,0xBFF70D61,0x198006D5,
    0x47ABD36E,0xE1DCD8DA,0xD034C247,0x7643C9F3,0xB3E4F77D,0x1593FCC9,0x247BE654,0x820CEDE0,
    0x74449D09,0xD23396BD,0xE3DB8C20,0x45AC8794,0x800BB91A,0x267CB2AE,0x1794A833,0xB1E3A387,
    0x20754FA0,0x86024414,0xB7EA5E89,0x119D553D,0xD43A6BB3,0x724D6007,0x43A57A9A,0xE5D2712E,
    0x139A01C7,0xB5ED0A73,0x840510EE,0x22721B5A,0xE7D525D4,0x41A22E60,0x704A34FD,0xD63D3F49,
    0xCC1D9F8B,0x6A6A943F,0x5B828EA2,0xFDF58516,0x3852BB98,0x9E25B02C,0xAFCDAAB1,0x09BAA105,
    0xFFF2D1EC,0x5985DA58,0x686DC0C5,0xCE1ACB71,0x0BBDF5FF,0xADCAFE4B,0x9C22E4D6,0x3A55EF62,
    0xABC30345,0x0DB408F1,0x3C5C126C,0x9A2B19D8,0x5F8C2756,0xF9FB2CE2,0xC813367F,0x6E643DCB,
    0x982C4D22,0x3E5B4696,0x0FB35C0B,0xA9C457BF,0x6C636931,0xCA146285,0xFBFC7818,0x5D8B73AC,
    0x03A0A617,0xA5D7ADA3,0x943FB73E,0x3248BC8A,0xF7EF8204,0x519889B0,0x6070932D,0xC6079899,
    0x304FE870,0x9638E3C4,0xA7D0F959,0x01A7F2ED,0xC400CC63,0x6277C7D7,0x539FDD4A,0xF5E8D6FE,
    0x647E3AD9,0xC209316D,0xF3E12BF0,0x55962044,0x90311ECA,0x3646157E,0x07AE0FE3,0xA1D90457,
    0x579174BE,0xF1E67F0A,0xC00E6597,0x66796E23,0xA3DE50AD,0x05A95B19,0x34414184,0x92364A30,
  },

  {
    0x00000000,0xCCAA009E,0x4225077D,0x8E8F07E3,0x844A0EFA,0x48E00E64,0xC66F0987,0x0AC50919,
    0xD3E51BB5,0x1F4F1B2B,0x91C01CC8,0x5D6A1C56,0x57AF154F,0x9B0515D1,0x158A1232,0xD92012AC,
    0x7CBB312B,0xB01131B5,0x3E9E3656,0xF23436C8,0xF8F13FD1,0x345B3F4F,0xBAD438AC,0x767E3832,
    0xAF5E2A9E,0x63F42A00,0xED7B2DE3,0x21D12D7D,0x2B142464,0xE7BE24FA,0x69312319,0xA59B2387,
    0xF9766256,0x35DC62C8,0xBB53652B,0x77F965B5,0x7D3C6CAC,0xB1966C32,0x3F196BD1,0xF3B36B4F,
    0x2A9379E3,0xE639797D,0x68B67E9E,0xA41C7E00,0xAED97719,0x62737787,0xECFC7064,0x205670FA,
    0x85CD537D,0x496753E3,0xC7E85400,0x0B42549E,0x01875D87,0xCD2D5D19,0x43A25AFA,0x8F085A64,
    0x562848C8,0x9A824856,0x140D4FB5,0xD8A74F2B,0xD2624632,0x1EC846AC,0x9047414F,0x5CED41D1,
    0x299DC2ED,0xE537C273,0x6BB8C590,0xA712C50E,0xADD7CC17,0x617DCC89,0xEFF2CB6A,0x2358CBF4,
    0xFA78D958,0x36D2D9C6,0xB85DDE25,0x74F7DEBB,0x7E32D7A2,0xB298D73C,0x3C17D0DF,0xF0BDD041,
    0x5526F3C6,0x998CF358,0x1703F4BB,0xDBA9F425,0xD16CFD3C,0x1DC6FDA2,0x9349FA41,0x5FE3FADF,
    0x86C3E873,0x4A69E8ED,0xC4E6EF0E,0x084CEF90,0x0289E689,0xCE23E617,0x40ACE1F4,0x8C06E16A,
    0xD0EBA0BB,0x1C41A025,0x92CEA7C6,0x5E64A758,0x54A1AE41,0x980BAEDF,0x1684A93C,0xDA2EA9A2,
    0x030EBB0E,0xCFA4BB90,0x412BBC73,0x8D81BCED,0x8744B5F4,0x4BEEB56A,0xC561B289,0x09CBB217,
    0xAC509190,0x60FA910E,0xEE7596ED,0x22DF9673,0x281A9F6A,0xE4B09FF4,0x6A3F9817,0xA6959889,
    0x7FB58A25,0xB31F8ABB,0x3D908D58,0xF13A8DC6,0xFBFF84DF,0x37558441,0xB9DA83A2,0x7570833C,
    0x533B85DA,0x9F918544,0x111E82A7,0xDDB48239,0xD7718B20,0x1BDB8BBE,0x95548C5D,0x59FE8CC3,
    0x80DE9E6F,0x4C749EF1,0xC2FB9912,0x0E51998C,0x04949095,0xC83E900B,0x46B197E8,0x8A1B9776,
    0x2F80B4F1,0xE32AB46F,0x6DA5B38C,0xA10FB312,0xABCABA0B,0x6760BA95,0xE9EFBD76,0x2545BDE8,
    0xFC65AF44,0x30CFAFDA,0xBE40A839,0x72EAA8A7,0x782FA1BE,0xB485A120,0x3A0AA6C3,0xF6A0A65D,
    0xAA4DE78C,0x66E7E712,0xE868E0F1,0x24C2E06F,0x2E07E976,0xE2ADE9E8,0x6C22EE0B,0xA088EE95,
    0x79A8FC39,0xB502FCA7,0x3B8DFB44,0xF727FBDA,0xFDE2F2C3,0x3148F25D,0xBFC7F5BE,0x736DF520,
    0xD6F6D6A7,0x1A5CD639,0x94D3D1DA,0x5879D144,0x52BCD85D,0x9E16D8C3,0x1099DF20,0xDC33DFBE,
    0x0513CD12,0xC9B9CD8C,0x4736CA6F,0x8B9CCAF1,0x8159C3E8,0x4DF3C376,0xC37CC495,0x0FD6C40B,
    0x7AA64737,0xB60C47A9,0x3883404A,0xF42940D4,0xFEEC49CD,0x32464953,0xBCC94EB0,0x70634E2E,
    0xA9435C82,0x65E95C1C,0xEB665BFF,0x27CC5B61,0x2D095278,0xE1A352E6,0x6F2C5505,0xA386559B,
    0x061D761C,0xCAB77682,0x44387161,0x889271FF,0x825778E6,0x4EFD7878,0xC0727F9B,0x0CD87F05,
    0xD5F86DA9,0x19526D37,0x97DD6AD4,0x5B776A4A,0x51B26353,0x9D1863CD,0x1397642E,0xDF3D64B0,
    0x83D02561,0x4F7A25FF,0xC1F5221C,0x0D5F2282,0x079A2B9B,0xCB302B05,0x45BF2CE6,0x89152C78,
    0x50353ED4,0x9C9F3E4A,0x121039A9,0xDEBA3937,0xD47F302E,0x18D530B0,0x965A3753,0x5AF037CD,
    0xFF6B144A,0x33C114D4,0xBD4E1337,0x71E413A9,0x7B211AB0,0xB78B1A2E,0x39041DCD,0xF5AE1D53,
    0x2C8E0FFF,0xE0240F61,0x6EAB0882,0xA201081C,0xA8C40105,0x646E019B,0xEAE10678,0x264B06E6,
  }
#endif // CRC32_USE_LOOKUP_TABLE_SLICING_BY_8 || CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
#ifdef CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
  // beyond this point only relevant for Slicing-by-16
  ,{
    0x00000000,0x177B1443,0x2EF62886,0x398D3CC5,0x5DEC510C,0x4A97454F,0x731A798A,0x64616DC9,
    0xBBD8A218,0xACA3B65B,0x952E8A9E,0x82559EDD,0xE634F314,0xF14FE757,0xC8C2DB92,0xDFB9CFD1,
    0xACC04271,0xBBBB5632,0x82366AF7,0x954D7EB4,0xF12C137D,0xE657073E,0xDFDA3BFB,0xC8A12FB8,
    0x1718E069,0x0063F42A,0x39EEC8EF,0x2E95DCAC,0x4AF4B165,0x5D8FA526,0x640299E3,0x73798DA0,
    0x82F182A3,0x958A96E0,0xAC07AA25,0xBB7CBE66,0xDF1DD3AF,0xC866C7EC,0xF1EBFB29,0xE690EF6A,
    0x392920BB,0x2E5234F8,0x17DF083D,0x00A41C7E,0x64C571B7,0x73BE65F4,0x4A335931,0x5D484D72,
    0x2E31C0D2,0x394AD491,0x00C7E854,0x17BCFC17,0x73DD91DE,0x64A6859D,0x5D2BB958,0x4A50AD1B,
    0x95E962CA,0x82927689,0xBB1F4A4C,0xAC645E0F,0xC80533C6,0xDF7E2785,0xE6F31B40,0xF1880F03,
    0xDE920307,0xC9E91744,0xF0642B81,0xE71F3FC2,0x837E520B,0x94054648,0xAD887A8D,0xBAF36ECE,
    0x654AA11F,0x7231B55C,0x4BBC8999,0x5CC79DDA,0x38A6F013,0x2FDDE450,0x1650D895,0x012BCCD6,
    0x72524176,0x65295535,0x5CA469F0,0x4BDF7DB3,0x2FBE107A,0x38C50439,0x014838FC,0x16332CBF,
    0xC98AE36E,0xDEF1F72D,0xE77CCBE8,0xF007DFAB,0x9466B262,0x831DA621,0xBA909AE4,0xADEB8EA7,
    0x5C6381A4,0x4B1895E7,0x7295A922,0x65EEBD61,0x018FD0A8,0x16F4C4EB,0x2F79F82E,0x3802EC6D,
    0xE7BB23BC,0xF0C037FF,0xC94D0B3A,0xDE361F79,0xBA5772B0,0xAD2C66F3,0x94A15A36,0x83DA4E75,
    0xF0A3C3D5,0xE7D8D796,0xDE55EB53,0xC92EFF10,0xAD4F92D9,0xBA34869A,0x83B9BA5F,0x94C2AE1C,
    0x4B7B61CD,0x5C00758E,0x658D494B,0x72F65D08,0x169730C1,0x01EC2482,0x38611847,0x2F1A0C04,
    0x6655004F,0x712E140C,0x48A328C9,0x5FD83C8A,0x3BB95143,0x2CC24500,0x154F79C5,0x02346D86,
    0xDD8DA257,0xCAF6B614,0xF37B8AD1,0xE4009E92,0x8061F35B,0x971AE718,0xAE97DBDD,0xB9ECCF9E,
    0xCA95423E,0xDDEE567D,0xE4636AB8,0xF3187EFB,0x97791332,0x80020771,0xB98F3BB4,0xAEF42FF7,
    0x714DE026,0x6636F465,0x5FBBC8A0,0x48C0DCE3,0x2CA1B12A,0x3BDAA569,0x025799AC,0x152C8DEF,
    0xE4A482EC,0xF3DF96AF,0xCA52AA6A,0xDD29BE29,0xB948D3E0,0xAE33C7A3,0x97BEFB66,0x80C5EF25,
    0x5F7C20F4,0x480734B7,0x718A0872,0x66F11C31,0x029071F8,0x15EB65BB,0x2C66597E,0x3B1D4D3D,
    0x4864C09D,0x5F1FD4DE,0x6692E81B,0x71E9FC58,0x15889191,0x02F385D2,0x3B7EB917,0x2C05AD54,
    0xF3BC6285,0xE4C776C6,0xDD4A4A03,0xCA315E40,0xAE503389,0xB92B27CA,0x80A61B0F,0x97DD0F4C,
    0xB8C70348,0xAFBC170B,0x96312BCE,0x814A3F8D,0xE52B5244,0xF2504607,0xCBDD7AC2,0xDCA66E81,
    0x031FA150,0x1464B513,0x2DE989D6,0x3A929D95,0x5EF3F05C,0x4988E41F,0x7005D8DA,0x677ECC99,
    0x14074139,0x037C557A,0x3AF169BF,0x2D8A7DFC,0x49EB1035,0x5E900476,0x671D38B3,0x70662CF0,
    0xAFDFE321,0xB8A4F762,0x8129CBA7,0x9652DFE4,0xF233B22D,0xE548A66E,0xDCC59AAB,0xCBBE8EE8,
    0x3A3681EB,0x2D4D95A8,0x14C0A96D,0x03BBBD2E,0x67DAD0E7,0x70A1C4A4,0x492CF861,0x5E57EC22,
    0x81EE23F3,0x969537B0,0xAF180B75,0xB8631F36,0xDC0272FF,0xCB7966BC,0xF2F45A79,0xE58F4E3A,
    0x96F6C39A,0x818DD7D9,0xB800EB1C,0xAF7BFF5F,0xCB1A9296,0xDC6186D5,0xE5ECBA10,0xF297AE53,
    0x2D2E6182,0x3A5575C1,0x03D84904,0x14A35D47,0x70C2308E,0x67B924CD,0x5E341808,0x494F0C4B,
  },

  {
    0x00000000,0xEFC26B3E,0x04F5D03D,0xEB37BB03,0x09EBA07A,0xE629CB44,0x0D1E7047,0xE2DC1B79,
    0x13D740F4,0xFC152BCA,0x172290C9,0xF8E0FBF7,0x1A3CE08E,0xF5FE8BB0,0x1EC930B3,0xF10B5B8D,
    0x27AE81E8,0xC86CEAD6,0x235B51D5,0xCC993AEB,0x2E452192,0xC1874AAC,0x2AB0F1AF,0xC5729A91,
    0x3479C11C,0xDBBBAA22,0x308C1121,0xDF4E7A1F,0x3D926166,0xD2500A58,0x3967B15B,0xD6A5DA65,
    0x4F5D03D0,0xA09F68EE,0x4BA8D3ED,0xA46AB8D3,0x46B6A3AA,0xA974C894,0x42437397,0xAD8118A9,
    0x5C8A4324,0xB348281A,0x587F9319,0xB7BDF827,0x5561E35E,0xBAA38860,0x51943363,0xBE56585D,
    0x68F38238,0x8731E906,0x6C065205,0x83C4393B,0x61182242,0x8EDA497C,0x65EDF27F,0x8A2F9941,
    0x7B24C2CC,0x94E6A9F2,0x7FD112F1,0x901379CF,0x72CF62B6,0x9D0D0988,0x763AB28B,0x99F8D9B5,
    0x9EBA07A0,0x71786C9E,0x9A4FD79D,0x758DBCA3,0x9751A7DA,0x7893CCE4,0x93A477E7,0x7C661CD9,
    0x8D6D4754,0x62AF2C6A,0x89989769,0x665AFC57,0x8486E72E,0x6B448C10,0x80733713,0x6FB15C2D,
    0xB9148648,0x56D6ED76,0xBDE15675,0x52233D4B,0xB0FF2632,0x5F3D4D0C,0xB40AF60F,0x5BC89D31,
    0xAAC3C6BC,0x4501AD82,0xAE361681,0x41F47DBF,0xA32866C6,0x4CEA0DF8,0xA7DDB6FB,0x481FDDC5,
    0xD1E70470,0x3E256F4E,0xD512D44D,0x3AD0BF73,0xD80CA40A,0x37CECF34,0xDCF97437,0x333B1F09,
    0xC2304484,0x2DF22FBA,0xC6C594B9,0x2907FF87,0xCBDBE4FE,0x24198FC0,0xCF2E34C3,0x20EC5FFD,
    0xF6498598,0x198BEEA6,0xF2BC55A5,0x1D7E3E9B,0xFFA225E2,0x10604EDC,0xFB57F5DF,0x14959EE1,
    0xE59EC56C,0x0A5CAE52,0xE16B1551,0x0EA97E6F,0xEC756516,0x03B70E28,0xE880B52B,0x0742DE15,
    0xE6050901,0x09C7623F,0xE2F0D93C,0x0D32B202,0xEFEEA97B,0x002CC245,0xEB1B7946,0x04D91278,
    0xF5D249F5,0x1A1022CB,0xF12799C8,0x1EE5F2F6,0xFC39E98F,0x13FB82B1,0xF8CC39B2,0x170E528C,
    0xC1AB88E9,0x2E69E3D7,0xC55E58D4,0x2A9C33EA,0xC8402893,0x278243AD,0xCCB5F8AE,0x23779390,
    0xD27CC81D,0x3DBEA323,0xD6891820,0x394B731E,0xDB976867,0x34550359,0xDF62B85A,0x30A0D364,
    0xA9580AD1,0x469A61EF,0xADADDAEC,0x426FB1D2,0xA0B3AAAB,0x4F71C195,0xA4467A96,0x4B8411A8,
    0xBA8F4A25,0x554D211B,0xBE7A9A18,0x51B8F126,0xB364EA5F,0x5CA68161,0xB7913A62,0x5853515C,
    0x8EF68B39,0x6134E007,0x8A035B04,0x65C1303A,0x871D2B43,0x68DF407D,0x83E8FB7E,0x6C2A9040,
    0x9D21CBCD,0x72E3A0F3,0x99D41BF0,0x761670CE,0x94CA6BB7,0x7B080089,0x903FBB8A,0x7FFDD0B4,
    0x78BF0EA1,0x977D659F,0x7C4ADE9C,0x9388B5A2,0x7154AEDB,0x9E96C5E5,0x75A17EE6,0x9A6315D8,
    0x6B684E55,0x84AA256B,0x6F9D9E68,0x805FF556,0x6283EE2F,0x8D418511,0x66763E12,0x89B4552C,
    0x5F118F49,0xB0D3E477,0x5BE45F74,0xB426344A,0x56FA2F33,0xB938440D,0x520FFF0E,0xBDCD9430,
    0x4CC6CFBD,0xA304A483,0x48331F80,0xA7F174BE,0x452D6FC7,0xAAEF04F9,0x41D8BFFA,0xAE1AD4C4,
    0x37E20D71,0xD820664F,0x3317DD4C,0xDCD5B672,0x3E09AD0B,0xD1CBC635,0x3AFC7D36,0xD53E1608,
    0x24354D85,0xCBF726BB,0x20C09DB8,0xCF02F686,0x2DDEEDFF,0xC21C86C1,0x292B3DC2,0xC6E956FC,
    0x104C8C99,0xFF8EE7A7,0x14B95CA4,0xFB7B379A,0x19A72CE3,0xF66547DD,0x1D52FCDE,0xF29097E0,
    0x039BCC6D,0xEC59A753,0x076E1C50,0xE8AC776E,0x0A706C17,0xE5B20729,0x0E85BC2A,0xE147D714,
  },

  {
    0x00000000,0xC18EDFC0,0x586CB9C1,0x99E26601,0xB0D97382,0x7157AC42,0xE8B5CA43,0x293B1583,
    0xBAC3E145,0x7B4D3E85,0xE2AF5884,0x23218744,0x0A1A92C7,0xCB944D07,0x52762B06,0x93F8F4C6,
    0xAEF6C4CB,0x6F781B0B,0xF69A7D0A,0x3714A2CA,0x1E2FB749,0xDFA16889,0x46430E88,0x87CDD148,
    0x1435258E,0xD5BBFA4E,0x4C599C4F,0x8DD7438F,0xA4EC560C,0x656289CC,0xFC80EFCD,0x3D0E300D,
    0x869C8FD7,0x47125017,0xDEF03616,0x1F7EE9D6,0x3645FC55,0xF7CB2395,0x6E294594,0xAFA79A54,
    0x3C5F6E92,0xFDD1B152,0x6433D753,0xA5BD0893,0x8C861D10,0x4D08C2D0,0xD4EAA4D1,0x15647B11,
    0x286A4B1C,0xE9E494DC,0x7006F2DD,0xB1882D1D,0x98B3389E,0x593DE75E,0xC0DF815F,0x01515E9F,
    0x92A9AA59,0x53277599,0xCAC51398,0x0B4BCC58,0x2270D9DB,0xE3FE061B,0x7A1C601A,0xBB92BFDA,
    0xD64819EF,0x17C6C62F,0x8E24A02E,0x4FAA7FEE,0x66916A6D,0xA71FB5AD,0x3EFDD3AC,0xFF730C6C,
    0x6C8BF8AA,0xAD05276A,0x34E7416B,0xF5699EAB,0xDC528B28,0x1DDC54E8,0x843E32E9,0x45B0ED29,
    0x78BEDD24,0xB93002E4,0x20D264E5,0xE15CBB25,0xC867AEA6,0x09E97166,0x900B1767,0x5185C8A7,
    0xC27D3C61,0x03F3E3A1,0x9A1185A0,0x5B9F5A60,0x72A44FE3,0xB32A9023,0x2AC8F622,0xEB4629E2,
    0x50D49638,0x915A49F8,0x08B82FF9,0xC936F039,0xE00DE5BA,0x21833A7A,0xB8615C7B,0x79EF83BB,
    0xEA17777D,0x2B99A8BD,0xB27BCEBC,0x73F5117C,0x5ACE04FF,0x9B40DB3F,0x02A2BD3E,0xC32C62FE,
    0xFE2252F3,0x3FAC8D33,0xA64EEB32,0x67C034F2,0x4EFB2171,0x8F75FEB1,0x169798B0,0xD7194770,
    0x44E1B3B6,0x856F6C76,0x1C8D0A77,0xDD03D5B7,0xF438C034,0x35B61FF4,0xAC5479F5,0x6DDAA635,
    0x77E1359F,0xB66FEA5F,0x2F8D8C5E,0xEE03539E,0xC738461D,0x06B699DD,0x9F54FFDC,0x5EDA201C,
    0xCD22D4DA,0x0CAC0B1A,0x954E6D1B,0x54C0B2DB,0x7DFBA758,0xBC757898,0x25971E99,0xE419C159,
    0xD917F154,0x18992E94,0x817B4895,0x40F59755,0x69CE82D6,0xA8405D16,0x31A23B17,0xF02CE4D7,
    0x63D41011,0xA25ACFD1,0x3BB8A9D0,0xFA367610,0xD30D6393,0x1283BC53,0x8B61DA52,0x4AEF0592,
    0xF17DBA48,0x30F36588,0xA9110389,0x689FDC49,0x41A4C9CA,0x802A160A,0x19C8700B,0xD846AFCB,
    0x4BBE5B0D,0x8A3084CD,0x13D2E2CC,0xD25C3D0C,0xFB67288F,0x3AE9F74F,0xA30B914E,0x62854E8E,
    0x5F8B7E83,0x9E05A143,0x07E7C742,0xC6691882,0xEF520D01,0x2EDCD2C1,0xB73EB4C0,0x76B06B00,
    0xE5489FC6,0x24C64006,0xBD242607,0x7CAAF9C7,0x5591EC44,0x941F3384,0x0DFD5585,0xCC738A45,
    0xA1A92C70,0x6027F3B0,0xF9C595B1,0x384B4A71,0x11705FF2,0xD0FE8032,0x491CE633,0x889239F3,
    0x1B6ACD35,0xDAE412F5,0x430674F4,0x8288AB34,0xABB3BEB7,0x6A3D6177,0xF3DF0776,0x3251D8B6,
    0x0F5FE8BB,0xCED1377B,0x5733517A,0x96BD8EBA,0xBF869B39,0x7E0844F9,0xE7EA22F8,0x2664FD38,
    0xB59C09FE,0x7412D63E,0xEDF0B03F,0x2C7E6FFF,0x05457A7C,0xC4CBA5BC,0x5D29C3BD,0x9CA71C7D,
    0x2735A3A7,0xE6BB7C67,0x7F591A66,0xBED7C5A6,0x97ECD025,0x56620FE5,0xCF8069E4,0x0E0EB624,
    0x9DF642E2,0x5C789D22,0xC59AFB23,0x041424E3,0x2D2F3160,0xECA1EEA0,0x754388A1,0xB4CD5761,
    0x89C3676C,0x484DB8AC,0xD1AFDEAD,0x1021016D,0x391A14EE,0xF894CB2E,0x6176AD2F,0xA0F872EF,
    0x33008629,0xF28E59E9,0x6B6C3FE8,0xAAE2E028,0x83D9F5AB,0x42572A6B,0xDBB54C6A,0x1A3B93AA,
  },

  {
    0x00000000,0x9BA54C6F,0xEC3B9E9F,0x779ED2F0,0x03063B7F,0x98A37710,0xEF3DA5E0,0x7498E98F,
    0x060C76FE,0x9DA93A91,0xEA37E861,0x7192A40E,0x050A4D81,0x9EAF01EE,0xE931D31E,0x72949F71,
    0x0C18EDFC,0x97BDA193,0xE0237363,0x7B863F0C,0x0F1ED683,0x94BB9AEC,0xE325481C,0x78800473,
    0x0A149B02,0x91B1D76D,0xE62F059D,0x7D8A49F2,0x0912A07D,0x92B7EC12,0xE5293EE2,0x7E8C728D,
    0x1831DBF8,0x83949797,0xF40A4567,0x6FAF0908,0x1B37E087,0x8092ACE8,0xF70C7E18,0x6CA93277,
    0x1E3DAD06,0x8598E169,0xF2063399,0x69A37FF6,0x1D3B9679,0x869EDA16,0xF10008E6,0x6AA54489,
    0x14293604,0x8F8C7A6B,0xF812A89B,0x63B7E4F4,0x172F0D7B,0x8C8A4114,0xFB1493E4,0x60B1DF8B,
    0x122540FA,0x89800C95,0xFE1EDE65,0x65BB920A,0x11237B85,0x8A8637EA,0xFD18E51A,0x66BDA975,
    0x3063B7F0,0xABC6FB9F,0xDC58296F,0x47FD6500,0x33658C8F,0xA8C0C0E0,0xDF5E1210,0x44FB5E7F,
    0x366FC10E,0xADCA8D61,0xDA545F91,0x41F113FE,0x3569FA71,0xAECCB61E,0xD95264EE,0x42F72881,
    0x3C7B5A0C,0xA7DE1663,0xD040C493,0x4BE588FC,0x3F7D6173,0xA4D82D1C,0xD346FFEC,0x48E3B383,
    0x3A772CF2,0xA1D2609D,0xD64CB26D,0x4DE9FE02,0x3971178D,0xA2D45BE2,0xD54A8912,0x4EEFC57D,
    0x28526C08,0xB3F72067,0xC469F297,0x5FCCBEF8,0x2B545777,0xB0F11B18,0xC76FC9E8,0x5CCA8587,
    0x2E5E1AF6,0xB5FB5699,0xC2658469,0x59C0C806,0x2D582189,0xB6FD6DE6,0xC163BF16,0x5AC6F379,
    0x244A81F4,0xBFEFCD9B,0xC8711F6B,0x53D45304,0x274CBA8B,0xBCE9F6E4,0xCB772414,0x50D2687B,
    0x2246F70A,0xB9E3BB65,0xCE7D6995,0x55D825FA,0x2140CC75,0xBAE5801A,0xCD7B52EA,0x56DE1E85,
    0x60C76FE0,0xFB62238F,0x8CFCF17F,0x1759BD10,0x63C1549F,0xF86418F0,0x8FFACA00,0x145F866F,
    0x66CB191E,0xFD6E5571,0x8AF08781,0x1155CBEE,0x65CD2261,0xFE686E0E,0x89F6BCFE,0x1253F091,
    0x6CDF821C,0xF77ACE73,0x80E41C83,0x1B4150EC,0x6FD9B963,0xF47CF50C,0x83E227FC,0x18476B93,
    0x6AD3F4E2,0xF176B88D,0x86E86A7D,0x1D4D2612,0x69D5CF9D,0xF27083F2,0x85EE5102,0x1E4B1D6D,
    0x78F6B418,0xE353F877,0x94CD2A87,0x0F6866E8,0x7BF08F67,0xE055C308,0x97CB11F8,0x0C6E5D97,
    0x7EFAC2E6,0xE55F8E89,0x92C15C79,0x09641016,0x7DFCF999,0xE659B5F6,0x91C76706,0x0A622B69,
    0x74EE59E4,0xEF4B158B,0x98D5C77B,0x03708B14,0x77E8629B,0xEC4D2EF4,0x9BD3FC04,0x0076B06B,
    0x72E22F1A,0xE9476375,0x9ED9B185,0x057CFDEA,0x71E41465,0xEA41580A,0x9DDF8AFA,0x067AC695,
    0x50A4D810,0xCB01947F,0xBC9F468F,0x273A0AE0,0x53A2E36F,0xC807AF00,0xBF997DF0,0x243C319F,
    0x56A8AEEE,0xCD0DE281,0xBA933071,0x21367C1E,0x55AE9591,0xCE0BD9FE,0xB9950B0E,0x22304761,
    0x5CBC35EC,0xC7197983,0xB087AB73,0x2B22E71C,0x5FBA0E93,0xC41F42FC,0xB381900C,0x2824DC63,
    0x5AB04312,0xC1150F7D,0xB68BDD8D,0x2D2E91E2,0x59B6786D,0xC2133402,0xB58DE6F2,0x2E28AA9D,
    0x489503E8,0xD3304F87,0xA4AE9D77,0x3F0BD118,0x4B933897,0xD03674F8,0xA7A8A608,0x3C0DEA67,
    0x4E997516,0xD53C3979,0xA2A2EB89,0x3907A7E6,0x4D9F4E69,0xD63A0206,0xA1A4D0F6,0x3A019C99,
    0x448DEE14,0xDF28A27B,0xA8B6708B,0x33133CE4,0x478BD56B,0xDC2E9904,0xABB04BF4,0x3015079B,
    0x428198EA,0xD924D485,0xAEBA0675,0x351F4A1A,0x4187A395,0xDA22EFFA,0xADBC3D0A,0x36197165,
  },

  {
    0x00000000,0xDD96D985,0x605CB54B,0xBDCA6CCE,0xC0B96A96,0x1D2FB313,0xA0E5DFDD,0x7D730658,
    0x5A03D36D,0x87950AE8,0x3A5F6626,0xE7C9BFA3,0x9ABAB9FB,0x472C607E,0xFAE60CB0,0x2770D535,
    0xB407A6DA,0x69917F5F,0xD45B1391,0x09CDCA14,0x74BECC4C,0xA92815C9,0x14E27907,0xC974A082,
    0xEE0475B7,0x3392AC32,0x8E58C0FC,0x53CE1979,0x2EBD1F21,0xF32BC6A4,0x4EE1AA6A,0x937773EF,
    0xB37E4BF5,0x6EE89270,0xD322FEBE,0x0EB4273B,0x73C72163,0xAE51F8E6,0x139B9428,0xCE0D4DAD,
    0xE97D9898,0x34EB411D,0x89212DD3,0x54B7F456,0x29C4F20E,0xF4522B8B,0x49984745,0x940E9EC0,
    0x0779ED2F,0xDAEF34AA,0x67255864,0xBAB381E1,0xC7C087B9,0x1A565E3C,0xA79C32F2,0x7A0AEB77,
    0x5D7A3E42,0x80ECE7C7,0x3D268B09,0xE0B0528C,0x9DC354D4,0x40558D51,0xFD9FE19F,0x2009381A,
    0xBD8D91AB,0x601B482E,0xDDD124E0,0x0047FD65,0x7D34FB3D,0xA0A222B8,0x1D684E76,0xC0FE97F3,
    0xE78E42C6,0x3A189B43,0x87D2F78D,0x5A442E08,0x27372850,0xFAA1F1D5,0x476B9D1B,0x9AFD449E,
    0x098A3771,0xD41CEEF4,0x69D6823A,0xB4405BBF,0xC9335DE7,0x14A58462,0xA96FE8AC,0x74F93129,
    0x5389E41C,0x8E1F3D99,0x33D55157,0xEE4388D2,0x93308E8A,0x4EA6570F,0xF36C3BC1,0x2EFAE244,
    0x0EF3DA5E,0xD36503DB,0x6EAF6F15,0xB339B690,0xCE4AB0C8,0x13DC694D,0xAE160583,0x7380DC06,
    0x54F00933,0x8966D0B6,0x34ACBC78,0xE93A65FD,0x944963A5,0x49DFBA20,0xF415D6EE,0x29830F6B,
    0xBAF47C84,0x6762A501,0xDAA8C9CF,0x073E104A,0x7A4D1612,0xA7DBCF97,0x1A11A359,0xC7877ADC,
    0xE0F7AFE9,0x3D61766C,0x80AB1AA2,0x5D3DC327,0x204EC57F,0xFDD81CFA,0x40127034,0x9D84A9B1,
    0xA06A2517,0x7DFCFC92,0xC036905C,0x1DA049D9,0x60D34F81,0xBD459604,0x008FFACA,0xDD19234F,
    0xFA69F67A,0x27FF2FFF,0x9A354331,0x47A39AB4,0x3AD09CEC,0xE7464569,0x5A8C29A7,0x871AF022,
    0x146D83CD,0xC9FB5A48,0x74313686,0xA9A7EF03,0xD4D4E95B,0x094230DE,0xB4885C10,0x691E8595,
    0x4E6E50A0,0x93F88925,0x2E32E5EB,0xF3A43C6E,0x8ED73A36,0x5341E3B3,0xEE8B8F7D,0x331D56F8,
    0x13146EE2,0xCE82B767,0x7348DBA9,0xAEDE022C,0xD3AD0474,0x0E3BDDF1,0xB3F1B13F,0x6E6768BA,
    0x4917BD8F,0x9481640A,0x294B08C4,0xF4DDD141,0x89AED719,0x54380E9C,0xE9F26252,0x3464BBD7,
    0xA713C838,0x7A8511BD,0xC74F7D73,0x1AD9A4F6,0x67AAA2AE,0xBA3C7B2B,0x07F617E5,0xDA60CE60,
    0xFD101B55,0x2086C2D0,0x9D4CAE1E,0x40DA779B,0x3DA971C3,0xE03FA846,0x5DF5C488,0x80631D0D,
    0x1DE7B4BC,0xC0716D39,0x7DBB01F7,0xA02DD872,0xDD5EDE2A,0x00C807AF,0xBD026B61,0x6094B2E4,
    0x47E467D1,0x9A72BE54,0x27B8D29A,0xFA2E0B1F,0x875D0D47,0x5ACBD4C2,0xE701B80C,0x3A976189,
    0xA9E01266,0x7476CBE3,0xC9BCA72D,0x142A7EA8,0x695978F0,0xB4CFA175,0x0905CDBB,0xD493143E,
    0xF3E3C10B,0x2E75188E,0x93BF7440,0x4E29ADC5,0x335AAB9D,0xEECC7218,0x53061ED6,0x8E90C753,
    0xAE99FF49,0x730F26CC,0xCEC54A02,0x13539387,0x6E2095DF,0xB3B64C5A,0x0E7C2094,0xD3EAF911,
    0xF49A2C24,0x290CF5A1,0x94C6996F,0x495040EA,0x342346B2,0xE9B59F37,0x547FF3F9,0x89E92A7C,
    0x1A9E5993,0xC7088016,0x7AC2ECD8,0xA754355D,0xDA273305,0x07B1EA80,0xBA7B864E,0x67ED5FCB,
    0x409D8AFE,0x9D0B537B,0x20C13FB5,0xFD57E630,0x8024E068,0x5DB239ED,0xE0785523,0x3DEE8CA6,
  },

  {
    0x00000000,0x9D0FE176,0xE16EC4AD,0x7C6125DB,0x19AC8F1B,0x84A36E6D,0xF8C24BB6,0x65CDAAC0,
    0x33591E36,0xAE56FF40,0xD237DA9B,0x4F383BED,0x2AF5912D,0xB7FA705B,0xCB9B5580,0x5694B4F6,
    0x66B23C6C,0xFBBDDD1A,0x87DCF8C1,0x1AD319B7,0x7F1EB377,0xE2115201,0x9E7077DA,0x037F96AC,
    0x55EB225A,0xC8E4C32C,0xB485E6F7,0x298A0781,0x4C47AD41,0xD1484C37,0xAD2969EC,0x3026889A,
    0xCD6478D8,0x506B99AE,0x2C0ABC75,0xB1055D03,0xD4C8F7C3,0x49C716B5,0x35A6336E,0xA8A9D218,
    0xFE3D66EE,0x63328798,0x1F53A243,0x825C4335,0xE791E9F5,0x7A9E0883,0x06FF2D58,0x9BF0CC2E,
    0xABD644B4,0x36D9A5C2,0x4AB88019,0xD7B7616F,0xB27ACBAF,0x2F752AD9,0x53140F02,0xCE1BEE74,
    0x988F5A82,0x0580BBF4,0x79E19E2F,0xE4EE7F59,0x8123D599,0x1C2C34EF,0x604D1134,0xFD42F042,
    0x41B9F7F1,0xDCB61687,0xA0D7335C,0x3DD8D22A,0x581578EA,0xC51A999C,0xB97BBC47,0x24745D31,
    0x72E0E9C7,0xEFEF08B1,0x938E2D6A,0x0E81CC1C,0x6B4C66DC,0xF64387AA,0x8A22A271,0x172D4307,
    0x270BCB9D,0xBA042AEB,0xC6650F30,0x5B6AEE46,0x3EA74486,0xA3A8A5F0,0xDFC9802B,0x42C6615D,
    0x1452D5AB,0x895D34DD,0xF53C1106,0x6833F070,0x0DFE5AB0,0x90F1BBC6,0xEC909E1D,0x719F7F6B,
    0x8CDD8F29,0x11D26E5F,0x6DB34B84,0xF0BCAAF2,0x95710032,0x087EE144,0x741FC49F,0xE91025E9,
    0xBF84911F,0x228B7069,0x5EEA55B2,0xC3E5B4C4,0xA6281E04,0x3B27FF72,0x4746DAA9,0xDA493BDF,
    0xEA6FB345,0x77605233,0x0B0177E8,0x960E969E,0xF3C33C5E,0x6ECCDD28,0x12ADF8F3,0x8FA21985,
    0xD936AD73,0x44394C05,0x385869DE,0xA55788A8,0xC09A2268,0x5D95C31E,0x21F4E6C5,0xBCFB07B3,
    0x8373EFE2,0x1E7C0E94,0x621D2B4F,0xFF12CA39,0x9ADF60F9,0x07D0818F,0x7BB1A454,0xE6BE4522,
    0xB02AF1D4,0x2D2510A2,0x51443579,0xCC4BD40F,0xA9867ECF,0x34899FB9,0x48E8BA62,0xD5E75B14,
    0xE5C1D38E,0x78CE32F8,0x04AF1723,0x99A0F655,0xFC6D5C95,0x6162BDE3,0x1D039838,0x800C794E,
    0xD698CDB8,0x4B972CCE,0x37F60915,0xAAF9E863,0xCF3442A3,0x523BA3D5,0x2E5A860E,0xB3556778,
    0x4E17973A,0xD318764C,0xAF795397,0x3276B2E1,0x57BB1821,0xCAB4F957,0xB6D5DC8C,0x2BDA3DFA,
    0x7D4E890C,0xE041687A,0x9C204DA1,0x012FACD7,0x64E20617,0xF9EDE761,0x858CC2BA,0x188323CC,
    0x28A5AB56,0xB5AA4A20,0xC9CB6FFB,0x54C48E8D,0x3109244D,0xAC06C53B,0xD067E0E0,0x4D680196,
    0x1BFCB560,0x86F35416,0xFA9271CD,0x679D90BB,0x02503A7B,0x9F5FDB0D,0xE33EFED6,0x7E311FA0,
    0xC2CA1813,0x5FC5F965,0x23A4DCBE,0xBEAB3DC8,0xDB669708,0x4669767E,0x3A0853A5,0xA707B2D3,
    0xF1930625,0x6C9CE753,0x10FDC288,0x8DF223FE,0xE83F893E,0x75306848,0x09514D93,0x945EACE5,
    0xA478247F,0x3977C509,0x4516E0D2,0xD81901A4,0xBDD4AB64,0x20DB4A12,0x5CBA6FC9,0xC1B58EBF,
    0x97213A49,0x0A2EDB3F,0x764FFEE4,0xEB401F92,0x8E8DB552,0x13825424,0x6FE371FF,0xF2EC9089,
    0x0FAE60CB,0x92A181BD,0xEEC0A466,0x73CF4510,0x1602EFD0,0x8B0D0EA6,0xF76C2B7D,0x6A63CA0B,
    0x3CF77EFD,0xA1F89F8B,0xDD99BA50,0x40965B26,0x255BF1E6,0xB8541090,0xC435354B,0x593AD43D,
    0x691C5CA7,0xF413BDD1,0x8872980A,0x157D797C,0x70B0D3BC,0xEDBF32CA,0x91DE1711,0x0CD1F667,
    0x5A454291,0xC74AA3E7,0xBB2B863C,0x2624674A,0x43E9CD8A,0xDEE62CFC,0xA2870927,0x3F88E851,
  },

  {
    0x00000000,0xB9FBDBE8,0xA886B191,0x117D6A79,0x8A7C6563,0x3387BE8B,0x22FAD4F2,0x9B010F1A,
    0xCF89CC87,0x7672176F,0x670F7D16,0xDEF4A6FE,0x45F5A9E4,0xFC0E720C,0xED731875,0x5488C39D,
    0x44629F4F,0xFD9944A7,0xECE42EDE,0x551FF536,0xCE1EFA2C,0x77E521C4,0x66984BBD,0xDF639055,
    0x8BEB53C8,0x32108820,0x236DE259,0x9A9639B1,0x019736AB,0xB86CED43,0xA911873A,0x10EA5CD2,
    0x88C53E9E,0x313EE576,0x20438F0F,0x99B854E7,0x02B95BFD,0xBB428015,0xAA3FEA6C,0x13C43184,
    0x474CF219,0xFEB729F1,0xEFCA4388,0x56319860,0xCD30977A,0x74CB4C92,0x65B626EB,0xDC4DFD03,
    0xCCA7A1D1,0x755C7A39,0x64211040,0xDDDACBA8,0x46DBC4B2,0xFF201F5A,0xEE5D7523,0x57A6AECB,
    0x032E6D56,0xBAD5B6BE,0xABA8DCC7,0x1253072F,0x89520835,0x30A9D3DD,0x21D4B9A4,0x982F624C,
    0xCAFB7B7D,0x7300A095,0x627DCAEC,0xDB861104,0x40871E1E,0xF97CC5F6,0xE801AF8F,0x51FA7467,
    0x0572B7FA,0xBC896C12,0xADF4066B,0x140FDD83,0x8F0ED299,0x36F50971,0x27886308,0x9E73B8E0,
    0x8E99E432,0x37623FDA,0x261F55A3,0x9FE48E4B,0x04E58151,0xBD1E5AB9,0xAC6330C0,0x1598EB28,
    0x411028B5,0xF8EBF35D,0xE9969924,0x506D42CC,0xCB6C4DD6,0x7297963E,0x63EAFC47,0xDA1127AF,
    0x423E45E3,0xFBC59E0B,0xEAB8F472,0x53432F9A,0xC8422080,0x71B9FB68,0x60C49111,0xD93F4AF9,
    0x8DB78964,0x344C528C,0x253138F5,0x9CCAE31D,0x07CBEC07,0xBE3037EF,0xAF4D5D96,0x16B6867E,
    0x065CDAAC,0xBFA70144,0xAEDA6B3D,0x1721B0D5,0x8C20BFCF,0x35DB6427,0x24A60E5E,0x9D5DD5B6,
    0xC9D5162B,0x702ECDC3,0x6153A7BA,0xD8A87C52,0x43A97348,0xFA52A8A0,0xEB2FC2D9,0x52D41931,
    0x4E87F0BB,0xF77C2B53,0xE601412A,0x5FFA9AC2,0xC4FB95D8,0x7D004E30,0x6C7D2449,0xD586FFA1,
    0x810E3C3C,0x38F5E7D4,0x29888DAD,0x90735645,0x0B72595F,0xB28982B7,0xA3F4E8CE,0x1A0F3326,
    0x0AE56FF4,0xB31EB41C,0xA263DE65,0x1B98058D,0x80990A97,0x3962D17F,0x281FBB06,0x91E460EE,
    0xC56CA373,0x7C97789B,0x6DEA12E2,0xD411C90A,0x4F10C610,0xF6EB1DF8,0xE7967781,0x5E6DAC69,
    0xC642CE25,0x7FB915CD,0x6EC47FB4,0xD73FA45C,0x4C3EAB46,0xF5C570AE,0xE4B81AD7,0x5D43C13F,
    0x09CB02A2,0xB030D94A,0xA14DB333,0x18B668DB,0x83B767C1,0x3A4CBC29,0x2B31D650,0x92CA0DB8,
    0x8220516A,0x3BDB8A82,0x2AA6E0FB,0x935D3B13,0x085C3409,0xB1A7EFE1,0xA0DA8598,0x19215E70,
    0x4DA99DED,0xF4524605,0xE52F2C7C,0x5CD4F794,0xC7D5F88E,0x7E2E2366,0x6F53491F,0xD6A892F7,
    0x847C8BC6,0x3D87502E,0x2CFA3A57,0x9501E1BF,0x0E00EEA5,0xB7FB354D,0xA6865F34,0x1F7D84DC,
    0x4BF54741,0xF20E9CA9,0xE373F6D0,0x5A882D38,0xC1892222,0x7872F9CA,0x690F93B3,0xD0F4485B,
    0xC01E1489,0x79E5CF61,0x6898A518,0xD1637EF0,0x4A6271EA,0xF399AA02,0xE2E4C07B,0x5B1F1B93,
    0x0F97D80E,0xB66C03E6,0xA711699F,0x1EEAB277,0x85EBBD6D,0x3C106685,0x2D6D0CFC,0x9496D714,
    0x0CB9B558,0xB5426EB0,0xA43F04C9,0x1DC4DF21,0x86C5D03B,0x3F3E0BD3,0x2E4361AA,0x97B8BA42,
    0xC33079DF,0x7ACBA237,0x6BB6C84E,0xD24D13A6,0x494C1CBC,0xF0B7C754,0xE1CAAD2D,0x583176C5,
    0x48DB2A17,0xF120F1FF,0xE05D9B86,0x59A6406E,0xC2A74F74,0x7B5C949C,0x6A21FEE5,0xD3DA250D,
    0x8752E690,0x3EA93D78,0x2FD45701,0x962F8CE9,0x0D2E83F3,0xB4D5581B,0xA5A83262,0x1C53E98A,
  },

  {
    0x00000000,0xAE689191,0x87A02563,0x29C8B4F2,0xD4314C87,0x7A59DD16,0x539169E4,0xFDF9F875,
    0x73139F4F,0xDD7B0EDE,0xF4B3BA2C,0x5ADB2BBD,0xA722D3C8,0x094A4259,0x2082F6AB,0x8EEA673A,
    0xE6273E9E,0x484FAF0F,0x61871BFD,0xCFEF8A6C,0x32167219,0x9C7EE388,0xB5B6577A,0x1BDEC6EB,
    0x9534A1D1,0x3B5C3040,0x129484B2,0xBCFC1523,0x4105ED56,0xEF6D7CC7,0xC6A5C835,0x68CD59A4,
    0x173F7B7D,0xB957EAEC,0x909F5E1E,0x3EF7CF8F,0xC30E37FA,0x6D66A66B,0x44AE1299,0xEAC68308,
    0x642CE432,0xCA4475A3,0xE38CC151,0x4DE450C0,0xB01DA8B5,0x1E753924,0x37BD8DD6,0x99D51C47,
    0xF11845E3,0x5F70D472,0x76B86080,0xD8D0F111,0x25290964,0x8B4198F5,0xA2892C07,0x0CE1BD96,
    0x820BDAAC,0x2C634B3D,0x05ABFFCF,0xABC36E5E,0x563A962B,0xF85207BA,0xD19AB348,0x7FF222D9,
    0x2E7EF6FA,0x8016676B,0xA9DED399,0x07B64208,0xFA4FBA7D,0x54272BEC,0x7DEF9F1E,0xD3870E8F,
    0x5D6D69B5,0xF305F824,0xDACD4CD6,0x74A5DD47,0x895C2532,0x2734B4A3,0x0EFC0051,0xA09491C0,
    0xC859C864,0x663159F5,0x4FF9ED07,0xE1917C96,0x1C6884E3,0xB2001572,0x9BC8A180,0x35A03011,
    0xBB4A572B,0x1522C6BA,0x3CEA7248,0x9282E3D9,0x6F7B1BAC,0xC1138A3D,0xE8DB3ECF,0x46B3AF5E,
    0x39418D87,0x97291C16,0xBEE1A8E4,0x10893975,0xED70C100,0x43185091,0x6AD0E463,0xC4B875F2,
    0x4A5212C8,0xE43A8359,0xCDF237AB,0x639AA63A,0x9E635E4F,0x300BCFDE,0x19C37B2C,0xB7ABEABD,
    0xDF66B319,0x710E2288,0x58C6967A,0xF6AE07EB,0x0B57FF9E,0xA53F6E0F,0x8CF7DAFD,0x229F4B6C,
    0xAC752C56,0x021DBDC7,0x2BD50935,0x85BD98A4,0x784460D1,0xD62CF140,0xFFE445B2,0x518CD423,
    0x5CFDEDF4,0xF2957C65,0xDB5DC897,0x75355906,0x88CCA173,0x26A430E2,0x0F6C8410,0xA1041581,
    0x2FEE72BB,0x8186E32A,0xA84E57D8,0x0626C649,0xFBDF3E3C,0x55B7AFAD,0x7C7F1B5F,0xD2178ACE,
    0xBADAD36A,0x14B242FB,0x3D7AF609,0x93126798,0x6EEB9FED,0xC0830E7C,0xE94BBA8E,0x47232B1F,
    0xC9C94C25,0x67A1DDB4,0x4E696946,0xE001F8D7,0x1DF800A2,0xB3909133,0x9A5825C1,0x3430B450,
    0x4BC29689,0xE5AA0718,0xCC62B3EA,0x620A227B,0x9FF3DA0E,0x319B4B9F,0x1853FF6D,0xB63B6EFC,
    0x38D109C6,0x96B99857,0xBF712CA5,0x1119BD34,0xECE04541,0x4288D4D0,0x6B406022,0xC528F1B3,
    0xADE5A817,0x038D3986,0x2A458D74,0x842D1CE5,0x79D4E490,0xD7BC7501,0xFE74C1F3,0x501C5062,
    0xDEF63758,0x709EA6C9,0x5956123B,0xF73E83AA,0x0AC77BDF,0xA4AFEA4E,0x8D675EBC,0x230FCF2D,
    0x72831B0E,0xDCEB8A9F,0xF5233E6D,0x5B4BAFFC,0xA6B25789,0x08DAC618,0x211272EA,0x8F7AE37B,
    0x01908441,0xAFF815D0,0x8630A122,0x285830B3,0xD5A1C8C6,0x7BC95957,0x5201EDA5,0xFC697C34,
    0x94A42590,0x3ACCB401,0x130400F3,0xBD6C9162,0x40956917,0xEEFDF886,0xC7354C74,0x695DDDE5,
    0xE7B7BADF,0x49DF2B4E,0x60179FBC,0xCE7F0E2D,0x3386F658,0x9DEE67C9,0xB426D33B,0x1A4E42AA,
    0x65BC6073,0xCBD4F1E2,0xE21C4510,0x4C74D481,0xB18D2CF4,0x1FE5BD65,0x362D0997,0x98459806,
    0x16AFFF3C,0xB8C76EAD,0x910FDA5F,0x3F674BCE,0xC29EB3BB,0x6CF6222A,0x453E96D8,0xEB560749,
    0x839B5EED,0x2DF3CF7C,0x043B7B8E,0xAA53EA1F,0x57AA126A,0xF9C283FB,0xD00A3709,0x7E62A698,
    0xF088C1A2,0x5EE05033,0x7728E4C1,0xD9407550,0x24B98D25,0x8AD11CB4,0xA319A846,0x0D7139D7,
  }
#endif // CRC32_USE_LOOKUP_TABLE_SLICING_BY_16
};
#endif // NO_LUT







/////////////// CRC-32C with hardware acceleration


/* crc32c.c -- compute CRC-32C using the Intel crc32 instruction
 * Copyright (C) 2013 Mark Adler
 * Version 1.1  1 Aug 2013  Mark Adler
 */
/* Table for a quadword-at-a-time software crc. */
static pthread_once_t crc32c_once_sw = PTHREAD_ONCE_INIT;
static uint32_t crc32c_table[8][256];

/* Construct table for software CRC-32C calculation. */
static void crc32c_init_sw(void)
{
    uint32_t n, crc, k;

    for (n = 0; n < 256; n++) {
        crc = n;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        crc32c_table[0][n] = crc;
    }
    for (n = 0; n < 256; n++) {
        crc = crc32c_table[0][n];
        for (k = 1; k < 8; k++) {
            crc = crc32c_table[0][crc & 0xff] ^ (crc >> 8);
            crc32c_table[k][n] = crc;
        }
    }
}

/* Table-driven software version as a fall-back.  This is about 15 times slower
   than using the hardware instructions.  This assumes little-endian integers,
   as is the case on Intel processors that the assembler code here is for. */
static uint32_t crc32c_sw(uint32_t crci, const unsigned char *buf, size_t len)
{
    const unsigned char *next = buf;
    uint64_t crc;

    pthread_once(&crc32c_once_sw, crc32c_init_sw);
    crc = crci ^ 0xffffffff;
    while (len && ((uintptr_t)next & 7) != 0) {
        crc = crc32c_table[0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
        len--;
    }
    while (len >= 8) {
        crc ^= *(uint64_t *)next;
        crc = crc32c_table[7][crc & 0xff] ^
              crc32c_table[6][(crc >> 8) & 0xff] ^
              crc32c_table[5][(crc >> 16) & 0xff] ^
              crc32c_table[4][(crc >> 24) & 0xff] ^
              crc32c_table[3][(crc >> 32) & 0xff] ^
              crc32c_table[2][(crc >> 40) & 0xff] ^
              crc32c_table[1][(crc >> 48) & 0xff] ^
              crc32c_table[0][crc >> 56];
        next += 8;
        len -= 8;
    }
    while (len) {
        crc = crc32c_table[0][(crc ^ *next++) & 0xff] ^ (crc >> 8);
        len--;
    }
    return (uint32_t)crc ^ 0xffffffff;
}

/* Multiply a matrix times a vector over the Galois field of two elements,
   GF(2).  Each element is a bit in an unsigned integer.  mat must have at
   least as many entries as the power of two for most significant one bit in
   vec. */
static inline uint32_t gf2_matrix_times(uint32_t *mat, uint32_t vec)
{
    uint32_t sum;

    sum = 0;
    while (vec) {
        if (vec & 1)
            sum ^= *mat;
        vec >>= 1;
        mat++;
    }
    return sum;
}

/* Multiply a matrix by itself over GF(2).  Both mat and square must have 32
   rows. */
static inline void gf2_matrix_square(uint32_t *square, uint32_t *mat)
{
    int n;

    for (n = 0; n < 32; n++)
        square[n] = gf2_matrix_times(mat, mat[n]);
}

/* Construct an operator to apply len zeros to a crc.  len must be a power of
   two.  If len is not a power of two, then the result is the same as for the
   largest power of two less than len.  The result for len == 0 is the same as
   for len == 1.  A version of this routine could be easily written for any
   len, but that is not needed for this application. */
static void crc32c_zeros_op(uint32_t *even, size_t len)
{
    int n;
    uint32_t row;
    uint32_t odd[32];       /* odd-power-of-two zeros operator */

    /* put operator for one zero bit in odd */
    odd[0] = POLY;              /* CRC-32C polynomial */
    row = 1;
    for (n = 1; n < 32; n++) {
        odd[n] = row;
        row <<= 1;
    }

    /* put operator for two zero bits in even */
    gf2_matrix_square(even, odd);

    /* put operator for four zero bits in odd */
    gf2_matrix_square(odd, even);

    /* first square will put the operator for one zero byte (eight zero bits),
       in even -- next square puts operator for two zero bytes in odd, and so
       on, until len has been rotated down to zero */
    do {
        gf2_matrix_square(even, odd);
        len >>= 1;
        if (len == 0)
            return;
        gf2_matrix_square(odd, even);
        len >>= 1;
    } while (len);

    /* answer ended up in odd -- copy to even */
    for (n = 0; n < 32; n++)
        even[n] = odd[n];
}

/* Take a length and build four lookup tables for applying the zeros operator
   for that length, byte-by-byte on the operand. */
static void crc32c_zeros(uint32_t zeros[][256], size_t len)
{
    uint32_t n;
    uint32_t op[32];

    crc32c_zeros_op(op, len);
    for (n = 0; n < 256; n++) {
        zeros[0][n] = gf2_matrix_times(op, n);
        zeros[1][n] = gf2_matrix_times(op, n << 8);
        zeros[2][n] = gf2_matrix_times(op, n << 16);
        zeros[3][n] = gf2_matrix_times(op, n << 24);
    }
}

/* Apply the zeros operator table to crc. */
static inline uint32_t crc32c_shift(uint32_t zeros[][256], uint32_t crc)
{
    return zeros[0][crc & 0xff] ^ zeros[1][(crc >> 8) & 0xff] ^
           zeros[2][(crc >> 16) & 0xff] ^ zeros[3][crc >> 24];
}

/* Block sizes for three-way parallel crc computation.  LONG and SHORT must
   both be powers of two.  The associated string constants must be set
   accordingly, for use in constructing the assembler instructions. */
#define LONG 8192
#define LONGx1 "8192"
#define LONGx2 "16384"
#define SHORT 256
#define SHORTx1 "256"
#define SHORTx2 "512"

/* Tables for hardware crc that shift a crc by LONG and SHORT zeros. */
static pthread_once_t crc32c_once_hw = PTHREAD_ONCE_INIT;
static uint32_t crc32c_long[4][256];
static uint32_t crc32c_short[4][256];

/* Initialize tables for shifting crcs. */
static void crc32c_init_hw(void)
{
    crc32c_zeros(crc32c_long, LONG);
    crc32c_zeros(crc32c_short, SHORT);
}

#if defined(_WIN64)

/* Compute CRC-32C using the Intel hardware instruction. */
static uint32_t crc32c_hw(uint32_t crc, const unsigned char *buf, size_t len)
{
    const unsigned char *next = buf;
    const unsigned char *end;
    uint64_t crc0, crc1, crc2;      /* need to be 64 bits for crc32q */

    /* populate shift tables the first time through */
    pthread_once(&crc32c_once_hw, crc32c_init_hw);

    /* pre-process the crc */
    crc0 = crc ^ 0xffffffff;

    /* compute the crc for up to seven leading bytes to bring the data pointer
       to an eight-byte boundary */
    while (len && ((uintptr_t)next & 7) != 0) {
        __asm__("crc32b\t" "(%1), %0"
                : "=r"(crc0)
                : "r"(next), "0"(crc0));
        next++;
        len--;
    }

    /* compute the crc on sets of LONG*3 bytes, executing three independent crc
       instructions, each on LONG bytes -- this is optimized for the Nehalem,
       Westmere, Sandy Bridge, and Ivy Bridge architectures, which have a
       throughput of one crc per cycle, but a latency of three cycles */
    while (len >= LONG*3) {
        crc1 = 0;
        crc2 = 0;
        end = next + LONG;
        do {
            __asm__("crc32q\t" "(%3), %0\n\t"
                    "crc32q\t" LONGx1 "(%3), %1\n\t"
                    "crc32q\t" LONGx2 "(%3), %2"
                    : "=r"(crc0), "=r"(crc1), "=r"(crc2)
                    : "r"(next), "0"(crc0), "1"(crc1), "2"(crc2));
            next += 8;
        } while (next < end);
        crc0 = crc32c_shift(crc32c_long, crc0) ^ crc1;
        crc0 = crc32c_shift(crc32c_long, crc0) ^ crc2;
        next += LONG*2;
        len -= LONG*3;
    }

    /* do the same thing, but now on SHORT*3 blocks for the remaining data less
       than a LONG*3 block */
    while (len >= SHORT*3) {
        crc1 = 0;
        crc2 = 0;
        end = next + SHORT;
        do {
            __asm__("crc32q\t" "(%3), %0\n\t"
                    "crc32q\t" SHORTx1 "(%3), %1\n\t"
                    "crc32q\t" SHORTx2 "(%3), %2"
                    : "=r"(crc0), "=r"(crc1), "=r"(crc2)
                    : "r"(next), "0"(crc0), "1"(crc1), "2"(crc2));
            next += 8;
        } while (next < end);
        crc0 = crc32c_shift(crc32c_short, crc0) ^ crc1;
        crc0 = crc32c_shift(crc32c_short, crc0) ^ crc2;
        next += SHORT*2;
        len -= SHORT*3;
    }

    /* compute the crc on the remaining eight-byte units less than a SHORT*3
       block */
    end = next + (len - (len & 7));
    while (next < end) {
        __asm__("crc32q\t" "(%1), %0"
                : "=r"(crc0)
                : "r"(next), "0"(crc0));
        next += 8;
    }
    len &= 7;

    /* compute the crc for up to seven trailing bytes */
    while (len) {
        __asm__("crc32b\t" "(%1), %0"
                : "=r"(crc0)
                : "r"(next), "0"(crc0));
        next++;
        len--;
    }

    /* return a post-processed crc */
    return (uint32_t)crc0 ^ 0xffffffff;
}

/* Check for SSE 4.2.  SSE 4.2 was first supported in Nehalem processors
   introduced in November, 2008.  This does not check for the existence of the
   cpuid instruction itself, which was introduced on the 486SL in 1992, so this
   will fail on earlier x86 processors.  cpuid works on all Pentium and later
   processors. */
#define SSE42(have) \
    do { \
        uint32_t eax, ecx; \
        eax = 1; \
        __asm__("cpuid" \
                : "=c"(ecx) \
                : "a"(eax) \
                : "%ebx", "%edx"); \
        (have) = (ecx >> 20) & 1; \
    } while (0)

/* Compute a CRC-32C.  If the crc32 instruction is available, use the hardware
   version.  Otherwise, use the software version. */
#endif

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len)
{
#if  defined(_WIN64)
    int sse42;
    SSE42(sse42);
	//printf("SSE42 %d\n",sse42);
    return sse42 ? crc32c_hw(crc, buf, len) : crc32c_sw(crc, buf, len);
#else
    return crc32c_sw(crc, buf, len);
#endif

}


void myreplaceall(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

bool fileexists(const string& i_filename) 
{
  struct stat buffer;   
  return (stat(i_filename.c_str(),&buffer)==0); 
}


void xcommand(string i_command,string i_parameter)
{
	if (i_command=="")
		exit;
	if (!fileexists(i_command))
		return;
	if (i_parameter=="")
		system(i_command.c_str());
	else
	{
		string mycommand=i_command+" \""+i_parameter+"\"";
		system(mycommand.c_str());
	}
	
}


///////////// support functions
string mytrim(const string& i_str)
{
	size_t first = i_str.find_first_not_of(' ');
	if (string::npos == first)
		return i_str;
	size_t last = i_str.find_last_not_of(' ');
	return i_str.substr(first, (last - first + 1));
}

#if defined(_WIN32) || defined(_WIN64)



/// something to get VSS done via batch file
string g_gettempdirectory()
{
	
	string temppath="";
	wchar_t charpath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, charpath))
	{
		wstring ws(charpath);
		string str(ws.begin(), ws.end());	
		return str;
	}
	return temppath;
}

void waitexecute(string i_filename,string i_parameters,int i_show)
{
	SHELLEXECUTEINFOA ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = i_filename.c_str();
	ShExecInfo.lpParameters = i_parameters.c_str();   
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = i_show;
	ShExecInfo.hInstApp = NULL; 
	ShellExecuteExA(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	CloseHandle(ShExecInfo.hProcess);
}

bool isadmin()
{
	BOOL fIsElevated = FALSE;
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		printf("\n Failed to get Process Token :%d.",GetLastError());
		goto Cleanup;  // if Failed, we treat as False
	}


	if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
	{	
		printf("\nFailed to get Token Information :%d.", GetLastError());
		goto Cleanup;// if Failed, we treat as False
	}

	fIsElevated = elevation.TokenIsElevated;

Cleanup:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	return fIsElevated; 
}
#endif

bool myreplace(string& i_str, const string& i_from, const string& i_to) 
{
    size_t start_pos = i_str.find(i_from);
    if(start_pos == std::string::npos)
        return false;
    i_str.replace(start_pos, i_from.length(), i_to);
    return true;
}

// Return true if a file or directory (UTF-8 without trailing /) exists.
bool exists(string filename) {
  int len=filename.size();
  if (len<1) return false;
  if (filename[len-1]=='/') filename=filename.substr(0, len-1);
#ifdef unix
  struct stat sb;
  return !lstat(filename.c_str(), &sb);
#else
  return GetFileAttributes(utow(filename.c_str()).c_str())
         !=INVALID_FILE_ATTRIBUTES;
#endif
}

// Delete a file, return true if successful
bool delete_file(const char* filename) {
#ifdef unix
  return remove(filename)==0;
#else
  return DeleteFile(utow(filename).c_str());
#endif
}

#ifdef unix

// Print last error message
void printerr(const char* filename) {
  perror(filename);
}

#else

// Print last error message
void printerr(const char* filename) 
{
  fflush(stdout);
  int err=GetLastError();
	fprintf(stderr,"\n");
  printUTF8(filename, stderr);
  
  
  if (err==ERROR_FILE_NOT_FOUND)
    g_exec_text=": error file not found";
  else if (err==ERROR_PATH_NOT_FOUND)
   g_exec_text=": error path not found";
  else if (err==ERROR_ACCESS_DENIED)
    g_exec_text=": error access denied";
  else if (err==ERROR_SHARING_VIOLATION)
    g_exec_text=": error sharing violation";
  else if (err==ERROR_BAD_PATHNAME)
    g_exec_text=": error bad pathname";
  else if (err==ERROR_INVALID_NAME)
    g_exec_text=": error invalid name";
  else if (err==ERROR_NETNAME_DELETED)
    g_exec_text=": error network name no longer available";
  else
    g_exec_text=": error Windows error";
	fprintf(stderr, g_exec_text.c_str());
	g_exec_text=filename+g_exec_text;
}

#endif

// Close fp if open. Set date and attributes unless 0
void close(const char* filename, int64_t date, int64_t attr, FP fp=FPNULL) {
  assert(filename);
#ifdef unix
  if (fp!=FPNULL) fclose(fp);
  if (date>0) {
    struct utimbuf ub;
    ub.actime=time(NULL);
    ub.modtime=unix_time(date);
    utime(filename, &ub);
  }
  if ((attr&255)=='u')
    chmod(filename, attr>>8);
#else
  const bool ads=strstr(filename, ":$DATA")!=0;  // alternate data stream?
  if (date>0 && !ads) {
    if (fp==FPNULL)
      fp=CreateFile(utow(filename).c_str(),
                    FILE_WRITE_ATTRIBUTES,
                    FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (fp!=FPNULL) {
      SYSTEMTIME st;
      st.wYear=date/10000000000LL%10000;
      st.wMonth=date/100000000%100;
      st.wDayOfWeek=0;  // ignored
      st.wDay=date/1000000%100;
      st.wHour=date/10000%100;
      st.wMinute=date/100%100;
      st.wSecond=date%100;
      st.wMilliseconds=0;
      FILETIME ft;
      SystemTimeToFileTime(&st, &ft);
      SetFileTime(fp, NULL, NULL, &ft);
    }
  }
  if (fp!=FPNULL) CloseHandle(fp);
  if ((attr&255)=='w' && !ads)
    SetFileAttributes(utow(filename).c_str(), attr>>8);
#endif
}

// Print file open error and throw exception
void ioerr(const char* msg) {
  printerr(msg);
  throw std::runtime_error(msg);
}

// Create directories as needed. For example if path="/tmp/foo/bar"
// then create directories /, /tmp, and /tmp/foo unless they exist.
// Set date and attributes if not 0.
void makepath(string path, int64_t date=0, int64_t attr=0) {
  for (unsigned i=0; i<path.size(); ++i) {
    if (path[i]=='\\' || path[i]=='/') {
      path[i]=0;
#ifdef unix
      mkdir(path.c_str(), 0777);
#else
      CreateDirectory(utow(path.c_str()).c_str(), 0);
#endif
      path[i]='/';
    }
  }

  // Set date and attributes
  string filename=path;
  if (filename!="" && filename[filename.size()-1]=='/')
    filename=filename.substr(0, filename.size()-1);  // remove trailing slash
  close(filename.c_str(), date, attr);
}

#ifndef unix

// Truncate filename to length. Return -1 if error, else 0.
int truncate(const char* filename, int64_t length) {
  std::wstring w=utow(filename);
  HANDLE out=CreateFile(w.c_str(), GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (out!=INVALID_HANDLE_VALUE) {
    //// LONG
	long hi=length>>32;
    if (SetFilePointer(out, length, &hi, FILE_BEGIN)
             !=INVALID_SET_FILE_POINTER
        && SetEndOfFile(out)
        && CloseHandle(out))
      return 0;
  }
  return -1;
}
#endif

/////////////////////////////// Archive ///////////////////////////////

// Convert non-negative decimal number x to string of at least n digits
string itos(int64_t x, int n=1) {
  assert(x>=0);
  assert(n>=0);
  string r;
  for (; x || n>0; x/=10, --n) r=string(1, '0'+x%10)+r;
  return r;
}

// Replace * and ? in fn with part or digits of part
string subpart(string fn, int part) {
  for (int j=fn.size()-1; j>=0; --j) {
    if (fn[j]=='?')
      fn[j]='0'+part%10, part/=10;
    else if (fn[j]=='*')
      fn=fn.substr(0, j)+itos(part)+fn.substr(j+1), part=0;
  }
  return fn;
}

// Base of InputArchive and OutputArchive
class ArchiveBase {
protected:
  libzpaq::AES_CTR* aes;  // NULL if not encrypted
  FP fp;          // currently open file or FPNULL
public:
  ArchiveBase(): aes(0), fp(FPNULL) {}
  ~ArchiveBase() {
    if (aes) delete aes;
    if (fp!=FPNULL) fclose(fp);
  }  
  bool isopen() {return fp!=FPNULL;}
};

// An InputArchive supports encrypted reading
class InputArchive: public ArchiveBase, public libzpaq::Reader {
  vector<int64_t> sz;  // part sizes
  int64_t off;  // current offset
  string fn;  // filename, possibly multi-part with wildcards
public:

  // Open filename. If password then decrypt input.
  InputArchive(const char* filename, const char* password=0);

  // Read and return 1 byte or -1 (EOF)
  int get() {
    error("get() not implemented");
    return -1;
  }

  // Read up to len bytes into obuf at current offset. Return 0..len bytes
  // actually read. 0 indicates EOF.
  int read(char* obuf, int len) {
    int nr=fread(obuf, 1, len, fp);
    if (nr==0) {
      seek(0, SEEK_CUR);
      nr=fread(obuf, 1, len, fp);
    }
    if (nr==0) return 0;
    if (aes) aes->encrypt(obuf, nr, off);
    off+=nr;
    return nr;
  }

  // Like fseeko()
  void seek(int64_t p, int whence);

  // Like ftello()
  int64_t tell() {
    return off;
  }
};

// Like fseeko. If p is out of range then close file.
void InputArchive::seek(int64_t p, int whence) {
  if (!isopen()) return;

  // Compute new offset
  if (whence==SEEK_SET) off=p;
  else if (whence==SEEK_CUR) off+=p;
  else if (whence==SEEK_END) {
    off=p;
    for (unsigned i=0; i<sz.size(); ++i) off+=sz[i];
  }

  // Optimization for single file to avoid close and reopen
  if (sz.size()==1) {
    fseeko(fp, off, SEEK_SET);
    return;
  }

  // Seek across multiple files
  assert(sz.size()>1);
  int64_t sum=0;
  unsigned i;
  for (i=0;; ++i) {
    sum+=sz[i];
    if (sum>off || i+1>=sz.size()) break;
  }
  const string next=subpart(fn, i+1);
  fclose(fp);
  fp=fopen(next.c_str(), RB);
  if (fp==FPNULL) ioerr(next.c_str());
  fseeko(fp, off-sum, SEEK_END);
}

// Open for input. Decrypt with password and using the salt in the
// first 32 bytes. If filename has wildcards then assume multi-part
// and read their concatenation.

InputArchive::InputArchive(const char* filename, const char* password):
    off(0), fn(filename) {
  assert(filename);

  // Get file sizes
  const string part0=subpart(filename, 0);
  for (unsigned i=1; ; ++i) {
    const string parti=subpart(filename, i);
    if (i>1 && parti==part0) break;
    fp=fopen(parti.c_str(), RB);
    if (fp==FPNULL) break;
    fseeko(fp, 0, SEEK_END);
    sz.push_back(ftello(fp));
    fclose(fp);
  }

  // Open first part
  const string part1=subpart(filename, 1);
  fp=fopen(part1.c_str(), RB);
  if (!isopen()) ioerr(part1.c_str());
  assert(fp!=FPNULL);

  // Get encryption salt
  if (password) {
    char salt[32], key[32];
    if (fread(salt, 1, 32, fp)!=32) error("cannot read salt");
    libzpaq::stretchKey(key, password, salt);
    aes=new libzpaq::AES_CTR(key, 32, salt);
    off=32;
  }
}

// An Archive is a file supporting encryption
class OutputArchive: public ArchiveBase, public libzpaq::Writer {
  int64_t off;    // preceding multi-part bytes
  unsigned ptr;   // write pointer in buf: 0 <= ptr <= BUFSIZE
  enum {BUFSIZE=1<<16};
  char buf[BUFSIZE];  // I/O buffer
public:

  // Open. If password then encrypt output.
  OutputArchive(const char* filename, const char* password=0,
                const char* salt_=0, int64_t off_=0);

  // Write pending output
  void flush() {
    assert(fp!=FPNULL);
    if (aes) aes->encrypt(buf, ptr, ftello(fp)+off);
    fwrite(buf, 1, ptr, fp);
    ptr=0;
  }

  // Position the next read or write offset to p.
  void seek(int64_t p, int whence) {
    if (fp!=FPNULL) {
      flush();
      fseeko(fp, p, whence);
    }
    else if (whence==SEEK_SET) off=p;
    else off+=p;  // assume at end
  }

  // Return current file offset.
  int64_t tell() const {
    if (fp!=FPNULL) return ftello(fp)+ptr;
    else return off;
  }

  // Write one byte
  void put(int c) {
    if (fp==FPNULL) ++off;
    else {
      if (ptr>=BUFSIZE) flush();
      buf[ptr++]=c;
    }
  }

  // Write buf[0..n-1]
  void write(const char* ibuf, int len) {
    if (fp==FPNULL) off+=len;
    else while (len-->0) put(*ibuf++);
  }

  // Flush output and close
  void close() {
    if (fp!=FPNULL) {
      flush();
      fclose(fp);
    }
    fp=FPNULL;
  }
};

// Create or update an existing archive or part. If filename is ""
// then keep track of position in off but do not write to disk. Otherwise
// open and encrypt with password if not 0. If the file exists then
// read the salt from the first 32 bytes and off_ must be 0. Otherwise
// encrypt assuming off_ previous bytes, of which the first 32 are salt_.
// If off_ is 0 then write salt_ to the first 32 bytes.

OutputArchive::OutputArchive(const char* filename, const char* password,
    const char* salt_, int64_t off_): off(off_), ptr(0) {
  assert(filename);
  if (!*filename) return;

  // Open existing file
  char salt[32]={0};
  fp=fopen(filename, RBPLUS);
  if (isopen()) {
    if (off!=0) error("file exists and off > 0");
    if (password) {
      if (fread(salt, 1, 32, fp)!=32) error("cannot read salt");
      if (salt_ && memcmp(salt, salt_, 32)) error("salt mismatch");
    }
    seek(0, SEEK_END);
  }

  // Create new file
  else {
    fp=fopen(filename, WB);
    if (!isopen()) ioerr(filename);
    if (password) {
      if (!salt_) error("salt not specified");
      memcpy(salt, salt_, 32);
      if (off==0 && fwrite(salt, 1, 32, fp)!=32) ioerr(filename);
    }
  }

  // Set up encryption
  if (password) {
    char key[32];
    libzpaq::stretchKey(key, password, salt);
    aes=new libzpaq::AES_CTR(key, 32, salt);
  }
}

///////////////////////// System info /////////////////////////////////

// Guess number of cores. In 32 bit mode, max is 2.
int numberOfProcessors() {
  int rc=0;  // result
#ifdef unix
#ifdef BSD  // BSD or Mac OS/X
  size_t rclen=sizeof(rc);
  int mib[2]={CTL_HW, HW_NCPU};
  if (sysctl(mib, 2, &rc, &rclen, 0, 0)!=0)
    perror("sysctl");

#else  // Linux
  // Count lines of the form "processor\t: %d\n" in /proc/cpuinfo
  // where %d is 0, 1, 2,..., rc-1
  FILE *in=fopen("/proc/cpuinfo", "r");
  if (!in) return 1;
  std::string s;
  int c;
  while ((c=getc(in))!=EOF) {
    if (c>='A' && c<='Z') c+='a'-'A';  // convert to lowercase
    if (c>' ') s+=c;  // remove white space
    if (c=='\n') {  // end of line?
      if (s.size()>10 && s.substr(0, 10)=="processor:") {
        c=atoi(s.c_str()+10);
        if (c==rc) ++rc;
      }
      s="";
    }
  }
  fclose(in);
#endif
#else

  // In Windows return %NUMBER_OF_PROCESSORS%
  //const char* p=getenv("NUMBER_OF_PROCESSORS");
  //if (p) rc=atoi(p);
  SYSTEM_INFO si={0};
  GetSystemInfo(&si);
  rc=si.dwNumberOfProcessors;
#endif
  if (rc<1) rc=1; /// numero massimo core 32bit
  if (sizeof(char*)==4 && rc>2) rc=2;
  return rc;
}

////////////////////////////// misc ///////////////////////////////////
//zpaq
// For libzpaq output to a string less than 64K chars
struct StringWriter: public libzpaq::Writer {
  string s;
  void put(int c) {
    if (s.size()>=65535) error("string too long");
    s+=char(c);
  }
};

// In Windows convert upper case to lower case.
inline int tolowerW(int c) {
#ifndef unix
  if (c>='A' && c<='Z') return c-'A'+'a';
#endif
  return c;
}

// Return true if strings a == b or a+"/" is a prefix of b
// or a ends in "/" and is a prefix of b.
// Match ? in a to any char in b.
// Match * in a to any string in b.
// In Windows, not case sensitive.
bool ispath(const char* a, const char* b) {
  
  /*
  printf("ZEKEa %s\n",a);
  printf("ZEKEb %s\n",b);
  */
  for (; *a; ++a, ++b) {
    const int ca=tolowerW(*a);
    const int cb=tolowerW(*b);
    if (ca=='*') {
      while (true) {
        if (ispath(a+1, b)) return true;
        if (!*b) return false;
        ++b;
      }
    }
    else if (ca=='?') {
      if (*b==0) return false;
    }
    else if (ca==cb && ca=='/' && a[1]==0)
      return true;
    else if (ca!=cb)
      return false;
  }
  return *b==0 || *b=='/';
}

// Read 4 byte little-endian int and advance s
unsigned btoi(const char* &s) {
  s+=4;
  return (s[-4]&255)|((s[-3]&255)<<8)|((s[-2]&255)<<16)|((s[-1]&255)<<24);
}

// Read 8 byte little-endian int and advance s
int64_t btol(const char* &s) {
  uint64_t r=btoi(s);
  return r+(uint64_t(btoi(s))<<32);
}

/////////////////////////////// Jidac /////////////////////////////////

// A Jidac object represents an archive contents: a list of file
// fragments with hash, size, and archive offset, and a list of
// files with date, attributes, and list of fragment pointers.
// Methods add to, extract from, compare, and list the archive.

// enum for version

static const int64_t EXTRACTED= 0x7FFFFFFFFFFFFFFELL;  // decompressed?
static const int64_t HT_BAD=   -0x7FFFFFFFFFFFFFFALL;  // no such frag
static const int64_t DEFAULT_VERSION=99999999999999LL; // unless -until


// fragment hash table entry
struct HT {
	uint32_t crc32;			// new: take the CRC-32 of the fragment
	uint32_t crc32size;

	unsigned char sha1[20];  // fragment hash
	int usize;      // uncompressed size, -1 if unknown, -2 if not init
	int64_t csize;  // if >=0 then block offset else -fragment number
  
	HT(const char* s=0, int u=-2) 
	{
		crc32=0;
		crc32size=0;
		csize=0;
		if (s) 
			memcpy(sha1, s, 20);
		else 
			memset(sha1, 0, 20);
		usize=u;
	}
	HT(string hello,const char* s=0, int u=-1, int64_t c=HT_BAD) 
	{
		crc32=0;
		crc32size=0;
		if (s) 
			memcpy(sha1, s, 20);
		else 
			memset(sha1, 0, 20);
		usize=u; 
		csize=c;
	}
};



struct DTV {
  int64_t date;          // decimal YYYYMMDDHHMMSS (UT) or 0 if deleted
  int64_t size;          // size or -1 if unknown
  int64_t attr;          // first 8 attribute bytes
  double csize;          // approximate compressed size
  vector<unsigned> ptr;  // list of fragment indexes to HT
  int version;           // which transaction was it added?
  
  DTV(): date(0), size(0), attr(0), csize(0), version(0) {}
};

// filename entry
struct DT {
  int64_t date;          // decimal YYYYMMDDHHMMSS (UT) or 0 if deleted
  int64_t size;          // size or -1 if unknown
  int64_t attr;          // first 8 attribute bytes
  int64_t data;          // sort key or frags written. -1 = do not write
  vector<unsigned> ptr;  // fragment list
  char sha1hex[66];		 // 1+32+32 (SHA256)+ zero. In fact 40 (SHA1)+zero+8 (CRC32)+zero 
  vector<DTV> dtv;       // list of versions
	int written;           // 0..ptr.size() = fragments output. -1=ignore
  uint32_t crc32;
  DT(): date(0), size(0), attr(0), data(0),written(-1),crc32(0) {memset(sha1hex,0,sizeof(sha1hex));}
};
typedef map<string, DT> DTMap;

typedef map<int, string> MAPPACOMMENTI;
	


////// calculate CRC32 by blocks and not by file. Need to sort before combine
////// list of CRC32 for every file  
struct s_crc32block
{
	string	filename;
	uint64_t crc32start;
	uint64_t crc32size;
	uint32_t crc32;
	unsigned char sha1[20];  // fragment hash
	char sha1hex[40];
	s_crc32block(): crc32(0),crc32start(0),crc32size(0) {memset(sha1,0,sizeof(sha1));memset(sha1hex,0,sizeof(sha1hex));}
};
vector <s_crc32block> g_crc32;

///// sort by offset, then by filename
///// use sprintf for debug, not very fast.
bool comparecrc32block(s_crc32block a, s_crc32block b)
{
	char a_start[40];
	char b_start[40];
	sprintf(a_start,"%014lld",a.crc32start);
	sprintf(b_start,"%014lld",b.crc32start);
	return a.filename+a_start<b.filename+b_start;
}


// list of blocks to extract
struct Block {
  int64_t offset;       // location in archive
  int64_t usize;        // uncompressed size, -1 if unknown (streaming)
  int64_t bsize;        // compressed size
  vector<DTMap::iterator> files;  // list of files pointing here
  unsigned start;       // index in ht of first fragment
  unsigned size;        // number of fragments to decompress
  unsigned frags;       // number of fragments in block
  unsigned extracted;   // number of fragments decompressed OK
  enum {READY, WORKING, GOOD, BAD} state;
  Block(unsigned s, int64_t o): offset(o), usize(-1), bsize(0), start(s),
      size(0), frags(0), extracted(0), state(READY) {}
};

// Version info
struct VER {
  int64_t date;          // Date of C block, 0 if streaming
  int64_t lastdate;      // Latest date of any block
  int64_t offset;        // start of transaction C block
  int64_t data_offset;   // start of first D block
  int64_t csize;         // size of compressed data, -1 = no index
  int updates;           // file updates
  int deletes;           // file deletions
  unsigned firstFragment;// first fragment ID
  int64_t usize;         // uncompressed size of files
  
  VER() {memset(this, 0, sizeof(*this));}
};

// Windows API functions not in Windows XP to be dynamically loaded
#ifndef unix
typedef HANDLE (WINAPI* FindFirstStreamW_t)
                   (LPCWSTR, STREAM_INFO_LEVELS, LPVOID, DWORD);
FindFirstStreamW_t findFirstStreamW=0;
typedef BOOL (WINAPI* FindNextStreamW_t)(HANDLE, LPVOID);
FindNextStreamW_t findNextStreamW=0;
#endif

class CompressJob;

// Do everything
class Jidac {
public:
  int doCommand(int argc, const char** argv);
  friend ThreadReturn decompressThread(void* arg);
  friend ThreadReturn testThread(void* arg);
  friend struct ExtractJob;
private:

  // Command line arguments
  uint64_t minsize;
  uint64_t maxsize;
  unsigned int menoenne;
  string versioncomment;
  bool flagcomment;
  char command;             // command 'a', 'x', or 'l'
  string archive;           // archive name
  vector<string> files;     // filename args
  vector<uint64_t> files_size;     // filename args
  vector<uint64_t> files_count;     // filename args
  vector<uint64_t> files_time;     // filename args
  int all;                  // -all option
  bool flagforce;               // -force option
  int fragment;             // -fragment option
  const char* index;        // index option
  char password_string[32]; // hash of -key argument
  const char* password;     // points to password_string or NULL
  string method;            // default "1"
  bool flagnoattributes;        // -noattributes option
  vector<string> notfiles;  // list of prefixes to exclude
  string nottype;           // -not =...
  vector<string> onlyfiles; // list of prefixes to include
  vector<string> alwaysfiles; // list of prefixes to pack ALWAYS
  const char* repack;       // -repack output file
  char new_password_string[32]; // -repack hashed password
  const char* new_password; // points to new_password_string or NULL
  int summary;              // summary option if > 0, detailed if -1
  
  bool flagtest;              // -test option
  bool flagskipzfs;				// -zfs option (hard-core skip of .zfs)
  bool flagnoqnap;				// exclude @Recently-Snapshot and @Recycle
  bool flagforcewindows;        // force alterate data stream $DATA$, system volume information
  bool flagdonotforcexls;       // do not always include xls
  bool flagnopath;				// do not store path
  bool flagnosort;              // do not sort files std::sort(vf.begin(), vf.end(), compareFilename);
  bool flagchecksum;			// get  checksum for every file (default SHA1)
  bool flagcrc32c;			// flag use CRC-32c instead
  bool flagxxhash;			// flag use xxHash64
  bool flagcrc32;			// flag use CRC32
  bool flagsha256;			// use SHA256
  string searchfrom;		// search something in path
  string replaceto;			// replace (something like awk)
  int threads;              // default is number of cores
  vector<string> tofiles;   // -to option
  int64_t date;             // now as decimal YYYYMMDDHHMMSS (UT)
  int64_t version;          // version number or 14 digit date
  // Archive state
  int64_t dhsize;           // total size of D blocks according to H blocks
  int64_t dcsize;           // total size of D blocks according to C blocks
  vector<HT> ht;            // list of fragments
  DTMap dt;                 // set of files in archive
  DTMap edt;                // set of external files to add or compare
  vector<Block> block;      // list of data blocks to extract
  vector<VER> ver;          // version info

  map<int, string> mappacommenti;

  bool getchecksum(string i_filename, char* o_sha1); //get SHA1 AND CRC32 of a file

  // Commands
	int add();                				// add, return 1 if error else 0
	int extract();            				// extract, return 1 if error else 0
	int list();               				// list (one parameter) / check (more than one)
	int verify();
	int test();           					// test, return 1 if error else 0
	int summa();							// get hash / or sum
	int paranoid();							// paranoid test by unz. Really slow & lot of RAM
	void usage();        					// help
	int dir();								// Windows-like dir command
	int dircompare(bool i_flagonlysize);
	
  // Support functions
  string rename(string name);           // rename from -to
  int64_t read_archive(const char* arc, int *errors=0, int i_myappend=0);  // read arc
  bool isselected(const char* filename, bool rn,int64_t i_size);// files, -only, -not
  void scandir(DTMap& i_edt,string filename, bool i_recursive=true);        // scan dirs to dt
  void addfile(DTMap& i_edt,string filename, int64_t edate, int64_t esize,
               int64_t eattr);          // add external file to dt
			   
  bool equal(DTMap::const_iterator p, const char* filename, uint32_t &o_crc32);
             // compare file contents with p
	
	string mygetalgo();					// checksum choosed
	void usagefull();         			// verbose help
	void examples();						// some examples
	void differences();					// differences from 7.15
	void write715attr(libzpaq::StringBuffer& i_sb, uint64_t i_data, unsigned int i_quanti);
	void writefranzattr(libzpaq::StringBuffer& i_sb, uint64_t i_data, unsigned int i_quanti,bool i_checksum,string i_filename,char *i_sha1,uint32_t i_crc32);
	int enumeratecomments();
	int searchcomments(string i_testo,vector<DTMap::iterator> &filelist);
	

 };

/*
static double seconds()
{
#if defined(_WIN32) || defined(_WIN64)
  LARGE_INTEGER frequency, now;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter  (&now);
  return now.QuadPart / double(frequency.QuadPart);
#else
  timespec now;
  //#clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
#endif
}
*/

int terminalwidth() 
{
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return (int) csbi.srWindow.Right - csbi.srWindow.Left + 1;
   /// return (int)(csbi.dwSize.X);
#else
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    return (int)(w.ws_col);
#endif 
}
int terminalheight()
{
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//    return (int)(csbi.dwSize.Y);
	return (int)csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    return (int)(w.ws_row);
#endif 
}

void mygetch()
{
	int mychar=0;
#if defined(_WIN32)
	mychar=getch();
#endif


#ifdef unix
/// BSD Unix
	struct termios oldt, newt;
	tcgetattr ( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
	mychar = getchar();
	tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
#endif



	if ((mychar==113) || (mychar==81) || (mychar==3))  /// q, Q, control-C
	{
#ifdef unix
		printf("\n\n");
#endif
	exit(0);
	}
}
void moreprint(const char* i_stringa)
{
	unsigned int larghezzaconsole=terminalwidth()-2;
	unsigned int altezzaconsole=terminalheight();
	unsigned static int righestampate=0;
	if (!i_stringa)
		return;
	
		
	///if (i_stringa=="\n")
	if (!strcmp(i_stringa,"\n"))
	{
		printf("\n");
		righestampate++;
		if (righestampate>(altezzaconsole-2))
		{
			printf("-- More (q, Q or control C to exit) --\r");
			mygetch();
			for (unsigned int i=0;i<altezzaconsole;i++)
				printf("\n");
			righestampate=0;
		}
		return;
	}
	unsigned int lunghezzastringa=strlen(i_stringa);
	
	if (!larghezzaconsole)
		return;

	unsigned int righe=(lunghezzastringa/larghezzaconsole)+1;
	unsigned int massimo=lunghezzastringa-(larghezzaconsole*(righe-1));
	
	for (int riga=1; riga<=righe;riga++)
	{
		
		int currentmax=larghezzaconsole;
		
		if (riga==righe)
			currentmax=massimo;

		int startcarattere=(riga-1)*larghezzaconsole;
		for (int i=startcarattere;i<startcarattere+currentmax;i++)
			printf("%c",i_stringa[i]);
		printf("\n");
		
		righestampate++;
		if (righestampate>(altezzaconsole-2))
		{
			printf("-- More (q, Q or control C to exit) --\r");
			mygetch();
			for (unsigned int i=0;i<altezzaconsole;i++)
				printf("\n");
			righestampate=0;
		}
	}	
}
void Jidac::differences() 
{
	moreprint("Differences from ZPAQ 7.15");
	moreprint("\n");
	moreprint("Changed default behaviors:");
	moreprint("A)  Output is somewhat different (-pakka for alternative)");
	moreprint("B)  During add() zpaqfranz stores by default the CRC-32 of the files");
	moreprint("    This can disabled by -crc32 switch");
	moreprint("C)  Add() using -checksum will store SHA1 hash for every file");
	moreprint("D)  By default every .XLS file is forcibily added (check of datetime");
	moreprint("    is not reliable for ancient XLS). Disabled by -xls");
	moreprint("E)  By default no Windows Alternate Data Stream is stored. -forcewindows to enable");
	moreprint("\n");
	moreprint("New functions:");
	moreprint("1)  Using -comment something to add ASCII text to the versions");
	moreprint("    in add() and list(). WARNING: NO duplicates check is done");
	moreprint("2)  In list() using -find pippo filter like |grep -i find");
	moreprint("    In list() -comment / -comment pippo / -comment -all");
	moreprint("    The list command can be used for comparing ZPAQ contents agains disk");
	moreprint("    zpaqfranz l z:\\1.zpaq              (show ZPAQ content)");
	moreprint("    zpaqfranz l z:\\1.zpaq c:\\dati z:\\  (compare files agains c:\\dati and z:\\)");
	moreprint("3)  New command t (test) for archive test -verify for filesystem post-check -verbose");
	moreprint("4)  Switch -test to command add. -verify for filesystem post-check");
	moreprint("5)  New command p (paranoid test). Need LOTS of RAM and painfully slow. Check");
	moreprint("    almost everything. -force -verbose");
	moreprint("6)  New command s (size). Get the cumulative size of one or more directory.");
	moreprint("    Skip .zfs and ADS. Use -all for multithread");
	moreprint("7)  New command sha1 (hashing). Calculate hash of something (default SHA1)");
	moreprint("    -sha256, -crc32, -crc32c (HW SSE if possible), -xxhash");
	moreprint("8)  New command dir. Something similar to dir command (for *nix)");
	moreprint("    Switches /s /a /os /od. Show cumulative size in the last line!");
	moreprint("    -crc32 find duplicates. -n Y like tain -n. -minsize Y -maxsize U");
	moreprint("9)  New command c (compare). Quickly compare (size and filename) a source");
	moreprint("    directory against one or more. Example c /tank/data /dup/1 /dup/2 /dup/3");
	moreprint("    -all create one scan thread for parameters. DO NOT use for single HDD!");
	moreprint("10) New command help. -he show some examples, -diff differences from 7.15");
	moreprint("11) -noeta. Do not show ETA (for batch file)");
	moreprint("12) -pakka. Alternative output (for ZPAQ's GUI PAKKA)");
	moreprint("13) -verbose. Show more info on files");
	moreprint("14) -zfs. Skip ZFS's snapshots");
	moreprint("15) -noqnap. Skip special directories");
	moreprint("16) -forcewindows. Turn on metafiles and system volume information");
	moreprint("17) -always files. Always add some file");
	moreprint("18) -nosort. Do not sort before adding files");
	moreprint("19) -nopath. Do not store filepath");
	moreprint("20) -vss. On Windows (if running with admin power) take a snapshot (VSS) of ");
	moreprint("     drive before add. Useful for backup entire user profiles");
	moreprint("21) -find something. Find text in full filename (ex list)");
	moreprint("22) -replace something. Replace a -find text (manipulate output)");
	moreprint("23) -verify. After a test command read again all files from media");
	moreprint("24) -minsize. Skip file if length < minsize");
	moreprint("25) -maxsize. Skip file if length > maxsize");
	moreprint("26) -exec_ok file.sh. After OK run file.sh");
	moreprint("27) -exec_error file.sh. After A NOT OK (error, warning) run file.sh");
	
	exit(1);
}

//// some examples (to be completed)
void Jidac::examples() 
{
	moreprint("+++ ADDING");
	moreprint("zpaqfranz a z:\\backup.zpaq c:\\data\\* d:\\pippo\\*");
	moreprint("Add all files from c:\\data and d:\\pippo to archive z:\\backup.zpaq");
	moreprint("\n");
	moreprint("zpaqfranz a z:\\1 c:\\nz\\");
	moreprint("zpaqfranz l z:\\1 c:\\nz\\");
	moreprint("Add then test (two commands)");
	moreprint("\n");
	moreprint("zpaqfranz a z:\\1 c:\\nz\\ -test");
	moreprint("Add then test (single command)");
	moreprint("\n");
	moreprint("zpaqfranz a z:\\backup.zpaq c:\\vecchio.sql r:\\1.txt -noeta -pakka");
	moreprint("Add two files archive z:\\backup.zpaq");
	moreprint("\n");	
	moreprint("zpaqfranz a z:\\4.zpaq c:\\audio -crc32");
	moreprint("Add to file, disabling CRC-32 test (just like ZPAQ 7.15)");
	moreprint("\n");
	moreprint("zpaqfranz a z:\\4.zpaq c:\\audio -checksum -test -pakka");
	moreprint("Add to z:\\4.zpaq the folder c:\\audio, do a paranoid test, brief output");
	moreprint("\n");
	moreprint("zpaqfranz a z:\\backup.zpaq c:\\data\\* -comment first_copy");
	moreprint("Add all files from c:\\data with ASCII version 'first_copy' n");
	moreprint("\n");
#if defined(_WIN32) || defined(_WIN64)
	moreprint("Windows running with administrator privileges");
	moreprint("zpaqfranz a z:\\backup.zpaq c:\\users\\utente\\* -vss -pakka");
	moreprint("Delete all VSS, create a fresh one of C:\\, backup the 'utente' profile");
	moreprint("\n");
#endif
	moreprint("+++ LISTING");
	moreprint("zpaqfranz l z:\\backup.zpaq");
	moreprint("Show last version");
	moreprint("\n");
	moreprint("zpaqfranz l z:\\backup.zpaq -all");
	moreprint("Show every version(s)");
	moreprint("\n");
	moreprint("zpaqfranz l z:\\backup.zpaq -comment");
	moreprint("Show (if any) version comments");
	moreprint("\n");
	moreprint("zpaqfranz l z:\\backup.zpaq -comment -all");
	moreprint("Show (if any) version comments verbose");
	moreprint("\n");
	moreprint("zpaqfranz l z:\\backup.zpaq -find zpaq.cpp -pakka");
	moreprint("Show from backup.zpaq only stored 'zpaq.cpp' in briefly mode");
	moreprint("\n");
	moreprint("zpaqfranz l r:\\test\\prova.zpaq -find c:/parte/ -replace z:\\mynewdir\\ -pakka");
	moreprint("Find-and-replace part of output (like awk or sed)");
	moreprint("\n");
	moreprint("zpaqfranz l z:\\1.zpaq -checksum -n 10");
	moreprint("List the 10 greatest file with SHA1 if available");
	moreprint("\n");
	moreprint("+++ TESTING");
	moreprint("zpaqfranz t z:\\1.zpaq");
	moreprint("Check archive. With CRC32 if created with -checksum. Use -verbose");
	moreprint("\n");
	moreprint("zpaqfranz p z:\\1.zpaq");
	moreprint("Paranoid test. Better if created with -checksum. Use lots of RAM. -verify paranoist");
	moreprint("\n");
	moreprint("+++ SUPPORT FUNCTIONS");
	moreprint("zpaqfranz s r:\\vbox s:\\uno");
	moreprint("Filesize sum from directories EXCEPT .zfs and NTFS extensions");
	moreprint("\n");
	moreprint("zpaqfranz sha1 r:\\vbox s:\\uno -all");
	moreprint("Get SHA1 of all files (contatenated) in two directories");
	moreprint("\n");
	moreprint("zpaqfranz sha1 r:\\vbox s:\\uno");
	moreprint("zpaqfranz sha1 r:\\vbox s:\\uno -crc32");
	moreprint("you can use -crc32c -sha256 -xxhash -crc32 (default SHA1)");
	moreprint("\n");
	moreprint("zpaqfranz dir /root/script /od");
	moreprint("Windows-dir command clone (/od /os /s /a)");
	moreprint("\n");
	moreprint("zpaqfranz dir c:\\ /s /os -n 10 -find .mp4");
	moreprint("Show the 10 largest .mp4 file in c:\\");
	moreprint("\n");
	moreprint("zpaqfranz dir c:\\ /s -crc32 -find .mp4");
	moreprint("Find .mp4 duplicate in C:\\");
	
	
	exit(1);
}


//// cut a too long filename 
string mymaxfile(string i_filename,const unsigned int i_lunghezza)
{
	if (i_lunghezza==0)
		return "";
	if (i_filename.length()<=i_lunghezza)
		return i_filename;
	
	if (i_lunghezza>10)
	{
		if (i_filename.length()>10)
		{
			unsigned int intestazione=i_lunghezza-10;
			return i_filename.substr(0,intestazione)+"(...)"+i_filename.substr(i_filename.length()-5,5);
		}
		else
		return i_filename.substr(0,i_lunghezza);
			
	}
	else
		return i_filename.substr(0,i_lunghezza);
}

//// default help
void Jidac::usage() 
{
		
	moreprint("Usage: zpaqfranz command archive[.zpaq] files|directory... -options...");
	moreprint("\?\?\?\? in archive name for multi-part ex \"test_????.zpaq\"");
	moreprint("Commands:");
	moreprint("   a             Append files (-checksum -force -crc32 -comment foo -test)");
	moreprint("   x             Extract versions of files (-until N -comment foo)");
	moreprint("   l             List files (-all -checksum -find something -comment foo)");
	moreprint("   l d0 d1 d2... Compare (test) content agains directory d0, d1, d2...");
	moreprint("   c d0 d1 d2... Quickly compare dir d0 to d1,d2... (-all M/T scan -crc32)");
	moreprint("   s d0 d1 d2... Cumulative size (excluding .zfs) of d0,d1,d2 (-all M/T)");
	moreprint("   t             Fast test of most recent version of files (-verify -verbose)");
	moreprint("   p             Paranoid test. Use lots (LOTS!) of RAM (-verify -verbose)");
	moreprint("   sha1          Calculate hash (-sha256 -crc32 -crc32c -xxhash)");
	moreprint("   dir           Like Windows dir (/s /a /os /od) -crc32 show duplicates");
	moreprint("   help          Long help (-h, -he examples, -diff differences from 7.15)");
	moreprint("Optional switch(es):");
	moreprint("  -all [N]       Extract/list versions in N [4] digit directories");
	moreprint("  -f -force      Add: append files if contents have changed");
	moreprint("                 Extract: overwrite existing output files");
	moreprint("  -key X         Create or access encrypted archive with password X");
	moreprint("  -mN  -method N Compress level N (0..5 = faster..better, default 1)");
	moreprint("  -test          Extract: verify but do not write files");
	moreprint("                 Add: Post-check of files (-verify for CRC-32)");
	moreprint("  -to out...     Prefix files... to out... or all to out/all");
	moreprint("  -until N       Roll back archive to N'th update or -N from end");
}


//// print a lot more
void Jidac::usagefull() 
{

usage();
char buffer[200];
	
  
sprintf(buffer,"  -until %s  Set date, roll back (UT, default time: 235959)",dateToString(date).c_str());
moreprint(buffer);
	
moreprint("  -not files...   Exclude. * and ? match any string or char");
moreprint("       =[+-#^?]   List: exclude by comparison result");
moreprint("  -only files...  Include only matches (default: *) example *pippo*.mp4");
moreprint("  -always files   Always (force) adding some file");
moreprint("  -noattributes   Ignore/don't save file attributes or permissions");
moreprint("  -index F        Extract: create index F for archive");
moreprint("                  Add: create suffix for archive indexed by F, update F");
moreprint("  -sN -summary N  IGNORED (deprecated)");
moreprint("  ########## franz's fork ###########");
moreprint("  -checksum       Store SHA1+CRC32 for every file");
moreprint("  -verify         Force re-read of file during t (test command)");
moreprint("  -noeta          Do not show ETA");
moreprint("  -pakka          Output for PAKKA (briefly)");
moreprint("  -verbose        Show excluded file");
moreprint("  -zfs            Skip path including .zfs");
moreprint("  -noqnap         Skip path including @Recently-Snapshot and @Recycle");
moreprint("  -forcewindows   Take $DATA$ and System Volume Information");
moreprint("  -xls            Do NOT always force XLS");
moreprint("  -nopath         Do not store path");
moreprint("  -nosort         Do not sort file when adding or listing");
moreprint("  -find      X    Search for X in full filename (ex. list)");
moreprint("  -replace   Y    Replace X with Y in full filename (ex. list)");
moreprint("  -n         X    Only print last X lines in dir (like tail)/first X (list)");
moreprint("  -minsize   X    Skip files by length (add(), list(), dir())");
moreprint("  -maxsize   X    Skip files by length (add(), list(), dir())");
moreprint("  -comment foo    Add/find ASCII comment string to versions");
#if defined(_WIN32) || defined(_WIN64)
moreprint("  -vss            Do a VSS for drive C: (Windows with administrative rights)");
#endif
moreprint("  -crc32c         In sha1 command use CRC32c instead of SHA1");
moreprint("  -crc32          In sha1 command use CRC32");
moreprint("  -xxhash         In sha1 command use xxHASH64");
moreprint("  -sha256         In sha1 command use SHA256");
moreprint("  -exec_ok fo.bat After OK launch fo.bat");
moreprint("  -exec_error kz  After NOT OK launch kz");
moreprint("  ########## Advanced options ###########");

moreprint("  -repack F [X]   Extract to new archive F with key X (default: none)");

sprintf(buffer,"  -tN -threads N  Use N threads (default: 0 = %d cores.",threads);
moreprint(buffer);
	
moreprint("  -fragment N     Use 2^N KiB average fragment size (default: 6)");
moreprint("  -mNB -method NB Use 2^B MiB blocks (0..11, default: 04, 14, 26..56)");
moreprint("  -method {xs}B[,N2]...[{ciawmst}[N1[,N2]...]]...  Advanced:");
moreprint("  x=journaling (default). s=streaming (no dedupe)");
moreprint("    N2: 0=no pre/post. 1,2=packed,byte LZ77. 3=BWT. 4..7=0..3 with E8E9");
moreprint("    N3=LZ77 min match. N4=longer match to try first (0=none). 2^N5=search");
moreprint("    depth. 2^N6=hash table size (N6=B+21: suffix array). N7=lookahead");
moreprint("    Context modeling defaults shown below:");
moreprint("  c0,0,0: context model. N1: 0=ICM, 1..256=CM max count. 1000..1256 halves");
moreprint("    memory. N2: 1..255=offset mod N2, 1000..1255=offset from N2-1000 byte");
moreprint("    N3...: order 0... context masks (0..255). 256..511=mask+byte LZ77");
moreprint("    parse state, >1000: gap of N3-1000 zeros");
moreprint("  i: ISSE chain. N1=context order. N2...=order increment");
moreprint("  a24,0,0: MATCH: N1=hash multiplier. N2=halve buffer. N3=halve hash tab");
moreprint("  w1,65,26,223,20,0: Order 0..N1-1 word ISSE chain. A word is bytes");
moreprint("    N2..N2+N3-1 ANDed with N4, hash mulitpiler N5, memory halved by N6");
moreprint("  m8,24: MIX all previous models, N1 context bits, learning rate N2");
moreprint("  s8,32,255: SSE last model. N1 context bits, count range N2..N3");
moreprint("  t8,24: MIX2 last 2 models, N1 context bits, learning rate N2");

  exit(1);
}


// return a/b such that there is exactly one "/" in between, and
// in Windows, any drive letter in b the : is removed and there
// is a "/" after.
string append_path(string a, string b) {
  int na=a.size();
  int nb=b.size();
#ifndef unix
  if (nb>1 && b[1]==':') {  // remove : from drive letter
    if (nb>2 && b[2]!='/') b[1]='/';
    else b=b[0]+b.substr(2), --nb;
  }
#endif
  if (nb>0 && b[0]=='/') b=b.substr(1);
  if (na>0 && a[na-1]=='/') a=a.substr(0, na-1);
  return a+"/"+b;
}

// Rename name using tofiles[]
string Jidac::rename(string name) 
{
	if (flagnopath)
	{
		string myname=name;
		for (unsigned i=0; i<files.size(); ++i) 
		{
			string pathwithbar=files[i];
			if (pathwithbar[pathwithbar.size()-1]!='/')
				pathwithbar+="/";

			  if (strstr(name.c_str(), pathwithbar.c_str())!=0)
			  {
				myreplace(myname,pathwithbar,"");
				if (flagverbose)
					printf("Cutting path <<%s>> to <<%s>>\n",files[i].c_str(),myname.c_str());
				return myname;
			  }

		}
		return myname;
		/*
		const size_t last_slash_idx = myname.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
			myname.erase(0, last_slash_idx + 1);
		myname="./"+myname;
		///printf("KKKKKKKKKKKKKKKKKKKK <<%s>> -> <<%s>>\n",name.c_str(),myname.c_str());
		return myname;
		*/
	}
	
  if (files.size()==0 && tofiles.size()>0)  // append prefix tofiles[0]
    name=append_path(tofiles[0], name);
  else {  // replace prefix files[i] with tofiles[i]
    const int n=name.size();
    for (unsigned i=0; i<files.size() && i<tofiles.size(); ++i) {
      const int fn=files[i].size();
      if (fn<=n && files[i]==name.substr(0, fn))
///        return tofiles[i]+"/"+name.substr(fn);
        return tofiles[i]+name.substr(fn);
    }
  }
  return name;
}

//// some support functions to string compare case insensitive
bool comparechar(char c1, char c2)
{
    if (c1 == c2)
        return true;
    else if (std::toupper(c1) == std::toupper(c2))
        return true;
    return false;
}

bool stringcomparei(std::string str1, std::string str2)
{
    return ( (str1.size() == str2.size() ) &&
             std::equal(str1.begin(), str1.end(), str2.begin(), &comparechar) );
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
inline char *  migliaia3(uint64_t n)
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
inline char *  migliaia4(uint64_t n)
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
inline char *  migliaia5(uint64_t n)
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
char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}
inline char* tohuman(uint64_t i_bytes)
{
	static char io_buf[30];
	
	char const *myappend[] = {"B","KB","MB","GB","TB","PB"};
	char length = sizeof(myappend)/sizeof(myappend[0]);
	double mybytes=i_bytes;
	unsigned int i=0;
	if (i_bytes > 1024) 
		for (i=0;(i_bytes / 1024) > 0 && i<length-1; i++, i_bytes /= 1024)
			mybytes = i_bytes / 1024.0;
	sprintf(io_buf, "%.02lf %s",mybytes,myappend[i]);
	return io_buf;
}
inline char* tohuman2(uint64_t i_bytes)
{
	static char io_buf[30];
	
	char const *myappend[] = {"B","KB","MB","GB","TB","PB"};
	char length = sizeof(myappend)/sizeof(myappend[0]);
	double mybytes=i_bytes;
	unsigned int i=0;
	if (i_bytes > 1024) 
		for (i=0;(i_bytes / 1024) > 0 && i<length-1; i++, i_bytes /= 1024)
			mybytes = i_bytes / 1024.0;
	sprintf(io_buf, "%.02lf %s",mybytes,myappend[i]);
	return io_buf;
}
inline char* tohuman3(uint64_t i_bytes)
{
	static char io_buf[30];
	
	char const *myappend[] = {"B","KB","MB","GB","TB","PB"};
	char length = sizeof(myappend)/sizeof(myappend[0]);
	double mybytes=i_bytes;
	unsigned int i=0;
	if (i_bytes > 1024) 
		for (i=0;(i_bytes / 1024) > 0 && i<length-1; i++, i_bytes /= 1024)
			mybytes = i_bytes / 1024.0;
	sprintf(io_buf, "%.02lf %s",mybytes,myappend[i]);
	return io_buf;
}
inline char* tohuman4(uint64_t i_bytes)
{
	static char io_buf[30];
	
	char const *myappend[] = {"B","KB","MB","GB","TB","PB"};
	char length = sizeof(myappend)/sizeof(myappend[0]);
	double mybytes=i_bytes;
	unsigned int i=0;
	if (i_bytes > 1024) 
		for (i=0;(i_bytes / 1024) > 0 && i<length-1; i++, i_bytes /= 1024)
			mybytes = i_bytes / 1024.0;
	sprintf(io_buf, "%.02lf %s",mybytes,myappend[i]);
	return io_buf;
}




// Parse the command line. Return 1 if error else 0.
int Jidac::doCommand(int argc, const char** argv) {

  // Initialize options to default values
  g_exec_error="";
  g_exec_ok="";
  g_exec_text="";
  command=0;
  flagforce=false;
  fragment=6;
  minsize=0;
  maxsize=0;
  all=0;
  password=0;  // no password
  index=0;
  method="";  // 0..5
  flagnoattributes=false;
  repack=0;
  new_password=0;
  summary=0; // detailed: -1
  menoenne=0;
  versioncomment="";
  flagtest=false;  // -test
  flagskipzfs=false; 
  flagverbose=false;
  flagnoqnap=false;
  flagforcewindows=false;
  flagnopath=false;
  flagnoeta=false;
  flagpakka=false;
  flagvss=false;
  flagnosort=false;
  flagchecksum=false;
  flagcrc32c=false;
  flagverify=false;
  flagxxhash=false;
  flagcrc32=false;
  flagsha256=false;
  flagdonotforcexls=false;
  flagcomment=false;
  threads=0; // 0 = auto-detect
  version=DEFAULT_VERSION;
  date=0;
  searchfrom="";
  replaceto="";

  /// if we have a -pakka, set the flag early
	for (unsigned int i=0; i<argc; i++)
	{
		const string parametro=argv[i];
		if (stringcomparei(parametro,"-pakka"))
				flagpakka=true;
	}
  
	if (!flagpakka)
	moreprint("zpaqfranz v" ZPAQ_VERSION " journaling archiver, compiled " __DATE__);
	
/// check some magic to show help in heuristic way
/// I know, it's bad, but help is always needed!

	if (argc==1) // zero 
	{
		usage();
		exit(0);
	}

	if (argc==2) // only ONE 
	{
		const string parametro=argv[1];
		if  ((stringcomparei(parametro,"help"))
			||
			(stringcomparei(parametro,"-h"))
			||
			(stringcomparei(parametro,"?"))
			||
			(stringcomparei(parametro,"--help"))
			||
			(stringcomparei(parametro,"-help")))
			{
				usagefull();
				exit(0);
			}
		if ((stringcomparei(parametro,"-he"))
			||
			(stringcomparei(parametro,"-helpe"))
			||
			(stringcomparei(parametro,"-examples")))
			{
				examples();
				exit(0);
			}
		if (stringcomparei(parametro,"-diff"))
		{
			differences();
			exit(0);
		}
	}

	
  // Init archive state
  ht.resize(1);  // element 0 not used
  ver.resize(1); // version 0
  dhsize=dcsize=0;

  // Get date
  time_t now=time(NULL);
  tm* t=gmtime(&now);
  date=(t->tm_year+1900)*10000000000LL+(t->tm_mon+1)*100000000LL
      +t->tm_mday*1000000+t->tm_hour*10000+t->tm_min*100+t->tm_sec;

  // Get optional options
  for (int i=1; i<argc; ++i) {
    const string opt=argv[i];  // read command
    if ((opt=="add" || opt=="extract" || opt=="list" 
         || opt=="a"  || opt=="x" || opt=="p" || opt=="t" ||opt=="test" || opt=="l")
        && i<argc-1 && argv[i+1][0]!='-' && command==0) {
      command=opt[0];
      if (opt=="extract") command='x';
      if (opt=="test") command='t';
	  archive=argv[++i];  // append ".zpaq" to archive if no extension
      const char* slash=strrchr(argv[i], '/');
      const char* dot=strrchr(slash ? slash : argv[i], '.');
      if (!dot && archive!="") archive+=".zpaq";
      while (++i<argc && argv[i][0]!='-')  // read filename args
        files.push_back(argv[i]);
      --i;
    }
	else if (opt=="s")
	{
		command=opt[0];
		while (++i<argc && argv[i][0]!='-')  // read filename args
			files.push_back(argv[i]);
                i--;
	}
    else if (opt=="c")
	{
		command=opt[0];
		while (++i<argc && argv[i][0]!='-')  // read filename args
			files.push_back(argv[i]);
                i--;
	}
    else if (opt=="sha1")
	{
		command=opt[3];
		while (++i<argc && argv[i][0]!='-')  // read filename args
			files.push_back(argv[i]);
                i--;
	}
    else if (opt=="dir")
	{
		command='d';
		while (++i<argc && argv[i][0]!='-')  // read filename args
			files.push_back(argv[i]);
                i--;
	}
    else if (opt.size()<2 || opt[0]!='-') usage();
    else if (opt=="-all") {
      all=4;
      if (i<argc-1 && isdigit(argv[i+1][0])) all=atoi(argv[++i]);
    }
	else if (opt=="-exec_error")
	{
		if (g_exec_error=="")
		{
			if (++i<argc && argv[i][0]!='-')  
				g_exec_error=argv[i];
			else
				i--;
		}
	}
    else if (opt=="-exec_ok")
	{
		if (g_exec_ok=="")
		{
			if (++i<argc && argv[i][0]!='-')  
				g_exec_ok=argv[i];
			else
				i--;
		}
	}
    else if (opt=="-comment")
	{
		flagcomment=true;
		
		if (++i<argc && argv[i][0]!='-')  
				versioncomment=argv[i];
		else
			i--;
		
	}
    else if (opt=="-force" || opt=="-f") flagforce=true;
    else if (opt=="-fragment" && i<argc-1) fragment=atoi(argv[++i]);
    else if (opt=="-minsize" && i<argc-1) minsize=atoi(argv[++i]);
    else if (opt=="-maxsize" && i<argc-1) maxsize=atoi(argv[++i]);
    else if (opt=="-n" && i<argc-1) menoenne=atoi(argv[++i]);
    else if (opt=="-index" && i<argc-1) index=argv[++i];
    else if (opt=="-key" && i<argc-1) {
      libzpaq::SHA256 sha256;
      for (const char* p=argv[++i]; *p; ++p) sha256.put(*p);
      memcpy(password_string, sha256.result(), 32);
      password=password_string;
    }
    else if (opt=="-method" && i<argc-1) method=argv[++i];
    else if (opt[1]=='m') method=argv[i]+2;
    else if (opt=="-noattributes") flagnoattributes=true;
    else if (opt=="-not") {  // read notfiles
      while (++i<argc && argv[i][0]!='-') {
        if (argv[i][0]=='=') nottype=argv[i];
        else notfiles.push_back(argv[i]);
      }
      --i;
    }
    else if (opt=="-only") {  // read onlyfiles
      while (++i<argc && argv[i][0]!='-')
        onlyfiles.push_back(argv[i]);
      --i;
    }
    else if (opt=="-always") {  // read alwaysfiles
      while (++i<argc && argv[i][0]!='-')
        alwaysfiles.push_back(argv[i]);
      --i;
    }
    else if (opt=="-repack" && i<argc-1) {
      repack=argv[++i];
      if (i<argc-1 && argv[i+1][0]!='-') {
        libzpaq::SHA256 sha256;
        for (const char* p=argv[++i]; *p; ++p) sha256.put(*p);
        memcpy(new_password_string, sha256.result(), 32);
        new_password=new_password_string;
      }
    }
    else if (opt=="-summary" && i<argc-1) summary=atoi(argv[++i]);
    else if (opt=="-test") flagtest=true;
    else if (opt=="-zfs") flagskipzfs=true;
    else if (opt=="-xls") flagdonotforcexls=true;
    else if (opt=="-verbose") flagverbose=true;
    else if (opt=="-noqnap") flagnoqnap=true;
    else if (opt=="-nopath") flagnopath=true;
    else if (opt=="-nosort") flagnosort=true;
    else if (opt=="-checksum") flagchecksum=true;
    else if (opt=="-crc32c") flagcrc32c=true;
    else if (opt=="-xxhash") flagxxhash=true;
    else if (opt=="-sha256") flagsha256=true;
    else if (opt=="-crc32") flagcrc32=true;
    else if (opt=="-verify") flagverify=true;
    else if (opt[1]=='s') summary=atoi(argv[i]+2);
    else if (opt=="-forcewindows") flagforcewindows=true;
	/*
	{		
        flagforcewindows=true;
        notfiles.push_back("*:*:$DATA");
    }
	*/
   else if (opt=="-noeta") flagnoeta=true;
   else if (opt=="-pakka") flagpakka=true;  // we already have pakka flag, but this is for remember
   else if (opt=="-vss") flagvss=true;
    else if (opt=="-to") {  // read tofiles
      while (++i<argc && argv[i][0]!='-')
	  {

// fix to (on windows) -to "z:\pippo pluto" going to a mess
		string mytemp=mytrim(argv[i]);
		myreplaceall(mytemp,"\"","");
///printf("Faccio to <<%s>>\n",mytemp.c_str());
///	   tofiles.push_back(argv[i]);
	   tofiles.push_back(mytemp);
	  }
      if (tofiles.size()==0) tofiles.push_back("");
      --i;
    }
	else if (opt=="-find")
	{
		if (searchfrom=="" && strlen(argv[i+1])>=1)
		{
			searchfrom=argv[i+1];
			i++;
		}
	}
	else if (opt=="-replace")
	{
		if (replaceto=="" && strlen(argv[i+1])>=1)
		{
			replaceto=argv[i+1];
			i++;
		}
	}
    else if (opt=="-threads" && i<argc-1) threads=atoi(argv[++i]);
    else if (opt[1]=='t') threads=atoi(argv[i]+2);
    else if (opt=="-until" && i+1<argc) {  // read date

      // Read digits from multiple args and fill in leading zeros
      version=0;
      int digits=0;
      if (argv[i+1][0]=='-') {  // negative version
        version=atol(argv[i+1]);
        if (version>-1) usage();
        ++i;
      }
      else {  // positive version or date
        while (++i<argc && argv[i][0]!='-') {
          for (int j=0; ; ++j) {
            if (isdigit(argv[i][j])) {
              version=version*10+argv[i][j]-'0';
              ++digits;
            }
            else {
              if (digits==1) version=version/10*100+version%10;
              digits=0;
              if (argv[i][j]==0) break;
            }
          }
        }
        --i;
      }
	  
	
		

      // Append default time
      if (version>=19000000LL     && version<=29991231LL)
        version=version*100+23;
      if (version>=1900000000LL   && version<=2999123123LL)
        version=version*100+59;
      if (version>=190000000000LL && version<=299912312359LL)
        version=version*100+59;
      if (version>9999999) {
        if (version<19000101000000LL || version>29991231235959LL) {
          fflush(stdout);
          fprintf(stderr,
            "Version date %1.0f must be 19000101000000 to 29991231235959\n",
             double(version));
			g_exec_text="Version date must be 19000101000000 to 29991231235959";
          exit(1);
        }
        date=version;
      }
    }
    else {
      printf("Unknown option ignored: %s\n", argv[i]);
    ///  usage();
    }
  }
	if (g_exec_error!="")
			printf("franz: exec_error %s\n",g_exec_error.c_str());
	if (g_exec_ok!="")
			printf("franz: exec_ok %s\n",g_exec_ok.c_str());
	if (minsize)
			printf("franz: minsize %s\n",migliaia(minsize));
	if (maxsize)
			printf("franz: maxsize %s\n",migliaia(maxsize));
	if (menoenne)
			printf("franz: -n %u\n",menoenne);
			
  	if (flagskipzfs)
			printf("franz:SKIP ZFS on (-zfs)\n");
	
	if (flagnoqnap)
			printf("franz:Exclude QNAP snap & trash (-noqnap)\n");

	if (flagforcewindows)
			printf("franz:force  Windows stuff (-forcewindows)\n");

	if (flagnopath)
			printf("franz:Do not store path (-nopath)\n");

	if (flagnosort)
			printf("franz:Do not sort files (-nosort)\n");

	if (flagdonotforcexls)
			printf("franz:Do not force XLS (-xls)\n");
	
	if (flagverbose)
			printf("franz:VERBOSE list of files on (-verbose)\n");
	
	if (flagchecksum)
			printf("franz:checksumming every file with CRC32, store SHA1\n");
	
	if (flagcrc32c)
			printf("franz:use CRC-32C\n");
	
	if (flagcrc32)
			printf("franz:use CRC32\n");
	
	if (flagxxhash)
			printf("franz:use xxhash\n");
	
	if (flagsha256)
			printf("franz:use SHA256\n");
	
	if (flagverify)
			printf("franz:do a VERIFY (re-read from filesystem)\n");
	
	if (searchfrom!="")
			printf("franz:FIND    >>>>%s<<<<<\n",searchfrom.c_str());

	if (replaceto!="")
			printf("franz:REPLACE >>>>%s<<<<<\n",replaceto.c_str());
#if defined(_WIN32) || defined(_WIN64)

	if (flagvss)
	{
			printf("franz:VSS Volume Shadow Copy (-vss)\n");
			if (!isadmin())
			{
				printf("Impossible to run VSS: admin rights required\n");
				return 2;
			}
			
	}
#endif
	if (flagcomment)
			printf("franz:use comment\n");
/*
	if (versioncomment!="")
			printf("franz: Version comment  >>>>%s<<<<<\n",versioncomment.c_str());
	*/	
  // Set threads
  if (threads<1) threads=numberOfProcessors();

#ifdef _OPENMP
  omp_set_dynamic(threads);
#endif

  // Test date
  if (now==-1 || date<19000000000000LL || date>30000000000000LL)
    error("date is incorrect, use -until YYYY-MM-DD HH:MM:SS to set");

  // Adjust negative version
  if (version<0) {
    Jidac jidac(*this);
    jidac.version=DEFAULT_VERSION;
    jidac.read_archive(archive.c_str());
    version+=jidac.ver.size()-1;
    printf("Version %1.0f\n", version+.0);
  }
  // Load dynamic functions in Windows Vista and later
#ifndef unix
  HMODULE h=GetModuleHandle(TEXT("kernel32.dll"));
  if (h==NULL) printerr("GetModuleHandle");
  else {
    findFirstStreamW=
      (FindFirstStreamW_t)GetProcAddress(h, "FindFirstStreamW");
    findNextStreamW=
      (FindNextStreamW_t)GetProcAddress(h, "FindNextStreamW");
  }
///  if (!findFirstStreamW || !findNextStreamW)
///broken XP parser    printf("Alternate streams not supported in Windows XP.\n");
#endif

  // Execute command
  if (command=='a' && files.size()>0) return add();
  else if (command=='d') return dir();
  else if (command=='c') return dircompare(false);
  else if (command=='x') return extract();
  else if (command=='t') return test();
  else if (command=='p') return paranoid();
  else if (command=='l') return list();
  else if (command=='s') return dircompare(true); 
  else if (command=='1') return summa();
  else usage();
  
  
  return 0;
}

/////////////////////////// read_archive //////////////////////////////

// Read arc up to -date into ht, dt, ver. Return place to
// append. If errors is not NULL then set it to number of errors found.
int64_t Jidac::read_archive(const char* arc, int *errors, int i_myappend) {
  if (errors) *errors=0;
  dcsize=dhsize=0;
  assert(ver.size()==1);
  unsigned files=0;  // count
///printf("ZA1\n");

  // Open archive
  InputArchive in(arc, password);
///printf("ZA2\n");

  if (!in.isopen()) {
    if (command!='a') {
      fflush(stdout);
      printUTF8(arc, stderr);
      fprintf(stderr, " not found.\n");
	  g_exec_text=arc;
	  g_exec_text+=" not found";
      if (errors) ++*errors;
    }
    return 0;
  }
  if (!flagpakka)
  {
  printUTF8(arc);
  if (version==DEFAULT_VERSION) printf(": ");
  else printf(" -until %1.0f: ", version+0.0);
  fflush(stdout);
  }
  // Test password
  {
    char s[4]={0};
    const int nr=in.read(s, 4);
    if (nr>0 && memcmp(s, "7kSt", 4) && (memcmp(s, "zPQ", 3) || s[3]<1))
	{
      printf("zpaqfranz error:password incorrect\n");
	  error("password incorrect");
	}
    in.seek(-nr, SEEK_CUR);
  }
///printf("ZA3\n");

  // Scan archive contents
  string lastfile=archive; // last named file in streaming format
  if (lastfile.size()>5 && lastfile.substr(lastfile.size()-5)==".zpaq")
    lastfile=lastfile.substr(0, lastfile.size()-5); // drop .zpaq
  int64_t block_offset=32*(password!=0);  // start of last block of any type
  int64_t data_offset=block_offset;    // start of last block of d fragments
  bool found_data=false;   // exit if nothing found
  bool first=true;         // first segment in archive?
  StringBuffer os(32832);  // decompressed block
  const bool renamed=command=='l' || command=='a';

	uint64_t parts=0;
  // Detect archive format and read the filenames, fragment sizes,
  // and hashes. In JIDAC format, these are in the index blocks, allowing
  // data to be skipped. Otherwise the whole archive is scanned to get
  // this information from the segment headers and trailers.
  bool done=false;
  printf("\n");
  while (!done) {
    libzpaq::Decompresser d;
	///printf("New iteration\r");
    try {
      d.setInput(&in);
      double mem=0;
      while (d.findBlock(&mem)) {
        found_data=true;

        // Read the segments in the current block
        StringWriter filename, comment;
        int segs=0;  // segments in block
        bool skip=false;  // skip decompression?
        while (d.findFilename(&filename)) {
          if (filename.s.size()) {
            for (unsigned i=0; i<filename.s.size(); ++i)
              if (filename.s[i]=='\\') filename.s[i]='/';
            lastfile=filename.s.c_str();
          }
          comment.s="";
          d.readComment(&comment);

          // Test for JIDAC format. Filename is jDC<fdate>[cdhi]<num>
          // and comment ends with " jDC\x01". Skip d (data) blocks.
          if (comment.s.size()>=4
              && comment.s.substr(comment.s.size()-4)=="jDC\x01") {
            if (filename.s.size()!=28 || filename.s.substr(0, 3)!="jDC")
              error("bad journaling block name");
            if (skip) error("mixed journaling and streaming block");

            // Read uncompressed size from comment
            int64_t usize=0;
            unsigned i;
            for (i=0; i<comment.s.size() && isdigit(comment.s[i]); ++i) {
              usize=usize*10+comment.s[i]-'0';
              if (usize>0xffffffff) error("journaling block too big");
            }

            // Read the date and number in the filename
            int64_t fdate=0, num=0;
            for (i=3; i<17 && isdigit(filename.s[i]); ++i)
              fdate=fdate*10+filename.s[i]-'0';
            if (i!=17 || fdate<19000000000000LL || fdate>=30000000000000LL)
              error("bad date");
            for (i=18; i<28 && isdigit(filename.s[i]); ++i)
              num=num*10+filename.s[i]-'0';
            if (i!=28 || num>0xffffffff) error("bad fragment");


            // Decompress the block.
            os.resize(0);
            os.setLimit(usize);
            d.setOutput(&os);
            libzpaq::SHA1 sha1;
            d.setSHA1(&sha1);
            if (strchr("chi", filename.s[17])) {
              if (mem>1.5e9) error("index block requires too much memory");
              d.decompress();
              char sha1result[21]={0};
              d.readSegmentEnd(sha1result);
              if ((int64_t)os.size()!=usize) error("bad block size");
              if (usize!=int64_t(sha1.usize())) error("bad checksum size");
              if (sha1result[0] && memcmp(sha1result+1, sha1.result(), 20))
                error("bad checksum");
			if ( ++parts % 1000 ==0)
				if (!flagnoeta)
				printf("Block %08lu K\r",parts/1000);
            }
            else
              d.readSegmentEnd();

            // Transaction header (type c).
            // If in the future then stop here, else read 8 byte data size
            // from input and jump over it.
            if (filename.s[17]=='c') {
              if (os.size()<8) error("c block too small");
              data_offset=in.tell()+1-d.buffered();
              const char* s=os.c_str();
              int64_t jmp=btol(s);
              if (jmp<0) printf("Incomplete transaction ignored\n");
              if (jmp<0
                  || (version<19000000000000LL && int64_t(ver.size())>version)
                  || (version>=19000000000000LL && version<fdate)) {
                done=true;  // roll back to here
                goto endblock;
              }
              else {
                dcsize+=jmp;
                if (jmp) in.seek(data_offset+jmp, SEEK_SET);
                ver.push_back(VER());
                ver.back().firstFragment=ht.size();
                ver.back().offset=block_offset;
                ver.back().data_offset=data_offset;
                ver.back().date=ver.back().lastdate=fdate;
                ver.back().csize=jmp;
                if (all) {
                  string fn=itos(ver.size()-1, all)+"/"; ///peusa1 versioni
                  if (renamed) fn=rename(fn);
                  if (isselected(fn.c_str(), false,-1))
                    dt[fn].date=fdate;
                }
                if (jmp) goto endblock;
              }
            }

            // Fragment table (type h).
            // Contents is bsize[4] (sha1[20] usize[4])... for fragment N...
            // where bsize is the compressed block size.
            // Store in ht[].{sha1,usize}. Set ht[].csize to block offset
            // assuming N in ascending order.
            else if (filename.s[17]=='h') {
              assert(ver.size()>0);
              if (fdate>ver.back().lastdate) ver.back().lastdate=fdate;
              if (os.size()%24!=4) error("bad h block size");
              const unsigned n=(os.size()-4)/24;
              if (num<1 || num+n>0xffffffff) error("bad h fragment");
              const char* s=os.c_str();
              const unsigned bsize=btoi(s);
              dhsize+=bsize;
              assert(ver.size()>0);
              if (int64_t(ht.size())>num) {
                fflush(stdout);
                fprintf(stderr,
                  "Unordered fragment tables: expected >= %d found %1.0f\n",
                  int(ht.size()), double(num));
				 g_exec_text="Unordered fragment tables";
              }
              for (unsigned i=0; i<n; ++i) {
                if (i==0) {
                  block.push_back(Block(num, data_offset));
                  block.back().usize=8;
                  block.back().bsize=bsize;
                  block.back().frags=os.size()/24;
                }
                while (int64_t(ht.size())<=num+i) ht.push_back(HT());
                memcpy(ht[num+i].sha1, s, 20);
                s+=20;
                assert(block.size()>0);
                unsigned f=btoi(s);
                if (f>0x7fffffff) error("fragment too big");
                block.back().usize+=(ht[num+i].usize=f)+4u;
              }
              data_offset+=bsize;
            }

            // Index (type i)
            // Contents is: 0[8] filename 0 (deletion)
            // or:       date[8] filename 0 na[4] attr[na] ni[4] ptr[ni][4]
            // Read into DT
            else if (filename.s[17]=='i') 
			{
				
              assert(ver.size()>0);
              if (fdate>ver.back().lastdate) ver.back().lastdate=fdate;
              const char* s=os.c_str();
              const char* const end=s+os.size();
			
				while (s+9<=end) 
				{
					DT dtr;
					dtr.date=btol(s);  // date
					if (dtr.date) 
						++ver.back().updates;
					else 
						++ver.back().deletes;
					const int64_t len=strlen(s);
					if (len>65535) 
						error("filename too long");
					
					string fn=s;  // filename renamed
					if (all) 
					{
						if (i_myappend)
							fn=itos(ver.size()-1, all)+"|$1"+fn;
						else
							fn=append_path(itos(ver.size()-1, all), fn);
					}
					const bool issel=isselected(fn.c_str(), renamed,-1);
					s+=len+1;  // skip filename
					if (s>end) 
						error("filename too long");
					if (dtr.date) 
					{
						++files;
						if (s+4>end) 
							error("missing attr");
						unsigned na=0;
	
						na=btoi(s);  // attr bytes
				  
						if (s+na>end || na>65535) 
							error("attr too long");
				  
						if (na>FRANZOFFSET) //this is the -checksum options. Get SHA1 0x0 CRC32 0x0
						{
							for (unsigned int i=0;i<50;i++)
								dtr.sha1hex[i]=*(s+(na-FRANZOFFSET)+i);
							dtr.sha1hex[50]=0x0;
						}
				
						for (unsigned i=0; i<na; ++i, ++s)  // read attr
							if (i<8) 
								dtr.attr+=int64_t(*s&255)<<(i*8);
					
						if (flagnoattributes) 
							dtr.attr=0;
						if (s+4>end) 
							error("missing ptr");
						unsigned ni=btoi(s);  // ptr list size
				  
						if (ni>(end-s)/4u) 
							error("ptr list too long");
						if (issel) 
							dtr.ptr.resize(ni);
						for (unsigned i=0; i<ni; ++i) 
						{  // read ptr
							const unsigned j=btoi(s);
							if (issel) 
								dtr.ptr[i]=j;
						}
					}
					if (issel) 
					dt[fn]=dtr;
				}  // end while more files
            }  // end if 'i'
            else 
			{
				printf("Skipping %s %s\n",filename.s.c_str(), comment.s.c_str());
				error("Unexpected journaling block");
            }
          }  // end if journaling

          // Streaming format
          else 
		  {

            // If previous version does not exist, start a new one
            if (ver.size()==1) 
			{
				if (version<1) 
				{
					done=true;
					goto endblock;
				}
				ver.push_back(VER());
				ver.back().firstFragment=ht.size();
				ver.back().offset=block_offset;
				ver.back().csize=-1;
			}

			char sha1result[21]={0};
			d.readSegmentEnd(sha1result);
            skip=true;
            string fn=lastfile;
            if (all) fn=append_path(itos(ver.size()-1, all), fn); ///peusa3
            if (isselected(fn.c_str(), renamed,-1)) {
              DT& dtr=dt[fn];
              if (filename.s.size()>0 || first) {
                ++files;
                dtr.date=date;
                dtr.attr=0;
                dtr.ptr.resize(0);
                ++ver.back().updates;
              }
              dtr.ptr.push_back(ht.size());
            }
            assert(ver.size()>0);
            if (segs==0 || block.size()==0)
              block.push_back(Block(ht.size(), block_offset));
            assert(block.size()>0);
            ht.push_back(HT(sha1result+1, -1));
          }  // end else streaming
          ++segs;
          filename.s="";
          first=false;
        }  // end while findFilename
        if (!done) block_offset=in.tell()-d.buffered();
      }  // end while findBlock
      done=true;
    }  // end try
    catch (std::exception& e) {
      in.seek(-d.buffered(), SEEK_CUR);
      fflush(stdout);
      fprintf(stderr, "Skipping block at %1.0f: %s\n", double(block_offset),
              e.what());
		g_exec_text="Skipping block";
      if (errors) ++*errors;
    }
endblock:;
  }  // end while !done
  if (in.tell()>32*(password!=0) && !found_data)
    error("archive contains no data");
	if (!flagpakka)
	printf("%d versions, %s files, %s fragments, %s bytes (%s)\n", 
      int(ver.size()-1), migliaia2(files), migliaia3(unsigned(ht.size())-1),
      migliaia(block_offset),tohuman(block_offset));

  // Calculate file sizes
	for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) 
	{
		for (unsigned i=0; i<p->second.ptr.size(); ++i) 
		{
			unsigned j=p->second.ptr[i];
			if (j>0 && j<ht.size() && p->second.size>=0) 
			{
				if (ht[j].usize>=0) 
					p->second.size+=ht[j].usize;
				else 
					p->second.size=-1;  // unknown size
			}
		}
	}
  
	return block_offset;
}

// Test whether filename and attributes are selected by files, -only, and -not
// If rn then test renamed filename.
// if i_size >0 check file size too (minsize,maxsize)

bool Jidac::isselected(const char* filename, bool rn,int64_t i_size) 
{
	bool matched=true;
    if (flagskipzfs) //this is an "automagically" exclude for zfs' snapshots
		if (iszfs(filename))
		{
			if (flagverbose)
				printf("Verbose: Skip .zfs %s\n",filename);
			return false;
		}

	if (flagnoqnap) //this is an "automagically" exclude for qnap's snapshots
		if (
			(strstr(filename, "@Recently-Snapshot")) 
			||
			(strstr(filename, "@Recycle")) 
			)
			{
				if (flagverbose)
					printf("Verbose: Skip qnap %s\n",filename);
				return false;
			}
			
	if (!flagforcewindows) //this "automagically" exclude anything with System Volume Information
	{
		if (strstr(filename, "System Volume Information")) 
		{
			if (flagverbose)
				printf("Verbose: Skip System Volume Information %s\n",filename);
			return false;
		}
		if (strstr(filename, "$RECYCLE.BIN")) 
		{
			if (flagverbose)
				printf("Verbose: Skip trash %s\n",filename);
			return false;
		}
	}			
	
	if  (!flagforcewindows)
		if (isads(filename))
			return false;
	
	if (i_size>0)
	{
		if (maxsize)
			if (maxsize<i_size)
			{
				if (flagverbose)
					printf("Verbose: (-maxsize) too big   %19s %s\n",migliaia(i_size),filename);
				return false;
			}
		if (minsize)
			if (minsize>i_size)
			{
				if (flagverbose)
					printf("Verbose: (-minsize) too small %19s %s\n",migliaia(i_size),filename);
				return false;
			}
	}
  if (files.size()>0) {
    matched=false;
    for (unsigned i=0; i<files.size() && !matched; ++i) {
      if (rn && i<tofiles.size()) {
        if (ispath(tofiles[i].c_str(), filename)) matched=true;
      }
      else if (ispath(files[i].c_str(), filename)) matched=true;
    }
  }
  if (!matched) return false;
  if (onlyfiles.size()>0) 
  {
	matched=false;
    for (unsigned i=0; i<onlyfiles.size() && !matched; ++i)
	{
		///printf("ZEKE:Testo %s su %s\n",onlyfiles[i].c_str(),filename);
	 if (ispath(onlyfiles[i].c_str(), filename))
	 {
        matched=true;
		///printf("MAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
	 }
	}
  }
  if (!matched) return false;
  for (unsigned i=0; i<notfiles.size(); ++i) {
    if (ispath(notfiles[i].c_str(), filename))
      return false;
  }
  return true;
}

// Return the part of fn up to the last slash
string path(const string& fn) {
  int n=0;
  for (int i=0; fn[i]; ++i)
    if (fn[i]=='/' || fn[i]=='\\') n=i+1;
  return fn.substr(0, n);
}

// Insert external filename (UTF-8 with "/") into dt if selected
// by files, onlyfiles, and notfiles. If filename
// is a directory then also insert its contents.
// In Windows, filename might have wildcards like "file.*" or "dir/*"
// In zpaqfranz, sometimes, we DO not want to recurse
void Jidac::scandir(DTMap& i_edt,string filename, bool i_recursive)
{

  // Don't scan diretories excluded by -not
  for (unsigned i=0; i<notfiles.size(); ++i)
    if (ispath(notfiles[i].c_str(), filename.c_str()))
      return;

	  
	if (flagskipzfs)
		if (iszfs(filename))
		{
			if (flagverbose)
				printf("Verbose: Skip .zfs ----> %s\n",filename.c_str());
			return;
		}
	if (flagnoqnap)
	{
		if (filename.find("@Recently-Snapshot")!=-1)
		{
			if (flagverbose)
				printf("Verbose: Skip qnap snapshot ----> %s\n",filename.c_str());

			return;
		}
		if (filename.find("@Recycle")!=-1)
		{
			if (flagverbose)
				printf("Verbose: Skip qnap recycle ----> %s\n",filename.c_str());
			return;
		}
	}
	
#ifdef unix
// Add regular files and directories
  while (filename.size()>1 && filename[filename.size()-1]=='/')
    filename=filename.substr(0, filename.size()-1);  // remove trailing /
	struct stat sb;
	if (!lstat(filename.c_str(), &sb)) 
	{
		if (S_ISREG(sb.st_mode))
		addfile(i_edt,filename, decimal_time(sb.st_mtime), sb.st_size,'u'+(sb.st_mode<<8));

    // Traverse directory
		if (S_ISDIR(sb.st_mode)) 
		{
			addfile(i_edt,filename=="/" ? "/" : filename+"/", decimal_time(sb.st_mtime),0, 'u'+(int64_t(sb.st_mode)<<8));
			DIR* dirp=opendir(filename.c_str());
			if (dirp) 
			{
				for (dirent* dp=readdir(dirp); dp; dp=readdir(dirp)) 
				{
					if (strcmp(".", dp->d_name) && strcmp("..", dp->d_name)) 
					{
						string s=filename;
						if (s!="/") s+="/";
						s+=dp->d_name;
						if (i_recursive)        
							scandir(i_edt,s);
						else
						{
							if (!lstat(s.c_str(), &sb)) 
							{
								if (S_ISREG(sb.st_mode))
									addfile(i_edt,s, decimal_time(sb.st_mtime), sb.st_size,'u'+(sb.st_mode<<8));
								if (S_ISDIR(sb.st_mode)) 
									addfile(i_edt,s=="/" ? "/" :s+"/", decimal_time(sb.st_mtime),0, 'u'+(int64_t(sb.st_mode)<<8));
							}
						}          			
					}
				}
				closedir(dirp);
			}
			else
				perror(filename.c_str());
		}
	}
	else
		perror(filename.c_str());

#else  // Windows: expand wildcards in filename

  // Expand wildcards
  WIN32_FIND_DATA ffd;
  string t=filename;
  if (t.size()>0 && t[t.size()-1]=='/') t+="*";
  HANDLE h=FindFirstFile(utow(t.c_str()).c_str(), &ffd);
  if (h==INVALID_HANDLE_VALUE
      && GetLastError()!=ERROR_FILE_NOT_FOUND
      && GetLastError()!=ERROR_PATH_NOT_FOUND)
    printerr(t.c_str());
  while (h!=INVALID_HANDLE_VALUE) {

    // For each file, get name, date, size, attributes
    SYSTEMTIME st;
    int64_t edate=0;
    if (FileTimeToSystemTime(&ffd.ftLastWriteTime, &st))
      edate=st.wYear*10000000000LL+st.wMonth*100000000LL+st.wDay*1000000
            +st.wHour*10000+st.wMinute*100+st.wSecond;
    const int64_t esize=ffd.nFileSizeLow+(int64_t(ffd.nFileSizeHigh)<<32);
    const int64_t eattr='w'+(int64_t(ffd.dwFileAttributes)<<8);

    // Ignore links, the names "." and ".." or any unselected file
    t=wtou(ffd.cFileName);
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT
        || t=="." || t=="..") edate=0;  // don't add
    string fn=path(filename)+t;

    // Save directory names with a trailing / and scan their contents
    // Otherwise, save plain files
    if (edate) 
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) fn+="/";
		addfile(i_edt,fn, edate, esize, eattr);
		
		if (i_recursive)
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				fn+="*";
				scandir(i_edt,fn);
			}
		

      // enumerate alternate streams (Win2003/Vista or later)
      else 
	  if (findFirstStreamW && findNextStreamW) 
	  {
		if (flagforcewindows)
		{
			WIN32_FIND_STREAM_DATA fsd;
			HANDLE ah=findFirstStreamW(utow(fn.c_str()).c_str(),
				FindStreamInfoStandard, &fsd, 0);
			while (ah!=INVALID_HANDLE_VALUE && findNextStreamW(ah, &fsd))
				addfile(i_edt,fn+wtou(fsd.cStreamName), edate,
              fsd.StreamSize.QuadPart, eattr);
			if (ah!=INVALID_HANDLE_VALUE) FindClose(ah);
		}
      }
    }
    if (!FindNextFile(h, &ffd)) {
      if (GetLastError()!=ERROR_NO_MORE_FILES) printerr(fn.c_str());
      break;
    }
  }
  FindClose(h);
#endif
}

	
	
// Add external file and its date, size, and attributes to dt
void Jidac::addfile(DTMap& i_edt,string filename, int64_t edate,
                    int64_t esize, int64_t eattr) {
  if (!isselected(filename.c_str(), false,esize)) 
	return;
  
  DT& d=i_edt[filename];
  d.date=edate;
  d.size=esize;
  d.attr=flagnoattributes?0:eattr;
  d.data=0;
  g_bytescanned+=esize;
  g_filescanned++;
 if (flagnoeta==false)
 { 
  if (!(i_edt.size() % 1000))
  {
    double scantime=mtime()-global_start+1;
	printf("Scanning %s %2.2fs %d file/s (%s)\r",migliaia(i_edt.size()),scantime/1000.0,(int)(i_edt.size()/(scantime/1000.0)),migliaia2(g_bytescanned));
	fflush(stdout);
 }
 
 }
}

//////////////////////////////// add //////////////////////////////////

// Append n bytes of x to sb in LSB order
inline void puti(libzpaq::StringBuffer& sb, uint64_t x, int n) {
  for (; n>0; --n) sb.put(x&255), x>>=8;
}

// Print percent done (td/ts) and estimated time remaining
// two modes: "normal" (old zpaqfranz) "pakka" (new)
void print_progress(int64_t ts, int64_t td,int64_t i_scritti) 
{
	static int ultimapercentuale=0;
	static int ultimaeta=0;
	
	if (flagnoeta==true)
		return;
		
	if (td>ts) 
		td=ts;
	
	if (td<1000000)
		return;
		
	double eta=0.001*(mtime()-global_start)*(ts-td)/(td+1.0);
	int secondi=(mtime()-global_start)/1000;
	if (secondi==0)
		secondi=1;
	
	int percentuale=int(td*100.0/(ts+0.5));

///	printf("%d %d\n",int(eta),ultimaeta);
	
	if (flagpakka)
	{
		if (((percentuale%10)==0) ||(percentuale==1))
			if (percentuale!=ultimapercentuale)
			{
				ultimapercentuale=percentuale;
				printf("%03d%% %d:%02d:%02d %20s of %20s %s/sec\n", percentuale,
					int(eta/3600), int(eta/60)%60, int(eta)%60, migliaia(td), migliaia2(ts),migliaia3(td/secondi));
			}
	}
	else 
	{
		if (int(eta)!=ultimaeta)
		{
			ultimaeta=int(eta);
			if (i_scritti>0)
			printf("%6.2f%% %d:%02d:%02d (%9s) -> (%9s) of (%9s) %9s/sec\r", td*100.0/(ts+0.5),
			int(eta/3600), int(eta/60)%60, int(eta)%60, tohuman(td),tohuman2(i_scritti),tohuman3(ts),tohuman4(td/secondi));
			else
			printf("%6.2f%% %d:%02d:%02d (%9s) of (%9s) %9s/sec\r", td*100.0/(ts+0.5),
			int(eta/3600), int(eta/60)%60, int(eta)%60, tohuman(td),tohuman2(ts),tohuman3(td/secondi));
			
			
			
		}
	}				
}

// A CompressJob is a queue of blocks to compress and write to the archive.
// Each block cycles through states EMPTY, FILLING, FULL, COMPRESSING,
// COMPRESSED, WRITING. The main thread waits for EMPTY buffers and
// fills them. A set of compressThreads waits for FULL threads and compresses
// them. A writeThread waits for COMPRESSED buffers at the front
// of the queue and writes and removes them.

// Buffer queue element
struct CJ {
  enum {EMPTY, FULL, COMPRESSING, COMPRESSED, WRITING} state;
  StringBuffer in;       // uncompressed input
  StringBuffer out;      // compressed output
  string filename;       // to write in filename field
  string comment;        // if "" use default
  string method;         // compression level or "" to mark end of data
  Semaphore full;        // 1 if in is FULL of data ready to compress
  Semaphore compressed;  // 1 if out contains COMPRESSED data
  CJ(): state(EMPTY) {}
};

// Instructions to a compression job
class CompressJob {
public:
  Mutex mutex;           // protects state changes
private:
  int job;               // number of jobs
  CJ* q;                 // buffer queue
  unsigned qsize;        // number of elements in q
  int front;             // next to remove from queue
  libzpaq::Writer* out;  // archive
  Semaphore empty;       // number of empty buffers ready to fill
  Semaphore compressors; // number of compressors available to run
public:
  friend ThreadReturn compressThread(void* arg);
  friend ThreadReturn writeThread(void* arg);
  CompressJob(int threads, int buffers, libzpaq::Writer* f):
      job(0), q(0), qsize(buffers), front(0), out(f) {
    q=new CJ[buffers];
    if (!q) throw std::bad_alloc();
    init_mutex(mutex);
    empty.init(buffers);
    compressors.init(threads);
    for (int i=0; i<buffers; ++i) {
      q[i].full.init(0);
      q[i].compressed.init(0);
    }
  }
  ~CompressJob() {
    for (int i=qsize-1; i>=0; --i) {
      q[i].compressed.destroy();
      q[i].full.destroy();
    }
    compressors.destroy();
    empty.destroy();
    destroy_mutex(mutex);
    delete[] q;
  }      
  void write(StringBuffer& s, const char* filename, string method,
             const char* comment=0);
  vector<int> csize;  // compressed block sizes
};

// Write s at the back of the queue. Signal end of input with method=""
void CompressJob::write(StringBuffer& s, const char* fn, string method,
                        const char* comment) {
  for (unsigned k=(method=="")?qsize:1; k>0; --k) {
    empty.wait();
    lock(mutex);
    unsigned i, j;
    for (i=0; i<qsize; ++i) {
      if (q[j=(i+front)%qsize].state==CJ::EMPTY) {
        q[j].filename=fn?fn:"";
        q[j].comment=comment?comment:"jDC\x01";
        q[j].method=method;
        q[j].in.resize(0);
        q[j].in.swap(s);
        q[j].state=CJ::FULL;
        q[j].full.signal();
        break;
      }
    }
    release(mutex);
    assert(i<qsize);  // queue should not be full
  }
}

// Compress data in the background, one per buffer
ThreadReturn compressThread(void* arg) {
  CompressJob& job=*(CompressJob*)arg;
  int jobNumber=0;
  try {

    // Get job number = assigned position in queue
    lock(job.mutex);
    jobNumber=job.job++;
    assert(jobNumber>=0 && jobNumber<int(job.qsize));
    CJ& cj=job.q[jobNumber];
    release(job.mutex);

    // Work until done
    while (true) {
      cj.full.wait();
      lock(job.mutex);

      // Check for end of input
      if (cj.method=="") {
        cj.compressed.signal();
        release(job.mutex);
        return 0;
      }

      // Compress
      assert(cj.state==CJ::FULL);
      cj.state=CJ::COMPRESSING;
      release(job.mutex);
      job.compressors.wait();
	       libzpaq::compressBlock(&cj.in, &cj.out, cj.method.c_str(),
          cj.filename.c_str(), cj.comment=="" ? 0 : cj.comment.c_str());
      cj.in.resize(0);
      lock(job.mutex);

	///	printf("\nFarei qualcosa <<%s>> %08X size %ld\n",myblock.filename.c_str(),myblock.crc32,myblock.crc32size);

	  cj.state=CJ::COMPRESSED;
      cj.compressed.signal();
      job.compressors.signal();
      release(job.mutex);
    }
  }
  catch (std::exception& e) {
    lock(job.mutex);
    fflush(stdout);
    fprintf(stderr, "job %d: %s\n", jobNumber+1, e.what());
	g_exec_text="job error";
    release(job.mutex);
    exit(1);
  }
  return 0;
}

// Write compressed data to the archive in the background
ThreadReturn writeThread(void* arg) {
  CompressJob& job=*(CompressJob*)arg;
  try {

    // work until done
    while (true) {

      // wait for something to write
      CJ& cj=job.q[job.front];  // no other threads move front
      cj.compressed.wait();

      // Quit if end of input
      lock(job.mutex);
      if (cj.method=="") {
        release(job.mutex);
        return 0;
      }

      // Write to archive
      assert(cj.state==CJ::COMPRESSED);
      cj.state=CJ::WRITING;
      job.csize.push_back(cj.out.size());
      if (job.out && cj.out.size()>0) {
        release(job.mutex);
        assert(cj.out.c_str());
        const char* p=cj.out.c_str();
        int64_t n=cj.out.size();
        
		g_scritti+=n; // very rude
		const int64_t N=1<<30;
        while (n>N) {
          job.out->write(p, N);
          p+=N;
          n-=N;
        }
        job.out->write(p, n);
        lock(job.mutex);
      }
      cj.out.resize(0);
      cj.state=CJ::EMPTY;
      job.front=(job.front+1)%job.qsize;
      job.empty.signal();
      release(job.mutex);
    }
  }
  catch (std::exception& e) {
    fflush(stdout);
    fprintf(stderr, "zpaqfranz exiting from writeThread: %s\n", e.what());
	g_exec_text="exiting from writethread";
    exit(1);
  }
  return 0;
}

// Write a ZPAQ compressed JIDAC block header. Output size should not
// depend on input data.
void writeJidacHeader(libzpaq::Writer *out, int64_t date,
                      int64_t cdata, unsigned htsize) {
  if (!out) return;
  assert(date>=19000000000000LL && date<30000000000000LL);
  StringBuffer is;
  puti(is, cdata, 8);
  libzpaq::compressBlock(&is, out, "0",
      ("jDC"+itos(date, 14)+"c"+itos(htsize, 10)).c_str(), "jDC\x01");
}

// Maps sha1 -> fragment ID in ht with known size
class HTIndex {
  vector<HT>& htr;  // reference to ht
  libzpaq::Array<unsigned> t;  // sha1 prefix -> index into ht
  unsigned htsize;  // number of IDs in t

  // Compuate a hash index for sha1[20]
  unsigned hash(const char* sha1) {
    return (*(const unsigned*)sha1)&(t.size()-1);
  }

public:
  // r = ht, sz = estimated number of fragments needed
  HTIndex(vector<HT>& r, size_t sz): htr(r), t(0), htsize(1) {
    int b;
    for (b=1; sz*3>>b; ++b);
    t.resize(1, b-1);
    update();
  }

  // Find sha1 in ht. Return its index or 0 if not found.
  unsigned find(const char* sha1) {
    unsigned h=hash(sha1);
    for (unsigned i=0; i<t.size(); ++i) {
      if (t[h^i]==0) return 0;
      if (memcmp(sha1, htr[t[h^i]].sha1, 20)==0) return t[h^i];
    }
    return 0;
  }

  // Update index of ht. Do not index if fragment size is unknown.
  void update() {
    char zero[20]={0};
    while (htsize<htr.size()) {
      if (htsize>=t.size()/4*3) {
        t.resize(t.size(), 1);
        htsize=1;
      }
      if (htr[htsize].usize>=0 && memcmp(htr[htsize].sha1, zero, 20)!=0) {
        unsigned h=hash((const char*)htr[htsize].sha1);
        for (unsigned i=0; i<t.size(); ++i) {
          if (t[h^i]==0) {
            t[h^i]=htsize;
            break;
          }
        }
      }
      ++htsize;
    }
  }    
};

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

// Sort by sortkey, then by full path
bool compareFilename(DTMap::iterator ap, DTMap::iterator bp) {
  if (ap->second.data!=bp->second.data)
    return ap->second.data<bp->second.data;
  return ap->first<bp->first;
}

// sort by sha1hex, without checks
bool comparesha1hex(DTMap::iterator i_primo, DTMap::iterator i_secondo) 
{
	return (strcmp(i_primo->second.sha1hex,i_secondo->second.sha1hex)<0);
}

// For writing to two archives at once
struct WriterPair: public libzpaq::Writer {
  OutputArchive *a, *b;
  void put(int c) {
    if (a) a->put(c);
    if (b) b->put(c);
  }
  void write(const char* buf, int n) {
    if (a) a->write(buf, n);
    if (b) b->write(buf, n);
  }
  WriterPair(): a(0), b(0) {}
};

#if defined(_WIN32) || defined(_WIN64)
//// VSS on Windows by... batchfile
//// delete all kind of shadows copies (if any)
void vss_deleteshadows()
{
	if (flagvss)
	{
		string	filebatch	=g_gettempdirectory()+"vsz.bat";
		
		printf("Starting delete VSS shadows\n");
    		
		if (fileexists(filebatch))
			if (remove(filebatch.c_str())!=0)
			{
				printf("Highlander batch  %s\n", filebatch.c_str());
				return;
			}
		
		FILE* batch=fopen(filebatch.c_str(), "wb");
		fprintf(batch,"@echo OFF\n");
		fprintf(batch,"@wmic shadowcopy delete /nointeractive\n");
		fclose(batch);
	
		waitexecute(filebatch,"",SW_HIDE);
	
		printf("End VSS delete shadows\n");
	}

}
#endif

/// work with a batch job
void avanzamento(int64_t i_lavorati,int64_t i_totali,int64_t i_inizio)
{
	static int ultimapercentuale=0;
	
	if (flagnoeta==true)
		return;
	
	int percentuale=int(i_lavorati*100.0/(i_totali+0.5));
	if (((percentuale%10)==0) && (percentuale>0))
	if (percentuale!=ultimapercentuale)
	{
		ultimapercentuale=percentuale;
				
		double eta=0.001*(mtime()-i_inizio)*(i_totali-i_lavorati)/(i_lavorati+1.0);
		int secondi=(mtime()-i_inizio)/1000;
		if (secondi==0)
			secondi=1;
				
		printf("%03d%% %d:%02d:%02d %20s of %20s %s/sec\n", percentuale,
		int(eta/3600), int(eta/60)%60, int(eta)%60, migliaia(i_lavorati), migliaia2(i_totali),migliaia3(i_lavorati/secondi));
		fflush(stdout);
	}
}


std::wstring utf8_to_utf16(const std::string& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            throw std::logic_error("not a UTF-8 string");
        }
        else if (ch <= 0xDF)
        {
            uni = ch&0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch&0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch&0x07;
            todo = 3;
        }
        else
        {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

//// get CRC32 of a file

uint32_t crc32_calc_file(const char * i_filename,const int64_t i_inizio,const int64_t i_totali,int64_t& io_lavorati)
{
	FILE* myfile = freadopen(i_filename);
	if(myfile==NULL )
		return 0;
		
	char data[65536*16];
    uint32_t crc=0;
	int got=0;
	
	while ((got=fread(data,sizeof(char),sizeof(data),myfile)) > 0) 
	{
		crc=crc32_16bytes (data, got, crc);
		io_lavorati+=got;	
		if ((flagnoeta==false) && (i_inizio>0))
			avanzamento(io_lavorati,i_totali,i_inizio);
	}
	fclose(myfile);
	return crc;
}


/// special function: get SHA1 AND CRC32 of a file
/// return a special string (HEX ASCII)
/// SHA1 (zero) CRC32 (zero)
/// output[40] and output[50] will be set to zero
/// a waste of space, but I do not care at all

/// note: take CRC-32 to a quick verify against CRC-32 of fragments
bool Jidac::getchecksum(string i_filename, char* o_sha1)
{
		
	if (i_filename=="")
		return false;
	
	if (!o_sha1)
		return false;
	if (isdirectory(i_filename))
		return false;
		
	
///	houston, we have a directory	
	if (isdirectory(i_filename))
	{
///	                    1234567890123456789012345678901234567890 12345678
//			sprintf(o_sha1,"THIS IS A DIRECTORY                    Z MYZZEKAN");
		sprintf(o_sha1,"THIS IS A DIRECTORY                      DIRECTOR");
		o_sha1[40]=0x0;
		o_sha1[50]=0x0;
		return true;
	}
	
	int64_t total_size		=0;  
	int64_t lavorati		=0;

	const int BUFSIZE	=65536*16;
	char 				buf[BUFSIZE];
	int 				n=BUFSIZE;

	uint32_t crc=0;
	
	FP myin=fopen(i_filename.c_str(), RB);
	if (myin==FPNULL)
	{
///		return false;
		ioerr(i_filename.c_str());
	
	}
    fseeko(myin, 0, SEEK_END);
    total_size=ftello(myin);
	fseeko(myin, 0, SEEK_SET);

	string nomecorto=mymaxfile(i_filename,40); /// cut filename to max 40 chars
	printf("  Checksumming (%19s) %40s",migliaia(total_size),nomecorto.c_str());

	int64_t blocco = (total_size / 8)+1;
	libzpaq::SHA1 sha1;
	unsigned int ultimapercentuale=200;
	char simboli[]= {'|','/','-','\\','|','/','-','\\'};
	while (1)
	{
		int r=fread(buf, 1, n, myin);
		sha1.write(buf, r);					// double tap: first SHA1 (I like it)
		crc=crc32_16bytes (buf, r, crc);	// than CRC32 (for checksum)

		if (r!=n) 
			break;
					
		lavorati+=r;
		unsigned int rapporto=lavorati/blocco;
		if (rapporto!=ultimapercentuale)
		{
			if (rapporto<=sizeof(simboli)) // just to be sure
				printf("\r%c",simboli[rapporto]); //,migliaia(total_size),nomecorto.c_str());
			ultimapercentuale=lavorati/blocco;
		}
	}
	fclose(myin);
	
	char sha1result[20];
	memcpy(sha1result, sha1.result(), 20);
	for (int j=0; j <= 19; j++)
		sprintf(o_sha1+j*2,"%02X", (unsigned char)sha1result[j]);
	o_sha1[40]=0x0;

	sprintf(o_sha1+41,"%08X",crc);
	o_sha1[50]=0x0;
	
	// example output 	96298A42DB2377092E75F5E649082758F2E0718F 
	//				  	(zero)
	//					3953EFF0
	//					(zero)
	printf("\r+\r");
		
	return true;
}



void Jidac::write715attr(libzpaq::StringBuffer& i_sb, uint64_t i_data, unsigned int i_quanti)
{
	assert(i_sb);
	assert(i_quanti<=8);
	puti(i_sb, i_quanti, 4);
	puti(i_sb, i_data, i_quanti);
}
void Jidac::writefranzattr(libzpaq::StringBuffer& i_sb, uint64_t i_data, unsigned int i_quanti,bool i_checksum,string i_filename,char *i_sha1,uint32_t i_crc32)
{
	
///	experimental fix: pad to 8 bytes (with zeros) for 7.15 enhanced compatibility

	assert(i_sb);
	if (i_checksum)
	{
//	ok, we are paranoid.

		assert(i_sha1);
		assert(i_filename.length()>0); // I do not like empty()
		assert(i_quanti<8);	//just to be sure at least 1 zero pad, so < and not <=

//	getchecksum return SHA1 (zero) CRC32.
		
		if (getchecksum(i_filename,i_sha1))
		{
//	we are paranoid: check if CRC-32, calculated from fragments during compression,
//	is equal to the CRC-32 taken from a post-read of the file
//	in other words we are checking that CRC32(file) == ( combine(CRC32{fragments-in-zpaq}) )
			char crc32fromfragments[9];
			sprintf(crc32fromfragments,"%08X",i_crc32);
			if (memcmp(crc32fromfragments,i_sha1+41,8)!=0)
			{
				printf("\nGURU: on file               %s\n",i_filename.c_str());
				printf("GURU: CRC-32 from fragments %s\n",crc32fromfragments);
				printf("GURU: CRC-32 from file      %s\n",i_sha1+41);
				error("Guru checking crc32");
			}

			puti(i_sb, 8+FRANZOFFSET, 4); 	// 8+FRANZOFFSET block
			puti(i_sb, i_data, i_quanti);
			puti(i_sb, 0, (8 - i_quanti));  // pad with zeros (for 7.15 little bug)
			i_sb.write(i_sha1,FRANZOFFSET);
			if (flagverbose)
				printf("SHA1 <<%s>> CRC32 <<%s>> %s\n",i_sha1,i_sha1+41,i_filename.c_str());
		}
		else
			write715attr(i_sb,i_data,i_quanti);
		
	}
	else
	if (flagcrc32)
	{
// with add -crc32 turn off, going back to 7.15 behaviour
		write715attr(i_sb,i_data,i_quanti);
	}
	else
	{
//	OK, we only write the CRC-32 from ZPAQ fragments (file integrity, not file check)
		memset(i_sha1,0,FRANZOFFSET);
		sprintf(i_sha1+41,"%08X",i_crc32);
		puti(i_sb, 8+FRANZOFFSET, 4); 	// 8+FRANZOFFSET block
		puti(i_sb, i_data, i_quanti);
		puti(i_sb, 0, (8 - i_quanti));  // pad with zeros (for 7.15 little bug)
		i_sb.write(i_sha1,FRANZOFFSET);
		if (flagverbose)
			printf("CRC32 by frag <<%s>> %s\n",i_sha1+41,i_filename.c_str());
		
	}
	
}

// Add or delete files from archive. Return 1 if error else 0.
int Jidac::add() 
{
	if (flagchecksum)
		if (flagnopath)
			error("incompatible -checksum and -nopath");
	
	if (flagvss)
	{
		if (flagtest)
			error("incompatible -vss and -test");
		if (flagnopath)
			error("incompatible -vss and -nopath");
	
	}
	
	g_scritti=0;
	
	if (flagvss)
	{

#if defined(_WIN32) || defined(_WIN64)

		char mydrive=0; // only ONE letter, only ONE VSS snapshot
		// abort for something like zpaqfranz a z:\1.zpaq c:\pippo d:\pluto -vss
		// abort for zpaqranz a z:\1.zpaq \\server\dati -vss
		// etc
		for (unsigned i=0; i<files.size(); ++i)
		{
			string temp=files[i];
		
			if (temp[1]!=':')
				error("VSS need X:something as path (missing :)");
				
			char currentdrive=toupper(temp[0]);
			if (!isalpha(currentdrive))
				error("VSS need X:something as path (X not isalpha)");
			if (mydrive)
			{
				if (mydrive!=currentdrive)
				error("VSS can only runs on ONE drive");
			}
			else	
			mydrive=currentdrive;
		}

			
		string	fileoutput	=g_gettempdirectory()+"outputz.txt";
		string	filebatch	=g_gettempdirectory()+"vsz.bat";
	
		printf("Starting VSS\n");
    
		if (fileexists(fileoutput))
			if (remove(fileoutput.c_str())!=0)
					error("Highlander outputz.txt");

		if (fileexists(filebatch))
			if (remove(filebatch.c_str())!=0)
				error("Highlander vsz.bat");
		
		
		FILE* batch=fopen(filebatch.c_str(), "wb");
		fprintf(batch,"@echo OFF\n");
		fprintf(batch,"@wmic shadowcopy delete /nointeractive\n");
		///fprintf(batch,"@wmic shadowcopy call create Volume=C:\\\n");
		fprintf(batch,"@wmic shadowcopy call create Volume=%c:\\\n",mydrive);
		fprintf(batch,"@vssadmin list shadows >%s\n",fileoutput.c_str());
		fclose(batch);
		
		waitexecute(filebatch,"",SW_HIDE);

		if (!fileexists(fileoutput))
			error("VSS Output file KO");
	
		string globalroot="";
		char line[1024];

		FILE* myoutput=fopen(fileoutput.c_str(), "rb");
		
		/* note that fgets don't strip the terminating \n (or \r\n) but we do not like it  */
		
		while (fgets(line, sizeof(line), myoutput)) 
        	if (strstr(line,"GLOBALROOT"))
			{
				globalroot=line;
				myreplaceall(globalroot,"\n","");
				myreplaceall(globalroot,"\r","");
				break;
			}
		
		fclose(myoutput);
		printf("global root |%s|\n",globalroot.c_str());
///	sometimes VSS is not available
		if (globalroot=="")
			error("VSS cannot continue. Maybe impossible to take snapshot?");


		string volume="";
		string vss_shadow="";
		
		int posstart=globalroot.find("\\\\");
			
		if (posstart>0)
			for (unsigned i=posstart; i<globalroot.length(); ++i)
				vss_shadow=vss_shadow+globalroot.at(i);
		printf("VSS VOLUME <<<%s>>>\n",vss_shadow.c_str());
		myreplaceall(vss_shadow,"\\","/");
		printf("VSS SHADOW <<<%s>>>\n",vss_shadow.c_str());
		
			
		tofiles.clear();
		for (unsigned i=0; i<files.size(); ++i)
			tofiles.push_back(files[i]);
		
		for (unsigned i=0; i<tofiles.size(); ++i)
			printf("TOFILESSS %s\n",tofiles[i].c_str());
		printf("\n");
		
		string replaceme=tofiles[0].substr(0,2);
		printf("-------------- %s -------\n",replaceme.c_str());
		
		for (unsigned i=0; i<files.size(); ++i)
		{
			printf("PRE  FILES %s\n",files[i].c_str());
			myreplace(files[i],replaceme,vss_shadow);
			printf("POST FILES %s\n",files[i].c_str());
			if (strstr(files[i].c_str(), "GLOBALROOT")==0)
				error("VSS fail: strange post files");

		}
#else
		printf("Volume Shadow Copies runs only on Windows\n");
#endif

		printf("End VSS\n");
	}


  // Read archive or index into ht, dt, ver.
  int errors=0;
  const bool archive_exists=exists(subpart(archive, 1).c_str());
  string arcname=archive;  // input archive name
  if (index) arcname=index;
  int64_t header_pos=0;
  if (exists(subpart(arcname, 1).c_str()))
    header_pos=read_archive(arcname.c_str(), &errors);

  // Set arcname, offset, header_pos, and salt to open out archive
  arcname=archive;  // output file name
  int64_t offset=0;  // total size of existing parts
  char salt[32]={0};  // encryption salt
  if (password) libzpaq::random(salt, 32);

  // Remote archive
  if (index) {
    if (dcsize>0) error("index is a regular archive");
    if (version!=DEFAULT_VERSION) error("cannot truncate with an index");
    offset=header_pos+dhsize;
    header_pos=32*(password && offset==0);
    arcname=subpart(archive, ver.size());
    if (exists(arcname)) {
      printUTF8(arcname.c_str(), stderr);
      fprintf(stderr, ": archive exists\n");
	  g_exec_text=arcname;
	  g_exec_text+=" archive exists";
      error("archive exists");
    }
    if (password) {  // derive archive salt from index
      FP fp=fopen(index, RB);
      if (fp!=FPNULL) {
        if (fread(salt, 1, 32, fp)!=32) error("cannot read salt from index");
        salt[0]^='7'^'z';
        fclose(fp);
      }
    }
  }

  // Local single or multi-part archive
  else {
    int parts=0;  // number of existing parts in multipart
    string part0=subpart(archive, 0);
    if (part0!=archive) {  // multi-part?
      for (int i=1;; ++i) {
        string partname=subpart(archive, i);
        if (partname==part0) error("too many archive parts");
        FP fp=fopen(partname.c_str(), RB);
        if (fp==FPNULL) break;
        ++parts;
        fseeko(fp, 0, SEEK_END);
        offset+=ftello(fp);
        fclose(fp);
      }
      header_pos=32*(password && parts==0);
      arcname=subpart(archive, parts+1);
      if (exists(arcname)) error("part exists");
    }

    // Get salt from first part if it exists
    if (password) {
      FP fp=fopen(subpart(archive, 1).c_str(), RB);
      if (fp==FPNULL) {
        if (header_pos>32) error("archive first part not found");
        header_pos=32;
      }
      else {
        if (fread(salt, 1, 32, fp)!=32) error("cannot read salt");
        fclose(fp);
      }
    }
  }
  if (exists(arcname)) printf("Updating ");
  else printf("Creating ");
  printUTF8(arcname.c_str());
  printf(" at offset %1.0f + %1.0f\n", double(header_pos), double(offset));

  // Set method
  if (method=="") method="1";
  if (method.size()==1) {  // set default blocksize
    if (method[0]>='2' && method[0]<='9') method+="6";
    else method+="4";
  }
  if (strchr("0123456789xs", method[0])==0)
    error("-method must begin with 0..5, x, s");
  assert(method.size()>=2);
  if (method[0]=='s' && index) error("cannot index in streaming mode");

  // Set block and fragment sizes
  if (fragment<0) fragment=0;
  const int log_blocksize=20+atoi(method.c_str()+1);
  if (log_blocksize<20 || log_blocksize>31) error("blocksize must be 0..11");
  const unsigned blocksize=(1u<<log_blocksize)-4096;
  const unsigned MAX_FRAGMENT=fragment>19 || (8128u<<fragment)>blocksize-12
      ? blocksize-12 : 8128u<<fragment;
  const unsigned MIN_FRAGMENT=fragment>25 || (64u<<fragment)>MAX_FRAGMENT
      ? MAX_FRAGMENT : 64u<<fragment;

  // Don't mix streaming and journaling
  for (unsigned i=0; i<block.size(); ++i) {
    if (method[0]=='s') {
      if (block[i].usize>=0)
        error("cannot update journaling archive in streaming format");
    }
    else if (block[i].usize<0)
      error("cannot update streaming archive in journaling format");
  }
  
  g_bytescanned=0;
  g_filescanned=0;
  g_worked=0;
  for (unsigned i=0; i<files.size(); ++i)
    scandir(edt,files[i].c_str());

/*
///	stub for fake file to be added (with someinfos)

	string tempfile=g_gettempdirectory()+"filelist.txt";
	myreplaceall(tempfile,"\\","/");

	printf("\nTemp dir <<%s>>\n",tempfile.c_str());
	FILE* myoutfile=fopen(tempfile.c_str(), "wb");
	fprintf(myoutfile,"This is filelist for version %d\n",ver.size());
	for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) 
	{
		if (!strstr(p->first.c_str(), ":$DATA"))
			{
				
				fprintf(myoutfile,"%s %19s ", dateToString(p->second.date).c_str(), migliaia(p->second.size));
				printUTF8(p->first.c_str(),myoutfile);
				fprintf(myoutfile,"\n");
    		}
	}
	fclose(myoutfile);
	
	files.push_back(tempfile);
	scandir(files[files.size()-1].c_str());
	*/
  // Sort the files to be added by filename extension and decreasing size
  vector<DTMap::iterator> vf;
  int64_t total_size=0;  // size of all input
  int64_t total_done=0;  // input deduped so far
  int64_t total_xls=0;
  int64_t file_xls=0;
  
  for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) {
		///printf("testo %s\n",p->first.c_str());
    DTMap::iterator a=dt.find(rename(p->first));
	string filename=rename(p->first);
	
	

	/// by default ALWAYS force XLS to be re-packed
	/// this is because sometimes Excel change the metadata (then SHA1 & CRC32)
	/// WITHOUT touching attr or filesize
	if (!flagdonotforcexls) 
		if (isxls(filename))
		{
			if (flagverbose)
				printf("ENFORCING XLS %s\n",filename.c_str());
			total_xls+=p->second.size;
			file_xls++;
		}
	
    if (a!=dt.end()) a->second.data=1;  // keep
    if (
			p->second.date && p->first!="" && p->first[p->first.size()-1]!='/' && 
			(
				flagforce || 
				a==dt.end() || 
				( (!(isads(filename))) && (!flagdonotforcexls) && (isxls(filename))) || 
				p->second.date!=a->second.date || 
				p->second.size!=a->second.size
			)
		) 
			
	{
      total_size+=p->second.size;

      // Key by first 5 bytes of filename extension, case insensitive
      int sp=0;  // sortkey byte position
      for (string::const_iterator q=p->first.begin(); q!=p->first.end(); ++q){
        uint64_t c=*q&255;
        if (c>='A' && c<='Z') c+='a'-'A';
        if (c=='/') sp=0, p->second.data=0;
        else if (c=='.') sp=8, p->second.data=0;
        else if (sp>3) p->second.data+=c<<(--sp*8);
      }

      // Key by descending size rounded to 16K
      int64_t s=p->second.size>>14;
      if (s>=(1<<24)) s=(1<<24)-1;
      p->second.data+=(1<<24)-s-1;
	  
		vf.push_back(p);
    }
	
  }
  
  
  if (!flagnosort)
	std::sort(vf.begin(), vf.end(), compareFilename);

 
  // Test for reliable access to archive
  if (archive_exists!=exists(subpart(archive, 1).c_str()))
    error("archive access is intermittent");

  // Open output
  OutputArchive out(arcname.c_str(), password, salt, offset);
  out.seek(header_pos, SEEK_SET);


  // Start compress and write jobs
  vector<ThreadID> tid(threads*2-1);
  ThreadID wid;
  CompressJob job(threads, tid.size(), &out);

  printf("Adding %s (%s) in %s files ",migliaia(total_size), tohuman(total_size),migliaia2(int(vf.size())));
	
	if (flagverbose)
		printf("-method %s -threads %d ",method.c_str(), threads);
	
	printf(" at %s",dateToString(date).c_str());
	
	
	if (flagcomment)
		if (versioncomment.length()>0)
			printf("<<%s>>",versioncomment.c_str());
	printf("\n");

  for (unsigned i=0; i<tid.size(); ++i) run(tid[i], compressThread, &job);
  run(wid, writeThread, &job);

  // Append in streaming mode. Each file is a separate block. Large files
  // are split into blocks of size blocksize.
  int64_t dedupesize=0;  // input size after dedupe
  if (method[0]=='s') {
    StringBuffer sb(blocksize+4096-128);
    for (unsigned fi=0; fi<vf.size(); ++fi) {
      DTMap::iterator p=vf[fi];
      print_progress(total_size, total_done,g_scritti);
	  
/*     
	 if (summary<=0) {
        printf("+ ");
        printUTF8(p->first.c_str());
        printf(" %1.0f\n", p->second.size+0.0);
      }
	  */
      FP in=fopen(p->first.c_str(), RB);
      if (in==FPNULL) {
        printerr(p->first.c_str());
        total_size-=p->second.size;
        ++errors;
        continue;
      }
      uint64_t i=0;
      const int BUFSIZE=4096;
      char buf[BUFSIZE];
      while (true) {
        int r=fread(buf, 1, BUFSIZE, in);
        sb.write(buf, r);
        i+=r;
        if (r==0 || sb.size()+BUFSIZE>blocksize) {
          string filename="";
          string comment="";
          if (i==sb.size()) {  // first block?
            filename=rename(p->first);
            comment=itos(p->second.date);
            if ((p->second.attr&255)>0) {
              comment+=" ";
              comment+=char(p->second.attr&255);
              comment+=itos(p->second.attr>>8);
            }
          }
          total_done+=sb.size();
          job.write(sb, filename.c_str(), method, comment.c_str());
          assert(sb.size()==0);
        }
        if (r==0) break;
      }
      fclose(in);
    }

    // Wait for jobs to finish
    job.write(sb, 0, "");  // signal end of input
    for (unsigned i=0; i<tid.size(); ++i) join(tid[i]);
    join(wid);

    // Done
    const int64_t outsize=out.tell();
    printf("%1.0f + (%1.0f -> %1.0f) = %s\n",
        double(header_pos),
        double(total_size),
        double(outsize-header_pos),
        migliaia(outsize));
    out.close();
    return errors>0;
  }  // end if streaming

  // Adjust date to maintain sequential order
  if (ver.size() && ver.back().lastdate>=date) {
    const int64_t newdate=decimal_time(unix_time(ver.back().lastdate)+1);
    fflush(stdout);
    fprintf(stderr, "Warning: adjusting date from %s to %s\n",
      dateToString(date).c_str(), dateToString(newdate).c_str());
    assert(newdate>date);
    date=newdate;
  }

  // Build htinv for fast lookups of sha1 in ht
  HTIndex htinv(ht, ht.size()+(total_size>>(10+fragment))+vf.size());
  const unsigned htsize=ht.size();  // fragments at start of update

  // reserve space for the header block
  writeJidacHeader(&out, date, -1, htsize);
  const int64_t header_end=out.tell();

  // Compress until end of last file
  assert(method!="");
  StringBuffer sb(blocksize+4096-128);  // block to compress
  unsigned frags=0;    // number of fragments in sb
  unsigned redundancy=0;  // estimated bytes that can be compressed out of sb
  unsigned text=0;     // number of fragents containing text
  unsigned exe=0;      // number of fragments containing x86 (exe, dll)
  const int ON=4;      // number of order-1 tables to save
  unsigned char o1prev[ON*256]={0};  // last ON order 1 predictions
  libzpaq::Array<char> fragbuf(MAX_FRAGMENT);
  vector<unsigned> blocklist;  // list of starting fragments
 

 // For each file to be added
  for (unsigned fi=0; fi<=vf.size(); ++fi) 
  {

        
	FP in=FPNULL;
    const int BUFSIZE=4096;  // input buffer
    char buf[BUFSIZE];
    int bufptr=0, buflen=0;  // read pointer and limit
    if (fi<vf.size()) {
      assert(vf[fi]->second.ptr.size()==0);
      DTMap::iterator p=vf[fi];
	
		///printf("lavoro %s\n",p->first.c_str());

      // Open input file
      bufptr=buflen=0;
      in=fopen(p->first.c_str(), RB);
	   
      if (in==FPNULL) {  // skip if not found
	  p->second.date=0;
        total_size-=p->second.size;
        printerr(p->first.c_str());
        ++errors;
        continue;
      }
	  
      p->second.data=1;  // add
    }

    // Read fragments
    int64_t fsize=0;  // file size after dedupe
    for (unsigned fj=0; true; ++fj) {
      int64_t sz=0;  // fragment size;
      unsigned hits=0;  // correct prediction count
      int c=EOF;  // current byte
      unsigned htptr=0;  // fragment index
      char sha1result[20]={0};  // fragment hash
      unsigned char o1[256]={0};  // order 1 context -> predicted byte
      if (fi<vf.size()) {
        int c1=0;  // previous byte
        unsigned h=0;  // rolling hash for finding fragment boundaries
        libzpaq::SHA1 sha1;
        assert(in!=FPNULL);
        while (true) {
          if (bufptr>=buflen) bufptr=0, buflen=fread(buf, 1, BUFSIZE, in);
          if (bufptr>=buflen) c=EOF;
          else c=(unsigned char)buf[bufptr++];
          if (c!=EOF) {
            if (c==o1[c1]) h=(h+c+1)*314159265u, ++hits;
            else h=(h+c+1)*271828182u;
            o1[c1]=c;
            c1=c;
            sha1.put(c);
            fragbuf[sz++]=c;
          }
          if (c==EOF
              || sz>=MAX_FRAGMENT
              || (fragment<=22 && h<(1u<<(22-fragment)) && sz>=MIN_FRAGMENT))
            break;
        }
        assert(sz<=MAX_FRAGMENT);
        total_done+=sz;

        // Look for matching fragment
        assert(uint64_t(sz)==sha1.usize());
        memcpy(sha1result, sha1.result(), 20);
        htptr=htinv.find(sha1result);
      }  // end if fi<vf.size()

/// OK, lets RE-compute CRC-32 of the fragment, and store
/// used for debug
        uint32_t crc;
		crc=crc32_16bytes (&fragbuf[0],(uint32_t) sz);
		if (htptr)
		{
	///		printf("\nHTPTR  %ld size %ld\n",htptr,sz);
			ht[htptr].crc32=crc;
			ht[htptr].crc32size=sz;
		}
	


      if (htptr==0) {  // not matched or last block

        // Analyze fragment for redundancy, x86, text.
        // Test for text: letters, digits, '.' and ',' followed by spaces
        //   and no invalid UTF-8.
        // Test for exe: 139 (mov reg, r/m) in lots of contexts.
        // 4 tests for redundancy, measured as hits/sz. Take the highest of:
        //   1. Successful prediction count in o1.
        //   2. Non-uniform distribution in o1 (counted in o2).
        //   3. Fraction of zeros in o1 (bytes never seen).
        //   4. Fraction of matches between o1 and previous o1 (o1prev).
        int text1=0, exe1=0;
        int64_t h1=sz;
        unsigned char o1ct[256]={0};  // counts of bytes in o1
        static const unsigned char dt[256]={  // 32768/((i+1)*204)
          160,80,53,40,32,26,22,20,17,16,14,13,12,11,10,10,
            9, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5,
            4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        for (int i=0; i<256; ++i) {
          if (o1ct[o1[i]]<255) h1-=(sz*dt[o1ct[o1[i]]++])>>15;
          if (o1[i]==' ' && (isalnum(i) || i=='.' || i==',')) ++text1;
          if (o1[i] && (i<9 || i==11 || i==12 || (i>=14 && i<=31) || i>=240))
            --text1;
          if (i>=192 && i<240 && o1[i] && (o1[i]<128 || o1[i]>=192))
            --text1;
          if (o1[i]==139) ++exe1;
        }
        text1=(text1>=3);
        exe1=(exe1>=5);
        if (sz>0) h1=h1*h1/sz; // Test 2: near 0 if random.
        unsigned h2=h1;
        if (h2>hits) hits=h2;
        h2=o1ct[0]*sz/256;  // Test 3: bytes never seen or that predict 0.
        if (h2>hits) hits=h2;
        h2=0;
        for (int i=0; i<256*ON; ++i)  // Test 4: compare to previous o1.
          h2+=o1prev[i]==o1[i&255];
        h2=h2*sz/(256*ON);
        if (h2>hits) hits=h2;
        if (hits>sz) hits=sz;

        // Start a new block if the current block is almost full, or at
        // the start of a file that won't fit or doesn't share mutual
        // information with the current block, or last file.
        bool newblock=false;
        if (frags>0 && fj==0 && fi<vf.size()) {
          const int64_t esize=vf[fi]->second.size;
          const int64_t newsize=sb.size()+esize+(esize>>14)+4096+frags*4;
          if (newsize>blocksize/4 && redundancy<sb.size()/128) newblock=true;
          if (newblock) {  // test for mutual information
            unsigned ct=0;
            for (unsigned i=0; i<256*ON; ++i)
              if (o1prev[i] && o1prev[i]==o1[i&255]) ++ct;
            if (ct>ON*2) newblock=false;
          }
          if (newsize>=blocksize) newblock=true;  // won't fit?
        }
        if (sb.size()+sz+80+frags*4>=blocksize) newblock=true; // full?
        if (fi==vf.size()) newblock=true;  // last file?
        if (frags<1) newblock=false;  // block is empty?

        // Pad sb with fragment size list, then compress
        if (newblock) {
          assert(frags>0);
          assert(frags<ht.size());
          for (unsigned i=ht.size()-frags; i<ht.size(); ++i)
            puti(sb, ht[i].usize, 4);  // list of frag sizes
          puti(sb, 0, 4); // omit first frag ID to make block movable
          puti(sb, frags, 4);  // number of frags
          string m=method;
          if (isdigit(method[0]))
            m+=","+itos(redundancy/(sb.size()/256+1))
                 +","+itos((exe>frags)*2+(text>frags));
          string fn="jDC"+itos(date, 14)+"d"+itos(ht.size()-frags, 10);
          print_progress(total_size, total_done,g_scritti);

/*         
		 if (summary<=0)
            printf("[%u..%u] %u -method %s\n",
                unsigned(ht.size())-frags, unsigned(ht.size())-1,
                unsigned(sb.size()), m.c_str());
*/  
  if (method[0]!='i')
		  {
				
				
			job.write(sb, fn.c_str(), m.c_str());
		  }
          else {  // index: don't compress data
            job.csize.push_back(sb.size());
            sb.resize(0);
          }
          assert(sb.size()==0);
          blocklist.push_back(ht.size()-frags);  // mark block start
		  frags=redundancy=text=exe=0;
          memset(o1prev, 0, sizeof(o1prev));
        }

        // Append fragbuf to sb and update block statistics
        assert(sz==0 || fi<vf.size());
        sb.write(&fragbuf[0], sz);
		
        
        ++frags;
        redundancy+=hits;
        exe+=exe1*4;
        text+=text1*2;
        if (sz>=MIN_FRAGMENT) {
          memmove(o1prev, o1prev+256, 256*(ON-1));
          memcpy(o1prev+256*(ON-1), o1, 256);
        }
      }  // end if frag not matched or last block

      // Update HT and ptr list
      if (fi<vf.size()) {
        if (htptr==0) {
          htptr=ht.size();
          ht.push_back(HT(sha1result, sz));
          htinv.update();
          fsize+=sz;
        }
        vf[fi]->second.ptr.push_back(htptr);

///OK store the crc. Very dirty (to be fixed in future)
		ht[htptr].crc32=crc;
		ht[htptr].crc32size=sz;
	}
      if (c==EOF) break;
	///printf("Fragment fj %ld\n",fj);
    }  // end for each fragment fj
	
	
    if (fi<vf.size()) {
      dedupesize+=fsize;
      DTMap::iterator p=vf[fi];
      print_progress(total_size, total_done,g_scritti);
/*     
	 if (summary<=0) {
        string newname=rename(p->first.c_str());
        DTMap::iterator a=dt.find(newname);
  	if (a==dt.end() || a->second.date==0) printf("+ ");
        else printf("# ");
        printUTF8(p->first.c_str());
		
 
		if (newname!=p->first) {
          printf(" -> ");
          printUTF8(newname.c_str());
        }
        printf(" %1.0f", p->second.size+0.0);
        if (fsize!=p->second.size) printf(" -> %1.0f", fsize+0.0);
        printf("\n");
		
      }
	  */
      assert(in!=FPNULL);
      fclose(in);
      in=FPNULL;
    }
  }  // end for each file fi
  assert(sb.size()==0);

  // Wait for jobs to finish
  job.write(sb, 0, "");  // signal end of input
  for (unsigned i=0; i<tid.size(); ++i) join(tid[i]);
  join(wid);

  // Open index
  salt[0]^='7'^'z';
  OutputArchive outi(index ? index : "", password, salt, 0);
  WriterPair wp;
  wp.a=&out;
  if (index) wp.b=&outi;
  writeJidacHeader(&outi, date, 0, htsize);

  // Append compressed fragment tables to archive
  int64_t cdatasize=out.tell()-header_end;
  StringBuffer is;
  assert(blocklist.size()==job.csize.size());
  blocklist.push_back(ht.size());
  for (unsigned i=0; i<job.csize.size(); ++i) {
    if (blocklist[i]<blocklist[i+1]) {
      puti(is, job.csize[i], 4);  // compressed size of block
      for (unsigned j=blocklist[i]; j<blocklist[i+1]; ++j) {
        is.write((const char*)ht[j].sha1, 20);
        puti(is, ht[j].usize, 4);
      }
      libzpaq::compressBlock(&is, &wp, "0",
          ("jDC"+itos(date, 14)+"h"+itos(blocklist[i], 10)).c_str(),
          "jDC\x01");
      is.resize(0);
    }
  }

  // Delete from archive
  int dtcount=0;  // index block header name
  int removed=0;  // count
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) {
    if (p->second.date && !p->second.data) {
      puti(is, 0, 8);
      is.write(p->first.c_str(), strlen(p->first.c_str()));
      is.put(0);
	  /*
      if (summary<=0) {
        printf("- ");
        printUTF8(p->first.c_str());
        printf("\n");
      }
	  */
      ++removed;
      if (is.size()>16000) {
        libzpaq::compressBlock(&is, &wp, "1",
            ("jDC"+itos(date)+"i"+itos(++dtcount, 10)).c_str(), "jDC\x01");
        is.resize(0);
      }
    }
  }
/*
	printf("\n#########################\n");
	
		for (int i=0;i<ht.size();i++)
		{
			unsigned int crc32=ht[i].crc32;
			if (crc32==0)
				printf("ZERO %d\n",i);
		}
	printf("#########################\n");
	*/

	bool flagshow;
	
  
  // Append compressed index to archive
  int added=0;  // count
  for (DTMap::iterator p=edt.begin();; ++p) 
  {
    if (p!=edt.end()) 
	{
		string filename=rename(p->first);
		
		if (searchfrom!="")
			if (replaceto!="")
				replace(filename,searchfrom,replaceto);
	

///	by using FRANZOFFSET we need to cut down the attr during this compare
	
 		DTMap::iterator a=dt.find(filename);
		if (p->second.date && (a==dt.end() // new file
         || a->second.date!=p->second.date  // date change
         || ((int32_t)a->second.attr && (int32_t)a->second.attr!=(int32_t)p->second.attr)  // attr ch. get less bits
         || a->second.size!=p->second.size  // size change
         || (p->second.data && a->second.ptr!=p->second.ptr))) 
		 { 


/*
			if (p->first==tempfile)
			{
				printf("FIX FILELIST\n");
				filename="filelist.txt:$DATA";
			}
*/	 
			uint32_t currentcrc32=0;
			for (unsigned i=0; i<p->second.ptr.size(); ++i)
					currentcrc32=crc32_combine(currentcrc32, ht[p->second.ptr[i]].crc32,ht[p->second.ptr[i]].crc32size);
		///	printf("!!!!!!!!!!!CRC32 %08X  <<%s>>\n",currentcrc32,p->first.c_str());
			
		 /*
				if (summary<=0 && p->second.data==0) 
				{  // not compressed?
					if (a==dt.end() || a->second.date==0) printf("+ ");
					else 
						printf("# ");
					
					printUTF8(p->first.c_str());
					if (filename!=p->first) 
					{
						printf(" -> ");
						printUTF8(filename.c_str());
					}
					printf("\n");
				}
			*/	
				++added;
				puti(is, p->second.date, 8);
				is.write(filename.c_str(), strlen(filename.c_str()));
				is.put(0);
				
				if ((p->second.attr&255)=='u') // unix attributes
					writefranzattr(is,p->second.attr,3,flagchecksum,filename,p->second.sha1hex,currentcrc32);
				else 
				if ((p->second.attr&255)=='w') // windows attributes
					writefranzattr(is,p->second.attr,5,flagchecksum,filename,p->second.sha1hex,currentcrc32);
				else 
					puti(is, 0, 4);  // no attributes
				
        if (a==dt.end() || p->second.data) a=p;  // use new frag pointers
        puti(is, a->second.ptr.size(), 4);  // list of frag pointers
        for (unsigned i=0; i<a->second.ptr.size(); ++i)
          puti(is, a->second.ptr[i], 4);
      }
    }
	else
	{
		if (versioncomment.length()>0)
		{
/// quickly store a fake file (for backward compatibility) with the version comment			
			///VCOMMENT 00000002 seconda_versione:$DATA
			char versioni8[9];
			sprintf(versioni8,"%08ld",ver.size());
			versioni8[8]=0x0;
			string fakefile="VCOMMENT "+string(versioni8)+" "+versioncomment+":$DATA"; //hidden windows file
			puti(is, 0, 8); // this is the "date". 0 is good, but do not pass paranoid compliance test. damn
			is.write(fakefile.c_str(), strlen(fakefile.c_str()));
			is.put(0);
		///	puti(is, 0, 4);  // no attributes
		///	puti(is, 0, 4);  // list of frag pointers
      
		}
	}
    if (is.size()>16000 || (is.size()>0 && p==edt.end())) {
      libzpaq::compressBlock(&is, &wp, "1",
          ("jDC"+itos(date)+"i"+itos(++dtcount, 10)).c_str(), "jDC\x01");
      is.resize(0);
    }
    if (p==edt.end()) break;
  }






  printf("\n%s +added, %s -removed.\n", migliaia(added), migliaia2(removed));
  assert(is.size()==0);

  // Back up and write the header
  outi.close();
  int64_t archive_end=out.tell();
  out.seek(header_pos, SEEK_SET);
  writeJidacHeader(&out, date, cdatasize, htsize);
  out.seek(0, SEEK_END);
  int64_t archive_size=out.tell();
  out.close();

  // Truncate empty update from archive (if not indexed)
  if (!index) {
    if (added+removed==0 && archive_end-header_pos==104) // no update
      archive_end=header_pos;
    if (archive_end<archive_size) {
      if (archive_end>0) {
        printf("truncating archive from %1.0f to %1.0f\n",
            double(archive_size), double(archive_end));
        if (truncate(arcname.c_str(), archive_end)) printerr(archive.c_str());
      }
      else if (archive_end==0) {
        if (delete_file(arcname.c_str())) {
          printf("deleted ");
          printUTF8(arcname.c_str());
          printf("\n");
        }
      }
    }
  }
  fflush(stdout);
  fprintf(stderr, "\n%s + (%s -> %s -> %s) = %s\n",
      migliaia(header_pos), migliaia2(total_size), migliaia3(dedupesize),
      migliaia4(archive_end-header_pos), migliaia5(archive_end));
	  
	if (total_xls)
  		printf("Forced XLS has included %s bytes in %s files\n",migliaia(total_xls),migliaia2(file_xls));
#if defined(_WIN32) || defined(_WIN64)
	if (flagvss)
		vss_deleteshadows();
#endif
	
	/// late -test    
	if (flagtest)
	{
		printf("\nzpaqfranz: doing a full (with file verify) test\n");
		ver.clear();
		block.clear();
		dt.clear();
		ht.clear();
		edt.clear();
		ht.resize(1);  // element 0 not used
		ver.resize(1); // version 0
		dhsize=dcsize=0;
		///return test();
		return verify();
	}	
		//unz(archive.c_str(), password,all);
  return errors>0;
}

/////////////////////////////// extract ///////////////////////////////

// Return true if the internal file p
// and external file contents are equal or neither exists.
// If filename is 0 then return true if it is possible to compare.
// In the meantime calc the crc32 of the entire file

bool Jidac::equal(DTMap::const_iterator p, const char* filename,uint32_t &o_crc32) 
{
	o_crc32=0;

  // test if all fragment sizes and hashes exist
  if (filename==0) 
  {
    static const char zero[20]={0};
    for (unsigned i=0; i<p->second.ptr.size(); ++i) {
      unsigned j=p->second.ptr[i];
      if (j<1 || j>=ht.size()
          || ht[j].usize<0 || !memcmp(ht[j].sha1, zero, 20))
        return false;
    }
    return true;
  }

  // internal or neither file exists
  if (p->second.date==0) return !exists(filename);

  // directories always match
  if (p->first!="" && p->first[p->first.size()-1]=='/')
    return exists(filename);

  // compare sizes
  FP in=fopen(filename, RB);
  if (in==FPNULL) return false;
  fseeko(in, 0, SEEK_END);
  if (ftello(in)!=p->second.size) return fclose(in), false;


	uint64_t lavorati=0;

  // compare hashes chunk by chunk.
  fseeko(in, 0, SEEK_SET);
  libzpaq::SHA1 sha1;
  const int BUFSIZE=4096;
  char buf[BUFSIZE];

	if (!flagnoeta)
		if (flagverbose || (p->second.size>100000000)) //only 100MB+ files
			printf("Checking %s                                          \r",migliaia(p->second.size));
	
	for (unsigned i=0; i<p->second.ptr.size(); ++i) 
	{
		unsigned f=p->second.ptr[i];
		if (f<1 || f>=ht.size() || ht[f].usize<0) 
			return fclose(in), false;
    
		for (int j=0; j<ht[f].usize;) 
		{
			int n=ht[f].usize-j;
			
			if (n>BUFSIZE) 
				n=BUFSIZE;
			
			int r=fread(buf, 1, n, in);
			o_crc32=crc32_16bytes (buf,r,o_crc32);
	
			g_worked+=r;
			
			if (r!=n) 
				return fclose(in), false;
			sha1.write(buf, n);
			j+=n;
		}
		if (memcmp(sha1.result(), ht[f].sha1, 20)!=0) 
			return fclose(in), false;
	}
	if (fread(buf, 1, BUFSIZE, in)!=0) 
		return fclose(in), false;
	fclose(in);
	return true;
}

// An extract job is a set of blocks with at least one file pointing to them.
// Blocks are extracted in separate threads, set READY -> WORKING.
// A block is extracted to memory up to the last fragment that has a file
// pointing to it. Then the checksums are verified. Then for each file
// pointing to the block, each of the fragments that it points to within
// the block are written in order.

struct ExtractJob {         // list of jobs
  Mutex mutex;              // protects state
  Mutex write_mutex;        // protects writing to disk
  int job;                  // number of jobs started
  Jidac& jd;                // what to extract
  FP outf;                  // currently open output file
  DTMap::iterator lastdt;   // currently open output file name
  double maxMemory;         // largest memory used by any block (test mode)
  int64_t total_size;       // bytes to extract
  int64_t total_done;       // bytes extracted so far
  ExtractJob(Jidac& j): job(0), jd(j), outf(FPNULL), lastdt(j.dt.end()),
      maxMemory(0), total_size(0), total_done(0) {
    init_mutex(mutex);
    init_mutex(write_mutex);
  }
  ~ExtractJob() {
    destroy_mutex(mutex);
    destroy_mutex(write_mutex);
  }
};

// Decompress blocks in a job until none are READY
ThreadReturn decompressThread(void* arg) {
  ExtractJob& job=*(ExtractJob*)arg;
  int jobNumber=0;

  // Get job number
  lock(job.mutex);
  jobNumber=++job.job;
  release(job.mutex);

  ///printf("K1 %s\n",job.jd.archive.c_str());
  // Open archive for reading
  InputArchive in(job.jd.archive.c_str(), job.jd.password);
  if (!in.isopen()) return 0;
  StringBuffer out;

  // Look for next READY job.
  int next=0;  // current job
  while (true) {
    lock(job.mutex);
    for (unsigned i=0; i<=job.jd.block.size(); ++i) {
      unsigned k=i+next;
      if (k>=job.jd.block.size()) k-=job.jd.block.size();
      if (i==job.jd.block.size()) {  // no more jobs?
        release(job.mutex);
        return 0;
      }
      Block& b=job.jd.block[k];
      if (b.state==Block::READY && b.size>0 && b.usize>=0) {
        b.state=Block::WORKING;
        release(job.mutex);
        next=k;
        break;
      }
    }
    Block& b=job.jd.block[next];

    // Get uncompressed size of block
    unsigned output_size=0;  // minimum size to decompress
    assert(b.start>0);
    for (unsigned j=0; j<b.size; ++j) {
      assert(b.start+j<job.jd.ht.size());
      assert(job.jd.ht[b.start+j].usize>=0);
      output_size+=job.jd.ht[b.start+j].usize;
    }

    // Decompress
    double mem=0;  // how much memory used to decompress
    try {
      assert(b.start>0);
      assert(b.start<job.jd.ht.size());
      assert(b.size>0);
      assert(b.start+b.size<=job.jd.ht.size());
	  ///printf("Andiamo su offset  %lld  %lld\n",b.offset,output_size);
      in.seek(b.offset, SEEK_SET);
      libzpaq::Decompresser d;
      d.setInput(&in);
      out.resize(0);
      assert(b.usize>=0);
      assert(b.usize<=0xffffffffu);
      out.setLimit(b.usize);
      d.setOutput(&out);
      if (!d.findBlock(&mem)) error("archive block not found");
      if (mem>job.maxMemory) job.maxMemory=mem;
      while (d.findFilename()) {
        d.readComment();
        while (out.size()<output_size && d.decompress(1<<14))
		{
		//		printf("|");
		};
        lock(job.mutex);
///		printf("\n");
	///		printf("out size %lld\n",out.size());
	//	printf(".........................................................\n");
    ///	printf("---------------- %s  %s\n",migliaia(job.total_size),migliaia2(job.total_done));
		print_progress(job.total_size, job.total_done,-1);
        /*
		if (job.jd.summary<=0)
          printf("[%d..%d] -> %1.0f\n", b.start, b.start+b.size-1,
              out.size()+0.0);
      */
	  
	  release(job.mutex);
        if (out.size()>=output_size) break;
        d.readSegmentEnd();
      }
      if (out.size()<output_size) {
        lock(job.mutex);
        fflush(stdout);
        fprintf(stderr, "output [%u..%u] %d of %u bytes\n",
             b.start, b.start+b.size-1, int(out.size()), output_size);
        release(job.mutex);
        error("unexpected end of compressed data");
      }

      // Verify fragment checksums if present
      uint64_t q=0;  // fragment start
      libzpaq::SHA1 sha1;
      assert(b.extracted==0);
      for (unsigned j=b.start; j<b.start+b.size; ++j) {
        assert(j>0 && j<job.jd.ht.size());
        assert(job.jd.ht[j].usize>=0);
        assert(job.jd.ht[j].usize<=0x7fffffff);
        if (q+job.jd.ht[j].usize>out.size())
          error("Incomplete decompression");
        char sha1result[20];
        sha1.write(out.c_str()+q, job.jd.ht[j].usize);
        memcpy(sha1result, sha1.result(), 20);
        q+=job.jd.ht[j].usize;
        if (memcmp(sha1result, job.jd.ht[j].sha1, 20)) {
          lock(job.mutex);
          fflush(stdout);
          fprintf(stderr, "Job %d: fragment %u size %d checksum failed\n",
                 jobNumber, j, job.jd.ht[j].usize);
		g_exec_text="fragment checksum failed";
          release(job.mutex);
          error("bad checksum");
        }
        ++b.extracted;
      }
    }

    // If out of memory, let another thread try
    catch (std::bad_alloc& e) {
      lock(job.mutex);
      fflush(stdout);
      fprintf(stderr, "Job %d killed: %s\n", jobNumber, e.what());
	  g_exec_text="Job killed";
      b.state=Block::READY;
      b.extracted=0;
      out.resize(0);
      release(job.mutex);
      return 0;
    }

    // Other errors: assume bad input
    catch (std::exception& e) {
      lock(job.mutex);
      fflush(stdout);
      fprintf(stderr, "Job %d: skipping [%u..%u] at %1.0f: %s\n",
              jobNumber, b.start+b.extracted, b.start+b.size-1,
              b.offset+0.0, e.what());
      release(job.mutex);
      continue;
    }

    // Write the files in dt that point to this block
    lock(job.write_mutex);
    for (unsigned ip=0; ip<b.files.size(); ++ip) {
      DTMap::iterator p=b.files[ip];
      if (p->second.date==0 || p->second.data<0
          || p->second.data>=int64_t(p->second.ptr.size()))
        continue;  // don't write

      // Look for pointers to this block
      const vector<unsigned>& ptr=p->second.ptr;
      int64_t offset=0;  // write offset
      for (unsigned j=0; j<ptr.size(); ++j) {
        if (ptr[j]<b.start || ptr[j]>=b.start+b.extracted) {
          offset+=job.jd.ht[ptr[j]].usize;
          continue;
        }

        // Close last opened file if different
        if (p!=job.lastdt) {
          if (job.outf!=FPNULL) {
            assert(job.lastdt!=job.jd.dt.end());
            assert(job.lastdt->second.date);
            assert(job.lastdt->second.data
                   <int64_t(job.lastdt->second.ptr.size()));
            fclose(job.outf);
            job.outf=FPNULL;
          }
          job.lastdt=job.jd.dt.end();
        }

        // Open file for output
        if (job.lastdt==job.jd.dt.end()) {
          string filename=job.jd.rename(p->first);
          assert(job.outf==FPNULL);
          if (p->second.data==0) {
            if (!job.jd.flagtest) makepath(filename);
            ///if (job.jd.summary<=0) 
			{
              lock(job.mutex);
              print_progress(job.total_size, job.total_done,-1);
              /*
			  if (job.jd.summary<=0) {
                printf("> ");
                printUTF8(filename.c_str());
                printf("\n");
              }
			  */
              release(job.mutex);
            }
            if (!job.jd.flagtest) {
              job.outf=fopen(filename.c_str(), WB);
              if (job.outf==FPNULL) {
                lock(job.mutex);
                printerr(filename.c_str());
                release(job.mutex);
              }
#ifndef unix
              else if ((p->second.attr&0x200ff)==0x20000+'w') {  // sparse?
                DWORD br=0;
                if (!DeviceIoControl(job.outf, FSCTL_SET_SPARSE,
                    NULL, 0, NULL, 0, &br, NULL))  // set sparse attribute
                  printerr(filename.c_str());
              }
#endif
            }
          }
          else if (!job.jd.flagtest)
            job.outf=fopen(filename.c_str(), RBPLUS);  // update existing file
          if (!job.jd.flagtest && job.outf==FPNULL) break;  // skip errors
          job.lastdt=p;
          assert(job.jd.flagtest || job.outf!=FPNULL);
        }
        assert(job.lastdt==p);

        // Find block offset of fragment
        uint64_t q=0;  // fragment offset from start of block
        for (unsigned k=b.start; k<ptr[j]; ++k) {
          assert(k>0);
          assert(k<job.jd.ht.size());
          if (job.jd.ht[k].usize<0) error("streaming fragment in file");
          assert(job.jd.ht[k].usize>=0);
          q+=job.jd.ht[k].usize;
        }
        assert(q+job.jd.ht[ptr[j]].usize<=out.size());

        // Combine consecutive fragments into a single write
        assert(offset>=0);
        ++p->second.data;
        uint64_t usize=job.jd.ht[ptr[j]].usize;
        assert(usize<=0x7fffffff);
        assert(b.start+b.size<=job.jd.ht.size());
        while (j+1<ptr.size() && ptr[j+1]==ptr[j]+1
               && ptr[j+1]<b.start+b.size
               && job.jd.ht[ptr[j+1]].usize>=0
               && usize+job.jd.ht[ptr[j+1]].usize<=0x7fffffff) {
          ++p->second.data;
          assert(p->second.data<=int64_t(ptr.size()));
          assert(job.jd.ht[ptr[j+1]].usize>=0);
          usize+=job.jd.ht[ptr[++j]].usize;
        }
        assert(usize<=0x7fffffff);
        assert(q+usize<=out.size());

        // Write the merged fragment unless they are all zeros and it
        // does not include the last fragment.
        uint64_t nz=q;  // first nonzero byte in fragments to be written
        while (nz<q+usize && out.c_str()[nz]==0) ++nz;
		
		
        if ((nz<q+usize || j+1==ptr.size())) 

			//if (stristr(job.lastdt->first.c_str(),"Globals.pas"))
			//if (stristr(job.lastdt->first.c_str(),"globals.pas"))
			{

/// let's calc the CRC32 of the block, and store (keyed by filename)
/// in comments debug code
				if (offset>g_scritti)
					g_scritti=offset;
					
				uint32_t crc;
				crc=crc32_16bytes (out.c_str()+q, usize);
	
				s_crc32block myblock;
				myblock.crc32=crc;
				myblock.crc32start=offset;
				myblock.crc32size=usize;
				myblock.filename=job.lastdt->first;
				g_crc32.push_back(myblock);
				/*
				char mynomefile[100];
				sprintf(mynomefile,"z:\\globals_%014lld_%014lld_%08X",offset,offset+usize,crc);
				///printf("Outfile %s %lld Seek to %08lld   size %08lld crc %08lld %s\n",mynomefile,job.outf,offset,usize,crc,job.lastdt->first.c_str());

				FILE* myfile=fopen(mynomefile, "wb");
				fwrite(out.c_str()+q, 1, usize, myfile);
				
				fclose(myfile);
				*/
			}


        ///if (nz<q+usize || j+1==ptr.size()) 
			///printf("OFFSETTONE   %08lld  size  %08lld \n",offset,usize);
		
        if (!job.jd.flagtest && (nz<q+usize || j+1==ptr.size())) 
		{
			///printf("OFFSETTONEWRITE   %08lld  size  %08lld \n",offset,usize);
		
///DTMap::iterator lastdt;   // currently open output file name
			fseeko(job.outf, offset, SEEK_SET);
			fwrite(out.c_str()+q, 1, usize, job.outf);
		}
        offset+=usize;
        lock(job.mutex);
        job.total_done+=usize;
        release(job.mutex);
		///printf("Scrittoni %f\n",(double)job.total_done);
		
        // Close file. If this is the last fragment then set date and attr.
        // Do not set read-only attribute in Windows yet.
        if (p->second.data==int64_t(ptr.size())) {
          assert(p->second.date);
          assert(job.lastdt!=job.jd.dt.end());
          assert(job.jd.flagtest || job.outf!=FPNULL);
          if (!job.jd.flagtest) {
            assert(job.outf!=FPNULL);
            string fn=job.jd.rename(p->first);
            int64_t attr=p->second.attr;
            int64_t date=p->second.date;
            if ((p->second.attr&0x1ff)=='w'+256) attr=0;  // read-only?
            if (p->second.data!=int64_t(p->second.ptr.size()))
              date=attr=0;  // not last frag
            close(fn.c_str(), date, attr, job.outf);
            job.outf=FPNULL;
          }
          job.lastdt=job.jd.dt.end();
        }
      } // end for j
    } // end for ip

    // Last file
    release(job.write_mutex);
  } // end while true

  
	
  // Last block
  return 0;
}

// Streaming output destination
struct OutputFile: public libzpaq::Writer {
  FP f;
  void put(int c) {
    char ch=c;
    if (f!=FPNULL) fwrite(&ch, 1, 1, f);
  }
  void write(const char* buf, int n) {if (f!=FPNULL) fwrite(buf, 1, n, f);}
  OutputFile(FP out=FPNULL): f(out) {}
};

// Copy at most n bytes from in to out (default all). Return how many copied.
int64_t copy(libzpaq::Reader& in, libzpaq::Writer& out, uint64_t n=~0ull) {
  const unsigned BUFSIZE=4096;
  int64_t result=0;
  char buf[BUFSIZE];
  while (n>0) {
    int nc=n>BUFSIZE ? BUFSIZE : n;
    int nr=in.read(buf, nc);
    if (nr<1) break;
    out.write(buf, nr);
    result+=nr;
    n-=nr;
  }
  return result;
}

// Extract files from archive. If force is true then overwrite
// existing files and set the dates and attributes of exising directories.
// Otherwise create only new files and directories. Return 1 if error else 0.
int Jidac::extract() {
	g_scritti=0;
  // Encrypt or decrypt whole archive
  if (repack && all) {
    if (files.size()>0 || tofiles.size()>0 || onlyfiles.size()>0
        || flagnoattributes || version!=DEFAULT_VERSION || method!="")
      error("-repack -all does not allow partial copy");
    InputArchive in(archive.c_str(), password);
    if (flagforce) delete_file(repack);
    if (exists(repack)) error("output file exists");

    // Get key and salt
    char salt[32]={0};
    if (new_password) libzpaq::random(salt, 32);

    // Copy
    OutputArchive out(repack, new_password, salt, 0);
    copy(in, out);
    printUTF8(archive.c_str());
    printf(" %1.0f ", in.tell()+.0);
    printUTF8(repack);
    printf(" -> %1.0f\n", out.tell()+.0);
    out.close();
    return 0;
  }
  
///printf("ZA\n");

	int64_t sz=read_archive(archive.c_str());
	if (sz<1) error("archive not found");
///printf("ZB\n");

	if (flagcomment)
		if (versioncomment.length()>0)
		{
			// try to find the comment, but be careful: 1, and only 1, allowed
			vector<DTMap::iterator> myfilelist;
			int versione=searchcomments(versioncomment,myfilelist);
			if (versione>0)
			{
				printf("Found version -until %d scanning again...\n",versione);
				version=versione;
	
				ver.clear();
				block.clear();
				dt.clear();
				ht.clear();
				ht.resize(1);  // element 0 not used
				ver.resize(1); // version 0
				dhsize=dcsize=0;
				sz=read_archive(archive.c_str());
			}
			else
			if (versione==0)
				error("Cannot find version comment");
			else
				error("Multiple match for version comment. Please use -until");
		}
///printf("ZC\n");

  // test blocks
  for (unsigned i=0; i<block.size(); ++i) {
    if (block[i].bsize<0) error("negative block size");
    if (block[i].start<1) error("block starts at fragment 0");
    if (block[i].start>=ht.size()) error("block start too high");
    if (i>0 && block[i].start<block[i-1].start) error("unordered frags");
    if (i>0 && block[i].start==block[i-1].start) error("empty block");
    if (i>0 && block[i].offset<block[i-1].offset+block[i-1].bsize)
      error("unordered blocks");
    if (i>0 && block[i-1].offset+block[i-1].bsize>block[i].offset)
      error("overlapping blocks");
  }
///printf("ZD\n");

  // Create index instead of extract files
  if (index) {
    if (ver.size()<2) error("no journaling data");
    if (flagforce) delete_file(index);
    if (exists(index)) error("index file exists");

    // Get salt
    char salt[32];
    if (ver[1].offset==32) {  // encrypted?
      FP fp=fopen(subpart(archive, 1).c_str(), RB);
      if (fp==FPNULL) error("cannot read part 1");
      if (fread(salt, 1, 32, fp)!=32) error("cannot read salt");
      salt[0]^='7'^'z';  // for index
      fclose(fp);
    }
    InputArchive in(archive.c_str(), password);
    OutputArchive out(index, password, salt, 0);
    for (unsigned i=1; i<ver.size(); ++i) {
      if (in.tell()!=ver[i].offset) error("I'm lost");

      // Read C block. Assume uncompressed and hash is present
      static char hdr[256]={0};  // Read C block
      int hsize=ver[i].data_offset-ver[i].offset;
      if (hsize<70 || hsize>255) error("bad C block size");
      if (in.read(hdr, hsize)!=hsize) error("EOF in header");
      if (hdr[hsize-36]!=9  // size of uncompressed block low byte
          || (hdr[hsize-22]&255)!=253  // start of SHA1 marker
          || (hdr[hsize-1]&255)!=255) {  // end of block marker
        for (int j=0; j<hsize; ++j)
          printf("%d%c", hdr[j]&255, j%10==9 ? '\n' : ' ');
        printf("at %1.0f\n", ver[i].offset+.0);
        error("C block in weird format");
      }
      memcpy(hdr+hsize-34, 
          "\x00\x00\x00\x00\x00\x00\x00\x00"  // csize = 0
          "\x00\x00\x00\x00"  // compressed data terminator
          "\xfd"  // start of hash marker
          "\x05\xfe\x40\x57\x53\x16\x6f\x12\x55\x59\xe7\xc9\xac\x55\x86"
          "\x54\xf1\x07\xc7\xe9"  // SHA-1('0'*8)
          "\xff", 34);  // EOB
      out.write(hdr, hsize);
      in.seek(ver[i].csize, SEEK_CUR);  // skip D blocks
      int64_t end=sz;
      if (i+1<ver.size()) end=ver[i+1].offset;
      int64_t n=end-in.tell();
      if (copy(in, out, n)!=n) error("EOF");  // copy H and I blocks
    }
    printUTF8(index);
    printf(" -> %1.0f\n", out.tell()+.0);
    out.close();
    return 0;
  }

///printf("ZE\n");

  // Label files to extract with data=0.
  // Skip existing output files. If force then skip only if equal
  // and set date and attributes.
  ExtractJob job(*this);
  int total_files=0, skipped=0;
  uint32_t crc32fromfile;
  
  /*
  if ((minsize>0) || (maxsize>0))
	for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) 
    	if (!isdirectory(p->first))
		{
			if (maxsize)
				if (maxsize<p->second.size)
				{
					if (flagverbose)
						printf("Verbose: maxsize skip extraction %19s %s\n",migliaia(p->second.size),p->first.c_str());
					if (p!=dt.end())					
						dt.erase(p);
				}
			if (minsize)
				if (minsize>p->second.size)
				{
					if (flagverbose)
						printf("Verbose: minsize skip extraction %19s %s\n",migliaia(p->second.size),p->first.c_str());
					if (p!=dt.end())
					dt.erase(p);
				}
		}
  */
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) {
	  ///printf("ZF\n");

    p->second.data=-1;  // skip
	
	
    if (p->second.date && p->first!="") 
	{
      const string fn=rename(p->first);
      const bool isdir=p->first[p->first.size()-1]=='/';
	
	if (!repack && !flagtest && flagforce && !isdir && equal(p, fn.c_str(),crc32fromfile)) 
	{
        ///if (summary<=0) 
		{  // identical
          printf("= ");
          printUTF8(fn.c_str());
          printf("\n");
        }
        close(fn.c_str(), p->second.date, p->second.attr);
        ++skipped;
      }
      else if (!repack && !flagtest && !flagforce && exists(fn)) {  // exists, skip
        //if (summary<=0) 
		{
          printf("? ");
          printUTF8(fn.c_str());
          printf("\n");
        }
        ++skipped;
      }
      else if (isdir)  // update directories later
        p->second.data=0;
      else if (block.size()>0) {  // files to decompress
        p->second.data=0;
        unsigned lo=0, hi=block.size()-1;  // block indexes for binary search
        for (unsigned i=0; p->second.data>=0 && i<p->second.ptr.size(); ++i) {
          unsigned j=p->second.ptr[i];  // fragment index
          if (j==0 || j>=ht.size() || ht[j].usize<-1) {
            fflush(stdout);
            printUTF8(p->first.c_str(), stderr);
            fprintf(stderr, ": bad frag IDs, skipping...\n");
            p->second.data=-1;  // skip
            continue;
          }
          assert(j>0 && j<ht.size());
          if (lo!=hi || lo>=block.size() || j<block[lo].start
              || (lo+1<block.size() && j>=block[lo+1].start)) {
            lo=0;  // find block with fragment j by binary search
            hi=block.size()-1;
            while (lo<hi) {
              unsigned mid=(lo+hi+1)/2;
              assert(mid>lo);
              assert(mid<=hi);
              if (j<block[mid].start) hi=mid-1;
              else (lo=mid);
            }
          }
          assert(lo==hi);
          assert(lo>=0 && lo<block.size());
          assert(j>=block[lo].start);
          assert(lo+1==block.size() || j<block[lo+1].start);
          unsigned c=j-block[lo].start+1;
          if (block[lo].size<c) block[lo].size=c;
          if (block[lo].files.size()==0 || block[lo].files.back()!=p)
            block[lo].files.push_back(p);
        }
        ++total_files;
        job.total_size+=p->second.size;
      }
    }  // end if selected
  }  // end for
  if (!flagforce && skipped>0)
    printf("%d ?existing files skipped (-force overwrites).\n", skipped);
  if (flagforce && skipped>0)
    printf("%d =identical files skipped.\n", skipped);
	///printf("ZGF\n");

  // Repack to new archive
  if (repack) {

    // Get total D block size
    if (ver.size()<2) error("cannot repack streaming archive");
    int64_t csize=0;  // total compressed size of D blocks
    for (unsigned i=0; i<block.size(); ++i) {
      if (block[i].bsize<1) error("empty block");
      if (block[i].size>0) csize+=block[i].bsize;
    }
///printf("ZH\n");

    // Open input
    InputArchive in(archive.c_str(), password);
///printf("ZI\n");

    // Open output
    if (!flagforce && exists(repack)) error("repack output exists");
    delete_file(repack);
    char salt[32]={0};
    if (new_password) libzpaq::random(salt, 32);
    OutputArchive out(repack, new_password, salt, 0);
    int64_t cstart=out.tell();

    // Write C block using first version date
    writeJidacHeader(&out, ver[1].date, -1, 1);
    int64_t dstart=out.tell();
///printf("ZJ\n");

    // Copy only referenced D blocks. If method then recompress.
    for (unsigned i=0; i<block.size(); ++i) {
      if (block[i].size>0) {
        in.seek(block[i].offset, SEEK_SET);
        copy(in, out, block[i].bsize);
      }
    }
    printf("Data %1.0f -> ", csize+.0);
    csize=out.tell()-dstart;
    printf("%1.0f\n", csize+.0);

    // Re-create referenced H blocks using latest date
    for (unsigned i=0; i<block.size(); ++i) {
      if (block[i].size>0) {
        StringBuffer is;
        puti(is, block[i].bsize, 4);
        for (unsigned j=0; j<block[i].frags; ++j) {
          const unsigned k=block[i].start+j;
          if (k<1 || k>=ht.size()) error("frag out of range");
          is.write((const char*)ht[k].sha1, 20);
          puti(is, ht[k].usize, 4);
        }
        libzpaq::compressBlock(&is, &out, "0",
            ("jDC"+itos(ver.back().date, 14)+"h"
            +itos(block[i].start, 10)).c_str(),
            "jDC\x01");
      }
    }
///printf("ZK\n");

    // Append I blocks of selected files
    unsigned dtcount=0;
    StringBuffer is;
    for (DTMap::iterator p=dt.begin();; ++p) {
		///printf("ZK-1\n");

      if (p!=dt.end() && p->second.date>0 && p->second.data>=0) {
        string filename=rename(p->first);
        puti(is, p->second.date, 8);
        is.write(filename.c_str(), strlen(filename.c_str()));
        is.put(0);
		
		
		/// let's write, if necessary, the checksum, or leave the default ZPAQ if no -checksum
        if ((p->second.attr&255)=='u') // unix attributes
			writefranzattr(is,p->second.attr,3,flagchecksum,filename,p->second.sha1hex,2);
		else 
		if ((p->second.attr&255)=='w') // windows attributes
			writefranzattr(is,p->second.attr,5,flagchecksum,filename,p->second.sha1hex,2);
		else 
			puti(is, 0, 4);  // no attributes

        puti(is, p->second.ptr.size(), 4);  // list of frag pointers
        for (unsigned i=0; i<p->second.ptr.size(); ++i)
          puti(is, p->second.ptr[i], 4);
      }
      if (is.size()>16000 || (is.size()>0 && p==dt.end())) {
        libzpaq::compressBlock(&is, &out, "1",
            ("jDC"+itos(ver.back().date)+"i"+itos(++dtcount, 10)).c_str(),
            "jDC\x01");
        is.resize(0);
      }
      if (p==dt.end()) break;
    }
///printf("ZL\n");

    // Summarize result
    printUTF8(archive.c_str());
    printf(" %1.0f -> ", sz+.0);
    printUTF8(repack);
    printf(" %1.0f\n", out.tell()+.0);

    // Rewrite C block
    out.seek(cstart, SEEK_SET);
    writeJidacHeader(&out, ver[1].date, csize, 1);
    out.close();
    return 0;
  }

	g_crc32.clear();
	
  // Decompress archive in parallel
  printf("Extracting %s in %d files -threads %d\n",
      migliaia(job.total_size), total_files, threads);
  vector<ThreadID> tid(threads);
  for (unsigned i=0; i<tid.size(); ++i) run(tid[i], decompressThread, &job);

  // Extract streaming files
  unsigned segments=0;  // count
  ///printf("Z1\n");
  
  InputArchive in(archive.c_str(), password);
  ///printf("Z2\n");
  if (in.isopen()) {
    FP outf=FPNULL;
    DTMap::iterator dtptr=dt.end();
    for (unsigned i=0; i<block.size(); ++i) {
      if (block[i].usize<0 && block[i].size>0) {
        Block& b=block[i];
        try {
			///printf("Z3 %08ud \n",b.offset);
  
          in.seek(b.offset, SEEK_SET);
          libzpaq::Decompresser d;
          d.setInput(&in);
          if (!d.findBlock()) error("block not found");
          StringWriter filename;
          for (unsigned j=0; j<b.size; ++j) {
            if (!d.findFilename(&filename)) error("segment not found");
            d.readComment();

            // Start of new output file
            if (filename.s!="" || segments==0) {
              unsigned k;
              for (k=0; k<b.files.size(); ++k) {  // find in dt
                if (b.files[k]->second.ptr.size()>0
                    && b.files[k]->second.ptr[0]==b.start+j
                    && b.files[k]->second.date>0
                    && b.files[k]->second.data==0)
                  break;
              }
              if (k<b.files.size()) {  // found new file
                if (outf!=FPNULL) fclose(outf);
                outf=FPNULL;
                string outname=rename(b.files[k]->first);
                dtptr=b.files[k];
                lock(job.mutex);
/*           
		   if (summary<=0) {
                  printf("> ");
                  printUTF8(outname.c_str());
                  printf("\n");
                }
*/  
				if (!flagtest) {
                  makepath(outname);
                  outf=fopen(outname.c_str(), WB);
                  if (outf==FPNULL) printerr(outname.c_str());
                }
                release(job.mutex);
              }
              else {  // end of file
                if (outf!=FPNULL) fclose(outf);
                outf=FPNULL;
                dtptr=dt.end();
              }
            }
			///printf("Z4\n");
            // Decompress segment
            libzpaq::SHA1 sha1;
            d.setSHA1(&sha1);
            OutputFile o(outf);
            d.setOutput(&o);
            d.decompress();

            ///printf("Z5\n");
            // Verify checksum
            char sha1result[21];
            d.readSegmentEnd(sha1result);
            if (sha1result[0]==1) {
              if (memcmp(sha1result+1, sha1.result(), 20)!=0)
                error("checksum failed");
            }
            else if (sha1result[0]!=0)
              error("unknown checksum type");
            ++b.extracted;
            if (dtptr!=dt.end()) ++dtptr->second.data;
            filename.s="";
            ++segments;
          }
        }
        catch(std::exception& e) {
          lock(job.mutex);
          printf("Skipping block: %s\n", e.what());
          release(job.mutex);
        }
      }
    }
    if (outf!=FPNULL) fclose(outf);
  }
  if (segments>0) printf("%u streaming segments extracted\n", segments);

	///printf("Z6\n");

  // Wait for threads to finish
  for (unsigned i=0; i<tid.size(); ++i) join(tid[i]);
///printf("Z7\n");

  // Create empty directories and set file dates and attributes
  if (!flagtest) {
    for (DTMap::reverse_iterator p=dt.rbegin(); p!=dt.rend(); ++p) {
      if (p->second.data>=0 && p->second.date && p->first!="") {
        string s=rename(p->first);
        if (p->first[p->first.size()-1]=='/')
          makepath(s, p->second.date, p->second.attr);
        else if ((p->second.attr&0x1ff)=='w'+256)  // read-only?
          close(s.c_str(), 0, p->second.attr);
      }
    }
  }
///printf("Z8\n");

  // Report failed extractions
  unsigned extracted=0, errors=0;
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) {
    string fn=rename(p->first);
    if (p->second.data>=0 && p->second.date
        && fn!="" && fn[fn.size()-1]!='/') {
      ++extracted;
      if (p->second.ptr.size()!=unsigned(p->second.data)) {
        fflush(stdout);
        if (++errors==1)
          fprintf(stderr,
          "\nFailed (extracted/total fragments, file):\n");
        fprintf(stderr, "%u/%u ",
                int(p->second.data), int(p->second.ptr.size()));
        printUTF8(fn.c_str(), stderr);
        fprintf(stderr, "\n");
      }
    }
  }
  ///printf("Z9\n");

  if (errors>0) {
    fflush(stdout);
    fprintf(stderr,
        "\nExtracted %u of %u files OK (%u errors)"
        " using %1.3f MB x %d threads\n",
        extracted-errors, extracted, errors, job.maxMemory/1000000,
        int(tid.size()));
  }
  return errors>0;
}


/////////////////////////////// list //////////////////////////////////

// Return p<q for sorting files by decreasing size, then fragment ID list
bool compareFragmentList(DTMap::const_iterator p, DTMap::const_iterator q) {
  if (p->second.size!=q->second.size) return p->second.size>q->second.size;
  if (p->second.ptr<q->second.ptr) return true;
  if (q->second.ptr<p->second.ptr) return false;
  if (p->second.data!=q->second.data) return p->second.data<q->second.data;
  return p->first<q->first;
}

// Return p<q for sort by name and comparison result
bool compareName(DTMap::const_iterator p, DTMap::const_iterator q) {
  if (p->first!=q->first) return p->first<q->first;
  return p->second.data<q->second.data;
}



uint32_t crchex2int(char *hex) 
{
	assert(hex);
	uint32_t val = 0;
	for (int i=0;i<8;i++)
	{
        uint8_t byte = *hex++; 
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}


string mydateToString(int64_t date) {
  if (date<=0) return "                   ";
  ///        0123456789012345678
  string  s="0000-00-00 00:00:00";
  string s2="00/00/0000 00:00:00";
  
  
  static const int t[]={18,17,15,14,12,11,9,8,6,5,3,2,1,0};
  for (int i=0; i<14; ++i) s[t[i]]+=int(date%10), date/=10;
  //// convert to italy's stile
  s2[0]=s[8];
  s2[1]=s[9];
  s2[3]=s[5];
  s2[4]=s[6];
  s2[6]=s[0];
  s2[7]=s[1];
  s2[8]=s[2];
  s2[9]=s[3];
  
  s2[11]=s[11];
  s2[12]=s[12];
  
  s2[14]=s[14];
  s2[15]=s[15];
  
  s2[17]=s[17];
  s2[18]=s[18];
    
  return s2;
}
void print_datetime(void)
{
	int hours, minutes, seconds, day, month, year;

	time_t now;
	time(&now);
	struct tm *local = localtime(&now);

	hours = local->tm_hour;			// get hours since midnight	(0-23)
	minutes = local->tm_min;		// get minutes passed after the hour (0-59)
	seconds = local->tm_sec;		// get seconds passed after the minute (0-59)

	day = local->tm_mday;			// get day of month (1 to 31)
	month = local->tm_mon + 1;		// get month of year (0 to 11)
	year = local->tm_year + 1900;	// get year since 1900

	printf("%02d/%02d/%d %02d:%02d:%02d ", day, month, year, hours,minutes,seconds);
}

/// quanti==1 => version (>0)
/// quanti==0 => 0
/// else  => -1 multiple match
int Jidac::searchcomments(string i_testo,vector<DTMap::iterator> &filelist)
{
	unsigned int quanti=0;
	int versione=-1;
	filelist.clear();
	
	for (DTMap::iterator a=dt.begin(); a!=dt.end(); ++a) 
	{
		a->second.data='-';
		filelist.push_back(a);
	}
		   
	///VCOMMENT 00000002 seconda_versione:$DATA
	for (unsigned i=0;i<filelist.size();i++) 
	{
		DTMap::iterator p=filelist[i];
		if (isads(p->first))
		{
			string fakefile=p->first;
			myreplace(fakefile,":$DATA","");
			size_t found = fakefile.find("VCOMMENT "); 
			if (found != string::npos)
			{
				///printf("KKKKKKKKKKKKKKKKK <<%s>>\n",fakefile.c_str());
					
				string numeroversione=fakefile.substr(found+9,8);
//esx
	///			int numver=0; //stoi(numeroversione.c_str());
				int numver=std::stoi(numeroversione.c_str());
				string commento=fakefile.substr(found+9+8+1,65000);
				if (i_testo.length()>0)
				{
					size_t start_comment = commento.find(i_testo);
					if (start_comment == std::string::npos)
					continue;
				}
    			mappacommenti.insert(std::pair<int, string>(numver, commento));
				versione=numver;
				quanti++;
			}
		}
	}
	if (quanti==1)
		return versione;
	else
	if (quanti==0)
		return 0;
	else
		return -1;
	
}
int Jidac::enumeratecomments()
{
	
	  // Read archive into dt, which may be "" for empty.
	int64_t csize=0;
	int errors=0;

	if (archive!="") 
		csize=read_archive(archive.c_str(),&errors,1); /// AND NOW THE MAGIC ONE!
	printf("\nVersion comments enumerator\n");
	
	vector<DTMap::iterator> filelist;
	searchcomments(versioncomment,filelist);

	for (MAPPACOMMENTI::const_iterator p=mappacommenti.begin(); p!=mappacommenti.end(); ++p) 
			printf("%08d <<%s>>\n",p->first,p->second.c_str());

  
	for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) 
	{
		DTMap::iterator a=dt.find(rename(p->first));
		if (a!=dt.end() && (true || a->second.date)) 
		{
			a->second.data='-';
			filelist.push_back(a);
		}
		p->second.data='+';
		filelist.push_back(p);
	}
  
	for (DTMap::iterator a=dt.begin(); a!=dt.end(); ++a) 
		if (a->second.data!='-' && (true || a->second.date)) 
		{
			a->second.data='-';
			filelist.push_back(a);
		}
	
  
	if (all)
	{
		printf("------------\n");
		for (unsigned fi=0;fi<filelist.size() /*&& (true || int(fi)<summary)*/; ++fi) 
		{
			DTMap::iterator p=filelist[fi];
			unsigned v;  
			if (p->first.size()==all+1u && (v=atoi(p->first.c_str()))>0 && v<ver.size()) 
			{
				printf("%08u %s ",v,dateToString(p->second.date).c_str());
				printf(" +%08d -%08d -> %20s", ver[v].updates, ver[v].deletes,
					migliaia((v+1<ver.size() ? ver[v+1].offset : csize)-ver[v].offset+0.0));
		
				std::map<int,string>::iterator commento;
				commento=mappacommenti.find(v); 
				if(commento!= mappacommenti.end()) 
					printf(" <<%s>>", commento->second.c_str());

				printf("\n");
			}
		}
	}

	return 0;		   
}



int Jidac::verify() 
{
	
	
	if (files.size()<=0)
		return -1;
		
	printf("Compare archive content of:");


	int64_t csize=0;
	if (archive!="") 
	csize=read_archive(archive.c_str());

  // Read external files into edt
	uint64_t howmanyfiles=0;
	
	g_bytescanned=0;
	g_filescanned=0;
	g_worked=0;
	files_count.clear();
	
	edt.clear();
	for (unsigned i=0; i<files.size(); ++i)
	{
		scandir(edt,files[i].c_str());
		files_count.push_back(edt.size()-howmanyfiles);
		howmanyfiles=edt.size();
	}
	printf("\n");
 	for (unsigned i=0; i<files.size(); ++i)
		printf("%9s in <<%s>>\n",migliaia(files_count[i]),files[i].c_str());
	
	
	if (files.size()) 
		printf("Total files found: %s\n", migliaia(edt.size()));
	printf("\n");

  // Compute directory sizes as the sum of their contents
	DTMap* dp[2]={&dt, &edt};
	for (int i=0; i<2; ++i) 
	{
		for (DTMap::iterator p=dp[i]->begin(); p!=dp[i]->end(); ++p) 
		{
			int len=p->first.size();
			if (len>0 && p->first[len]!='/') 
			{
				for (int j=0; j<len; ++j) 
				{
					if (p->first[j]=='/') 
					{
						DTMap::iterator q=dp[i]->find(p->first.substr(0, j+1));
						if (q!=dp[i]->end())
							q->second.size+=p->second.size;
					}
				}
			}
		}
	}

	vector<DTMap::iterator> filelist;
	for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) 
	{
		DTMap::iterator a=dt.find(rename(p->first));
		if (a!=dt.end() && (all || a->second.date)) 
		{
			a->second.data='-';
			filelist.push_back(a);
		}
		p->second.data='+';
		filelist.push_back(p);
	}
	
	for (DTMap::iterator a=dt.begin(); a!=dt.end(); ++a) 
		if (a->second.data!='-' && (all || a->second.date)) 
		{
			a->second.data='-';
			filelist.push_back(a);
		}
	
	int64_t usize=0;
	unsigned matches=0, mismatches=0, internal=0, external=0;
	uint32_t crc32fromfile;
	
	vector<string> risultati;
	vector<bool> risultati_utf8;
	char linebuffer[1000];
	unsigned int ultimapercentuale=200;
	unsigned int percentuale;
	
	for (unsigned fi=0;fi<filelist.size(); ++fi) 
	{
		DTMap::iterator p=filelist[fi];
		
		if (menoenne)
			if ((mismatches+external+internal)>menoenne)
				break;
		
		if (!flagnoeta)
		{
			percentuale=100*fi/filelist.size();
			if (ultimapercentuale!=percentuale)
			{
				printf("Done %02d%% %10s of %9s, diff %s bytes\r",percentuale,tohuman(g_worked),tohuman2(g_bytescanned),migliaia2(usize));
				ultimapercentuale=percentuale;
			}
		}
		
    // Compare external files
		if (p->second.data=='-' && fi+1<filelist.size() && filelist[fi+1]->second.data=='+') 
		{
			DTMap::iterator p1=filelist[fi+1];
				
			if (equal(p, p1->first.c_str(),crc32fromfile))
			{
				
				if (p->second.sha1hex[41]!=0)
				{
					if (!isdirectory(p1->first))
					{
						uint32_t crc32stored=crchex2int(p->second.sha1hex+41);
						if (crc32stored!=crc32fromfile)
						{
							p->second.data='#';
							sprintf(linebuffer,"\nGURU SHA1 COLLISION! %08X vs %08X ",crc32stored,crc32fromfile);
							risultati.push_back(linebuffer);
							risultati_utf8.push_back(false);
							
							sprintf(linebuffer,"%s\n",p1->first.c_str());
							risultati.push_back(linebuffer);
							risultati_utf8.push_back(true);
							
						}
						else
						{	
							p->second.data='=';
							++fi;
						}
					}
					else
					{
						p->second.data='=';
						++fi;
					}
					
				}
				else
				{
					p->second.data='=';
					++fi;
				}
			}
			else
			{
				p->second.data='#';
				p1->second.data='!';
			}
		}


		if (p->second.data=='=') ++matches;
		if (p->second.data=='#') ++mismatches;
		if (p->second.data=='-') ++internal;
		if (p->second.data=='+') ++external;

		if (p->second.data!='=') 
		{
			if (!isdirectory(p->first))
				usize+=p->second.size;
				
			sprintf(linebuffer,"%c %s %19s ", char(p->second.data),dateToString(p->second.date).c_str(), migliaia(p->second.size));
			risultati.push_back(linebuffer);
			risultati_utf8.push_back(false);
			
			
			sprintf(linebuffer,"%s\n",p->first.c_str());
			risultati.push_back(linebuffer);
			risultati_utf8.push_back(true);
	  
			if (p->second.data=='!')
			{
				sprintf(linebuffer,"\n");
				risultati.push_back(linebuffer);
				risultati_utf8.push_back(false);
			}
		}
	}  
	printf("\n\n");

	bool myerror=false;
	
	if (menoenne)
		if ((mismatches+external+internal)>menoenne)
			printf("**** STOPPED BY TOO MANY ERRORS -n %d\n",menoenne);
			
	if  (mismatches || external || internal)
		for (unsigned int i=0;i<risultati.size();i++)
		{
			if (risultati_utf8[i])
				printUTF8(risultati[i].c_str());
			else
			printf("%s",risultati[i].c_str());
		}

	if (matches)
		printf("%08d =same\n",matches);
	if (mismatches)
	{
		printf("%08d #different\n",mismatches);
		myerror=true;
	}
	if (external)
	{
		printf("%08d +external (file missing in ZPAQ)\n",external);
		myerror=true;
	}
	if (internal)
	{
		printf("%08d -internal (file in ZPAQ but not on disk)\n",internal);
		myerror=true;
	}
	printf("Total different file size: %s bytes\n",migliaia(usize));
 	if (myerror)
		return 2;
	else
		return 0;
}


// List contents
int Jidac::list() 
{
	if (flagcomment)
	{
		enumeratecomments();
		return 0;
	}
	if (files.size()>=1)
		return verify();

  // Read archive into dt, which may be "" for empty.
  int64_t csize=0;
  int errors=0;
  bool flagshow;
  
	if (archive!="") csize=read_archive(archive.c_str(),&errors,1); /// AND NOW THE MAGIC ONE!
	printf("\n");


	g_bytescanned=0;
	g_filescanned=0;
	g_worked=0;
	vector<DTMap::iterator> filelist;

	for (DTMap::iterator a=dt.begin(); a!=dt.end(); ++a) 
		if (a->second.data!='-' && (all || a->second.date)) 
		{
			a->second.data='-';
			filelist.push_back(a);
		}
	

  // Sort by size desc
 	if (menoenne) // sort by size desc
	{
		printf("Sort by size desc, limit to %d\n",menoenne);
		sort(filelist.begin(), filelist.end(), compareFragmentList);
	}

	int64_t usize=0;
	map<int, string> mappacommenti;
	
	//searchcomments(versioncomment,filelist);
		
	///VCOMMENT 00000002 seconda_versione:$DATA
	for (unsigned i=0;i<filelist.size();i++) 
	{
		DTMap::iterator p=filelist[i];
		if (isads(p->first))
		{
			string fakefile=p->first;
			myreplace(fakefile,":$DATA","");
			size_t found = fakefile.find("VCOMMENT "); 
			if (found != string::npos)
			{
    			string numeroversione=fakefile.substr(found+9,8);
//esx		
	//	int numver=0;//std::stoi(numeroversione.c_str());
		int numver=std::stoi(numeroversione.c_str());
				string commento=fakefile.substr(found+9+8+1,65000);
				mappacommenti.insert(std::pair<int, string>(numver, commento));
			}
		}
	}
	   
		   
	uint32_t crc32fromfile;	   
	unsigned fi;
	for (fi=0;fi<filelist.size(); ++fi) 
	{
    
		if (menoenne) /// list -n 10 => sort by size and stop at 10
			if (fi>=menoenne)
				break;
			
		DTMap::iterator p=filelist[fi];
		flagshow=true;
		
		if (isads(p->first))
			if (strstr(p->first.c_str(),"VCOMMENT "))
				flagshow=false;
		
/// a little of change if -search is used
		if (searchfrom!="")
			flagshow=stristr(p->first.c_str(),searchfrom.c_str());
			
/// redundant, but not a really big deal

		if (flagchecksum)
			if (isdirectory(p->first))
				flagshow=false;

		if (minsize ||  maxsize)
			if (isdirectory(p->first))
				flagshow=false;
						
			if (maxsize)
				if (maxsize<p->second.size)
					flagshow=false;
			if (minsize)
				if (minsize>p->second.size)
					flagshow=false;
		
		if (flagshow)
		{
			if (!strchr(nottype.c_str(), p->second.data)) 
			{
				if (p->first!="" && (!isdirectory(p->first)))
					usize+=p->second.size;
	
				if (!flagchecksum) // special output
				{
					printf("%c %s %19s ", char(p->second.data),dateToString(p->second.date).c_str(), migliaia(p->second.size));
					if (!flagnoattributes)
					printf("%s ", attrToString(p->second.attr).c_str());
				}
				
				string myfilename=p->first;
				
	/// houston, we have checksums?
				string mysha1="";
				string mycrc32="";
				
				if (p->second.sha1hex[0]!=0)
						if (p->second.sha1hex[0+40]==0)
							mysha1=(string)"SHA1: "+p->second.sha1hex+" ";
							
				if (p->second.sha1hex[41]!=0)
						if (p->second.sha1hex[41+8]==0)
						{
							if (isdirectory(myfilename))
								mycrc32=(string)"CRC32:          ";
							else
								mycrc32=(string)"CRC32: "+(p->second.sha1hex+41)+" ";
						}
						
							
				if (all)
				{
					string rimpiazza="|";
				
					if (!isdirectory(myfilename))
	//					rimpiazza+="FOLDER ";
		//			else
					{
						rimpiazza+=mysha1;
						rimpiazza+=mycrc32;
					}
					if (!myreplace(myfilename,"|$1",rimpiazza))
						myreplace(myfilename,"/","|");
				}
				else
				{
						myfilename=mysha1+mycrc32+myfilename;
					
				}
				
				
/// search and replace, if requested	
				if ((searchfrom!="") && (replaceto!=""))
					replace(myfilename,searchfrom,replaceto);
								
				printUTF8(myfilename.c_str());
			  
				unsigned v;  // list version updates, deletes, compressed size
			if (all>0 && p->first.size()==all+1u && (v=atoi(p->first.c_str()))>0
          && v<ver.size()) 
				{  // version info
		
					std::map<int,string>::iterator commento;
		
					commento=mappacommenti.find(v); 
					if(commento== mappacommenti.end()) 
						printf(" +%d -%d -> %s", ver[v].updates, ver[v].deletes,
						(v+1<ver.size() ? migliaia(ver[v+1].offset-ver[v].offset) : migliaia(csize-ver[v].offset)));
					else
						printf(" +%d -%d -> %s <<%s>>", ver[v].updates, ver[v].deletes,
						(v+1<ver.size() ? migliaia(ver[v+1].offset-ver[v].offset) : migliaia(csize-ver[v].offset)),commento->second.c_str());
        		
				/*
			if (summary<0)  // print fragment range
			printf(" %u-%u", ver[v].firstFragment,
              v+1<ver.size()?ver[v+1].firstFragment-1:unsigned(ht.size())-1);
*/
				}
				
				printf("\n");
			}
		}
	}  // end for i = each file version

  // Compute dedupe size

	int64_t ddsize=0, allsize=0;
	unsigned nfiles=0, nfrags=0, unknown_frags=0, refs=0;
	vector<bool> ref(ht.size());
	for (DTMap::const_iterator p=dt.begin(); p!=dt.end(); ++p) 
	{
		if (p->second.date) 
		{
			++nfiles;
			for (unsigned j=0; j<p->second.ptr.size(); ++j) 
			{
				unsigned k=p->second.ptr[j];
				if (k>0 && k<ht.size()) 
				{
					++refs;
					if (ht[k].usize>=0) 
						allsize+=ht[k].usize;
					if (!ref[k]) 
					{
						ref[k]=true;
						++nfrags;
						if (ht[k].usize>=0) 
							ddsize+=ht[k].usize;
						else 
						++unknown_frags;
					}
				}
			}
		}
	}
	
  // Print archive statistics
	printf("\n%s (%s) of %s (%s) in %s files shown\n",migliaia(usize),tohuman(usize),migliaia2(allsize),tohuman2(allsize),migliaia3(fi));
  if (unknown_frags)
    printf("%u fragments have unknown size\n", unknown_frags);

  if (dhsize!=dcsize)  // index?
    printf("Note: %1.0f of %1.0f compressed bytes are in archive\n",
        dcsize+0.0, dhsize+0.0);
		
		
  return 0;
}






long long fsbtoblk(int64_t num, uint64_t fsbs, u_long bs)
{
	return (num * (intmax_t) fsbs / (int64_t) bs);
}





/// it is not easy, at all, to take *nix free filesystem space
uint64_t getfreespace(const char* i_path)
{
	
#ifdef BSD
	struct statfs stat;
 
	if (statfs(i_path, &stat) != 0) 
	{
		// error happens, just quits here
		return 0;
	}
///	long long used = stat.f_blocks - stat.f_bfree;
///	long long availblks = stat.f_bavail + used;

	static long blocksize = 0;
	int dummy;

	if (blocksize == 0)
		getbsize(&dummy, &blocksize);
	return  fsbtoblk(stat.f_bavail,
	stat.f_bsize, blocksize)*1024;
#else
#ifdef unix //this sould be Linux
	return 0;
#else
	uint64_t spazio=0;
	
	BOOL  fResult;
	unsigned __int64 i64FreeBytesToCaller,i64TotalBytes,i64FreeBytes;
	WCHAR  *pszDrive  = NULL, szDrive[4];

	const size_t WCHARBUF = 512;
	wchar_t  wszDest[WCHARBUF];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, i_path, -1, wszDest, WCHARBUF);

	pszDrive = wszDest;
	if (i_path[1] == ':')
	{
		szDrive[0] = pszDrive[0];
		szDrive[1] = ':';
        szDrive[2] = '\\';
        szDrive[3] = '\0';
		pszDrive = szDrive;
	}
	fResult = GetDiskFreeSpaceEx ((LPCTSTR)pszDrive,
                                 (PULARGE_INTEGER)&i64FreeBytesToCaller,
                                 (PULARGE_INTEGER)&i64TotalBytes,
                                 (PULARGE_INTEGER)&i64FreeBytes);
	if (fResult)
		spazio=i64FreeBytes;
	return spazio; // Windows
#endif





#endif


}




/////////////// calc xxhash64

// //////////////////////////////////////////////////////////
// xxhash64.h
// Copyright (c) 2016 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//


/// XXHash (64 bit), based on Yann Collet's descriptions, see http://cyan4973.github.io/xxHash/
/** How to use:
    uint64_t myseed = 0;
    XXHash64 myhash(myseed);
    myhash.add(pointerToSomeBytes,     numberOfBytes);
    myhash.add(pointerToSomeMoreBytes, numberOfMoreBytes); // call add() as often as you like to ...
    // and compute hash:
    uint64_t result = myhash.hash();

    // or all of the above in one single line:
    uint64_t result2 = XXHash64::hash(mypointer, numBytes, myseed);

    Note: my code is NOT endian-aware !
**/
class XXHash64
{
public:
  /// create new XXHash (64 bit)
  /** @param seed your seed value, even zero is a valid seed **/
  explicit XXHash64(uint64_t seed)
  {
    state[0] = seed + Prime1 + Prime2;
    state[1] = seed + Prime2;
    state[2] = seed;
    state[3] = seed - Prime1;
    bufferSize  = 0;
    totalLength = 0;
  }

  /// add a chunk of bytes
  /** @param  input  pointer to a continuous block of data
      @param  length number of bytes
      @return false if parameters are invalid / zero **/
  bool add(const void* input, uint64_t length)
  {
    // no data ?
    if (!input || length == 0)
      return false;

    totalLength += length;
    // byte-wise access
    const unsigned char* data = (const unsigned char*)input;

    // unprocessed old data plus new data still fit in temporary buffer ?
    if (bufferSize + length < MaxBufferSize)
    {
      // just add new data
      while (length-- > 0)
        buffer[bufferSize++] = *data++;
      return true;
    }

    // point beyond last byte
    const unsigned char* stop      = data + length;
    const unsigned char* stopBlock = stop - MaxBufferSize;

    // some data left from previous update ?
    if (bufferSize > 0)
    {
      // make sure temporary buffer is full (16 bytes)
      while (bufferSize < MaxBufferSize)
        buffer[bufferSize++] = *data++;

      // process these 32 bytes (4x8)
      process(buffer, state[0], state[1], state[2], state[3]);
    }

    // copying state to local variables helps optimizer A LOT
    uint64_t s0 = state[0], s1 = state[1], s2 = state[2], s3 = state[3];
    // 32 bytes at once
    while (data <= stopBlock)
    {
      // local variables s0..s3 instead of state[0]..state[3] are much faster
      process(data, s0, s1, s2, s3);
      data += 32;
    }
    // copy back
    state[0] = s0; state[1] = s1; state[2] = s2; state[3] = s3;

    // copy remainder to temporary buffer
    bufferSize = stop - data;
    for (unsigned int i = 0; i < bufferSize; i++)
      buffer[i] = data[i];

    // done
    return true;
  }

  /// get current hash
  /** @return 64 bit XXHash **/
  uint64_t hash() const
  {
    // fold 256 bit state into one single 64 bit value
    uint64_t result;
    if (totalLength >= MaxBufferSize)
    {
      result = rotateLeft(state[0],  1) +
               rotateLeft(state[1],  7) +
               rotateLeft(state[2], 12) +
               rotateLeft(state[3], 18);
      result = (result ^ processSingle(0, state[0])) * Prime1 + Prime4;
      result = (result ^ processSingle(0, state[1])) * Prime1 + Prime4;
      result = (result ^ processSingle(0, state[2])) * Prime1 + Prime4;
      result = (result ^ processSingle(0, state[3])) * Prime1 + Prime4;
    }
    else
    {
      // internal state wasn't set in add(), therefore original seed is still stored in state2
      result = state[2] + Prime5;
    }

    result += totalLength;

    // process remaining bytes in temporary buffer
    const unsigned char* data = buffer;
    // point beyond last byte
    const unsigned char* stop = data + bufferSize;

    // at least 8 bytes left ? => eat 8 bytes per step
    for (; data + 8 <= stop; data += 8)
      result = rotateLeft(result ^ processSingle(0, *(uint64_t*)data), 27) * Prime1 + Prime4;

    // 4 bytes left ? => eat those
    if (data + 4 <= stop)
    {
      result = rotateLeft(result ^ (*(uint32_t*)data) * Prime1,   23) * Prime2 + Prime3;
      data  += 4;
    }

    // take care of remaining 0..3 bytes, eat 1 byte per step
    while (data != stop)
      result = rotateLeft(result ^ (*data++) * Prime5,            11) * Prime1;

    // mix bits
    result ^= result >> 33;
    result *= Prime2;
    result ^= result >> 29;
    result *= Prime3;
    result ^= result >> 32;
    return result;
  }


  /// combine constructor, add() and hash() in one static function (C style)
  /** @param  input  pointer to a continuous block of data
      @param  length number of bytes
      @param  seed your seed value, e.g. zero is a valid seed
      @return 64 bit XXHash **/
  static uint64_t hash(const void* input, uint64_t length, uint64_t seed)
  {
    XXHash64 hasher(seed);
    hasher.add(input, length);
      return hasher.hash();
  }

private:
  /// magic constants :-)
  static const uint64_t Prime1 = 11400714785074694791ULL;
  static const uint64_t Prime2 = 14029467366897019727ULL;
  static const uint64_t Prime3 =  1609587929392839161ULL;
  static const uint64_t Prime4 =  9650029242287828579ULL;
  static const uint64_t Prime5 =  2870177450012600261ULL;

  /// temporarily store up to 31 bytes between multiple add() calls
  static const uint64_t MaxBufferSize = 31+1;

  uint64_t      state[4];
  unsigned char buffer[MaxBufferSize];
  unsigned int  bufferSize;
  uint64_t      totalLength;

  /// rotate bits, should compile to a single CPU instruction (ROL)
  static inline uint64_t rotateLeft(uint64_t x, unsigned char bits)
  {
    return (x << bits) | (x >> (64 - bits));
  }

  /// process a single 64 bit value
  static inline uint64_t processSingle(uint64_t previous, uint64_t input)
  {
    return rotateLeft(previous + input * Prime2, 31) * Prime1;
  }

  /// process a block of 4x4 bytes, this is the main part of the XXHash32 algorithm
  static inline void process(const void* data, uint64_t& state0, uint64_t& state1, uint64_t& state2, uint64_t& state3)
  {
    const uint64_t* block = (const uint64_t*) data;
    state0 = processSingle(state0, block[0]);
    state1 = processSingle(state1, block[1]);
    state2 = processSingle(state2, block[2]);
    state3 = processSingle(state3, block[3]);
  }
};



/////////// other functions

int64_t prendidimensionefile(const char* i_filename)
{
	if (!i_filename)
		return 0;
	FILE* myfile = freadopen(i_filename);
	if (myfile)
    {
		fseeko(myfile, 0, SEEK_END);
		int64_t dimensione=ftello(myfile);
		fclose(myfile);
		return dimensione;
	}
	else
	return 0;
}

/// take sha256
string sha256_calc_file(const char * i_filename,const int64_t i_inizio,const int64_t i_totali,int64_t& io_lavorati)
{
	string risultato="ERROR";
	FILE* myfile = freadopen(i_filename);
	if(myfile==NULL )
 		return risultato;
	
	const int BUFSIZE	=65536*8;
	char 				buf[BUFSIZE];
	int 				n=BUFSIZE;
				
	libzpaq::SHA256 sha256;
	while (1)
	{
		int r=fread(buf, 1, n, myfile);
		
		for (int i=0;i<r;i++)
			sha256.put(*(buf+i));
 		if (r!=n) 
			break;
		io_lavorati+=n;
		if (flagnoeta==false)
			avanzamento(io_lavorati,i_totali,i_inizio);
	}
	fclose(myfile);
	
	char sha256result[32];
	memcpy(sha256result, sha256.result(), 32);
	char myhex[4];
	risultato="";
	for (int j=0; j <= 31; j++)
	{
		sprintf(myhex,"%02X", (unsigned char)sha256result[j]);
		risultato.push_back(myhex[0]);
		risultato.push_back(myhex[1]);
	}
	return risultato;
}	
	


//// calc xxhash64 (maybe)
uint64_t xxhash_calc_file(const char * i_filename,const int64_t i_inizio,const int64_t i_totali,int64_t& io_lavorati)
{
	FILE* myfile = freadopen(i_filename);
	if(myfile==NULL )
    	return 0;
	const int BUFSIZE	=65536*8;
	char 				buf[BUFSIZE];
	int 				n=BUFSIZE;
	uint64_t myseed = 0;
	XXHash64 myhash(myseed);
	while (1)
	{
		int r=fread(buf, 1, n, myfile);
		myhash.add(buf,n);
		if (r!=n) 
			break;
		io_lavorati+=n;
		if (flagnoeta==false)
			avanzamento(io_lavorati,i_totali,i_inizio);

	}
	fclose(myfile);
	return myhash.hash();
}




/// finally get crc32-c 
#define SIZE (262144*3)
#define CHUNK SIZE
unsigned int crc32c_calc_file(const char * i_filename,const int64_t i_inizio,const int64_t i_totali,int64_t& io_lavorati)
{
	FILE* myfile = freadopen(i_filename);
	
	if( myfile==NULL )
    	return 0;
    
	char buf[SIZE];
    ssize_t got;
    size_t off, n;
    uint32_t crc=0;
	
    while ((got = fread(buf, sizeof(char),SIZE,myfile)) > 0) 
	{
		off = 0;
        do 
		{
            n = (size_t)got - off;
            if (n > CHUNK)
                n = CHUNK;
            crc = crc32c(crc, (const unsigned char*)buf + off, n);
            off += n;
        } while (off < (size_t)got);
				
		io_lavorati+=got;	
		if (flagnoeta==false)
			if (i_totali)
			avanzamento(io_lavorati,i_totali,i_inizio);

    }
    fclose(myfile);
	return crc;
}


///////////////////////////////////////////////
//// support functions
string Jidac::mygetalgo()
{
/*
  for (algoritmi::iterator p=myalgoritmi.begin(); p!=myalgoritmi.end(); ++p) 
	printf("%s\n",p->second.c_str());
*/
	if (flagcrc32)
		return "CRC32";
	else
	if (flagcrc32c)
		return "CRC-32C";
	else
	if (flagxxhash)
		return "xxHASH64";
	else
	if (flagsha256)
		return "SHA256";
	else
		return "SHA1";
	
}

int Jidac::summa() 
{
	printf("Getting %s",mygetalgo().c_str());
	printf(" ignoring .zfs and :$DATA\n");


//// sha1 -all. Get a single SHA1 of some files. For backup/restore check
	libzpaq::SHA1 			sha1all;
	vector<DTMap::iterator> vf;

	int quantifiles			=0;
	int64_t total_size		=0;  
	int64_t lavorati		=0;

	flagskipzfs					=true;  // strip down zfs
	int64_t startscan=mtime();
	
	g_bytescanned=0;
	g_filescanned=0;
	g_worked=0;
	for (unsigned i=0; i<files.size(); ++i)
		scandir(edt,files[i].c_str());
	
/// firs step: how many file to elaborate ? Fast with filesystem cache
	printf("Computing filesize for %s files/directory...\n",migliaia(files.size()));
	for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) 
	{
		string filename=rename(p->first);
		if (
			p->second.date && p->first!="" && p->first[p->first.size()-1]!='/' 
			&& (!isads(filename))
			) 
		{
			total_size+=p->second.size;
			quantifiles++;
		}
    }
	
	
	int64_t inizio		=mtime();
	double scantime=(inizio-startscan)/1000.0;
	printf("Found %s bytes in %f\n",migliaia(total_size),scantime);
	
	
		
		for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) 
		{
			string filename=rename(p->first);
		/// flagskipzfs already setted!
			if (
				p->second.date && p->first!="" && p->first[p->first.size()-1]!='/' 
				&& (!isads(filename))
				) 
	
			{
				if (flagcrc32)
				{
					sprintf(p->second.sha1hex,"%08X\0x0",crc32_calc_file(filename.c_str(),inizio,total_size,lavorati));
					vf.push_back(p);
				}
				else
				if (flagcrc32c)
				{
					sprintf(p->second.sha1hex,"%08X\0x0",crc32c_calc_file(filename.c_str(),inizio,total_size,lavorati));
					vf.push_back(p);
				}
				else
				if (flagxxhash)
				{
					uint64_t result2 = xxhash_calc_file(filename.c_str(),inizio,total_size,lavorati);
					sprintf(p->second.sha1hex,"%016llX\0x0",result2);
					vf.push_back(p);
				}
				else
				if (flagsha256)
				{
					string ris256=sha256_calc_file(filename.c_str(),inizio,total_size,lavorati);
					sprintf(p->second.sha1hex,"%s",ris256.c_str());
					vf.push_back(p);
				}
				else //sha1
				{
					FP myin=fopen(filename.c_str(), RB);
					if (myin==FPNULL) ioerr(filename.c_str());
					const int BUFSIZE	=65536*8;
					char 				buf[BUFSIZE];
					int 				n=BUFSIZE;
				
					libzpaq::SHA1 sha1;
					
					while (1)
					{
						int r=fread(buf, 1, n, myin);
						if (all)
							sha1all.write(buf, r);
						else
							sha1.write(buf, r);
										
						lavorati+=r;

						if (r!=n) 
							break;
					
						if (flagnoeta==false)
							avanzamento(lavorati,total_size,inizio);
					}
					if (!all)
					{
						char sha1result[20];
						memcpy(sha1result, sha1.result(), 20);
						for (int j=0; j <= 19; j++)
							sprintf(p->second.sha1hex+j*2,"%02X", (unsigned char)sha1result[j]);
						p->second.sha1hex[40]=0x0;
						vf.push_back(p);
					}
					
					fclose(myin);
				
				}
			}
		}
	
	
	double mytime=mtime()-inizio+1;
	int64_t startprint=mtime();
	/// sometimes we want sort, sometimes no.   -sort
	if (!flagnosort)
		std::sort(vf.begin(), vf.end(), comparesha1hex);

	for (unsigned i=0; i<vf.size(); ++i) 
	{
		DTMap::iterator p=vf[i];
		printf("%s: %s %s\n",mygetalgo().c_str(),p->second.sha1hex,p->first.c_str());
	}

		printf("Used %s\n",mygetalgo().c_str());
		
		printf("Scanning filesystem time  %f\n",scantime);
		printf("Data transfer+CPU   time  %f\n",mytime/1000.0);
		printf("Data output         time  %f\n",(mtime()-startprint)/1000.0);
	
		int64_t myspeed=(int64_t)(lavorati*1000.0/(mytime));
		printf("Worked on %s average speed %s B/s\n",migliaia(lavorati),migliaia2(myspeed));
	
		if (all)
		{
			printf("SHA1ALL: ");
			if (quantifiles)
			{
				char sha1result[20];
				memcpy(sha1result, sha1all.result(), 20);
				for (int j=0; j <= 19; j++)
				{
					if (j > 0)
						printf(":");
					printf("%02X", (unsigned char)sha1result[j]);
				}
			}		
	
			printf("\n");
		}
	
  return 0;
}


/////////////////////////////// main //////////////////////////////////

// Convert argv to UTF-8 and replace \ with /
#ifdef unix
int main(int argc, const char** argv) {
#else
#ifdef _MSC_VER
int wmain(int argc, LPWSTR* argw) {
#else
int main() {
  int argc=0;
  LPWSTR* argw=CommandLineToArgvW(GetCommandLine(), &argc);
#endif
  vector<string> args(argc);
  libzpaq::Array<const char*> argp(argc);
  for (int i=0; i<argc; ++i) {
    args[i]=wtou(argw[i]);
    argp[i]=args[i].c_str();
  }
  const char** argv=&argp[0];
#endif

  global_start=mtime();  // get start time

#ifdef _WIN32
//// set UTF-8 for console
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	
	
	/*
	UINT in_cp = GetConsoleCP();
	UINT out_cp = GetConsoleOutputCP();
	printf("XXXXXXXXXXXX CodePage in=%u out=%u\n", in_cp, out_cp);
		*/
		
  OSVERSIONINFO vi = {0};
  vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&vi);
  windows7_or_above = (vi.dwMajorVersion >= 6) && (vi.dwMinorVersion >= 1);
#endif

  int errorcode=0;
  try {
    Jidac jidac;
    errorcode=jidac.doCommand(argc, argv);
  }
  catch (std::exception& e) {
    fflush(stdout);
    fprintf(stderr, "zpaq error: %s\n", e.what());
    errorcode=2;
  }
  ///printf("Staerro %d\n",errorcode);
  fflush(stdout);
  fprintf(stderr, "\n%1.3f seconds %s\n", (mtime()-global_start)/1000.0,
      errorcode>1 ? "(with errors)" :
      errorcode>0 ? "(with warnings)" : "(all OK)");
	  
	if (errorcode)
		xcommand(g_exec_error,g_exec_text);
	else
		xcommand(g_exec_ok,g_exec_text);
	
  return errorcode;
}




///////// This is a merge of unzpaq206.cpp, patched by me to become unz.cpp
///////// Now support FRANZOFFSET (embedding SHA1 into ZPAQ's c block)


int Jidac::paranoid() 
{
#ifdef _WIN32
#ifndef _WIN64
	printf("WARNING: paranoid test use lots of RAM, not good for Win32, better Win64\n");
#endif
#endif
#ifdef ESX
	printf("GURU: sorry: ESXi does not like to give so much RAM\n");
	exit(0);
#endif

	unz(archive.c_str(), password,all);
	return 0;
}


// Callback for user defined ZPAQ error handling.
// It will be called on input error with an English language error message.
// This function should not return.
extern void unzerror(const char* msg);

// Virtual base classes for input and output
// get() and put() must be overridden to read or write 1 byte.
class unzReader {
public:
  virtual int get() = 0;  // should return 0..255, or -1 at EOF
  virtual ~unzReader() {}
};

class unzWriter {
public:
  virtual void put(int c) = 0;  // should output low 8 bits of c
  virtual ~unzWriter() {}
};

// An Array of T is cleared and aligned on a 64 byte address
//   with no constructors called. No copy or assignment.
// Array<T> a(n, ex=0);  - creates n<<ex elements of type T
// a[i] - index
// a(i) - index mod n, n must be a power of 2
// a.size() - gets n
template <typename T>
class Array {
  T *data;     // user location of [0] on a 64 byte boundary
  size_t n;    // user size
  int offset;  // distance back in bytes to start of actual allocation
  void operator=(const Array&);  // no assignment
  Array(const Array&);  // no copy
public:
  Array(size_t sz=0, int ex=0): data(0), n(0), offset(0) {
    resize(sz, ex);} // [0..sz-1] = 0
  void resize(size_t sz, int ex=0); // change size, erase content to zeros
  ~Array() {resize(0);}  // free memory
  size_t size() const {return n;}  // get size
  int isize() const {return int(n);}  // get size as an int
  T& operator[](size_t i) {assert(n>0 && i<n); return data[i];}
  T& operator()(size_t i) {assert(n>0 && (n&(n-1))==0); return data[i&(n-1)];}
};

// Change size to sz<<ex elements of 0
template<typename T>
void Array<T>::resize(size_t sz, int ex) {
  assert(size_t(-1)>0);  // unsigned type?
  while (ex>0) {
    if (sz>sz*2) unzerror("Array too big");
    sz*=2, --ex;
  }
  if (n>0) {
    assert(offset>=0 && offset<=64);
    assert((char*)data-offset);
    free((char*)data-offset);
  }
  n=0;
  if (sz==0) return;
  n=sz;
  const size_t nb=128+n*sizeof(T);  // test for overflow
  if (nb<=128 || (nb-128)/sizeof(T)!=n) unzerror("Array too big");
  data=(T*)calloc(nb, 1);
  if (!data) unzerror("out of memory");

  // Align array on a 64 byte address.
  // This optimization is NOT required by the ZPAQ standard.
  offset=64-(((char*)data-(char*)0)&63);
  assert(offset>0 && offset<=64);
  data=(T*)((char*)data+offset);
}

//////////////////////////// unzSHA1 ////////////////////////////

// For computing SHA-1 checksums
class unzSHA1 {
public:
  void put(int c) {  // hash 1 byte
    uint32_t& r=w[len0>>5&15];
    r=(r<<8)|(c&255);
    if (!(len0+=8)) ++len1;
    if ((len0&511)==0) process();
  }
  double size() const {return len0/8+len1*536870912.0;} // size in bytes
  const char* result();  // get hash and reset
  unzSHA1() {init();}
private:
  void init();      // reset, but don't clear hbuf
  uint32_t len0, len1;   // length in bits (low, high)
  uint32_t h[5];         // hash state
  uint32_t w[80];        // input buffer
  char hbuf[20];    // result
  void process();   // hash 1 block
};

// Start a new hash
void unzSHA1::init() {
  len0=len1=0;
  h[0]=0x67452301;
  h[1]=0xEFCDAB89;
  h[2]=0x98BADCFE;
  h[3]=0x10325476;
  h[4]=0xC3D2E1F0;
}

// Return old result and start a new hash
const char* unzSHA1::result() {

  // pad and append length
  const uint32_t s1=len1, s0=len0;
  put(0x80);
  while ((len0&511)!=448)
    put(0);
  put(s1>>24);
  put(s1>>16);
  put(s1>>8);
  put(s1);
  put(s0>>24);
  put(s0>>16);
  put(s0>>8);
  put(s0);

  // copy h to hbuf
  for (int i=0; i<5; ++i) {
    hbuf[4*i]=h[i]>>24;
    hbuf[4*i+1]=h[i]>>16;
    hbuf[4*i+2]=h[i]>>8;
    hbuf[4*i+3]=h[i];
  }

  // return hash prior to clearing state
  init();
  return hbuf;
}

// Hash 1 block of 64 bytes
void unzSHA1::process() {
  for (int i=16; i<80; ++i) {
    w[i]=w[i-3]^w[i-8]^w[i-14]^w[i-16];
    w[i]=w[i]<<1|w[i]>>31;
  }
  uint32_t a=h[0];
  uint32_t b=h[1];
  uint32_t c=h[2];
  uint32_t d=h[3];
  uint32_t e=h[4];
  const uint32_t k1=0x5A827999, k2=0x6ED9EBA1, k3=0x8F1BBCDC, k4=0xCA62C1D6;
#define f1(a,b,c,d,e,i) e+=(a<<5|a>>27)+((b&c)|(~b&d))+k1+w[i]; b=b<<30|b>>2;
#define f5(i) f1(a,b,c,d,e,i) f1(e,a,b,c,d,i+1) f1(d,e,a,b,c,i+2) \
              f1(c,d,e,a,b,i+3) f1(b,c,d,e,a,i+4)
  f5(0) f5(5) f5(10) f5(15)
#undef f1
#define f1(a,b,c,d,e,i) e+=(a<<5|a>>27)+(b^c^d)+k2+w[i]; b=b<<30|b>>2;
  f5(20) f5(25) f5(30) f5(35)
#undef f1
#define f1(a,b,c,d,e,i) e+=(a<<5|a>>27)+((b&c)|(b&d)|(c&d))+k3+w[i]; \
        b=b<<30|b>>2;
  f5(40) f5(45) f5(50) f5(55)
#undef f1
#define f1(a,b,c,d,e,i) e+=(a<<5|a>>27)+(b^c^d)+k4+w[i]; b=b<<30|b>>2;
  f5(60) f5(65) f5(70) f5(75)
#undef f1
#undef f5
  h[0]+=a;
  h[1]+=b;
  h[2]+=c;
  h[3]+=d;
  h[4]+=e;
}

//////////////////////////// unzSHA256 //////////////////////////

// For computing SHA-256 checksums
// http://en.wikipedia.org/wiki/SHA-2
class unzSHA256 {
public:
  void put(int c) {  // hash 1 byte
    unsigned& r=w[len0>>5&15];
    r=(r<<8)|(c&255);
    if (!(len0+=8)) ++len1;
    if ((len0&511)==0) process();
  }
  double size() const {return len0/8+len1*536870912.0;} // size in bytes
  uint64_t usize() const {return len0/8+(uint64_t(len1)<<29);} //size in bytes
  const char* result();  // get hash and reset
  unzSHA256() {init();}
private:
  void init();           // reset, but don't clear hbuf
  unsigned len0, len1;   // length in bits (low, high)
  unsigned s[8];         // hash state
  unsigned w[16];        // input buffer
  char hbuf[32];         // result
  void process();        // hash 1 block
};

void unzSHA256::init() {
  len0=len1=0;
  s[0]=0x6a09e667;
  s[1]=0xbb67ae85;
  s[2]=0x3c6ef372;
  s[3]=0xa54ff53a;
  s[4]=0x510e527f;
  s[5]=0x9b05688c;
  s[6]=0x1f83d9ab;
  s[7]=0x5be0cd19;
  memset(w, 0, sizeof(w));
}

void unzSHA256::process() {

  #define ror(a,b) ((a)>>(b)|(a<<(32-(b))))

  #define m(i) \
     w[(i)&15]+=w[(i-7)&15] \
       +(ror(w[(i-15)&15],7)^ror(w[(i-15)&15],18)^(w[(i-15)&15]>>3)) \
       +(ror(w[(i-2)&15],17)^ror(w[(i-2)&15],19)^(w[(i-2)&15]>>10))

  #define r(a,b,c,d,e,f,g,h,i) { \
    unsigned t1=ror(e,14)^e; \
    t1=ror(t1,5)^e; \
    h+=ror(t1,6)+((e&f)^(~e&g))+k[i]+w[(i)&15]; } \
    d+=h; \
    {unsigned t1=ror(a,9)^a; \
    t1=ror(t1,11)^a; \
    h+=ror(t1,2)+((a&b)^(c&(a^b))); }

  #define mr(a,b,c,d,e,f,g,h,i) m(i); r(a,b,c,d,e,f,g,h,i);

  #define r8(i) \
    r(a,b,c,d,e,f,g,h,i);   \
    r(h,a,b,c,d,e,f,g,i+1); \
    r(g,h,a,b,c,d,e,f,i+2); \
    r(f,g,h,a,b,c,d,e,i+3); \
    r(e,f,g,h,a,b,c,d,i+4); \
    r(d,e,f,g,h,a,b,c,i+5); \
    r(c,d,e,f,g,h,a,b,i+6); \
    r(b,c,d,e,f,g,h,a,i+7);

  #define mr8(i) \
    mr(a,b,c,d,e,f,g,h,i);   \
    mr(h,a,b,c,d,e,f,g,i+1); \
    mr(g,h,a,b,c,d,e,f,i+2); \
    mr(f,g,h,a,b,c,d,e,i+3); \
    mr(e,f,g,h,a,b,c,d,i+4); \
    mr(d,e,f,g,h,a,b,c,i+5); \
    mr(c,d,e,f,g,h,a,b,i+6); \
    mr(b,c,d,e,f,g,h,a,i+7);

  static const unsigned k[64]={
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

  unsigned a=s[0];
  unsigned b=s[1];
  unsigned c=s[2];
  unsigned d=s[3];
  unsigned e=s[4];
  unsigned f=s[5];
  unsigned g=s[6];
  unsigned h=s[7];

  r8(0);
  r8(8);
  mr8(16);
  mr8(24);
  mr8(32);
  mr8(40);
  mr8(48);
  mr8(56);

  s[0]+=a;
  s[1]+=b;
  s[2]+=c;
  s[3]+=d;
  s[4]+=e;
  s[5]+=f;
  s[6]+=g;
  s[7]+=h;

  #undef mr8
  #undef r8
  #undef mr
  #undef r
  #undef m
  #undef ror
};

// Return old result and start a new hash
const char* unzSHA256::result() {

  // pad and append length
  const unsigned s1=len1, s0=len0;
  put(0x80);
  while ((len0&511)!=448) put(0);
  put(s1>>24);
  put(s1>>16);
  put(s1>>8);
  put(s1);
  put(s0>>24);
  put(s0>>16);
  put(s0>>8);
  put(s0);

  // copy s to hbuf
  for (int i=0; i<8; ++i) {
    hbuf[4*i]=s[i]>>24;
    hbuf[4*i+1]=s[i]>>16;
    hbuf[4*i+2]=s[i]>>8;
    hbuf[4*i+3]=s[i];
  }

  // return hash prior to clearing state
  init();
  return hbuf;
}

//////////////////////////// AES /////////////////////////////

// For encrypting with AES in CTR mode.
// The i'th 16 byte block is encrypted by XOR with AES(i)
// (i is big endian or MSB first, starting with 0).
class unzAES_CTR {
  uint32_t Te0[256], Te1[256], Te2[256], Te3[256], Te4[256]; // encryption tables
  uint32_t ek[60];  // round key
  int Nr;  // number of rounds (10, 12, 14 for AES 128, 192, 256)
  uint32_t iv0, iv1;  // first 8 bytes in CTR mode
public:
  unzAES_CTR(const char* key, int keylen, const char* iv=0);
    // Schedule: keylen is 16, 24, or 32, iv is 8 bytes or NULL
  void encrypt(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, unsigned char* ct);
  void encrypt(char* unzBuf, int n, uint64_t offset);  // encrypt n bytes of unzBuf
};

// Some AES code is derived from libtomcrypt 1.17 (public domain).

#define Te4_0 0x000000FF & Te4
#define Te4_1 0x0000FF00 & Te4
#define Te4_2 0x00FF0000 & Te4
#define Te4_3 0xFF000000 & Te4

// Extract byte n of x
static inline unsigned unzbyte(unsigned x, unsigned n) {return (x>>(8*n))&255;}

// x = y[0..3] MSB first
static inline void LOAD32H(uint32_t& x, const char* y) {
  const unsigned char* u=(const unsigned char*)y;
  x=u[0]<<24|u[1]<<16|u[2]<<8|u[3];
}

// y[0..3] = x MSB first
static inline void STORE32H(uint32_t& x, unsigned char* y) {
  y[0]=x>>24;
  y[1]=x>>16;
  y[2]=x>>8;
  y[3]=x;
}

#define setup_mix(temp) \
  ((Te4_3[unzbyte(temp, 2)]) ^ (Te4_2[unzbyte(temp, 1)]) ^ \
   (Te4_1[unzbyte(temp, 0)]) ^ (Te4_0[unzbyte(temp, 3)]))

// Initialize encryption tables and round key. keylen is 16, 24, or 32.
unzAES_CTR::unzAES_CTR(const char* key, int keylen, const char* iv) {
  assert(key  != NULL);
  assert(keylen==16 || keylen==24 || keylen==32);

  // Initialize IV (default 0)
  iv0=iv1=0;
  if (iv) {
    LOAD32H(iv0, iv);
    LOAD32H(iv1, iv+4);
  }

  // Initialize encryption tables
  for (int i=0; i<256; ++i) {
    unsigned s1=
    "\x63\x7c\x77\x7b\xf2\x6b\x6f\xc5\x30\x01\x67\x2b\xfe\xd7\xab\x76"
    "\xca\x82\xc9\x7d\xfa\x59\x47\xf0\xad\xd4\xa2\xaf\x9c\xa4\x72\xc0"
    "\xb7\xfd\x93\x26\x36\x3f\xf7\xcc\x34\xa5\xe5\xf1\x71\xd8\x31\x15"
    "\x04\xc7\x23\xc3\x18\x96\x05\x9a\x07\x12\x80\xe2\xeb\x27\xb2\x75"
    "\x09\x83\x2c\x1a\x1b\x6e\x5a\xa0\x52\x3b\xd6\xb3\x29\xe3\x2f\x84"
    "\x53\xd1\x00\xed\x20\xfc\xb1\x5b\x6a\xcb\xbe\x39\x4a\x4c\x58\xcf"
    "\xd0\xef\xaa\xfb\x43\x4d\x33\x85\x45\xf9\x02\x7f\x50\x3c\x9f\xa8"
    "\x51\xa3\x40\x8f\x92\x9d\x38\xf5\xbc\xb6\xda\x21\x10\xff\xf3\xd2"
    "\xcd\x0c\x13\xec\x5f\x97\x44\x17\xc4\xa7\x7e\x3d\x64\x5d\x19\x73"
    "\x60\x81\x4f\xdc\x22\x2a\x90\x88\x46\xee\xb8\x14\xde\x5e\x0b\xdb"
    "\xe0\x32\x3a\x0a\x49\x06\x24\x5c\xc2\xd3\xac\x62\x91\x95\xe4\x79"
    "\xe7\xc8\x37\x6d\x8d\xd5\x4e\xa9\x6c\x56\xf4\xea\x65\x7a\xae\x08"
    "\xba\x78\x25\x2e\x1c\xa6\xb4\xc6\xe8\xdd\x74\x1f\x4b\xbd\x8b\x8a"
    "\x70\x3e\xb5\x66\x48\x03\xf6\x0e\x61\x35\x57\xb9\x86\xc1\x1d\x9e"
    "\xe1\xf8\x98\x11\x69\xd9\x8e\x94\x9b\x1e\x87\xe9\xce\x55\x28\xdf"
    "\x8c\xa1\x89\x0d\xbf\xe6\x42\x68\x41\x99\x2d\x0f\xb0\x54\xbb\x16"
    [i]&255;
    unsigned s2=s1<<1;
    if (s2>=0x100) s2^=0x11b;
    unsigned s3=s1^s2;
    Te0[i]=s2<<24|s1<<16|s1<<8|s3;
    Te1[i]=s3<<24|s2<<16|s1<<8|s1;
    Te2[i]=s1<<24|s3<<16|s2<<8|s1;
    Te3[i]=s1<<24|s1<<16|s3<<8|s2;
    Te4[i]=s1<<24|s1<<16|s1<<8|s1;
  }

  // setup the forward key
  Nr = 10 + ((keylen/8)-2)*2;  // 10, 12, or 14 rounds
  int i = 0;
  uint32_t* rk = &ek[0];
  uint32_t temp;
  static const uint32_t rcon[10] = {
    0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL,
    0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
    0x1B000000UL, 0x36000000UL};  // round constants

  LOAD32H(rk[0], key   );
  LOAD32H(rk[1], key +  4);
  LOAD32H(rk[2], key +  8);
  LOAD32H(rk[3], key + 12);
  if (keylen == 16) {
    for (;;) {
      temp  = rk[3];
      rk[4] = rk[0] ^ setup_mix(temp) ^ rcon[i];
      rk[5] = rk[1] ^ rk[4];
      rk[6] = rk[2] ^ rk[5];
      rk[7] = rk[3] ^ rk[6];
      if (++i == 10) {
         break;
      }
      rk += 4;
    }
  }
  else if (keylen == 24) {
    LOAD32H(rk[4], key + 16);
    LOAD32H(rk[5], key + 20);
    for (;;) {
      temp = rk[5];
      rk[ 6] = rk[ 0] ^ setup_mix(temp) ^ rcon[i];
      rk[ 7] = rk[ 1] ^ rk[ 6];
      rk[ 8] = rk[ 2] ^ rk[ 7];
      rk[ 9] = rk[ 3] ^ rk[ 8];
      if (++i == 8) {
        break;
      }
      rk[10] = rk[ 4] ^ rk[ 9];
      rk[11] = rk[ 5] ^ rk[10];
      rk += 6;
    }
  }
  else if (keylen == 32) {
    LOAD32H(rk[4], key + 16);
    LOAD32H(rk[5], key + 20);
    LOAD32H(rk[6], key + 24);
    LOAD32H(rk[7], key + 28);
    for (;;) {
      temp = rk[7];
      rk[ 8] = rk[ 0] ^ setup_mix(temp) ^ rcon[i];
      rk[ 9] = rk[ 1] ^ rk[ 8];
      rk[10] = rk[ 2] ^ rk[ 9];
      rk[11] = rk[ 3] ^ rk[10];
      if (++i == 7) {
        break;
      }
      temp = rk[11];
      rk[12] = rk[ 4] ^ setup_mix(temp<<24|temp>>8);
      rk[13] = rk[ 5] ^ rk[12];
      rk[14] = rk[ 6] ^ rk[13];
      rk[15] = rk[ 7] ^ rk[14];
      rk += 8;
    }
  }
}

// Encrypt to ct[16]
void unzAES_CTR::encrypt(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, unsigned char* ct) {
  int r = Nr >> 1;
  uint32_t *rk = &ek[0];
  uint32_t t0=0, t1=0, t2=0, t3=0;
  s0 ^= rk[0];
  s1 ^= rk[1];
  s2 ^= rk[2];
  s3 ^= rk[3];
  for (;;) {
    t0 =
      Te0[unzbyte(s0, 3)] ^
      Te1[unzbyte(s1, 2)] ^
      Te2[unzbyte(s2, 1)] ^
      Te3[unzbyte(s3, 0)] ^
      rk[4];
    t1 =
      Te0[unzbyte(s1, 3)] ^
      Te1[unzbyte(s2, 2)] ^
      Te2[unzbyte(s3, 1)] ^
      Te3[unzbyte(s0, 0)] ^
      rk[5];
    t2 =
      Te0[unzbyte(s2, 3)] ^
      Te1[unzbyte(s3, 2)] ^
      Te2[unzbyte(s0, 1)] ^
      Te3[unzbyte(s1, 0)] ^
      rk[6];
    t3 =
      Te0[unzbyte(s3, 3)] ^
      Te1[unzbyte(s0, 2)] ^
      Te2[unzbyte(s1, 1)] ^
      Te3[unzbyte(s2, 0)] ^
      rk[7];

    rk += 8;
    if (--r == 0) {
      break;
    }

    s0 =
      Te0[unzbyte(t0, 3)] ^
      Te1[unzbyte(t1, 2)] ^
      Te2[unzbyte(t2, 1)] ^
      Te3[unzbyte(t3, 0)] ^
      rk[0];
    s1 =
      Te0[unzbyte(t1, 3)] ^
      Te1[unzbyte(t2, 2)] ^
      Te2[unzbyte(t3, 1)] ^
      Te3[unzbyte(t0, 0)] ^
      rk[1];
    s2 =
      Te0[unzbyte(t2, 3)] ^
      Te1[unzbyte(t3, 2)] ^
      Te2[unzbyte(t0, 1)] ^
      Te3[unzbyte(t1, 0)] ^
      rk[2];
    s3 =
      Te0[unzbyte(t3, 3)] ^
      Te1[unzbyte(t0, 2)] ^
      Te2[unzbyte(t1, 1)] ^
      Te3[unzbyte(t2, 0)] ^
      rk[3];
  }

  // apply last round and map cipher state to byte array block:
  s0 =
    (Te4_3[unzbyte(t0, 3)]) ^
    (Te4_2[unzbyte(t1, 2)]) ^
    (Te4_1[unzbyte(t2, 1)]) ^
    (Te4_0[unzbyte(t3, 0)]) ^
    rk[0];
  STORE32H(s0, ct);
  s1 =
    (Te4_3[unzbyte(t1, 3)]) ^
    (Te4_2[unzbyte(t2, 2)]) ^
    (Te4_1[unzbyte(t3, 1)]) ^
    (Te4_0[unzbyte(t0, 0)]) ^
    rk[1];
  STORE32H(s1, ct+4);
  s2 =
    (Te4_3[unzbyte(t2, 3)]) ^
    (Te4_2[unzbyte(t3, 2)]) ^
    (Te4_1[unzbyte(t0, 1)]) ^
    (Te4_0[unzbyte(t1, 0)]) ^
    rk[2];
  STORE32H(s2, ct+8);
  s3 =
    (Te4_3[unzbyte(t3, 3)]) ^
    (Te4_2[unzbyte(t0, 2)]) ^
    (Te4_1[unzbyte(t1, 1)]) ^
    (Te4_0[unzbyte(t2, 0)]) ^ 
    rk[3];
  STORE32H(s3, ct+12);
}

// Encrypt or decrypt slice unzBuf[0..n-1] at offset by XOR with AES(i) where
// i is the 128 bit big-endian distance from the start in 16 byte blocks.
void unzAES_CTR::encrypt(char* unzBuf, int n, uint64_t offset) {
  for (uint64_t i=offset/16; i<=(offset+n)/16; ++i) {
    unsigned char ct[16];
    encrypt(iv0, iv1, i>>32, i, ct);
    for (int j=0; j<16; ++j) {
      const int k=i*16-offset+j;
      if (k>=0 && k<n)
        unzBuf[k]^=ct[j];
    }
  }
}

#undef setup_mix
#undef Te4_3
#undef Te4_2
#undef Te4_1
#undef Te4_0

//////////////////////////// stretchKey //////////////////////

// PBKDF2(pw[0..pwlen], salt[0..saltlen], c) to unzBuf[0..dkLen-1]
// using HMAC-unzSHA256, for the special case of c = 1 iterations
// output size dkLen a multiple of 32, and pwLen <= 64.
static void unzpbkdf2(const char* pw, int pwLen, const char* salt, int saltLen,
                   int c, char* unzBuf, int dkLen) {
  assert(c==1);
  assert(dkLen%32==0);
  assert(pwLen<=64);

  unzSHA256 sha256;
  char b[32];
  for (int i=1; i*32<=dkLen; ++i) {
    for (int j=0; j<pwLen; ++j) sha256.put(pw[j]^0x36);
    for (int j=pwLen; j<64; ++j) sha256.put(0x36);
    for (int j=0; j<saltLen; ++j) sha256.put(salt[j]);
    for (int j=24; j>=0; j-=8) sha256.put(i>>j);
    memcpy(b, sha256.result(), 32);
    for (int j=0; j<pwLen; ++j) sha256.put(pw[j]^0x5c);
    for (int j=pwLen; j<64; ++j) sha256.put(0x5c);
    for (int j=0; j<32; ++j) sha256.put(b[j]);
    memcpy(unzBuf+i*32-32, sha256.result(), 32);
  }
}

// Hash b[0..15] using 8 rounds of salsa20
// Modified from http://cr.yp.to/salsa20.html (public domain) to 8 rounds
static void salsa8(uint32_t* b) {
  unsigned x[16]={0};
  memcpy(x, b, 64);
  for (int i=0; i<4; ++i) {
    #define R(a,b) (((a)<<(b))+((a)>>(32-b)))
    x[ 4] ^= R(x[ 0]+x[12], 7);  x[ 8] ^= R(x[ 4]+x[ 0], 9);
    x[12] ^= R(x[ 8]+x[ 4],13);  x[ 0] ^= R(x[12]+x[ 8],18);
    x[ 9] ^= R(x[ 5]+x[ 1], 7);  x[13] ^= R(x[ 9]+x[ 5], 9);
    x[ 1] ^= R(x[13]+x[ 9],13);  x[ 5] ^= R(x[ 1]+x[13],18);
    x[14] ^= R(x[10]+x[ 6], 7);  x[ 2] ^= R(x[14]+x[10], 9);
    x[ 6] ^= R(x[ 2]+x[14],13);  x[10] ^= R(x[ 6]+x[ 2],18);
    x[ 3] ^= R(x[15]+x[11], 7);  x[ 7] ^= R(x[ 3]+x[15], 9);
    x[11] ^= R(x[ 7]+x[ 3],13);  x[15] ^= R(x[11]+x[ 7],18);
    x[ 1] ^= R(x[ 0]+x[ 3], 7);  x[ 2] ^= R(x[ 1]+x[ 0], 9);
    x[ 3] ^= R(x[ 2]+x[ 1],13);  x[ 0] ^= R(x[ 3]+x[ 2],18);
    x[ 6] ^= R(x[ 5]+x[ 4], 7);  x[ 7] ^= R(x[ 6]+x[ 5], 9);
    x[ 4] ^= R(x[ 7]+x[ 6],13);  x[ 5] ^= R(x[ 4]+x[ 7],18);
    x[11] ^= R(x[10]+x[ 9], 7);  x[ 8] ^= R(x[11]+x[10], 9);
    x[ 9] ^= R(x[ 8]+x[11],13);  x[10] ^= R(x[ 9]+x[ 8],18);
    x[12] ^= R(x[15]+x[14], 7);  x[13] ^= R(x[12]+x[15], 9);
    x[14] ^= R(x[13]+x[12],13);  x[15] ^= R(x[14]+x[13],18);
    #undef R
  }
  for (int i=0; i<16; ++i) b[i]+=x[i];
}

// BlockMix_{Salsa20/8, r} on b[0..128*r-1]
static void blockmix(uint32_t* b, int r) {
  assert(r<=8);
  uint32_t x[16];
  uint32_t y[256];
  memcpy(x, b+32*r-16, 64);
  for (int i=0; i<2*r; ++i) {
    for (int j=0; j<16; ++j) x[j]^=b[i*16+j];
    salsa8(x);
    memcpy(&y[i*16], x, 64);
  }
  for (int i=0; i<r; ++i) memcpy(b+i*16, &y[i*32], 64);
  for (int i=0; i<r; ++i) memcpy(b+(i+r)*16, &y[i*32+16], 64);
}

// Mix b[0..128*r-1]. Uses 128*r*n bytes of memory and O(r*n) time
static void smix(char* b, int r, int n) {
  Array<uint32_t> x(32*r), v(32*r*n);
  for (int i=0; i<r*128; ++i) x[i/4]+=(b[i]&255)<<i%4*8;
  for (int i=0; i<n; ++i) {
    memcpy(&v[i*r*32], &x[0], r*128);
    blockmix(&x[0], r);
  }
  for (int i=0; i<n; ++i) {
    uint32_t j=x[(2*r-1)*16]&(n-1);
    for (int k=0; k<r*32; ++k) x[k]^=v[j*r*32+k];
    blockmix(&x[0], r);
  }
  for (int i=0; i<r*128; ++i) b[i]=x[i/4]>>(i%4*8);
}

// Strengthen password pw[0..pwlen-1] and salt[0..saltlen-1]
// to produce key unzBuf[0..buflen-1]. Uses O(n*r*p) time and 128*r*n bytes
// of memory. n must be a power of 2 and r <= 8.
void unzscrypt(const char* pw, int pwlen,
            const char* salt, int saltlen,
            int n, int r, int p, char* unzBuf, int buflen) {
  assert(r<=8);
  assert(n>0 && (n&(n-1))==0);  // power of 2?
  Array<char> b(p*r*128);
  unzpbkdf2(pw, pwlen, salt, saltlen, 1, &b[0], p*r*128);
  for (int i=0; i<p; ++i) smix(&b[i*r*128], r, n);
  unzpbkdf2(pw, pwlen, &b[0], p*r*128, 1, unzBuf, buflen);
}

// Stretch key in[0..31], assumed to be unzSHA256(password), with
// NUL terminate salt to produce new key out[0..31]
void stretchKey(char* out, const char* in, const char* salt) {
  unzscrypt(in, 32, salt, 32, 1<<14, 8, 1, out, 32);
}

//////////////////////////// unzZPAQL ///////////////////////////

// Symbolic instructions and their sizes
typedef enum {NONE,CONS,CM,ICM,MATCH,AVG,MIX2,MIX,ISSE,SSE} CompType;
const int compsize[256]={0,2,3,2,3,4,6,6,3,5};

// A unzZPAQL virtual machine COMP+HCOMP or PCOMP.
class unzZPAQL {
public:
  unzZPAQL();
  void clear();           // Free memory, erase program, reset machine state
  void inith();           // Initialize as HCOMP to run
  void initp();           // Initialize as PCOMP to run
  void run(uint32_t input);    // Execute with input
  int read(unzReader* in2);  // Read header
  void outc(int c);       // output a byte

  unzWriter* output;         // Destination for OUT instruction, or 0 to suppress
  unzSHA1* sha1;             // Points to checksum computer
  uint32_t H(int i) {return h(i);}  // get element of h

  // unzZPAQL block header
  Array<uint8_t> header;   // hsize[2] hh hm ph pm n COMP (guard) HCOMP (guard)
  int cend;           // COMP in header[7...cend-1]
  int hbegin, hend;   // HCOMP/PCOMP in header[hbegin...hend-1]

private:
  // Machine state for executing HCOMP
  Array<uint8_t> m;        // memory array M for HCOMP
  Array<uint32_t> h;       // hash array H for HCOMP
  Array<uint32_t> r;       // 256 element register array
  uint32_t a, b, c, d;     // machine registers
  int f;              // condition flag
  int pc;             // program counter

  // Support code
  void init(int hbits, int mbits);  // initialize H and M sizes
  int execute();  // execute 1 instruction, return 0 after HALT, else 1
  void div(uint32_t x) {if (x) a/=x; else a=0;}
  void mod(uint32_t x) {if (x) a%=x; else a=0;}
  void swap(uint32_t& x) {a^=x; x^=a; a^=x;}
  void swap(uint8_t& x)  {a^=x; x^=a; a^=x;}
  void err();  // exit with run time error
};

// Read header from in2
int unzZPAQL::read(unzReader* in2) {

  // Get header size and allocate
  int hsize=in2->get();
  if (hsize<0) unzerror("end of header");
  hsize+=in2->get()*256;
  if (hsize<0) unzerror("end of header");
  header.resize(hsize+300);
  cend=hbegin=hend=0;
  header[cend++]=hsize&255;
  header[cend++]=hsize>>8;
  while (cend<7) header[cend++]=in2->get(); // hh hm ph pm n

  // Read COMP
  int n=header[cend-1];
  for (int i=0; i<n; ++i) {
    int type=in2->get();  // component type
    if (type==-1) unzerror("unexpected end of file");
    header[cend++]=type;  // component type
    int size=compsize[type];
    if (size<1) unzerror("Invalid component type");
    if (cend+size>hsize) unzerror("COMP overflows header");
    for (int j=1; j<size; ++j)
      header[cend++]=in2->get();
  }
  if ((header[cend++]=in2->get())!=0) unzerror("missing COMP END");

  // Insert a guard gap and read HCOMP
  hbegin=hend=cend+128;
  if (hend>hsize+129) unzerror("missing HCOMP");
  while (hend<hsize+129) {
    assert(hend<header.isize()-8);
    int op=in2->get();
    if (op<0) unzerror("unexpected end of file");
    header[hend++]=op;
  }
  if ((header[hend++]=in2->get())!=0) unzerror("missing HCOMP END");
  assert(cend>=7 && cend<header.isize());
  assert(hbegin==cend+128 && hbegin<header.isize());
  assert(hend>hbegin && hend<header.isize());
  assert(hsize==header[0]+256*header[1]);
  assert(hsize==cend-2+hend-hbegin);
  return cend+hend-hbegin;
}

// Free memory, but preserve output, sha1 pointers
void unzZPAQL::clear() {
  cend=hbegin=hend=0;  // COMP and HCOMP locations
  a=b=c=d=f=pc=0;      // machine state
  header.resize(0);
  h.resize(0);
  m.resize(0);
  r.resize(0);
}

// Constructor
unzZPAQL::unzZPAQL() {
  output=0;
  sha1=0;
  clear();
}

// Initialize machine state as HCOMP
void unzZPAQL::inith() {
  assert(header.isize()>6);
  assert(output==0);
  assert(sha1==0);
  init(header[2], header[3]); // hh, hm
}

// Initialize machine state as PCOMP
void unzZPAQL::initp() {
  assert(header.isize()>6);
  init(header[4], header[5]); // ph, pm
}

// Initialize machine state to run a program.
void unzZPAQL::init(int hbits, int mbits) {
  assert(header.isize()>0);
  assert(cend>=7);
  assert(hbegin>=cend+128);
  assert(hend>=hbegin);
  assert(hend<header.isize()-130);
  assert(header[0]+256*header[1]==cend-2+hend-hbegin);
  h.resize(1, hbits);
  m.resize(1, mbits);
  r.resize(256);
  a=b=c=d=pc=f=0;
}

// Run program on input by interpreting header
void unzZPAQL::run(uint32_t input) {
  assert(cend>6);
  assert(hbegin>=cend+128);
  assert(hend>=hbegin);
  assert(hend<header.isize()-130);
  assert(m.size()>0);
  assert(h.size()>0);
  assert(header[0]+256*header[1]==cend+hend-hbegin-2);
  pc=hbegin;
  a=input;
  while (execute()) ;
}

void unzZPAQL::outc(int c) {
  if (output) output->put(c);
  if (sha1) sha1->put(c);
}

// Execute one instruction, return 0 after HALT else 1
int unzZPAQL::execute() {
  switch(header[pc++]) {
    case 0: err(); break; // ERROR
    case 1: ++a; break; // A++
    case 2: --a; break; // A--
    case 3: a = ~a; break; // A!
    case 4: a = 0; break; // A=0
    case 7: a = r[header[pc++]]; break; // A=R N
    case 8: swap(b); break; // B<>A
    case 9: ++b; break; // B++
    case 10: --b; break; // B--
    case 11: b = ~b; break; // B!
    case 12: b = 0; break; // B=0
    case 15: b = r[header[pc++]]; break; // B=R N
    case 16: swap(c); break; // C<>A
    case 17: ++c; break; // C++
    case 18: --c; break; // C--
    case 19: c = ~c; break; // C!
    case 20: c = 0; break; // C=0
    case 23: c = r[header[pc++]]; break; // C=R N
    case 24: swap(d); break; // D<>A
    case 25: ++d; break; // D++
    case 26: --d; break; // D--
    case 27: d = ~d; break; // D!
    case 28: d = 0; break; // D=0
    case 31: d = r[header[pc++]]; break; // D=R N
    case 32: swap(m(b)); break; // *B<>A
    case 33: ++m(b); break; // *B++
    case 34: --m(b); break; // *B--
    case 35: m(b) = ~m(b); break; // *B!
    case 36: m(b) = 0; break; // *B=0
    case 39: if (f) pc+=((header[pc]+128)&255)-127; else ++pc; break; // JT N
    case 40: swap(m(c)); break; // *C<>A
    case 41: ++m(c); break; // *C++
    case 42: --m(c); break; // *C--
    case 43: m(c) = ~m(c); break; // *C!
    case 44: m(c) = 0; break; // *C=0
    case 47: if (!f) pc+=((header[pc]+128)&255)-127; else ++pc; break; // JF N
    case 48: swap(h(d)); break; // *D<>A
    case 49: ++h(d); break; // *D++
    case 50: --h(d); break; // *D--
    case 51: h(d) = ~h(d); break; // *D!
    case 52: h(d) = 0; break; // *D=0
    case 55: r[header[pc++]] = a; break; // R=A N
    case 56: return 0  ; // HALT
    case 57: outc(a&255); break; // OUT
    case 59: a = (a+m(b)+512)*773; break; // HASH
    case 60: h(d) = (h(d)+a+512)*773; break; // HASHD
    case 63: pc+=((header[pc]+128)&255)-127; break; // JMP N
    case 64: break; // A=A
    case 65: a = b; break; // A=B
    case 66: a = c; break; // A=C
    case 67: a = d; break; // A=D
    case 68: a = m(b); break; // A=*B
    case 69: a = m(c); break; // A=*C
    case 70: a = h(d); break; // A=*D
    case 71: a = header[pc++]; break; // A= N
    case 72: b = a; break; // B=A
    case 73: break; // B=B
    case 74: b = c; break; // B=C
    case 75: b = d; break; // B=D
    case 76: b = m(b); break; // B=*B
    case 77: b = m(c); break; // B=*C
    case 78: b = h(d); break; // B=*D
    case 79: b = header[pc++]; break; // B= N
    case 80: c = a; break; // C=A
    case 81: c = b; break; // C=B
    case 82: break; // C=C
    case 83: c = d; break; // C=D
    case 84: c = m(b); break; // C=*B
    case 85: c = m(c); break; // C=*C
    case 86: c = h(d); break; // C=*D
    case 87: c = header[pc++]; break; // C= N
    case 88: d = a; break; // D=A
    case 89: d = b; break; // D=B
    case 90: d = c; break; // D=C
    case 91: break; // D=D
    case 92: d = m(b); break; // D=*B
    case 93: d = m(c); break; // D=*C
    case 94: d = h(d); break; // D=*D
    case 95: d = header[pc++]; break; // D= N
    case 96: m(b) = a; break; // *B=A
    case 97: m(b) = b; break; // *B=B
    case 98: m(b) = c; break; // *B=C
    case 99: m(b) = d; break; // *B=D
    case 100: break; // *B=*B
    case 101: m(b) = m(c); break; // *B=*C
    case 102: m(b) = h(d); break; // *B=*D
    case 103: m(b) = header[pc++]; break; // *B= N
    case 104: m(c) = a; break; // *C=A
    case 105: m(c) = b; break; // *C=B
    case 106: m(c) = c; break; // *C=C
    case 107: m(c) = d; break; // *C=D
    case 108: m(c) = m(b); break; // *C=*B
    case 109: break; // *C=*C
    case 110: m(c) = h(d); break; // *C=*D
    case 111: m(c) = header[pc++]; break; // *C= N
    case 112: h(d) = a; break; // *D=A
    case 113: h(d) = b; break; // *D=B
    case 114: h(d) = c; break; // *D=C
    case 115: h(d) = d; break; // *D=D
    case 116: h(d) = m(b); break; // *D=*B
    case 117: h(d) = m(c); break; // *D=*C
    case 118: break; // *D=*D
    case 119: h(d) = header[pc++]; break; // *D= N
    case 128: a += a; break; // A+=A
    case 129: a += b; break; // A+=B
    case 130: a += c; break; // A+=C
    case 131: a += d; break; // A+=D
    case 132: a += m(b); break; // A+=*B
    case 133: a += m(c); break; // A+=*C
    case 134: a += h(d); break; // A+=*D
    case 135: a += header[pc++]; break; // A+= N
    case 136: a -= a; break; // A-=A
    case 137: a -= b; break; // A-=B
    case 138: a -= c; break; // A-=C
    case 139: a -= d; break; // A-=D
    case 140: a -= m(b); break; // A-=*B
    case 141: a -= m(c); break; // A-=*C
    case 142: a -= h(d); break; // A-=*D
    case 143: a -= header[pc++]; break; // A-= N
    case 144: a *= a; break; // A*=A
    case 145: a *= b; break; // A*=B
    case 146: a *= c; break; // A*=C
    case 147: a *= d; break; // A*=D
    case 148: a *= m(b); break; // A*=*B
    case 149: a *= m(c); break; // A*=*C
    case 150: a *= h(d); break; // A*=*D
    case 151: a *= header[pc++]; break; // A*= N
    case 152: div(a); break; // A/=A
    case 153: div(b); break; // A/=B
    case 154: div(c); break; // A/=C
    case 155: div(d); break; // A/=D
    case 156: div(m(b)); break; // A/=*B
    case 157: div(m(c)); break; // A/=*C
    case 158: div(h(d)); break; // A/=*D
    case 159: div(header[pc++]); break; // A/= N
    case 160: mod(a); break; // A%=A
    case 161: mod(b); break; // A%=B
    case 162: mod(c); break; // A%=C
    case 163: mod(d); break; // A%=D
    case 164: mod(m(b)); break; // A%=*B
    case 165: mod(m(c)); break; // A%=*C
    case 166: mod(h(d)); break; // A%=*D
    case 167: mod(header[pc++]); break; // A%= N
    case 168: a &= a; break; // A&=A
    case 169: a &= b; break; // A&=B
    case 170: a &= c; break; // A&=C
    case 171: a &= d; break; // A&=D
    case 172: a &= m(b); break; // A&=*B
    case 173: a &= m(c); break; // A&=*C
    case 174: a &= h(d); break; // A&=*D
    case 175: a &= header[pc++]; break; // A&= N
    case 176: a &= ~ a; break; // A&~A
    case 177: a &= ~ b; break; // A&~B
    case 178: a &= ~ c; break; // A&~C
    case 179: a &= ~ d; break; // A&~D
    case 180: a &= ~ m(b); break; // A&~*B
    case 181: a &= ~ m(c); break; // A&~*C
    case 182: a &= ~ h(d); break; // A&~*D
    case 183: a &= ~ header[pc++]; break; // A&~ N
    case 184: a |= a; break; // A|=A
    case 185: a |= b; break; // A|=B
    case 186: a |= c; break; // A|=C
    case 187: a |= d; break; // A|=D
    case 188: a |= m(b); break; // A|=*B
    case 189: a |= m(c); break; // A|=*C
    case 190: a |= h(d); break; // A|=*D
    case 191: a |= header[pc++]; break; // A|= N
    case 192: a ^= a; break; // A^=A
    case 193: a ^= b; break; // A^=B
    case 194: a ^= c; break; // A^=C
    case 195: a ^= d; break; // A^=D
    case 196: a ^= m(b); break; // A^=*B
    case 197: a ^= m(c); break; // A^=*C
    case 198: a ^= h(d); break; // A^=*D
    case 199: a ^= header[pc++]; break; // A^= N
    case 200: a <<= (a&31); break; // A<<=A
    case 201: a <<= (b&31); break; // A<<=B
    case 202: a <<= (c&31); break; // A<<=C
    case 203: a <<= (d&31); break; // A<<=D
    case 204: a <<= (m(b)&31); break; // A<<=*B
    case 205: a <<= (m(c)&31); break; // A<<=*C
    case 206: a <<= (h(d)&31); break; // A<<=*D
    case 207: a <<= (header[pc++]&31); break; // A<<= N
    case 208: a >>= (a&31); break; // A>>=A
    case 209: a >>= (b&31); break; // A>>=B
    case 210: a >>= (c&31); break; // A>>=C
    case 211: a >>= (d&31); break; // A>>=D
    case 212: a >>= (m(b)&31); break; // A>>=*B
    case 213: a >>= (m(c)&31); break; // A>>=*C
    case 214: a >>= (h(d)&31); break; // A>>=*D
    case 215: a >>= (header[pc++]&31); break; // A>>= N
    case 216: f = (a == a); break; // A==A
    case 217: f = (a == b); break; // A==B
    case 218: f = (a == c); break; // A==C
    case 219: f = (a == d); break; // A==D
    case 220: f = (a == uint32_t(m(b))); break; // A==*B
    case 221: f = (a == uint32_t(m(c))); break; // A==*C
    case 222: f = (a == h(d)); break; // A==*D
    case 223: f = (a == uint32_t(header[pc++])); break; // A== N
    case 224: f = (a < a); break; // A<A
    case 225: f = (a < b); break; // A<B
    case 226: f = (a < c); break; // A<C
    case 227: f = (a < d); break; // A<D
    case 228: f = (a < uint32_t(m(b))); break; // A<*B
    case 229: f = (a < uint32_t(m(c))); break; // A<*C
    case 230: f = (a < h(d)); break; // A<*D
    case 231: f = (a < uint32_t(header[pc++])); break; // A< N
    case 232: f = (a > a); break; // A>A
    case 233: f = (a > b); break; // A>B
    case 234: f = (a > c); break; // A>C
    case 235: f = (a > d); break; // A>D
    case 236: f = (a > uint32_t(m(b))); break; // A>*B
    case 237: f = (a > uint32_t(m(c))); break; // A>*C
    case 238: f = (a > h(d)); break; // A>*D
    case 239: f = (a > uint32_t(header[pc++])); break; // A> N
    case 255: if((pc=hbegin+header[pc]+256*header[pc+1])>=hend)err();break;//LJ
    default: err();
  }
  return 1;
}

// Print illegal instruction error message and exit
void unzZPAQL::err() {
  unzerror("unzZPAQL execution error");
}

///////////////////////// Component //////////////////////////

// A Component is a context model, indirect context model, match model,
// fixed weight mixer, adaptive 2 input mixer without or with current
// partial byte as context, adaptive m input mixer (without or with),
// or SSE (without or with).

struct unzComponent {
  uint32_t limit;      // max count for cm
  uint32_t cxt;        // saved context
  uint32_t a, b, c;    // multi-purpose variables
  Array<uint32_t> cm;  // cm[cxt] -> p in bits 31..10, n in 9..0; MATCH index
  Array<uint8_t> ht;   // ICM/ISSE hash table[0..size1][0..15] and MATCH unzBuf
  Array<uint16_t> a16; // MIX weights
  void init();    // initialize to all 0
  unzComponent() {init();}
};

void unzComponent::init() {
  limit=cxt=a=b=c=0;
  cm.resize(0);
  ht.resize(0);
  a16.resize(0);
}

////////////////////////// StateTable ////////////////////////

// Next state table generator
class StateTable {
  enum {N=64}; // sizes of b, t
  int num_states(int n0, int n1);  // compute t[n0][n1][1]
  void discount(int& n0);  // set new value of n0 after 1 or n1 after 0
  void next_state(int& n0, int& n1, int y);  // new (n0,n1) after bit y
public:
  uint8_t ns[1024]; // state*4 -> next state if 0, if 1, n0, n1
  int next(int state, int y) {  // next state for bit y
    assert(state>=0 && state<256);
    assert(y>=0 && y<4);
    return ns[state*4+y];
  }
  int cminit(int state) {  // initial probability of 1 * 2^23
    assert(state>=0 && state<256);
    return ((ns[state*4+3]*2+1)<<22)/(ns[state*4+2]+ns[state*4+3]+1);
  }
  StateTable();
};

// How many states with count of n0 zeros, n1 ones (0...2)
int StateTable::num_states(int n0, int n1) {
  const int B=6;
  const int bound[B]={20,48,15,8,6,5}; // n0 -> max n1, n1 -> max n0
  if (n0<n1) return num_states(n1, n0);
  if (n0<0 || n1<0 || n1>=B || n0>bound[n1]) return 0;
  return 1+(n1>0 && n0+n1<=17);
}

// New value of count n0 if 1 is observed (and vice versa)
void StateTable::discount(int& n0) {
  n0=(n0>=1)+(n0>=2)+(n0>=3)+(n0>=4)+(n0>=5)+(n0>=7)+(n0>=8);
}

// compute next n0,n1 (0 to N) given input y (0 or 1)
void StateTable::next_state(int& n0, int& n1, int y) {
  if (n0<n1)
    next_state(n1, n0, 1-y);
  else {
    if (y) {
      ++n1;
      discount(n0);
    }
    else {
      ++n0;
      discount(n1);
    }
    // 20,0,0 -> 20,0
    // 48,1,0 -> 48,1
    // 15,2,0 -> 8,1
    //  8,3,0 -> 6,2
    //  8,3,1 -> 5,3
    //  6,4,0 -> 5,3
    //  5,5,0 -> 5,4
    //  5,5,1 -> 4,5
    while (!num_states(n0, n1)) {
      if (n1<2) --n0;
      else {
        n0=(n0*(n1-1)+(n1/2))/n1;
        --n1;
      }
    }
  }
}

// Initialize next state table ns[state*4] -> next if 0, next if 1, n0, n1
StateTable::StateTable() {

  // Assign states by increasing priority
  const int N=50;
  uint8_t t[N][N][2]={{{0}}}; // (n0,n1,y) -> state number
  int state=0;
  for (int i=0; i<N; ++i) {
    for (int n1=0; n1<=i; ++n1) {
      int n0=i-n1;
      int n=num_states(n0, n1);
      assert(n>=0 && n<=2);
      if (n) {
        t[n0][n1][0]=state;
        t[n0][n1][1]=state+n-1;
        state+=n;
      }
    }
  }
       
  // Generate next state table
  memset(ns, 0, sizeof(ns));
  for (int n0=0; n0<N; ++n0) {
    for (int n1=0; n1<N; ++n1) {
      for (int y=0; y<num_states(n0, n1); ++y) {
        int s=t[n0][n1][y];
        assert(s>=0 && s<256);
        int s0=n0, s1=n1;
        next_state(s0, s1, 0);
        assert(s0>=0 && s0<N && s1>=0 && s1<N);
        ns[s*4+0]=t[s0][s1][0];
        s0=n0, s1=n1;
        next_state(s0, s1, 1);
        assert(s0>=0 && s0<N && s1>=0 && s1<N);
        ns[s*4+1]=t[s0][s1][1];
        ns[s*4+2]=n0;
        ns[s*4+3]=n1;
      }
    }
  }
}

///////////////////////// Predictor //////////////////////////

// A predictor guesses the next bit
class unzPredictor {
public:
  unzPredictor(unzZPAQL&);
  void init();          // build model
  int predict();        // probability that next bit is a 1 (0..32767)
  void update(int y);   // train on bit y (0..1)
  bool isModeled() {    // n>0 components?
    assert(z.header.isize()>6);
    return z.header[6]!=0;
  }

private:

  // unzPredictor state
  int c8;               // last 0...7 bits.
  int hmap4;            // c8 split into nibbles
  int p[256];           // predictions
  uint32_t h[256];           // unrolled copy of z.h
  unzZPAQL& z;             // VM to compute context hashes, includes H, n
  unzComponent comp[256];  // the model, includes P

  // Modeling support functions
  int dt2k[256];        // division table for match: dt2k[i] = 2^12/i
  int dt[1024];         // division table for cm: dt[i] = 2^16/(i+1.5)
  uint16_t squasht[4096];    // squash() lookup table
  short stretcht[32768];// stretch() lookup table
  StateTable st;        // next, cminit functions

  // reduce prediction error in cr.cm
  void train(unzComponent& cr, int y) {
    assert(y==0 || y==1);
    uint32_t& pn=cr.cm(cr.cxt);
    uint32_t count=pn&0x3ff;
    int error=y*32767-(cr.cm(cr.cxt)>>17);
    pn+=(error*dt[count]&-1024)+(count<cr.limit);
  }

  // x -> floor(32768/(1+exp(-x/64)))
  int squash(int x) {
    assert(x>=-2048 && x<=2047);
    return squasht[x+2048];
  }

  // x -> round(64*log((x+0.5)/(32767.5-x))), approx inverse of squash
  int stretch(int x) {
    assert(x>=0 && x<=32767);
    return stretcht[x];
  }

  // bound x to a 12 bit signed int
  int clamp2k(int x) {
    if (x<-2048) return -2048;
    else if (x>2047) return 2047;
    else return x;
  }

  // bound x to a 20 bit signed int
  int clamp512k(int x) {
    if (x<-(1<<19)) return -(1<<19);
    else if (x>=(1<<19)) return (1<<19)-1;
    else return x;
  }

  // Get cxt in ht, creating a new row if needed
  size_t find(Array<uint8_t>& ht, int sizebits, uint32_t cxt);
};

// Initailize model-independent tables
unzPredictor::unzPredictor(unzZPAQL& zr):
    c8(1), hmap4(1), z(zr) {
  assert(sizeof(uint8_t)==1);
  assert(sizeof(uint16_t)==2);
  assert(sizeof(uint32_t)==4);
  assert(sizeof(uint64_t)==8);
  assert(sizeof(short)==2);
  assert(sizeof(int)==4);

  // Initialize tables
  dt2k[0]=0;
  for (int i=1; i<256; ++i)
    dt2k[i]=2048/i;
  for (int i=0; i<1024; ++i)
    dt[i]=(1<<17)/(i*2+3)*2;
  for (int i=0; i<32768; ++i)
    stretcht[i]=int(log((i+0.5)/(32767.5-i))*64+0.5+100000)-100000;
  for (int i=0; i<4096; ++i)
    squasht[i]=int(32768.0/(1+exp((i-2048)*(-1.0/64))));

  // Verify floating point math for squash() and stretch()
  uint32_t sqsum=0, stsum=0;
  for (int i=32767; i>=0; --i)
    stsum=stsum*3+stretch(i);
  for (int i=4095; i>=0; --i)
    sqsum=sqsum*3+squash(i-2048);
  assert(stsum==3887533746u);
  assert(sqsum==2278286169u);
}

// Initialize the predictor with a new model in z
void unzPredictor::init() {

  // Initialize context hash function
  z.inith();

  // Initialize predictions
  for (int i=0; i<256; ++i) h[i]=p[i]=0;

  // Initialize components
  for (int i=0; i<256; ++i)  // clear old model
    comp[i].init();
  int n=z.header[6]; // hsize[0..1] hh hm ph pm n (comp)[n] 0 0[128] (hcomp) 0
  const uint8_t* cp=&z.header[7];  // start of component list
  for (int i=0; i<n; ++i) {
    assert(cp<&z.header[z.cend]);
    assert(cp>&z.header[0] && cp<&z.header[z.header.isize()-8]);
    unzComponent& cr=comp[i];
    switch(cp[0]) {
      case CONS:  // c
        p[i]=(cp[1]-128)*4;
        break;
      case CM: // sizebits limit
        if (cp[1]>32) unzerror("max size for CM is 32");
        cr.cm.resize(1, cp[1]);  // packed CM (22 bits) + CMCOUNT (10 bits)
        cr.limit=cp[2]*4;
        for (size_t j=0; j<cr.cm.size(); ++j)
          cr.cm[j]=0x80000000;
        break;
      case ICM: // sizebits
        if (cp[1]>26) unzerror("max size for ICM is 26");
        cr.limit=1023;
        cr.cm.resize(256);
        cr.ht.resize(64, cp[1]);
        for (size_t j=0; j<cr.cm.size(); ++j)
          cr.cm[j]=st.cminit(j);
        break;
      case MATCH:  // sizebits
        if (cp[1]>32 || cp[2]>32) unzerror("max size for MATCH is 32 32");
        cr.cm.resize(1, cp[1]);  // index
        cr.ht.resize(1, cp[2]);  // unzBuf
        cr.ht(0)=1;
        break;
      case AVG: // j k wt
        if (cp[1]>=i) unzerror("AVG j >= i");
        if (cp[2]>=i) unzerror("AVG k >= i");
        break;
      case MIX2:  // sizebits j k rate mask
        if (cp[1]>32) unzerror("max size for MIX2 is 32");
        if (cp[3]>=i) unzerror("MIX2 k >= i");
        if (cp[2]>=i) unzerror("MIX2 j >= i");
        cr.c=(size_t(1)<<cp[1]); // size (number of contexts)
        cr.a16.resize(1, cp[1]);  // wt[size][m]
        for (size_t j=0; j<cr.a16.size(); ++j)
          cr.a16[j]=32768;
        break;
      case MIX: {  // sizebits j m rate mask
        if (cp[1]>32) unzerror("max size for MIX is 32");
        if (cp[2]>=i) unzerror("MIX j >= i");
        if (cp[3]<1 || cp[3]>i-cp[2]) unzerror("MIX m not in 1..i-j");
        int m=cp[3];  // number of inputs
        assert(m>=1);
        cr.c=(size_t(1)<<cp[1]); // size (number of contexts)
        cr.cm.resize(m, cp[1]);  // wt[size][m]
        for (size_t j=0; j<cr.cm.size(); ++j)
          cr.cm[j]=65536/m;
        break;
      }
      case ISSE:  // sizebits j
        if (cp[1]>32) unzerror("max size for ISSE is 32");
        if (cp[2]>=i) unzerror("ISSE j >= i");
        cr.ht.resize(64, cp[1]);
        cr.cm.resize(512);
        for (int j=0; j<256; ++j) {
          cr.cm[j*2]=1<<15;
          cr.cm[j*2+1]=clamp512k(stretch(st.cminit(j)>>8)*1024);
        }
        break;
      case SSE: // sizebits j start limit
        if (cp[1]>32) unzerror("max size for SSE is 32");
        if (cp[2]>=i) unzerror("SSE j >= i");
        if (cp[3]>cp[4]*4) unzerror("SSE start > limit*4");
        cr.cm.resize(32, cp[1]);
        cr.limit=cp[4]*4;
        for (size_t j=0; j<cr.cm.size(); ++j)
          cr.cm[j]=squash((j&31)*64-992)<<17|cp[3];
        break;
      default: unzerror("unknown component type");
    }
    assert(compsize[*cp]>0);
    cp+=compsize[*cp];
    assert(cp>=&z.header[7] && cp<&z.header[z.cend]);
  }
}

// Return next bit prediction using interpreted COMP code
int unzPredictor::predict() {
  assert(c8>=1 && c8<=255);

  // Predict next bit
  int n=z.header[6];
  assert(n>0 && n<=255);
  const uint8_t* cp=&z.header[7];
  assert(cp[-1]==n);
  for (int i=0; i<n; ++i) {
    assert(cp>&z.header[0] && cp<&z.header[z.header.isize()-8]);
    unzComponent& cr=comp[i];
    switch(cp[0]) {
      case CONS:  // c
        break;
      case CM:  // sizebits limit
        cr.cxt=h[i]^hmap4;
        p[i]=stretch(cr.cm(cr.cxt)>>17);
        break;
      case ICM: // sizebits
        assert((hmap4&15)>0);
        if (c8==1 || (c8&0xf0)==16) cr.c=find(cr.ht, cp[1]+2, h[i]+16*c8);
        cr.cxt=cr.ht[cr.c+(hmap4&15)];
        p[i]=stretch(cr.cm(cr.cxt)>>8);
        break;
      case MATCH: // sizebits bufbits: a=len, b=offset, c=bit, cxt=bitpos,
                  //                   ht=unzBuf, limit=pos
        assert(cr.cm.size()==(size_t(1)<<cp[1]));
        assert(cr.ht.size()==(size_t(1)<<cp[2]));
        assert(cr.a<=255);
        assert(cr.c==0 || cr.c==1);
        assert(cr.cxt<8);
        assert(cr.limit<cr.ht.size());
        if (cr.a==0) p[i]=0;
        else {
          cr.c=(cr.ht(cr.limit-cr.b)>>(7-cr.cxt))&1; // predicted bit
          p[i]=stretch(dt2k[cr.a]*(cr.c*-2+1)&32767);
        }
        break;
      case AVG: // j k wt
        p[i]=(p[cp[1]]*cp[3]+p[cp[2]]*(256-cp[3]))>>8;
        break;
      case MIX2: { // sizebits j k rate mask
                   // c=size cm=wt[size] cxt=input
        cr.cxt=((h[i]+(c8&cp[5]))&(cr.c-1));
        assert(cr.cxt<cr.a16.size());
        int w=cr.a16[cr.cxt];
        assert(w>=0 && w<65536);
        p[i]=(w*p[cp[2]]+(65536-w)*p[cp[3]])>>16;
        assert(p[i]>=-2048 && p[i]<2048);
      }
        break;
      case MIX: {  // sizebits j m rate mask
                   // c=size cm=wt[size][m] cxt=index of wt in cm
        int m=cp[3];
        assert(m>=1 && m<=i);
        cr.cxt=h[i]+(c8&cp[5]);
        cr.cxt=(cr.cxt&(cr.c-1))*m; // pointer to row of weights
        assert(cr.cxt<=cr.cm.size()-m);
        int* wt=(int*)&cr.cm[cr.cxt];
        p[i]=0;
        for (int j=0; j<m; ++j)
          p[i]+=(wt[j]>>8)*p[cp[2]+j];
        p[i]=clamp2k(p[i]>>8);
      }
        break;
      case ISSE: { // sizebits j -- c=hi, cxt=bh
        assert((hmap4&15)>0);
        if (c8==1 || (c8&0xf0)==16)
          cr.c=find(cr.ht, cp[1]+2, h[i]+16*c8);
        cr.cxt=cr.ht[cr.c+(hmap4&15)];  // bit history
        int *wt=(int*)&cr.cm[cr.cxt*2];
        p[i]=clamp2k((wt[0]*p[cp[2]]+wt[1]*64)>>16);
      }
        break;
      case SSE: { // sizebits j start limit
        cr.cxt=(h[i]+c8)*32;
        int pq=p[cp[2]]+992;
        if (pq<0) pq=0;
        if (pq>1983) pq=1983;
        int wt=pq&63;
        pq>>=6;
        assert(pq>=0 && pq<=30);
        cr.cxt+=pq;
        p[i]=stretch(((cr.cm(cr.cxt)>>10)*(64-wt)+(cr.cm(cr.cxt+1)>>10)*wt)
             >>13);
        cr.cxt+=wt>>5;
      }
        break;
      default:
        unzerror("component predict not implemented");
    }
    cp+=compsize[cp[0]];
    assert(cp<&z.header[z.cend]);
    assert(p[i]>=-2048 && p[i]<2048);
  }
  assert(cp[0]==NONE);
  return squash(p[n-1]);
}

// Update model with decoded bit y (0...1)
void unzPredictor::update(int y) {
  assert(y==0 || y==1);
  assert(c8>=1 && c8<=255);
  assert(hmap4>=1 && hmap4<=511);

  // Update components
  const uint8_t* cp=&z.header[7];
  int n=z.header[6];
  assert(n>=1 && n<=255);
  assert(cp[-1]==n);
  for (int i=0; i<n; ++i) {
    unzComponent& cr=comp[i];
    switch(cp[0]) {
      case CONS:  // c
        break;
      case CM:  // sizebits limit
        train(cr, y);
        break;
      case ICM: { // sizebits: cxt=ht[b]=bh, ht[c][0..15]=bh row, cxt=bh
        cr.ht[cr.c+(hmap4&15)]=st.next(cr.ht[cr.c+(hmap4&15)], y);
        uint32_t& pn=cr.cm(cr.cxt);
        pn+=int(y*32767-(pn>>8))>>2;
      }
        break;
      case MATCH: // sizebits bufbits:
                  //   a=len, b=offset, c=bit, cm=index, cxt=bitpos
                  //   ht=unzBuf, limit=pos
      {
        assert(cr.a<=255);
        assert(cr.c==0 || cr.c==1);
        assert(cr.cxt<8);
        assert(cr.cm.size()==(size_t(1)<<cp[1]));
        assert(cr.ht.size()==(size_t(1)<<cp[2]));
        assert(cr.limit<cr.ht.size());
        if (int(cr.c)!=y) cr.a=0;  // mismatch?
        cr.ht(cr.limit)+=cr.ht(cr.limit)+y;
        if (++cr.cxt==8) {
          cr.cxt=0;
          ++cr.limit;
          cr.limit&=(1<<cp[2])-1;
          if (cr.a==0) {  // look for a match
            cr.b=cr.limit-cr.cm(h[i]);
            if (cr.b&(cr.ht.size()-1))
              while (cr.a<255
                     && cr.ht(cr.limit-cr.a-1)==cr.ht(cr.limit-cr.a-cr.b-1))
                ++cr.a;
          }
          else cr.a+=cr.a<255;
          cr.cm(h[i])=cr.limit;
        }
      }
        break;
      case AVG:  // j k wt
        break;
      case MIX2: { // sizebits j k rate mask
                   // cm=wt[size], cxt=input
        assert(cr.a16.size()==cr.c);
        assert(cr.cxt<cr.a16.size());
        int err=(y*32767-squash(p[i]))*cp[4]>>5;
        int w=cr.a16[cr.cxt];
        w+=(err*(p[cp[2]]-p[cp[3]])+(1<<12))>>13;
        if (w<0) w=0;
        if (w>65535) w=65535;
        cr.a16[cr.cxt]=w;
      }
        break;
      case MIX: {   // sizebits j m rate mask
                    // cm=wt[size][m], cxt=input
        int m=cp[3];
        assert(m>0 && m<=i);
        assert(cr.cm.size()==m*cr.c);
        assert(cr.cxt+m<=cr.cm.size());
        int err=(y*32767-squash(p[i]))*cp[4]>>4;
        int* wt=(int*)&cr.cm[cr.cxt];
        for (int j=0; j<m; ++j)
          wt[j]=clamp512k(wt[j]+((err*p[cp[2]+j]+(1<<12))>>13));
      }
        break;
      case ISSE: { // sizebits j  -- c=hi, cxt=bh
        assert(cr.cxt==uint32_t(cr.ht[cr.c+(hmap4&15)]));
        int err=y*32767-squash(p[i]);
        int *wt=(int*)&cr.cm[cr.cxt*2];
        wt[0]=clamp512k(wt[0]+((err*p[cp[2]]+(1<<12))>>13));
        wt[1]=clamp512k(wt[1]+((err+16)>>5));
        cr.ht[cr.c+(hmap4&15)]=st.next(cr.cxt, y);
      }
        break;
      case SSE:  // sizebits j start limit
        train(cr, y);
        break;
      default:
        assert(0);
    }
    cp+=compsize[cp[0]];
    assert(cp>=&z.header[7] && cp<&z.header[z.cend] 
           && cp<&z.header[z.header.isize()-8]);
  }
  assert(cp[0]==NONE);

  // Save bit y in c8, hmap4
  c8+=c8+y;
  if (c8>=256) {
    z.run(c8-256);
    hmap4=1;
    c8=1;
    for (int i=0; i<n; ++i) h[i]=z.H(i);
  }
  else if (c8>=16 && c8<32)
    hmap4=(hmap4&0xf)<<5|y<<4|1;
  else
    hmap4=(hmap4&0x1f0)|(((hmap4&0xf)*2+y)&0xf);
}

// Find cxt row in hash table ht. ht has rows of 16 indexed by the
// low sizebits of cxt with element 0 having the next higher 8 bits for
// collision detection. If not found after 3 adjacent tries, replace the
// row with lowest element 1 as priority. Return index of row.
size_t unzPredictor::find(Array<uint8_t>& ht, int sizebits, uint32_t cxt) {
  assert(ht.size()==size_t(16)<<sizebits);
  int chk=cxt>>sizebits&255;
  size_t h0=(cxt*16)&(ht.size()-16);
  if (ht[h0]==chk) return h0;
  size_t h1=h0^16;
  if (ht[h1]==chk) return h1;
  size_t h2=h0^32;
  if (ht[h2]==chk) return h2;
  if (ht[h0+1]<=ht[h1+1] && ht[h0+1]<=ht[h2+1])
    return memset(&ht[h0], 0, 16), ht[h0]=chk, h0;
  else if (ht[h1+1]<ht[h2+1])
    return memset(&ht[h1], 0, 16), ht[h1]=chk, h1;
  else
    return memset(&ht[h2], 0, 16), ht[h2]=chk, h2;
}

//////////////////////////// unzDecoder /////////////////////////

// unzDecoder decompresses using an arithmetic code
class unzDecoder {
public:
  unzReader* in;        // destination
  unzDecoder(unzZPAQL& z);
  int decompress();  // return a byte or EOF
  void init();       // initialize at start of block
private:
  uint32_t low, high;     // range
  uint32_t curr;          // last 4 bytes of archive
  unzPredictor pr;      // to get p
  int decode(int p); // return decoded bit (0..1) with prob. p (0..65535)
};

unzDecoder::unzDecoder(unzZPAQL& z):
    in(0), low(1), high(0xFFFFFFFF), curr(0), pr(z) {
}

void unzDecoder::init() {
  pr.init();
  if (pr.isModeled()) low=1, high=0xFFFFFFFF, curr=0;
  else low=high=curr=0;
}

// Return next bit of decoded input, which has 16 bit probability p of being 1
int unzDecoder::decode(int p) {
  assert(p>=0 && p<65536);
  assert(high>low && low>0);
  if (curr<low || curr>high) unzerror("archive corrupted");
  assert(curr>=low && curr<=high);
  uint32_t mid=low+uint32_t(((high-low)*uint64_t(uint32_t(p)))>>16);  // split range
  assert(high>mid && mid>=low);
  int y=curr<=mid;
  if (y) high=mid; else low=mid+1; // pick half
  while ((high^low)<0x1000000) { // shift out identical leading bytes
    high=high<<8|255;
    low=low<<8;
    low+=(low==0);
    int c=in->get();
    if (c<0) unzerror("unexpected end of file");
    curr=curr<<8|c;
  }
  return y;
}

// Decompress 1 byte or -1 at end of input
int unzDecoder::decompress() {
  if (pr.isModeled()) {  // n>0 components?
    if (curr==0) {  // segment initialization
      for (int i=0; i<4; ++i)
        curr=curr<<8|in->get();
    }
    if (decode(0)) {
      if (curr!=0) unzerror("decoding end of input");
      return -1;
    }
    else {
      int c=1;
      while (c<256) {  // get 8 bits
        int p=pr.predict()*2+1;
        c+=c+decode(p);
        pr.update(c&1);
      }
      return c-256;
    }
  }
  else {
    if (curr==0) {  // segment initialization
      for (int i=0; i<4; ++i)
        curr=curr<<8|in->get();
      if (curr==0) return -1;
    }
    assert(curr>0);
    --curr;
    return in->get();
  }
}

/////////////////////////// unzPostProcessor ////////////////////

class unzPostProcessor {
  int state;   // input parse state: 0=INIT, 1=PASS, 2..4=loading, 5=POST
  int hsize;   // header size
  int ph, pm;  // sizes of H and M in z
public:
  unzZPAQL z;     // holds PCOMP
  unzPostProcessor(): state(0), hsize(0), ph(0), pm(0) {}
  void init(int h, int m);  // ph, pm sizes of H and M
  int write(int c);  // Input a byte, return state
  void setOutput(unzWriter* out) {z.output=out;}
  void setSHA1(unzSHA1* sha1ptr) {z.sha1=sha1ptr;}
  int getState() const {return state;}
};

// Copy ph, pm from block header
void unzPostProcessor::init(int h, int m) {
  state=hsize=0;
  ph=h;
  pm=m;
  z.clear();
}

// (PASS=0 | PROG=1 psize[0..1] pcomp[0..psize-1]) data... EOB=-1
// Return state: 1=PASS, 2..4=loading PROG, 5=PROG loaded
int unzPostProcessor::write(int c) {
  assert(c>=-1 && c<=255);
  switch (state) {
    case 0:  // initial state
      if (c<0) unzerror("Unexpected EOS");
      state=c+1;  // 1=PASS, 2=PROG
      if (state>2) unzerror("unknown post processing type");
      if (state==1) z.clear();
      break;
    case 1:  // PASS
      if (c>=0) z.outc(c);
      break;
    case 2: // PROG
      if (c<0) unzerror("Unexpected EOS");
      hsize=c;  // low byte of size
      state=3;
      break;
    case 3:  // PROG psize[0]
      if (c<0) unzerror("Unexpected EOS");
      hsize+=c*256;  // high byte of psize
      if (hsize<1) unzerror("Empty PCOMP");
      z.header.resize(hsize+300);
      z.cend=8;
      z.hbegin=z.hend=z.cend+128;
      z.header[4]=ph;
      z.header[5]=pm;
      state=4;
      break;
    case 4:  // PROG psize[0..1] pcomp[0...]
      if (c<0) unzerror("Unexpected EOS");
      assert(z.hend<z.header.isize());
      z.header[z.hend++]=c;  // one byte of pcomp
      if (z.hend-z.hbegin==hsize) {  // last byte of pcomp?
        hsize=z.cend-2+z.hend-z.hbegin;
        z.header[0]=hsize&255;  // header size with empty COMP
        z.header[1]=hsize>>8;
        z.initp();
        state=5;
      }
      break;
    case 5:  // PROG ... data
      z.run(c);
      break;
  }
  return state;
}

//////////////////////// unzDecompresser ////////////////////////

// For decompression and listing archive contents
class unzDecompresser {
public:
  unzDecompresser(): z(), dec(z), pp(), state(BLOCK), decode_state(FIRSTSEG) {}
  void setInput(unzReader* in) {dec.in=in;}
  bool findBlock();
  bool findFilename(unzWriter* = 0);
  void readComment(unzWriter* = 0);
  void setOutput(unzWriter* out) {pp.setOutput(out);}
  void setSHA1(unzSHA1* sha1ptr) {pp.setSHA1(sha1ptr);}
  void decompress();  // decompress segment
  void readSegmentEnd(char* sha1string = 0);
private:
  unzZPAQL z;
  unzDecoder dec;
  unzPostProcessor pp;
  enum {BLOCK, FILENAME, COMMENT, DATA, SEGEND} state;  // expected next
  enum {FIRSTSEG, SEG} decode_state;  // which segment in block?
};

// Find the start of a block and return true if found. Set memptr
// to memory used.
bool unzDecompresser::findBlock() {
  assert(state==BLOCK);

  // Find start of block
  uint32_t h1=0x3D49B113, h2=0x29EB7F93, h3=0x2614BE13, h4=0x3828EB13;
  // Rolling hashes initialized to hash of first 13 bytes
  int c;
  while ((c=dec.in->get())!=-1) {
    h1=h1*12+c;
    h2=h2*20+c;
    h3=h3*28+c;
    h4=h4*44+c;
    if (h1==0xB16B88F1 && h2==0xFF5376F1 && h3==0x72AC5BF1 && h4==0x2F909AF1)
      break;  // hash of 16 byte string
  }
  if (c==-1) return false;

  // Read header
  if ((c=dec.in->get())!=1 && c!=2) unzerror("unsupported ZPAQ level");
  if (dec.in->get()!=1) unzerror("unsupported unzZPAQL type");
  z.read(dec.in);
  if (c==1 && z.header.isize()>6 && z.header[6]==0)
    unzerror("ZPAQ level 1 requires at least 1 component");
  state=FILENAME;
  decode_state=FIRSTSEG;
  return true;
}

// Read the start of a segment (1) or end of block code (255).
// If a segment is found, write the filename and return true, else false.
bool unzDecompresser::findFilename(unzWriter* filename) {
  assert(state==FILENAME);
  int c=dec.in->get();
  if (c==1) {  // segment found
    while (true) {
      c=dec.in->get();
      if (c==-1) unzerror("unexpected EOF");
      if (c==0) {
        state=COMMENT;
        return true;
      }
      if (filename) filename->put(c);
    }
  }
  else if (c==255) {  // end of block found
    state=BLOCK;
    return false;
  }
  else
    unzerror("missing segment or end of block");
  return false;
}

// Read the comment from the segment header
void unzDecompresser::readComment(unzWriter* comment) {
  assert(state==COMMENT);
  state=DATA;
  while (true) {
    int c=dec.in->get();
    if (c==-1) unzerror("unexpected EOF");
    if (c==0) break;
    if (comment) comment->put(c);
  }
  if (dec.in->get()!=0) unzerror("missing reserved byte");
}

// Decompress n bytes, or all if n < 0. Return false if done
void unzDecompresser::decompress() {
  assert(state==DATA);

  // Initialize models to start decompressing block
  if (decode_state==FIRSTSEG) {
    dec.init();
    assert(z.header.size()>5);
    pp.init(z.header[4], z.header[5]);
    decode_state=SEG;
  }

  // Decompress and load PCOMP into postprocessor
  while ((pp.getState()&3)!=1)
    pp.write(dec.decompress());

  // Decompress n bytes, or all if n < 0
  while (true) {
    int c=dec.decompress();
    pp.write(c);
    if (c==-1) {
      state=SEGEND;
      return;
    }
  }
}

// Read end of block. If a unzSHA1 checksum is present, write 1 and the
// 20 byte checksum into sha1string, else write 0 in first byte.
// If sha1string is 0 then discard it.
void unzDecompresser::readSegmentEnd(char* sha1string) {
  assert(state==SEGEND);

  // Read checksum
  int c=dec.in->get();
  if (c==254) {
    if (sha1string) sha1string[0]=0;  // no checksum
  }
  else if (c==253) {
    if (sha1string) sha1string[0]=1;
    for (int i=1; i<=20; ++i) {
      c=dec.in->get();
      if (sha1string) sha1string[i]=c;
    }
  }
  else
    unzerror("missing end of segment marker");
  state=FILENAME;
}

///////////////////////// Driver program ////////////////////

uint64_t offset=0;  // number of bytes input prior to current block

// Handle errors
void unzerror(const char* msg) {
  printf("\nError at offset %1.0f: %s\n", double(offset), msg);
  exit(1);
}

// Input archive
class unzInputFile: public unzReader {
  FILE* f;  // input file
  enum {BUFSIZE=4096};
  uint64_t offset;  // number of bytes read
  unsigned p, end;  // start and end of unread bytes in unzBuf
  unzAES_CTR* aes;  // to decrypt
  char unzBuf[BUFSIZE];  // input buffer
  int64_t filesize;
public:
  unzInputFile(): f(0), offset(0), p(0), end(0), aes(0),filesize(-1) {}

  void open(const char* filename, const char* key);

  // Return one input byte or -1 for EOF
  int get() {
    if (f && p>=end) {
      p=0;
      end=fread(unzBuf, 1, BUFSIZE, f);
      if (aes) aes->encrypt(unzBuf, end, offset);
    }
    if (p>=end) return -1;
    ++offset;
    return unzBuf[p++]&255;
  }

  // Return number of bytes read
  uint64_t tell() {return offset;}
  int64_t getfilesize() {return filesize;}
};

	
// Open input. Decrypt with key.
void unzInputFile::open(const char* filename, const char* key) {
  f=fopen(filename, "rb");
  if (!f) {
    perror(filename);
    return ;
  }
	fseeko(f, 0, SEEK_END);
	filesize=ftello(f);
	fseeko(f, 0, SEEK_SET);
	
  if (key) {
    char salt[32], stretched_key[32];
    unzSHA256 sha256;
    for (int i=0; i<32; ++i) salt[i]=get();
    if (offset!=32) unzerror("no salt");
    while (*key) sha256.put(*key++);
    stretchKey(stretched_key, sha256.result(), salt);
    aes=new unzAES_CTR(stretched_key, 32, salt);
    if (!aes) unzerror("out of memory");
    aes->encrypt(unzBuf, end, 0);
  }
}

// File to extract
class unzOutputFile: public unzWriter {
  FILE* f;  // output file or NULL
  unsigned p;  // number of pending bytes to write
  enum {BUFSIZE=4096};
  char unzBuf[BUFSIZE];  // output buffer
public:
  unzOutputFile(): f(0), p(0) {}
  void open(const char* filename);
  void close();

  // write 1 byte
  void put(int c) {
    if (f) {
      unzBuf[p++]=c;
      if (p==BUFSIZE) fwrite(unzBuf, 1, p, f), p=0;
    }
  }

  virtual ~unzOutputFile() {close();}
};

// Open file unless it exists. Print error message if unsuccessful.
void unzOutputFile::open(const char* filename) {
  close();
  f=fopen(filename, "rb");
  if (f) {
    fclose(f);
    f=0;
    fprintf(stderr, "file exists: %s\n", filename);
  }
  f=fopen(filename, "wb");
  if (!f) perror(filename);
}

// Flush output and close file
void unzOutputFile::close() {
  if (f && p>0) fwrite(unzBuf, 1, p, f);
  if (f) fclose(f), f=0;
  p=0;
}


// Write to string
struct unzBuf: public unzWriter {
  size_t limit;  // maximum size
  std::string s;  // saved output
  unzBuf(size_t l): limit(l) {}

  // Save c in s
  void put(int c) {
    if (s.size()>=limit) unzerror("output overflow");
    s+=char(c);
  }
};

// Test if 14 digit date is valid YYYYMMDDHHMMSS format
void verify_date(uint64_t date) {
  int year=date/1000000/10000;
  int month=date/100000000%100;
  int day=date/1000000%100;
  int hour=date/10000%100;
  int min=date/100%100;
  int sec=date%100;
  if (year<1900 || year>2999 || month<1 || month>12 || day<1 || day>31
      || hour<0 || hour>59 || min<0 || min>59 || sec<0 || sec>59)
    unzerror("invalid date");
}

// Test if string is valid UTF8
void unzverify_utf8(const char* s) {
  while (true) {
    int c=uint8_t(*s);
    if (c==0) return;
    if ((c>=128 && c<194) || c>=245) unzerror("invalid UTF-8 first byte");
    int len=1+(c>=192)+(c>=224)+(c>=240);
    for (int i=1; i<len; ++i)
      if ((s[i]&192)!=128) unzerror("invalid UTF-8 extra byte");
    if (c==224 && uint8_t(s[1])<160) unzerror("UTF-8 3 byte long code");
    if (c==240 && uint8_t(s[1])<144) unzerror("UTF-8 4 byte long code");
    s+=len;
  }
}

// read 8 byte LSB number
uint64_t unzget8(const char* p) {
  uint64_t r=0;
  for (int i=0; i<8; ++i)
    r+=(p[i]&255ull)<<(i*8);
  return r;
}

// read 4 byte LSB number
uint32_t unzget4(const char* p) {
  uint32_t r=0;
  for (int i=0; i<4; ++i)
    r+=(p[i]&255u)<<(i*8);
  return r;
}

// file metadata
struct unzDT {
  uint64_t date;  // YYYYMMDDHHMMSS or 0 if deleted
  uint64_t attr;  // first 8 bytes, LSB first
  std::vector<uint32_t> ptr;  // fragment IDs
  char sha1hex[66];		 // 1+32+32 (unzSHA256)+ zero
  char sha1decompressedhex[66];		 // 1+32+32 (unzSHA256)+ zero
  std::string sha1fromfile;		 // 1+32+32 (unzSHA256)+ zero

  unzDT(): date(0), attr(0) {sha1hex[0]=0x0;sha1decompressedhex[0]=0x0;sha1fromfile="";}
};

typedef std::map<std::string, unzDT> unzDTMap;

bool unzcomparesha1hex(unzDTMap::iterator i_primo, unzDTMap::iterator i_secondo) 
{
	return (strcmp(i_primo->second.sha1hex,i_secondo->second.sha1hex)<0);
}

bool unzcompareprimo(unzDTMap::iterator i_primo, unzDTMap::iterator i_secondo) 
{
	return (i_primo->first<i_secondo->first);
}
template <typename T>
std::string unznumbertostring( T Number )
{
     std::ostringstream ss;
     ss << Number;
     return ss.str();
}


void myavanzamento(int64_t i_lavorati,int64_t i_totali,int64_t i_inizio)
{
	static int ultimapercentuale=0;
	
	
	int percentuale=int(i_lavorati*100.0/(i_totali+0.5));
	if (((percentuale%10)==0) && (percentuale>0))
	if (percentuale!=ultimapercentuale)
	{
		ultimapercentuale=percentuale;
			
			
		double eta=0.001*(mtime()-i_inizio)*(i_totali-i_lavorati)/(i_lavorati+1.0);
		int secondi=(mtime()-i_inizio)/1000;
		if (secondi==0)
			secondi=1;
				
		printf("%03d%% %d:%02d:%02d %20s of %20s %s/sec\n", percentuale,
		int(eta/3600), int(eta/60)%60, int(eta)%60, migliaia(i_lavorati), migliaia2(i_totali),migliaia3(i_lavorati/secondi));
		fflush(stdout);
	}
}


std::string sha1_calc_file(const char * i_filename)
{
	std::string risultato="";
	FILE* myfile = freadopen(i_filename);
	if(myfile==NULL )
 		return risultato;
	
	const int BUFSIZE	=65536*8;
	char 				unzBuf[BUFSIZE];
	int 				n=BUFSIZE;
				
	unzSHA1 sha1;
	while (1)
	{
		int r=fread(unzBuf, 1, n, myfile);
		
		for (int i=0;i<r;i++)
			sha1.put(*(unzBuf+i));
 		if (r!=n) 
			break;
//		if (!noeta)
	///		printf("%lld\r",lavorati);
	}
	fclose(myfile);
	
	char sha1result[20];
	memcpy(sha1result, sha1.result(), 20);
	char myhex[4];
	risultato="";
	for (int j=0; j <20; j++)
	{
		sprintf(myhex,"%02X", (unsigned char)sha1result[j]);
		risultato.push_back(myhex[0]);
		risultato.push_back(myhex[1]);
	}
	return risultato;
}
	
	void list_print_datetime(void)
{
	int hours, minutes, seconds, day, month, year;

	time_t now;
	time(&now);
	struct tm *local = localtime(&now);

	hours = local->tm_hour;			// get hours since midnight	(0-23)
	minutes = local->tm_min;		// get minutes passed after the hour (0-59)
	seconds = local->tm_sec;		// get seconds passed after the minute (0-59)

	day = local->tm_mday;			// get day of month (1 to 31)
	month = local->tm_mon + 1;		// get month of year (0 to 11)
	year = local->tm_year + 1900;	// get year since 1900

	printf("%02d/%02d/%d %02d:%02d:%02d ", day, month, year, hours,minutes,seconds);

}

	/// a patched... main()!
int unz(const char * archive,const char * key,bool all)
{

	
	/// really cannot run on ESXi: take WAY too much RAM
	if (!archive)
		return 0;
	
  uint64_t until=0;
  bool index=false;
 	printf("PARANOID TEST: working on %s\n",archive);
	
  // Journaling archive state
	std::map<uint32_t, unsigned> bsize;  // frag ID -> d block compressed size
	std::map<std::string, unzDT> dt;   // filename -> date, attr, frags
	std::vector<std::string> frag;  // ID -> hash[20] size[4] data
	std::string last_filename;      // streaming destination
	uint64_t ramsize=0;
	uint64_t csize=0;                    // expected offset of next non d block
	bool streaming=false, journaling=false;  // archive type
	int64_t inizio=mtime();
	uint64_t lavorati=0;
  // Decompress blocks
	unzInputFile in;  // Archive
	in.open(archive, key);
	int64_t total_size=in.getfilesize();
	
	
	
	
	unzDecompresser d;
	d.setInput(&in);
	////unzOutputFile out;   // streaming output
	bool done=false;  // stop reading?
	bool firstSegment=true;
  
	while (!done && d.findBlock()) 
  {
	unzBuf filename(65535);
    while (!done && d.findFilename(&filename)) 
	{
		unzBuf comment(65535);
		d.readComment(&comment);
		
		unzverify_utf8(filename.s.c_str());
		
		// Test for journaling or streaming block. They cannot be mixed.
		uint64_t jsize=0;  // journaling block size in comment
		if (comment.s.size()>=4 && comment.s.substr(comment.s.size()-4)=="jDC\x01") 
		{

        // read jsize = uncompressed size from comment as a decimal string
			unsigned i;
			for (i=0; i<comment.s.size()-5 && isdigit(comment.s[i]); ++i) 
			{
				jsize=jsize*10+comment.s[i]-'0';
				if (jsize>>32) unzerror("size in comment > 4294967295");
			}
			if (i<1) 
				unzerror("missing size in comment");
			if (streaming) 
				unzerror("journaling block after streaming block");
			journaling=true;
		}
		else 
		{
			if (journaling) 
			unzerror("streaming block after journaling block");
			if (index) 
				unzerror("streaming block in index");
			streaming=true;
			///d.setOutput(&out);
			///d.setOutput();
		}
		/*
		if (streaming)
		printf("Streaming\n");
		if (journaling)
		printf("journaling\n");
		*/
      // Test journaling filename. The format must be
      // jDC[YYYYMMDDHHMMSS][t][NNNNNNNNNN]
      // where YYYYMMDDHHMMSS is the date, t is the type {c,d,h,i}, and
      // NNNNNNNNNN is the 10 digit first fragment ID for types c,d,h.
      // They must be in ascending lexicographical order.

		uint64_t date=0, id=0;  // date and frag ID from filename
		char type=0;  // c,d,h,i
		if (journaling)  // di solito
		{
			if (filename.s.size()!=28) 
				unzerror("filename size not 28");
			if (filename.s.substr(0, 3)!="jDC") 
				unzerror("filename not jDC");
			type=filename.s[17];
			if (!strchr("cdhi", type)) 
				unzerror("type not c,d,h,i");
			
			if (filename.s<=last_filename) 
			unzerror("filenames out of order");
        
			last_filename=filename.s;

        // Read date
			for (int i=3; i<17; ++i) 
			{
				if (!isdigit(filename.s[i])) 
					unzerror("non-digit in date");
				date=date*10+filename.s[i]-'0';
			}
			verify_date(date);

        // Read frag ID
			for (int i=18; i<28; ++i) 
			{
				if (!isdigit(filename.s[i])) 
					unzerror("non-digit in fragment ID");
				id=id*10+filename.s[i]-'0';
			}
			if (id<1 || id>4294967295llu) 
				unzerror("fragment ID out of range");
		}

      // Select streaming output file
		if (streaming && (firstSegment || filename.s!="")) 
		{
			std::string fn=filename.s;
			/// out.open(fn.c_str());
		}
		firstSegment=false;

		// Decompress
		fflush(stdout);
		unzBuf seg(jsize);
		if (journaling) 
			d.setOutput(&seg);
		unzSHA1 sha1;
		d.setSHA1(&sha1);
		d.decompress();
		if (journaling && seg.s.size()!=jsize) 
			unzerror("incomplete output");

      // Verify checksum
		char checksum[21];
		d.readSegmentEnd(checksum);
//		if (!noeta)
///		printf("s");
		if (checksum[0]==1) 
		{
			if (memcmp(checksum+1, sha1.result(), 20)) 
				unzerror("unzSHA1 mismatch");
      //  else printf("OK");
   ///   printf("\nZ1: SHA1 checksum OK\n");
		}
		else 
		if (checksum[0]==0) 
			printf("not checked");
		else unzerror("invalid checksum type");
      ///printf("\n");
      
		filename.s="";

      // check csize at first non-d block
		if (csize && strchr("chi", type)) 
		{
			if (csize!=offset) 
			{
				printf("Z2:    csize=%1.0f, offset=%1.0f\n",double(csize), double(offset));		
				unzerror("csize does not point here");
			}
			csize=0;
		}
///printf("=======================================\n");

      // Get csize from c block
		const size_t len=seg.s.size();
		if (type=='c') 
		{
			if (len<8) 
				unzerror("c block too small");
        
			csize=unzget8(seg.s.data());
			lavorati+=csize;
		
			if (flagnoeta==false)
			{
				///printf("\n");
				list_print_datetime();
				printf("%20s (%15s)\n", migliaia(lavorati),migliaia(csize));
			}
        // test for end of archive marker
			if (csize>>63) 
			{
				printf("Incomplete transaction at end of archive (OK)\n");
				done=true;
			}
			else 
			if (index && csize!=0) 
				unzerror("nonzero csize in index");

        // test for rollback
			if (until && date>until) 
			{
				printf("Rollback: %1.0f is after %1.0f\n",double(date), double(until));
			done=true;
			}

        // Set csize to expected offset of first non d block
        // assuming 1 more byte for unread end of block marker.
			csize+=in.tell()+1;
		}

      // Test and save d block
		if (type=='d') 
		{
			if (index) 
				unzerror("d block in index");
			bsize[id]=in.tell()+1-offset;  // compressed size
   ///     printf("    %u -> %1.0f ", bsize[id], double(seg.s.size()));

        // Test frag size list at end. The format is f[id..id+n-1] fid n
        // where fid may be id or 0. sizes must sum to the rest of block.
			if (len<8) 
				unzerror("d block too small");
			
			const char* end=seg.s.data()+len;  // end of block
			uint32_t fid=unzget4(end-8);  // 0 or ID
			const uint32_t n=unzget4(end-4);    // number of frags
			
			if (fid==0) 
			fid=id;
			///if (!noeta)
				//printf("u");
				///printf(".");
        ///printf("[%u..%u) ", fid, fid+n);
			if (fid!=id) 
				unzerror("missing ID");
			if (n>(len-8)/4) 
				unzerror("frag list too big");
			uint64_t sum=0;  // computed sum of frag sizes
			for (unsigned i=0; i<n; ++i) 
				sum+=unzget4(end-12-4*i);
		//if (!noeta)
			//printf("");	//printf("m");
        ///printf("= %1.0f ", double(sum));
			if (sum+n*4+8!=len) 
				unzerror("bad frag size list");

        // Save frag hashes and sizes. For output, save data too.
			const char* data=seg.s.data();  // uncompressed data
			const char* flist=data+sum;  // frag size list
			assert(flist+n*4+8==end);
			for (uint32_t i=0; i<n; ++i) 
			{
				while (frag.size()<=id+i) 
					frag.push_back("");
				if (frag[id+i]!="") 
					unzerror("duplicate frag ID");
				uint32_t f=unzget4(flist);  // frag size
				unzSHA1 sha1;
				for (uint32_t j=0; j<f; ++j) 
					sha1.put(data[j]);
				const char* h=sha1.result();  // frag hash
				frag[id+i]=std::string(h, h+20)+std::string(flist, flist+4);
				frag[id+i]+=std::string(data, data+f);
				data+=f;
				flist+=4;
				ramsize+=frag[id+i].size();
			}
			assert(data+n*4+8==end);
			assert(flist+8==end);
			///if (!noeta)
				///printf("o"); //printf("O");
			///printf("OK\n");
		}

      // Test and save h block. Format is: bsize (sha1[20] size)...
      // where bsize is the compressed size of the d block with the same id,
      // and each size corresonds to a fragment in that block. The list
      // must match the list in the d block if present.

		if (type=='h') 
		{
			if (len%24!=4) 
			unzerror("bad h block size");
			uint32_t b=unzget4(seg.s.data());
			///if (!noeta)
				///printf("O");//printf("i");
        ///printf("    [%u..%u) %u ", uint32_t(id), uint32_t(id+len/24), b);

        // Compare hashes and sizes
			const char* p=seg.s.data()+4;  // next hash, size
			uint32_t sum=0;  // uncompressed size of all frags
			for (uint32_t i=0; i<len/24; ++i) 
			{
				if (index) 
				{
					while (frag.size()<=id+i) frag.push_back("");
					if (frag[id+i]!="") unzerror("data in index");
						frag[id+i]=std::string(p, p+24);
				}
				else 
				if (id+i>=frag.size() || frag[id+i].size()<24)
					unzerror("no matching d block");
				else 
				if (frag[id+i].substr(0, 24)!=std::string(p, p+24))
					unzerror("frag size or hash mismatch");
				
				sum+=unzget4(p+20);
				p+=24;
			}
			///if (!noeta)
				///printf("0");//printf("o");
        ///printf("-> %u OK\n", sum);
		}
		
      // Test i blocks and save files to extract. Format is:
      //   date filename 0 na attr[0..na) ni ptr[0..ni)   (to update)
      //   0    filename                                  (to delete)
      // Date is 64 bits in YYYYMMDDHHMMSS format.

		if (type=='i') 
		{
			const char* p=seg.s.data();
			const char* end=p+seg.s.size();
			while (p<end) 
			{

          // read date
				if (end-p<8) 
				unzerror("missing date");
				
				unzDT f;
				f.date=unzget8(p), p+=8;
				if (f.date!=0) 
					verify_date(f.date);

          // read filename
				std::string fn;
				while (p<end && *p) 
				{
				///		if (*p>=0 && *p<32) printf("^%c", *p+64);
       ///     else putchar(*p);
					fn+=*p++;
				}
				if (p==end) 
					unzerror("missing NUL in filename");
				++p;
				if (fn.size()>65535) 
				unzerror("filename size > 65535");
				unzverify_utf8(fn.c_str());

          // read attr
				if (f.date>0) 
				{
					if (end-p<4) 
						unzerror("missing attr size");
					uint32_t na=unzget4(p);
					p+=4;
					if (na>65535) 
					unzerror("attr size > 65535");
			
					if (na>FRANZOFFSET) // houston we have a FRANZBLOCK? SHA1 0 CRC32?
					{
						/// paranoid works with SHA1, not CRC32. Get (if any)
						for (unsigned int i=0;i<40;i++)
							f.sha1hex[i]=*(p+(na-FRANZOFFSET)+i);
						f.sha1hex[40]=0x0;
					///printf("---FRANZ OFFSET---\n");
					}
					else
					{
						f.sha1hex[0]=0x0;
					///printf("---NORMAL OFFSET---\n");
					}
			
					for (unsigned i=0; i<na; ++i) 
					{
						if (end-p<1) 
							unzerror("missing attr");
						uint64_t a=*p++&255;
						if (i<8) 
							f.attr|=a<<(i*8);
					}

					// read frag IDs
					if (end-p<4) 
						unzerror("missing frag ptr size");
					uint32_t ni=unzget4(p);
					p+=4;
					for (uint32_t i=0; i<ni; ++i) 
					{
						if (end-p<4) 
							unzerror("missing frag ID");
						uint32_t a=unzget4(p);
						p+=4;
						f.ptr.push_back(a);

					// frag must refer to valid data except in an index
						if (!index) 
						{
							if (a<1 || a>=frag.size()) 
								unzerror("frag ID out of range");
							if (frag[a]=="") 
								unzerror("missing frag data");
						}
					}
				}
				dt[fn]=f;
			}
		}
		printf("\r");
		list_print_datetime();
		printf("%02d frags %s (RAM used ~ %s)\r",100-(offset*100/(total_size+1)),migliaia(frag.size()),migliaia2(ramsize));

    }  // end while findFilename
    offset=in.tell();
  }  // end while findBlock
  printf("\n%s bytes of %s tested\n", migliaia(in.tell()), archive);

  	
	std::vector<unzDTMap::iterator> mappadt;
	std::vector<unzDTMap::iterator> vf;
	std::vector<unzDTMap::iterator> errori;
	std::vector<unzDTMap::iterator> warning;
	std::map<std::string, unzDT>::iterator p;

	int64_t iniziocalcolo=mtime();
		
	for (p=dt.begin(); p!=dt.end(); ++p)
		mappadt.push_back(p);
		
	std::sort(mappadt.begin(), mappadt.end(),unzcompareprimo);

	for (unsigned i=0; i<mappadt.size(); ++i) 
	{
		unzDTMap::iterator p=mappadt[i];
		unzDT f=p->second;
		if (f.date) 
		{
			uint64_t size=0;
			for (uint32_t i=0; i<f.ptr.size(); ++i)
				if (f.ptr[i]>0 && f.ptr[i]<frag.size() && frag[f.ptr[i]].size()>=24)
					size+=unzget4(frag[f.ptr[i]].data()+20);
			if (!index) 
			{
				std::string fn=p->first;
				
				if 	((fn.find(":$DATA")==std::string::npos) 	&&
						(fn.find(".zfs")==std::string::npos)	&&
						(fn.back()!='/')) ///changeme
					
				{
					int64_t startrecalc=mtime();
					unzSHA1 mysha1;
					for (uint32_t i=0; i<f.ptr.size(); ++i) 
						if (f.ptr[i]<frag.size())
							for (uint32_t j=24; j<frag[f.ptr[i]].size(); ++j)
								mysha1.put(frag[f.ptr[i]][j]);
									
					char sha1result[20];
					memcpy(sha1result, mysha1.result(), 20);
					for (int j=0; j <= 19; j++)
						sprintf(p->second.sha1decompressedhex+j*2,"%02X", (unsigned char)sha1result[j]);
					p->second.sha1decompressedhex[40]=0x0;
					vf.push_back(p);
					
					if (flagnoeta==false)
						printf("File %08lld of %08lld (%20s) %1.3f %s\n",i+1,(long long)mappadt.size(),migliaia(size),(mtime()-startrecalc)/1000.0,fn.c_str());
						
				}
			}
		}
	}
	if (flagnoeta==false)
		printf("\n");
		std::sort(vf.begin(), vf.end(), unzcomparesha1hex);
	printf("\n");
	int64_t finecalcolo=mtime();
	
	printf("Calc time %f\n",(finecalcolo-iniziocalcolo)/1000.0);
	
//////////////////////////////
/////// now some tests
/////// if possible check SHA1 into the ZPAQ with SHA1 extracted with SHA1 of the current file

		unsigned int status_e=0;
		unsigned int status_w=0;

		unsigned int status_0=0;
		unsigned int status_1=0;
		unsigned int status_2=0;
		
		std::string sha1delfile;
		
		for (unsigned i=0; i<vf.size(); ++i) 
		{
			
			unzDTMap::iterator p=vf[i];
			
			unsigned int statusok=0;
			bool flagerror=false;
		
			
			if (flagverify)
			{
				printf("Re-hashing %s\r",p->first.c_str());
				sha1delfile=sha1_calc_file(p->first.c_str());
			}
			std::string sha1decompresso=p->second.sha1decompressedhex;
			std::string sha1stored=p->second.sha1hex;

	
			if (flagverbose)
			{
				printf("SHA1 OF FILE:   %s\n",p->first.c_str());
				printf("DECOMPRESSED:   %s\n",sha1decompresso.c_str());
			
				if (!sha1stored.empty())
				printf("STORED IN ZPAQ: %s\n",p->second.sha1hex);
			
				if (!sha1delfile.empty())
				printf("FROM FILE:      %s\n",sha1delfile.c_str());
			}
			
			if (!sha1delfile.empty())
			{
				if (sha1delfile==sha1decompresso)
					statusok++;
				else
					flagerror=true;
			}
			if (!sha1stored.empty())
			{
				if (sha1stored==sha1decompresso)
					statusok++;
				else
					flagerror=true;
			}
			if (flagerror)
			{
				if (flagverbose)
				printf("ERROR IN VERIFY!\n");
				status_e++;
				p->second.sha1fromfile=sha1delfile;
				errori.push_back(p);
			}
			else
			{
				if (flagverbose)
					printf("OK status:      %d (0=N/A, 1=good, 2=sure)\n",statusok);
				if (statusok==1)
					status_1++;
				if (statusok==2)
					status_2++;
			}
			if (statusok==0)
			{
				status_w++;
				p->second.sha1fromfile=sha1delfile;
				warning.push_back(p);
			}
			
			if (flagverbose)
				printf("\n");
		}

		int64_t fine=mtime();
		
		if (warning.size()>0)
		{
			printf("WARNING(S) FOUND!\n");
			
			for (unsigned i=0; i<warning.size(); ++i) 
			{
				unzDTMap::iterator p=warning[i];
				
				std::string sha1delfile=p->second.sha1fromfile;
				std::string sha1decompresso=p->second.sha1decompressedhex;
				std::string sha1stored=p->second.sha1hex;
				
				printf("SHA1 OF FILE:   %s\n",p->first.c_str());
				printf("DECOMPRESSED:   %s\n",sha1decompresso.c_str());
			
				if (!sha1stored.empty())
				printf("STORED IN ZPAQ: %s\n",p->second.sha1hex);
			
				if (!sha1delfile.empty())
				printf("FROM FILE:      %s\n",sha1delfile.c_str());
				printf("\n");
			}
			printf("\n\n");
		}
		if (errori.size()>0)
		{
			printf("ERROR(S) FOUND!\n");
			
			for (unsigned i=0; i<errori.size(); ++i) 
			{
				unzDTMap::iterator p=errori[i];
				
				std::string sha1delfile=p->second.sha1fromfile;
				std::string sha1decompresso=p->second.sha1decompressedhex;
				std::string sha1stored=p->second.sha1hex;
				
				printf("SHA1 OF FILE:   %s\n",p->first.c_str());
				printf("DECOMPRESSED:   %s\n",sha1decompresso.c_str());
			
				if (!sha1stored.empty())
				printf("STORED IN ZPAQ: %s\n",sha1stored.c_str());
			
				if (!sha1delfile.empty())
				printf("FROM FILE:      %s\n",sha1delfile.c_str());
				printf("\n");
			}
		}
	
	unsigned int total_files=vf.size();
	
	printf("SUMMARY  %1.3f s\n",(fine-inizio)/1000.0);
	printf("Total file: %08d\n",total_files);


	if (status_e)
	printf("ERRORS  : %08d (ERROR:  something WRONG)\n",status_e);
	
	if (status_0)
	printf("WARNING : %08d (UNKNOWN:cannot verify)\n",status_0);
	
	if (status_1)
	printf("GOOD    : %08d of %08d (stored=decompressed)\n",status_1,total_files);
	
	if (status_2)
	printf("SURE    : %08d of %08d (stored=decompressed=file on disk)\n",status_2,total_files);
		
	if (status_e==0)
	{
		if (status_0)
			printf("Unknown (cannot verify)\n");
		else
			printf("All OK (paranoid test)\n");
	}
	else
	{
		printf("WITH ERRORS\n");
		return 2;
	}
  return 0;
}



/*
ZPAQ does not store blocks of zeros at all.
This means that they are not, materially, in the file, 
and therefore cannot be used to calculate the CRC-32.

This is a function capable of doing it, 
even for large sizes (hundreds of gigabytes or more in the case of thick virtual machine disks),
up to ~9.000TB

The function I wrote split a number into 
its powers of 2, takes the CRC-32 code from a precomputed table, 
and finally combines them. 
It's not the most efficient method (some 10-20 iterations are typically needed), 
but it's still decent

*/
const uint32_t zero_block_crc32[54] =
{
0xD202EF8D,0x41D912FF,0x2144DF1C,0x6522DF69,0xECBB4B55,0x190A55AD,0x758D6336,0xC2A8FA9D,
0x0D968558,0xB2AA7578,0xEFB5AF2E,0xF1E8BA9E,0xC71C0011,0xD8F49994,0xAB54D286,0x011FFCA6,
0xD7978EEB,0x7EE8CDCD,0xE20EEA22,0x75660AAC,0xA738EA1C,0x8D89877E,0x1147406A,0x1AD2BC45,
0xA47CA14A,0x59450445,0xB2EB30ED,0x80654151,0x2A0E7DBB,0x6DB88320,0x5B64C2B0,0x4DBDF21C,
0xD202EF8D,0x41D912FF,0x2144DF1C,0x6522DF69,0xECBB4B55,0x190A55AD,0x758D6336,0xC2A8FA9D,
0x0D968558,0xB2AA7578,0xEFB5AF2E,0xF1E8BA9E,0xC71C0011,0xD8F49994,0xAB54D286,0x011FFCA6,
0xD7978EEB,0x7EE8CDCD,0xE20EEA22,0x75660AAC,0xA738EA1C,0x8D89877E
};
uint32_t crc32ofzeroblock(uint64_t i_size) 
{
	assert(i_size<9.007.199.254.740.992); //8D89877E 2^53 9.007.199.254.740.992
	if (i_size==0)
		return 0;
	uint32_t mycrc=0;
	unsigned int i=0;
	while (i_size > 0)
	{
		if ((i_size%2)==1)
			mycrc=crc32_combine(mycrc,zero_block_crc32[i],1<<i);
		i_size=i_size/2;
		i++;
	}
   	return mycrc;
}


uint32_t crc32zeros(int64_t i_size)
{
	uint64_t inizio=mtime();
	uint32_t crc=crc32ofzeroblock(i_size);
	g_zerotime+=mtime()-inizio;
	return crc;
}


/// franz-test
/// extract data (multithread) and check CRC-32, if any
int Jidac::test() 
{
	flagtest=true;
	summary=1;
  
	const int64_t sz=read_archive(archive.c_str());
	if (sz<1) error("archive not found");
	
 	
	for (unsigned i=0; i<block.size(); ++i) 
	{
		if (block[i].bsize<0) error("negative block size");
		if (block[i].start<1) error("block starts at fragment 0");
		if (block[i].start>=ht.size()) error("block start too high");
		if (i>0 && block[i].start<block[i-1].start) error("unordered frags");
		if (i>0 && block[i].start==block[i-1].start) error("empty block");
		if (i>0 && block[i].offset<block[i-1].offset+block[i-1].bsize)
			error("unordered blocks");
		if (i>0 && block[i-1].offset+block[i-1].bsize>block[i].offset)
			error("overlapping blocks");
	}

	///printf("franz: fine test blocchi\n");
	int64_t dimtotalefile=0;
  // Label files to extract with data=0.
  ExtractJob job(*this);
  int total_files=0, skipped=0;
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) 
  {
		p->second.data=-1;  // skip
		if (p->second.date && p->first!="") 
		{
			const string fn=rename(p->first);
///			printf("--------------------> %s\n",fn.c_str());
			const bool isdir=p->first[p->first.size()-1]=='/';
			
			if (isdir)  // update directories later
				p->second.data=0;
			else 
				if (block.size()>0) 
			{  // files to decompress
				p->second.data=0;
				unsigned lo=0, hi=block.size()-1;  // block indexes for binary search
				for (unsigned i=0; p->second.data>=0 && i<p->second.ptr.size(); ++i) 
				{
					unsigned j=p->second.ptr[i];  // fragment index
					///printf("Fragment index %lld\n",j);
					if (j==0 || j>=ht.size() || ht[j].usize<-1) 
					{
						fflush(stdout);
						printUTF8(p->first.c_str(), stderr);
						fprintf(stderr, ": bad frag IDs, skipping...\n");
						p->second.data=-1;  // skip
						continue;
					}
					assert(j>0 && j<ht.size());
					
					if (lo!=hi || lo>=block.size() || j<block[lo].start
						|| (lo+1<block.size() && j>=block[lo+1].start)) 
						{
							lo=0;  // find block with fragment j by binary search
							hi=block.size()-1;
							while (lo<hi) 
							{
								unsigned mid=(lo+hi+1)/2;
								assert(mid>lo);
								assert(mid<=hi);
								if (j<block[mid].start) 
									hi=mid-1;
								else 
									(lo=mid);
							}
						}
						assert(lo==hi);
						assert(lo>=0 && lo<block.size());
						assert(j>=block[lo].start);
						assert(lo+1==block.size() || j<block[lo+1].start);
						unsigned c=j-block[lo].start+1;
						if (block[lo].size<c) 
							block[lo].size=c;
						if (block[lo].files.size()==0 || block[lo].files.back()!=p)
						{
							block[lo].files.push_back(p);
						///	printf("+++++++++++Pushato %s\n",p->first.c_str());
						}
				}
				++total_files;
				job.total_size+=p->second.size;
			}
		}  // end if selected
  }  // end for

	dimtotalefile=job.total_size;
	printf("Check %s in %s files -threads %d\n",migliaia(job.total_size), migliaia2(total_files), threads);
	vector<ThreadID> tid(threads);
	
	for (unsigned i=0; i<tid.size(); ++i) 
		run(tid[i], decompressThread, &job);

  // Wait for threads to finish
	for (unsigned i=0; i<tid.size(); ++i) 
		join(tid[i]);

 
 
  // Report failed extractions
  unsigned extracted=0, errors=0;
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) 
  {
    string fn=rename(p->first);
    if (p->second.data>=0 && p->second.date
        && fn!="" && fn[fn.size()-1]!='/') {
      ++extracted;
      if (p->second.ptr.size()!=unsigned(p->second.data)) {
        fflush(stdout);
        if (++errors==1)
          fprintf(stderr,
          "\nFailed (extracted/total fragments, file):\n");
        fprintf(stderr, "%u/%u ",
                int(p->second.data), int(p->second.ptr.size()));
        printUTF8(fn.c_str(), stderr);
        fprintf(stderr, "\n");
      }
    }
  }
  if (errors>0) {
    fflush(stdout);
    fprintf(stderr,
        "\nChecked %u of %u files OK (%u errors)"
        " using %1.3f MB x %d threads\n",
        extracted-errors, extracted, errors, job.maxMemory/1000000,
        int(tid.size()));
  }
  
//// OK now check against CRC32   

	uint64_t startverify=mtime();
	
	sort(g_crc32.begin(),g_crc32.end(),comparecrc32block);
	

	unsigned int status_e=0;
	unsigned int status_w=0;
	unsigned int status_0=0;
	unsigned int status_1=0;
	unsigned int status_2=0;
	uint32_t 	checkedfiles=0;
	uint32_t 	uncheckedfiles=0;
	
	unsigned int i=0;
	unsigned int parti=1;
	int64_t lavorati=0;
	uint32_t currentcrc32=0;
	char * crc32storedhex;
	uint32_t crc32stored;
	uint64_t dalavorare=0;
	
	
	for (unsigned int i=0;i<g_crc32.size();i++)
		dalavorare+=g_crc32[i].crc32size;
	
	printf("\n\nChecking  %s blocks with CRC32 (%s)\n",migliaia(g_crc32.size()),migliaia2(dalavorare));
	if (flagverify)
		printf("Re-testing CRC-32 from filesystem\n");
		
	uint32_t parts=0;
	uint64_t	zeroedblocks=0;
	uint32_t	howmanyzero=0;
	
	g_zerotime=0;
	/*
	printf("======================\n");
	for (unsigned int i=0;i<g_crc32.size();i++)
	{
				printf("Start %08d %08lld\n",i,g_crc32[i].crc32start);
				printf("Size  %08d %08lld\n",i,g_crc32[i].crc32size);
	}
	printf("======================\n");
	*/
	while (i<g_crc32.size())
	{
		if ( ++parts % 1000 ==0)
			if (!flagnoeta)
				printf("Block %08lu K %s\r",i/1000,migliaia(lavorati));
            
		s_crc32block it=g_crc32[i];
		DTMap::iterator p=dt.find(it.filename);
		
		crc32storedhex=0;
		crc32stored=0;
		if (p != dt.end())
			if (p->second.sha1hex[41]!=0)
			{
				crc32storedhex=p->second.sha1hex+41;
				crc32stored=crchex2int(crc32storedhex);
			}
		
/// Houston, we have something that start with a sequence of zeros, let's compute the missing CRC		
		if (it.crc32start>0)
		{
			uint64_t holestart=0;
			uint64_t holesize=it.crc32start;
			/*
			printf("Holesize iniziale %ld\n",holesize);
			*/
			
			uint32_t zerocrc=crc32zeros(holesize);
			currentcrc32=crc32_combine(currentcrc32, zerocrc,holesize);
			lavorati+=holesize;	
			zeroedblocks+=holesize;
			howmanyzero++;
			/*
			char mynomefile[100];
			sprintf(mynomefile,"z:\\globals_%014lld_%014lld_%08X_prezero",holestart,holestart+holesize,zerocrc);
			FILE* myfile=fopen(mynomefile, "wb");
			fwrite(g_allzeros, 1, holesize, myfile);
			fclose(myfile);				
			*/
		}
		
		
		currentcrc32=crc32_combine(currentcrc32, it.crc32,it.crc32size);
		lavorati+=it.crc32size;
		
		while (g_crc32[i].filename==g_crc32[i+1].filename)
		{
			if ((g_crc32[i].crc32start+g_crc32[i].crc32size) != (g_crc32[i+1].crc32start))
			{
/// Houston: we have an "hole" of zeros from i to i+1 block. Need to take the fair CRC32
				/*
				printf("############################## HOUSTON WE HAVE AN HOLE %d %d\n",i,g_crc32.size());
				printf("Start %08lld\n",g_crc32[i].crc32start);
				printf("Size  %08lld\n",g_crc32[i].crc32size);
				printf("End   %08lld\n",g_crc32[i].crc32start+g_crc32[i].crc32size);
				printf("Next  %08lld\n",g_crc32[i+1].crc32start);
				*/
				uint64_t holestart=g_crc32[i].crc32start+g_crc32[i].crc32size;
				///printf("Holestart  %08lld\n",holestart);
				
				uint64_t holesize=g_crc32[i+1].crc32start-(g_crc32[i].crc32start+g_crc32[i].crc32size);
				///printf("Hole Size  %08lld\n",holesize);
				/*
				uint32_t zerocrc;
				zerocrc=crc32_16bytes (allzeros,(uint32_t)holesize);
				//printf("zero  %08X\n",zerocrc);
				currentcrc32=crc32_combine(currentcrc32, zerocrc,(uint32_t)holesize);
				
				*/
				uint32_t zerocrc=crc32zeros(holesize);
				currentcrc32=crc32_combine(currentcrc32, zerocrc,holesize);
				lavorati+=holesize;
				zeroedblocks+=holesize;
				howmanyzero++;
				/*
				char mynomefile[100];
				sprintf(mynomefile,"z:\\globals_%014lld_%014lld_%08X_zero",holestart,holestart+holesize,zerocrc);
				FILE* myfile=fopen(mynomefile, "wb");
				fwrite(g_allzeros, 1, holesize, myfile);
				fclose(myfile);
				*/
				
			}
			i++;
			s_crc32block myit=g_crc32[i];
			currentcrc32=crc32_combine (currentcrc32, myit.crc32,myit.crc32size);
			lavorati+=myit.crc32size;
			parti++;
		}
		
		string filedefinitivo=g_crc32[i].filename;
		uint32_t crc32fromfilesystem=0; 
		
		if (flagverify)
		{
			checkedfiles++;
			if (flagverbose)
			printf("Taking CRC32 for %s\r",filedefinitivo.c_str());
			crc32fromfilesystem=crc32_calc_file(filedefinitivo.c_str(),-1,dimtotalefile,lavorati);
/*		
		if (crc32fromfilesystem==0)
			{
				printf("WARNING impossible to read file %s\n",filedefinitivo.c_str());
				status_w++;
				status_0++;
			}
			*/
		}
		
		if (flagverbose)
			printf("Stored %08X calculated %08X fromfile %08X %s\n",crc32stored,currentcrc32,crc32fromfilesystem,filedefinitivo.c_str());

		if (crc32storedhex)
		{
			
			if (currentcrc32==crc32stored)
			{
				if (flagverify)
				{
					if (crc32stored==crc32fromfilesystem)
					{
						if (flagverbose)
						printf("SURE:  STORED %08X = DECOMPRESSED = FROM FILE %08Xn",crc32stored,filedefinitivo.c_str());
						status_2++;
					}
					else
					{
						printf("ERROR:  STORED %08X XTRACTED %08X FILE %08X NOT THE SAME! (ck %08d) %s\n",crc32stored,currentcrc32,(unsigned int)crc32fromfilesystem,parti,filedefinitivo.c_str());
						status_e++;
					}
				
				}
				else
				{
					if (flagverbose)
					printf("GOOD: STORED %08X = DECOMPRESSED %08X\n",crc32stored,filedefinitivo.c_str());
					status_1++;
				}
	
			}
			else
			{
				printf("ERROR: STORED %08X != DECOMPRESSED %08X (ck %08d) %s\n",crc32stored,currentcrc32,parti,filedefinitivo.c_str());
				status_e++;
			}
		}
		else
		{	// we have and old style ZPAQ without CRC32
		
			uncheckedfiles++;
			///printf("CRC32: %08X %08X (parti %08d) %s\n",currentcrc32,chekkone,parti,filedefinitivo.c_str());
		}
		parti=1;
		currentcrc32=0;
		i++;
	}
	printf("\nVerify time %f s\n",(mtime()-startverify)/1000.0);
	
	printf("Blocks %19s (%12s)\n",migliaia(dalavorare),migliaia2(g_crc32.size()));
	printf("Zeros  %19s (%12s) %f s\n",migliaia(zeroedblocks),migliaia2(howmanyzero),(g_zerotime/1000.0));
	printf("Total  %19s speed %s/sec\n",migliaia(dalavorare+zeroedblocks),migliaia2((dalavorare+zeroedblocks)/((mtime()-startverify+1)/1000.0)));
	
	if (checkedfiles>0)
	printf("Checked   files %19s (zpaqfranz)\n",migliaia(checkedfiles));
	if (uncheckedfiles>0)
	{
		printf("UNchecked files %19s (zpaq 7.15)\n",migliaia(uncheckedfiles));
		status_0=uncheckedfiles;
	}
	
	
	if (status_e)
	printf("ERRORS  : %08d (ERROR:  something WRONG)\n",status_e);
	
	if (status_0)
	printf("WARNING : %08d (UNKNOWN:cannot verify)\n",status_0);
	
	if (status_1)
	printf("GOOD    : %08d of %08d (stored=decompressed)\n",status_1,total_files);
	
	if (status_2)
	printf("SURE    : %08d of %08d (stored=decompressed=file on disk)\n",status_2,total_files);
		
	if (status_e==0)
	{
		if (status_0)
			printf("Unknown (cannot say anything)\n");
		else
		{
			if (flagverify)
			printf("All OK (with verification from filesystem)\n");
			else
			printf("All OK (normal test)\n");
				
		}
	}
	else
	{
		printf("WITH ERRORS\n");
		return 2;
	}
	
	
  return (errors+status_e)>0;
}
template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    return rc;
}


struct s_fileandsize
{
	string	filename;
	uint64_t size;
	int64_t attr;
	int64_t date;
	bool isdir;
	char crc32hex[16];
	uint32_t crc32;
	bool crc32stored;
	s_fileandsize(): size(0),attr(0),date(0),isdir(false),crc32stored(false),crc32(0) {crc32hex[0]=0x0;}
};

bool comparecrc32(s_fileandsize a, s_fileandsize b)
{
	return a.crc32>b.crc32;
}
bool comparesizecrc32(s_fileandsize a, s_fileandsize b)
{
	
	return (a.size < b.size) ||
           ((a.size == b.size) && (a.crc32 > b.crc32)) || 
           ((a.size == b.size) && (a.crc32 == b.crc32) &&
              (a.filename<b.filename));
			  ///(strcmp(a.filename.c_str(), b.filename.c_str()) <0));
			  
}
bool comparefilenamesize(s_fileandsize a, s_fileandsize b)
{
	char a_size[40];
	char b_size[40];
	sprintf(a_size,"%014lld",a.size);
	sprintf(b_size,"%014lld",b.size);
	return a_size+a.filename<b_size+b.filename;
}
bool comparefilenamedate(s_fileandsize a, s_fileandsize b)
{
	char a_size[40];
	char b_size[40];
	sprintf(a_size,"%014lld",a.date);
	sprintf(b_size,"%014lld",b.date);
	return a_size+a.filename<b_size+b.filename;
}

/*
If there's one thing I hate about UNIX and Linux in general it's the ls command, 
as opposed to Windows dir. 
This is a kind of mini clone, to be used with a shell alias for convenience.

Main switch are /s, /os, /on, /a
-n X like tail -n

Using -crc32 or -crc32c find duplicate files, just about like rar a -oi3:1 -r dummy s:\
		

zpaqfranz dir r:\minc\2 /s -crc32 -n 100
will show the 100 biggest duplicate file in r:\minc\2

zpaqfranz dir r:\minc\2 /s /os -n 10
will show the 10 biggest file in r:\minc\2


----

The UNIX philosophy is "do one thing, and do it well"
Mine is "build everything to make you comfortable"
*/


int  Jidac::dir() 
{
	bool barras	=false;
	bool barraos=false;
	bool barraod=false;
	bool barraa	=false;
	
/*esx*/

	if (files.size()==0)
		files.push_back(".");
	else
	if (files[0].front()=='/')
		files.insert(files.begin(),1, ".");


	string cartella=files[0];
	
	if	(!isdirectory(cartella))
		cartella+='/';
		
	if (files.size()>1)
		for (unsigned i=0; i<files.size(); i++) 
		{
				barras	|=(stringcomparei(files[i],"/s"));
				barraos	|=(stringcomparei(files[i],"/os"));
				barraod	|=(stringcomparei(files[i],"/od"));
				barraa	|=(stringcomparei(files[i],"/a"));
		}
	
	printf("==== Scanning dir <<%s>> ",cartella.c_str());
		
	if (barras)
		printf(" /s");
	if (barraos)
		printf(" /os");
	if (barraod)
		printf(" /od");
	if (barraa)
		printf(" /a");
			
	printf("\n");
			
			
	bool		duplicati		=false;
	int64_t 	total_size		=0;
	int 		quantifiles		=0;
	int 		quantedirectory	=0;
	int64_t		dummylavorati	=0;
	int32_t 	crc;
	int			quanticrc		=0;
	int64_t		crc_calcolati	=0;
	if (flagcrc32 || flagcrc32c)
		duplicati=true;
	uint64_t	iniziocrc=0;
	uint64_t	finecrc=0;
			
	g_bytescanned=0;
	g_filescanned=0;
	g_worked=0;
	scandir(edt,cartella,barras);
	
	vector <s_fileandsize> fileandsize;
	
	for (DTMap::iterator p=edt.begin(); p!=edt.end(); ++p) 
	{
		bool flagaggiungi=true;

#ifdef _WIN32	
		if (!barraa)
			flagaggiungi=((int)attrToString(p->second.attr).find("H") < 0);
		flagaggiungi &= (!isads(p->first));
#endif
		
		if (searchfrom!="")
			flagaggiungi &= (stristr(p->first.c_str(),searchfrom.c_str())!=0);
	
		if (flagaggiungi)
		{
			s_fileandsize myblock;
			myblock.filename=p->first;
			myblock.size=p->second.size;
			myblock.attr=p->second.attr;
			myblock.date=p->second.date;
			myblock.isdir=p->first[p->first.size()-1]=='/';
			myblock.crc32stored=false;
				
			if (duplicati)
			{
				if (!myblock.isdir)
				{
					if (minsize)
					{
						if (myblock.size>minsize)
							fileandsize.push_back(myblock);
					}
					else
					if (maxsize)
					{
						if (myblock.size<minsize)
							fileandsize.push_back(myblock);
					}
					else
						fileandsize.push_back(myblock);
					
				}
			}
			else
				fileandsize.push_back(myblock);
		}
	}
	
	
	if (duplicati)
	{
		if (flagverbose)
		{
			printf("Pre-sort\n");
			for (int i=0;i<fileandsize.size();i++)
				printf("PRE %08d  %08d %s\n",i,fileandsize[i].size,fileandsize[i].filename.c_str());
		}
	
		sort(fileandsize.begin(),fileandsize.end(),comparefilenamesize);
	
		if (flagverbose)
		{
			printf("Post-sort\n");
			for (int i=0;i<fileandsize.size();i++)
				printf("POST %08d  %08d %s\n",i,fileandsize[i].size,fileandsize[i].filename.c_str());
		}
	
		iniziocrc=mtime();
	
		int64_t dalavorare=0;
		for (int i=0;i<fileandsize.size();i++)
			if (i<fileandsize.size()-1)
			{
				bool entrato=false;
				while (fileandsize[i].size==fileandsize[i+1].size)
				{
					if (!entrato)
					{
						dalavorare+=fileandsize[i].size;
						entrato=true;
					}
					
					dalavorare+=fileandsize[i+1].size;
					i++;
					if (i==fileandsize.size())
						break;
				}
			}

		
		/*
		rar a -oi3:1 -r dummy s:\
		*/
		printf("\nStart checksumming %s / %s bytes ...\n",migliaia(fileandsize.size()),migliaia2(dalavorare));
			
		int larghezzaconsole=terminalwidth()-2;
		if (larghezzaconsole<10)
			larghezzaconsole=80;

		int64_t ultimapercentuale=0;
		int64_t rapporto;
		int64_t percentuale;
		///larghezzaconsole=50;
		///printf("Rapporto %d\n",larghezzaconsole);
		for (int i=0;i<fileandsize.size();i++)
		{
			if (i<fileandsize.size()-1)
			{
				bool entrato=false;
				while (fileandsize[i].size==fileandsize[i+1].size)
				{
					if (!entrato)
					{
						crc=crc32c_calc_file(fileandsize[i].filename.c_str(),0,0,dummylavorati);
						fileandsize[i].crc32=crc;
						fileandsize[i].crc32stored=true;
						quanticrc++;
						crc_calcolati+=fileandsize[i].size;
						entrato=true;
						if (flagverbose)
							printf("%08ld CRC-1 %08X %s %19s %s\n",i,fileandsize[i].crc32,dateToString(fileandsize[i].date).c_str(),migliaia(fileandsize[i].size),fileandsize[i].filename.c_str());
						else
						{
							rapporto=larghezzaconsole*crc_calcolati/(dalavorare+1);
							percentuale=100*crc_calcolati/(dalavorare+1);
							printf("Done %03d ",percentuale);
							if (percentuale>ultimapercentuale)
							{
								if (rapporto>10)
								{
									for (unsigned j=0;j<rapporto-10;j++)
										printf(".");
									ultimapercentuale=percentuale;
								}
							}
							printf("\r");
							
						}
					
					}
					
					crc=crc32c_calc_file(fileandsize[i+1].filename.c_str(),0,0,dummylavorati);
					fileandsize[i+1].crc32=crc;
					fileandsize[i+1].crc32stored=true;
					quanticrc++;
					crc_calcolati+=fileandsize[i+1].size;
					if (flagverbose)
						printf("%08ld CRC-2 %08X %s %19s %s\n",i+1,fileandsize[i+1].crc32,dateToString(fileandsize[i+1].date).c_str(),migliaia(fileandsize[i+1].size),fileandsize[i+1].filename.c_str());
					else
					{
						rapporto=larghezzaconsole*crc_calcolati/(dalavorare+1);
						percentuale=100*crc_calcolati/(dalavorare+1);
						printf("Done %03d ",percentuale);
						if (percentuale>ultimapercentuale)
							{
								if (rapporto>10)
								{
									for (unsigned j=0;j<rapporto-10;j++)
										printf(".");
									ultimapercentuale=percentuale;
								}
							}
							
						printf("\r");
						
					}
					
					i++;
					if (i==fileandsize.size())
						break;
					
				}
				
			}
		}
		finecrc=mtime();
		printf("\n");
		sort(fileandsize.begin(),fileandsize.end(),comparecrc32);
	
		if (flagverbose)
		{
			printf("CRC taken %08d %s\n",quanticrc,migliaia(crc_calcolati));
			printf("Before shrink %s\n",migliaia(fileandsize.size()));
			printf("Time %f\n",(finecrc-iniziocrc)/1000.0);
	
			for (int i=0;i<fileandsize.size();i++)
				printf("before shrink %08d  %08X %s\n",i,fileandsize[i].crc32,fileandsize[i].filename.c_str());
		}
	
		int limite=-1;
		for (int i=0;i<fileandsize.size(); i++)
		{
			if (flagverbose)
				printf("%ld size %s <<%08X>>\n",i,migliaia(fileandsize.size()),fileandsize[i].crc32);
		
			if (!fileandsize[i].crc32stored)
			{
				limite=i;
				break;
			}	
		}
	
		if (flagverbose)
		printf("Limit founded %ld\n",limite);
		
		if (limite>-1)
			for (int i=fileandsize.size()-1;i>=limite;i--)
				fileandsize.pop_back();
	
	
		if (flagverbose)
		{
			printf("After shrink %s\n",migliaia(fileandsize.size()));

			for (int i=0;i<fileandsize.size();i++)
				printf("After shrinking %08d  %08X %s\n",i,fileandsize[i].crc32,fileandsize[i].filename.c_str());
		}
	
		sort(fileandsize.begin(),fileandsize.end(),comparesizecrc32);
	
		if (flagverbose)
		{
			printf("After re-sort %s\n",migliaia(fileandsize.size()));

			for (int i=0;i<fileandsize.size();i++)
				printf("After re-sort %08d  %08X %s\n",i,fileandsize[i].crc32,fileandsize[i].filename.c_str());
		}
	
	}
	else
	{

		if (barraod)
			sort(fileandsize.begin(),fileandsize.end(),comparefilenamedate);
	
		if (barraos)
			sort(fileandsize.begin(),fileandsize.end(),comparefilenamesize);
	}
	unsigned int inizio=0;
	if (menoenne)
		if (menoenne<fileandsize.size())
			inizio=fileandsize.size()-menoenne;
	
	
	int64_t tot_duplicati=0;
	
	for (int i=inizio;i<fileandsize.size();i++)
		if (fileandsize[i].isdir)
		{
			printf("%s <DIR>               %s\n",dateToString(fileandsize[i].date).c_str(),fileandsize[i].filename.c_str());
			quantedirectory++;
		}
		else
		{
			total_size+=fileandsize[i].size;
			quantifiles++;
			
			if (duplicati)
			{
			
				if (i<fileandsize.size()-1)
				{
					bool entrato=false;

					//if (memcmp(fileandsize[i].crc32hex,fileandsize[i+1].crc32hex,8)==0)
					if (fileandsize[i].crc32==fileandsize[i+1].crc32)
					{
						if (flagverbose)
							printf("%08X ",fileandsize[i].crc32);
						
						printf("%s %19s %s\n",dateToString(fileandsize[i].date).c_str(),migliaia(fileandsize[i].size),fileandsize[i].filename.c_str());
						
						while (fileandsize[i].crc32==fileandsize[i+1].crc32)
						{
							if (flagverbose)
							printf("%08X ",fileandsize[i].crc32);
							
							printf("=================== %19s %s\n",migliaia(fileandsize[i+1].size),fileandsize[i+1].filename.c_str());
							tot_duplicati+=fileandsize[i].size;
							entrato=true;
							i++;
							if (i==fileandsize.size())
								break;
						}
					}
					if (entrato)
						printf("\n");
				}
				
			}
			else
				printf("%s %19s %s\n",dateToString(fileandsize[i].date).c_str(),migliaia(fileandsize[i].size),fileandsize[i].filename.c_str());
			
		}
	
	           
	printf("  %8ld File     %19s byte\n",quantifiles,migliaia(total_size));
	if (!duplicati)
	printf("  %8ld directory",quantedirectory);
	
	int64_t spazio=getfreespace(cartella.c_str());
	printf(" %s bytes (%10s) free",migliaia(spazio),tohuman(spazio));
	
	printf("\n");
	if (duplicati)
	{
		printf("          Duplicate %19s byte\n",migliaia(tot_duplicati));
		printf("CRC taken %08d %s in %f s %s /s\n",quanticrc,migliaia(crc_calcolati),(finecrc-iniziocrc)/1000.0,migliaia2(crc_calcolati/((finecrc-iniziocrc+1)/1000.0)));
	}
	return 0;
}





/*
///////////////////////////////////////////
During the verification phase of the correct functioning of the backups 
it is normal to extract them on several different media (devices).

Using for example folders synchronized with rsync on NAS, ZIP files, ZPAQ
via NFS-mounted shares, smbfs, internal HDD etc.

Comparing multiple copies can takes a long time.

Suppose to have a /tank/condivisioni master (or source) directory (hundreds of GB, hundred thousand files)

Suppose to have some internal (HDD) and external (NAS)
rsynced copy (/rsynced-copy-1, /rsynced-copy-2, /rsynced-copy-3...)

Suppose to have  internal ZIP backup, internal ZPAQ backup,
external (NAS1 zip backup),  external (NAS2 zpaq backup) and so on.
Let's extract all of them (ZIP and ZPAQs) into
/temporaneo/1, /temporaneo/2, /temporaneo/3...

You can do something like
diff -qr /temporaneo/condivisioni /temporaneo/1
diff -qr /temporaneo/condivisioni /temporaneo/2
diff -qr /temporaneo/condivisioni /temporaneo/3
(...)
diff -qr /temporaneo/condivisioni /rsynced-copy-1
diff -qr /temporaneo/condivisioni /rsynced-copy-2
diff -qr /temporaneo/condivisioni /rsynced-copy-3
(...)

But this can take a lot of time (many hours) even for fast machines


The command c compares a master folder (the first indicated) 
to N slave folders (all the others) in two particular operating modes.

By default it just checks the correspondence of files and their size 
(for example for rsync copies with different charsets, 
ex unix vs linux, mac vs linux, unix vs ntfs it is extremely useful)

Using the -crc32 switch a check of this code is also made (with HW CPU support, if available).

The interesting aspect is the -all switch: N threads will be created 
(one for each specified folder) and executed in parallel, 
both for scanning and for calculating the CRC.

On modern servers (eg Xeon with 8, 10 or more CPUs) 
with different media (internal) and multiple connections (NICs) to NAS 
you can drastically reduce times compared to multiple, sequential diff -qr. 
It clearly makes no sense for single magnetic disks

In the given example
zpaqfranz c /tank/condivisioni /temporaneo/1 /temporaneo/2 /temporaneo/3 /rsynced-copy-1 /rsynced-copy-2 /rsynced-copy 3 -crc32 -all
will run 7 threads which take care of one directory.

The hypothesis is that the six copies are each on a different device, and the server
have plenty of cores
It's normal in datastorage and virtualization environments
*/

pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;


/*
We need something out of an object (Jidac), addfile() and scandir(), 
because pthread does not like very much
*/
void myaddfile(uint32_t i_tnumber,DTMap& i_edt,string i_filename, int64_t i_date,int64_t i_size, bool i_calccrc32) 
{

	///Raze to the ground ads and zfs as soon as possible
	if (isads(i_filename))
		return;
		
	if (iszfs(i_filename))
		return;
		
	uint32_t risultato=0;
	int64_t dummy;
	
  	if (i_calccrc32)
		if (i_filename[i_filename.size()-1]!='/')
			risultato=crc32_calc_file(i_filename.c_str(),-1,-1,dummy);
	
 
	DT& d=i_edt[i_filename];
	d.date=i_date;
	d.size=i_size;
	d.attr=0;
	d.data=0;
	d.crc32=risultato;
  
///  thread safe, but... who cares? 
	pthread_mutex_lock(&mylock);
	g_bytescanned+=i_size; 
	g_filescanned++;
	
	if (!flagnoeta)
	{
		if (i_calccrc32)
		{
			if (!(g_filescanned % 100))
				printf("Checksumming |%d| %12s (%10s)\r",i_tnumber,migliaia(g_filescanned),tohuman(g_bytescanned));
		}
		else
		{
			if (!(g_filescanned % 1000))
				printf("Scanning |%d| %12s (%10s)\r",i_tnumber,migliaia(g_filescanned),tohuman(g_bytescanned));
		}
		fflush(stdout);
	}
	
	pthread_mutex_unlock(&mylock);

}

void myscandir(uint32_t i_tnumber,DTMap& i_edt,string filename, bool i_recursive,bool i_calccrc32)
{
	///Raze to the ground ads and zfs as soon as possible
	if (isads(filename))
	{
		if (flagverbose)
			printf("Skip :$DATA ----> %s\n",filename.c_str());
		return;
	}
	
	if (iszfs(filename))
	{
		if (flagverbose)
			printf("Skip .zfs ----> %s\n",filename.c_str());
		return;
	}
	
#ifdef unix
// Add regular files and directories
  while (filename.size()>1 && filename[filename.size()-1]=='/')
    filename=filename.substr(0, filename.size()-1);  // remove trailing /
	struct stat sb;
	if (!lstat(filename.c_str(), &sb)) 
	{
		if (S_ISREG(sb.st_mode))
		myaddfile(i_tnumber,i_edt,filename, decimal_time(sb.st_mtime), sb.st_size,i_calccrc32);

    // Traverse directory
		if (S_ISDIR(sb.st_mode)) 
		{
			myaddfile(i_tnumber,i_edt,filename=="/" ? "/" : filename+"/", decimal_time(sb.st_mtime),0, i_calccrc32);
			DIR* dirp=opendir(filename.c_str());
			if (dirp) 
			{
				for (dirent* dp=readdir(dirp); dp; dp=readdir(dirp)) 
				{
					if (strcmp(".", dp->d_name) && strcmp("..", dp->d_name)) 
					{
						string s=filename;
						if (s!="/") s+="/";
						s+=dp->d_name;
						if (i_recursive)        
							myscandir(i_tnumber,i_edt,s,true,i_calccrc32);
						else
						{
							if (!lstat(s.c_str(), &sb)) 
							{
								if (S_ISREG(sb.st_mode))
									myaddfile(i_tnumber,i_edt,s, decimal_time(sb.st_mtime), sb.st_size,i_calccrc32);
								if (S_ISDIR(sb.st_mode)) 
									myaddfile(i_tnumber,i_edt,s=="/" ? "/" :s+"/", decimal_time(sb.st_mtime),0, i_calccrc32);
							}
						}          			
					}
				}
				closedir(dirp);
			}
			else
				perror(filename.c_str());
		}
	}
	else
		perror(filename.c_str());

#else  // Windows: expand wildcards in filename

  // Expand wildcards
	WIN32_FIND_DATA ffd;
	string t=filename;
	if (t.size()>0 && t[t.size()-1]=='/') 
		t+="*";
  
	HANDLE h=FindFirstFile(utow(t.c_str()).c_str(), &ffd);
	if (h==INVALID_HANDLE_VALUE && GetLastError()!=ERROR_FILE_NOT_FOUND && GetLastError()!=ERROR_PATH_NOT_FOUND)
		printerr(t.c_str());
	
	while (h!=INVALID_HANDLE_VALUE) 
	{

    // For each file, get name, date, size, attributes
		SYSTEMTIME st;
		int64_t edate=0;
		if (FileTimeToSystemTime(&ffd.ftLastWriteTime, &st))
			edate=st.wYear*10000000000LL+st.wMonth*100000000LL+st.wDay*1000000
				+st.wHour*10000+st.wMinute*100+st.wSecond;
		const int64_t esize=ffd.nFileSizeLow+(int64_t(ffd.nFileSizeHigh)<<32);
    
    // Ignore links, the names "." and ".." or any unselected file
		t=wtou(ffd.cFileName);
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT || t=="." || t=="..") 
		edate=0;  // don't add
		
		string fn=path(filename)+t;

    // Save directory names with a trailing / and scan their contents
    // Otherwise, save plain files
		if (edate) 
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				fn+="/";
		
			myaddfile(i_tnumber,i_edt,fn, edate, esize, i_calccrc32);
		
			if (i_recursive)
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				{
					fn+="*";
					myscandir(i_tnumber,i_edt,fn,true,i_calccrc32);
				}
		}
		if (!FindNextFile(h, &ffd)) 
		{
			if (GetLastError()!=ERROR_NO_MORE_FILES) 
				printerr(fn.c_str());
			break;
		}
	}
  FindClose(h);
#endif
}


/*
	parameters to run scan threads
*/
struct tparametri
{
	string 		directorytobescanned;
	DTMap		theDT;
	bool		flagcalccrc32;
	uint64_t	timestart;
	uint64_t	timeend;
	int	tnumber;
};

/*
	run a myscandir() instead of Jidac::scandir() (too hard to use a member)
*/
void * scansiona(void *t) 
{
	assert(t);
	tparametri* par= ((struct tparametri*)(t));
	DTMap& tempDTMap = par->theDT;
	myscandir(par->tnumber,tempDTMap,par->directorytobescanned,true,par->flagcalccrc32);
	par->timeend=mtime();
	pthread_exit(NULL);
}


int Jidac::dircompare(bool i_flagonlysize)
{
	int risultato=0;
	
	if (i_flagonlysize)
		printf("Get directory size, ignoring .zfs and :$DATA\n");
	else
	{
		printf("Dir compare (%d dirs to be checked), ignoring .zfs and :$DATA\n",files.size());
		if (files.size()<2)
			error("At least two directories required\n");
	}
	
	for (unsigned i=0; i<files.size(); ++i)
		if (!isdirectory(files[i]))
			files[i]+='/';

	vector<DTMap> files_edt;
  
	files_size.clear();
	files_count.clear();
	files_time.clear();

	int64_t startscan=mtime();
	
	int 	quantifiles		=0;
	int64_t lavorati		=0;
	
	if (flagverbose)
		flagnoeta=true;
	g_bytescanned			=0;
	g_worked=0;
	if (all)	// OK let's make different threads
	{
		vector<tparametri> 	vettoreparametri;
		vector<DTMap>		vettoreDT;
		
		tparametri 	myblock;
		DTMap		mydtmap;
	
		for (int i=0;i<files.size();i++)
		{
			vettoreDT.push_back(mydtmap);
			myblock.tnumber=i;
			myblock.directorytobescanned=files[i];
			myblock.theDT=vettoreDT[i];
			myblock.flagcalccrc32=flagcrc32;
			myblock.timestart=mtime();
			vettoreparametri.push_back(myblock);
		}
	
		int rc;
		pthread_t threads[files.size()];
		pthread_attr_t attr;
		void *status;

		// ini and set thread joinable
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		uint64_t inizioscansione=mtime();
		printf("Creating %d scan threads\n",files.size());
		for(int i = 0; i < files.size(); i++ ) 
		{
			print_datetime();
			printf("Scan dir || <<%s>>\n",files[i].c_str());
			rc = pthread_create(&threads[i], &attr, scansiona, (void*)&vettoreparametri[i]);
			if (rc) 
			{
				printf("Error creating thread\n");
				exit(-1);
			}
		}

		// free attribute and wait for the other threads
		pthread_attr_destroy(&attr);
		for(int i = 0; i < files.size(); i++ ) 
		{
			rc = pthread_join(threads[i], &status);
			if (rc) 
			{
				error("Unable to join\n");
				exit(-1);
			}
			///printf("Thread completed %d status %d\n",i,status);
		}
		printf("\nParallel scan ended in %f\n",(mtime()-startscan)/1000.0);
	
		for (unsigned i=0; i<files.size(); ++i)
		{
			uint64_t sizeofdir=0;
			uint64_t dircount=0;
			for (DTMap::iterator p=vettoreparametri[i].theDT.begin(); p!=vettoreparametri[i].theDT.end(); ++p) 
			{
				string filename=rename(p->first);
				if (p->second.date && p->first!="" && (!isdirectory(p->first)) && (!isads(filename.c_str()))) 
				{
					sizeofdir+=p->second.size;
					dircount++;
					quantifiles++;
				}
				if (flagverbose)
				{
					printUTF8(filename.c_str());
					printf("\n");
				}
			}
			files_edt.push_back(vettoreparametri[i].theDT);
			files_size.push_back(sizeofdir);
			files_count.push_back(dircount);
			files_time.push_back(vettoreparametri[i].timeend-vettoreparametri[i].timestart+1);
		}
	}
	else
	{	// single thread. Do a sequential scan
		for (unsigned i=0; i<files.size(); ++i)
		{
			DTMap myblock;
		
			print_datetime();
			printf("Scan dir <<%s>>\n",files[i].c_str());
			uint64_t blockstart=mtime();
			myscandir(i,myblock,files[i].c_str(),true,flagcrc32);
			
			uint64_t sizeofdir=0;
			uint64_t dircount=0;
			for (DTMap::iterator p=myblock.begin(); p!=myblock.end(); ++p) 
			{
				string filename=rename(p->first);
				if (p->second.date && p->first!="" && (!isdirectory(p->first)) && (!isads(filename))) 
				{
					sizeofdir+=p->second.size;
					dircount++;
					quantifiles++;
				}
				if (flagverbose)
				{
					printUTF8(filename.c_str());
					printf("\n");
				}
			}
			files_edt.push_back(myblock);
			files_size.push_back(sizeofdir);
			files_count.push_back(dircount);
			files_time.push_back(mtime()-blockstart);
		}
	}
	
/// return free space (USEFUL for crontabbed-emalied backups)
	for (unsigned i=0; i<files.size(); ++i)
	{
		uint64_t spazio=getfreespace(files[i].c_str());
		printf("Free %d  %24s   %12s    <<%s>>\n",i,migliaia(spazio),tohuman(spazio),files[i].c_str());
	}

	uint64_t total_size=0;
	uint64_t total_files=0;
	printf("\n=============================================\n");
	for (unsigned int i=0;i<files_size.size();i++)
	{
		printf("Dir %d   %24s %12s %6g <<%s>>\n",i,migliaia(files_size[i]),migliaia2(files_count[i]),files_time[i]/1000.0,files[i].c_str());
		total_size+=files_size[i];
		total_files+=files_count[i];
	}
	printf("=============================================\n");
	printf("        %24s %12s %6g sec (%s)\n",migliaia(total_size),migliaia2(total_files),(mtime()-startscan)/1000.0,tohuman(total_size));
	
	if (i_flagonlysize)
		return 0;

///// Now check the folders (sequentially)

	printf("\nDir 0 (master) time %6g <<%s>>\n",files_time[0]/1000.0,files[0].c_str());
	printf("size  %24s (files %s)\n",migliaia(files_size[0]),migliaia2(files_count[0]));
	printf("-------------------------\n");

	for (unsigned i=1; i<files.size(); ++i)
	{
		if (!exists(files[i]))
		{
			printf("Dir %d (slave) DOES NOT EXISTS %s\n",i,files[i].c_str());
			risultato=1;
		}
		else
		{
			bool flagerror=false;

			if ((files_size[i]!=files_size[0]) || (files_count[i]!=files_count[0]))
			{
				printf("Dir %d (slave) IS DIFFERENT time %6g <<%s>>\n",i,files_time[i]/1000.0,files[i].c_str());
				printf("size  %24s (files %s)\n",migliaia(files_size[i]),migliaia2(files_count[i]));
			}
				
			for (DTMap::iterator p=files_edt[0].begin(); p!=files_edt[0].end(); ++p) 
			{
				string filename0=p->first;
				string filenamei=filename0;
				myreplace(filenamei,files[0],files[i]);
				DTMap::iterator cerca=files_edt[i].find(filenamei);
				if  (cerca==files_edt[i].end())
				{
					printf("missing (not in %d) ",i);
					printUTF8(filename0.c_str());
					printf("\n");
					flagerror=true;
				}
			}
			for (DTMap::iterator p=files_edt[i].begin(); p!=files_edt[i].end(); ++p) 
			{
				string filenamei=p->first;
				string filename0=filenamei;
				myreplace(filename0,files[i],files[0]);
				DTMap::iterator cerca=files_edt[0].find(filename0);
				if  (cerca==files_edt[0].end())
				{
					printf("excess  (not in 0) ");
					printUTF8(filename0.c_str());
					printf("\n");
					flagerror=true;
				}
			}	
			for (DTMap::iterator p=files_edt[0].begin(); p!=files_edt[0].end(); ++p) 
			{
				string filename0=p->first;
				string filenamei=filename0;
				myreplace(filenamei,files[0],files[i]);
				DTMap::iterator cerca=files_edt[i].find(filenamei);
				if  (cerca!=files_edt[i].end())
				{
					if (p->second.size!=cerca->second.size)
					{
						printf("diff size  %18s (0) %18s (%d) %s\n",migliaia(p->second.size),migliaia2(cerca->second.size),i,filename0.c_str());
						flagerror=true;
					}
					else
					if (flagcrc32)
					{
						if (p->second.crc32!=cerca->second.crc32)
						{
							printf("CRC-32 different %08X (0) vs %08X (%d) %s\n",p->second.crc32,cerca->second.crc32,i,filename0.c_str());
							flagerror=true;
						}
					}
				}
			}
			if (!flagerror)
				if ((files_size[i]==files_size[0]) && (files_count[i]==files_count[0]))
					printf("= %s scantime %f \n",files[i].c_str(),files_time[i]/1000.0);
			printf("-------------------------\n");
		}
	}	
	return risultato;
}

