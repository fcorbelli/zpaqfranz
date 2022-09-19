/*
zsfx
Windows 32/64 self extracting for ZPAQ archives,
with multithread support and everything, just like zpaq 7.15
Embedded in zpaqfranz for Windows

Written by Franco Corbelli
https://github.com/fcorbelli

Lots of improvements to do

Targets
Windows 64 (g++ 7.3.0)
g++ -O3 zsfx.cpp libzpaq.cpp -o zsfx -static

(more aggressive) 10.3.0 beware of -flto
g++ -Os zsfx.cpp libzpaq.cpp -o zsfx -static -fno-rtti -Wl,--gc-sections -fdata-sections -flto

Windows 32 (g++ 7.3.0)
c:\mingw32\bin\g++ -m32 -O3  zsfx.cpp libzpaq.cpp -o zsfx32 -pthread -static

(more aggressive) 10.3.0 -flto
c:\mingw32\bin\g++ -m32 -Os  zsfx.cpp libzpaq.cpp -o zsfx32 -pthread -static -fno-rtti  -Wl,--gc-sections -flto


*/

#define ZSFX_VERSION "55.1"
#define _FILE_OFFSET_BITS 64  // In Linux make sizeof(off_t) == 8
#define UNICODE  // For Windows
#include "libzpaq.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <io.h>
#include <conio.h>

using std::string;
using std::vector;
using std::map;
using libzpaq::StringBuffer;
// Global variables

string 	g_franzo_start;
string 	g_franzo_end;
int64_t g_global_start=0;  // set to mtime() at start of main()
int64_t g_startzpaq=0; 	// where the data begin

#define DEBUGX		// define to see a lot of things

string extractfilename(const string& i_string) 
{
	size_t i = i_string.rfind('/', i_string.length());
	if (i != string::npos) 
		return(i_string.substr(i+1, i_string.length() - i));
	return(i_string);
}

string extractfilepath(const string& i_string) 
{
	size_t i = i_string.rfind('/', i_string.length());
	if (i != string::npos) 
		return(i_string.substr(0, i+1));
	return("");
}
string prendinomefileebasta(const string& s) 
{
	string nomefile=extractfilename(s);
	size_t i = nomefile.rfind('.', nomefile.length());
	if (i != string::npos) 
		return(nomefile.substr(0,i));
	return(nomefile);
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

// Handle errors in libzpaq and elsewhere
void libzpaq::error(const char* msg) 
{
	if (strstr(msg, "ut of memory")) 
		throw std::bad_alloc();
	printf("%s\n",msg);
	exit(0);
}
using libzpaq::error;

typedef DWORD ThreadReturn;
typedef HANDLE ThreadID;
void run(ThreadID& tid, ThreadReturn(*f)(void*), void* arg) 
{
	tid=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f, arg, 0, NULL);
	if (tid==NULL) 
		error("CreateThread failed");
}
void join(ThreadID& tid) 
{
		WaitForSingleObject(tid, INFINITE);
}

typedef HANDLE Mutex;
void init_mutex(Mutex& m) 
{
	m=CreateMutex(NULL, FALSE, NULL);
}

void lock(Mutex& m) 
{
	WaitForSingleObject(m, INFINITE);
}
void release(Mutex& m) 
{
		ReleaseMutex(m);
}
void destroy_mutex(Mutex& m) 
{
	CloseHandle(m);
}

// In Windows, convert 16-bit wide string to UTF-8 and \ to /
string wtou(const wchar_t* s) 
{
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
std::wstring utow(const char* ss, char slash='\\') 
{
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

bool fileexists(const string& i_filename) 
{
	HANDLE	myhandle;
	WIN32_FIND_DATA findfiledata;
	std::wstring wpattern=utow(i_filename.c_str());
	myhandle=FindFirstFile(wpattern.c_str(),&findfiledata);
	if (myhandle!=INVALID_HANDLE_VALUE)
	{
		FindClose(myhandle);
		return true;
	}
	return false;
}

// Print a UTF-8 string to f (stdout, stderr) so it displays properly
void printUTF8(const char* s, FILE* f=stdout) 
{
  assert(f);
  assert(s);
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
}

// Return relative time in milliseconds
int64_t mtime() 
{
  int64_t t=GetTickCount();
  if (t<g_global_start) 
	t+=0x100000000LL;
  return t;
}


/////////////////////////////// File //////////////////////////////////

typedef HANDLE FP;
const FP FPNULL=INVALID_HANDLE_VALUE;
typedef enum {RB, WB, RBPLUS, WBPLUS} MODE;  // fopen modes

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
int fclose(FP fp) 
{
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

// Return true if a file or directory (UTF-8 without trailing /) exists.
bool exists(string filename) 
{
  int len=filename.size();
  if (len<1) return false;
  if (filename[len-1]=='/') filename=filename.substr(0, len-1);
  return GetFileAttributes(utow(filename.c_str()).c_str())
         !=INVALID_FILE_ATTRIBUTES;
}


// Print last error message
void printerr(const char* filename) 
{
  fflush(stdout);
  int err=GetLastError();
  printUTF8(filename, stderr);
  if (err==ERROR_FILE_NOT_FOUND)
    fprintf(stderr, ": file not found\n");
  else if (err==ERROR_PATH_NOT_FOUND)
    fprintf(stderr, ": path not found\n");
  else if (err==ERROR_ACCESS_DENIED)
    fprintf(stderr, ": access denied\n");
  else if (err==ERROR_SHARING_VIOLATION)
    fprintf(stderr, ": sharing violation\n");
  else if (err==ERROR_BAD_PATHNAME)
    fprintf(stderr, ": bad pathname\n");
  else if (err==ERROR_INVALID_NAME)
    fprintf(stderr, ": invalid name\n");
  else if (err==ERROR_NETNAME_DELETED)
    fprintf(stderr, ": network name no longer available\n");
  else
    fprintf(stderr, ": Windows error %d\n", err);
}


// Close fp if open. Set date and attributes unless 0
void close(const char* filename, int64_t date, int64_t attr, FP fp=FPNULL) {
  assert(filename);
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
}

// Print file open error and throw exception
void ioerr(const char* msg) 
{
  printerr(msg);
  exit(0);
}

// Create directories as needed. For example if path="/tmp/foo/bar"
// then create directories /, /tmp, and /tmp/foo unless they exist.
// Set date and attributes if not 0.
void makepath(string path, int64_t date=0, int64_t attr=0) {
  for (unsigned i=0; i<path.size(); ++i) {
    if (path[i]=='\\' || path[i]=='/') {
      path[i]=0;
      CreateDirectory(utow(path.c_str()).c_str(), 0);
      path[i]='/';
    }
  }

  // Set date and attributes
  string filename=path;
  if (filename!="" && filename[filename.size()-1]=='/')
    filename=filename.substr(0, filename.size()-1);  // remove trailing slash
  close(filename.c_str(), date, attr);
}


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
#if defined(DEBUG)
  int64_t tello()
  {
	return ftello(fp);
  
 }
#endif
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
  if (sz.size()==1) 
  {
    fseeko(fp, off+g_startzpaq, SEEK_SET);
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
  fseeko(fp, off-sum+g_startzpaq, SEEK_END);
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
  seek(0,SEEK_SET);  /// go to right position

  // Get encryption salt
  if (password) 
  {
#if defined(DEBUG)
	printf("Prendo il sale\n");
	printf("Z1 tello                    %ld\n",ftello(fp));
#endif

    char salt[32], key[32];
    if (fread(salt, 1, 32, fp)!=32) error("cannot read salt");
#if defined(DEBUG)
	printf("Sale ");
	for (int i=0;i<32;i++)
		printf("%02X",(unsigned char)salt[i]);
	printf("\n");
#endif

    libzpaq::stretchKey(key, password, salt);
    aes=new libzpaq::AES_CTR(key, 32, salt);
	off=32;
#if defined(DEBUG)
	printf("Off vale ora %d\n",off);
#endif
  }
}


///////////////////////// System info /////////////////////////////////

// Guess number of cores. In 32 bit mode, max is 2.
int numberOfProcessors() {
  int rc=0;  // result
  // In Windows return %NUMBER_OF_PROCESSORS%
  const char* p=getenv("NUMBER_OF_PROCESSORS");
  if (p) rc=atoi(p);
  if (rc<1) rc=1;
  if (sizeof(char*)==4 && rc>2) rc=2;
  return rc;
}

////////////////////////////// misc ///////////////////////////////////

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
  if (c>='A' && c<='Z') return c-'A'+'a';
  return c;
}

// Return true if strings a == b or a+"/" is a prefix of b
// or a ends in "/" and is a prefix of b.
// Match ? in a to any char in b.
// Match * in a to any string in b.
// In Windows, not case sensitive.
bool ispath(const char* a, const char* b) {
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
static const int64_t DEFAULT_VERSION=99999999999999LL; // unless -until

// fragment hash table entry
struct HT {
  unsigned char sha1[20];  // fragment hash
  int usize;      // uncompressed size, -1 if unknown, -2 if not init
  HT(const char* s=0, int u=-2) {
    if (s) memcpy(sha1, s, 20);
    else memset(sha1, 0, 20);
    usize=u;
  }
};

// filename entry
struct DT {
  int64_t date;          // decimal YYYYMMDDHHMMSS (UT) or 0 if deleted
  int64_t size;          // size or -1 if unknown
  int64_t attr;          // first 8 attribute bytes
  int64_t data;          // sort key or frags written. -1 = do not write
  vector<unsigned> ptr;  // fragment list
  DT(): date(0), size(0), attr(0), data(0) {}
};
typedef map<string, DT> DTMap;

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
  VER() {memset(this, 0, sizeof(*this));}
};

// Windows API functions not in Windows XP to be dynamically loaded
typedef HANDLE (WINAPI* FindFirstStreamW_t)
                   (LPCWSTR, STREAM_INFO_LEVELS, LPVOID, DWORD);
FindFirstStreamW_t findFirstStreamW=0;
typedef BOOL (WINAPI* FindNextStreamW_t)(HANDLE, LPVOID);
FindNextStreamW_t findNextStreamW=0;


/// we want the fullpath too!
string getmyname()
{
	wchar_t buffer[MAX_PATH]; 
	GetModuleFileName(NULL,buffer,MAX_PATH);
	return (wtou(buffer));
}


// Do everything
class Jidac 
{
public:
	int doCommand(int argc,  char** argv);
	friend ThreadReturn decompressThread(void* arg);
	friend ThreadReturn testThread(void* arg);
	friend struct ExtractJob;
	string archive;           // archive name
private:
	string exename;
	string myname;
	string myoutput;
  // Command line arguments
	char command;             // command 'a', 'x', or 'l'
	vector<string> files;     // filename args
	int all;                  // -all option
	bool force;               // -force option
	
	char password_string[32]; // hash of -key argument
	const char* password;     // points to password_string or NULL
	bool noattributes;        // -noattributes option
	vector<string> notfiles;  // list of prefixes to exclude
	string nottype;           // -not =...
	vector<string> onlyfiles; // list of prefixes to include
	int summary;              // summary option if > 0, detailed if -1
	int threads;              // default is number of cores
	vector<string> tofiles;   // -to option
	int64_t date;             // now as decimal YYYYMMDDHHMMSS (UT)
	int64_t version;          // version number or 14 digit date

  // Archive state
	int64_t dhsize;           // total size of D blocks according to H blocks
	int64_t dcsize;           // total size of D blocks according to C blocks
	vector<HT> ht;            // list of fragments
	DTMap dt;                 // set of files in archive
	vector<Block> block;      // list of data blocks to extract
	vector<VER> ver;          // version info

  // Commands
	int extract();            // extract, return 1 if error else 0
	int sfx(string i_thecommands);				// make a sfx
	void usage();             // help

	string findcommand(int64_t& o_offset);
	void getpasswordifempty();

  // Support functions
	string rename(string name);           // rename from -to
	int64_t read_archive(const char* arc, int *errors=0);  // read arc
	bool isselected(const char* filename, bool rn=false);// files, -only, -not
	bool equal(DTMap::const_iterator p, const char* filename);
 };


/// no buffer overflow protection etc etc

string getline(string i_default) 
{
    size_t maxline 	= 255+1;
    char* line 		= (char*)malloc(maxline); // sfx module, keep it small
	char* p_line 	= line;
	int c;

    if (line==NULL)
		error("744: allocating memory");
	
	if (i_default!="")
		printf("%s",i_default.c_str());

    while((unsigned)(p_line-line)<maxline) 
	{
        c = fgetc(stdin);
		
        if(c==EOF)
            break;

        if((*p_line++ = c)=='\n')
            break;
    }
    *--p_line = '\0';
    return line;
}


// Print help message
void Jidac::usage() 
{
	printf("\nGitHub https://github.com/fcorbelli/zsfx\n\n");

	printf("Usage 1: %s x nameofzpaqfile (...zpaq extraction switches) (-output mynewsfx.exe)\n",exename.c_str());
	printf("Ex    1: %s x z:\\1.zpaq    [z:\\1.exe will ask for -to from console] \n",exename.c_str());
	printf("Ex    1: %s x z:\\1.zpaq -to .\\xtracted\\\n",exename.c_str());
	printf("Ex    1: %s x z:\\1.zpaq -to y:\\testme\\ -output z:\\mynewsfx.exe",exename.c_str());
	printf("\n\n");
	printf("Usage 2: Autoextract .zpaq with the same name of the .exe (if any)\n");
	printf("Ex    2: c:\\pluto\\testme.exe (a copy of %s.exe) will autoextract c:\\pluto\\testme.zpaq\n",exename.c_str());
	printf("\nFilenames zsfx.exe/zsfx32.exe are reserved, do NOT rename\n");
	printf("Switches -all -force -noattributes -not -only -to -summary -threads -until\n");

	exit(1);
}

// return a/b such that there is exactly one "/" in between, and
// in Windows, any drive letter in b the : is removed and there
// is a "/" after.
string append_path(string a, string b) {
  int na=a.size();
  int nb=b.size();
  if (nb>1 && b[1]==':') {  // remove : from drive letter
    if (nb>2 && b[2]!='/') b[1]='/';
    else b=b[0]+b.substr(2), --nb;
  }
  if (nb>0 && b[0]=='/') b=b.substr(1);
  if (na>0 && a[na-1]=='/') a=a.substr(0, na-1);
  return a+"/"+b;
}

// Rename name using tofiles[]
string Jidac::rename(string name) {
  if (files.size()==0 && tofiles.size()>0)  // append prefix tofiles[0]
    name=append_path(tofiles[0], name);
  else {  // replace prefix files[i] with tofiles[i]
    const int n=name.size();
    for (unsigned i=0; i<files.size() && i<tofiles.size(); ++i) {
      const int fn=files[i].size();
      if (fn<=n && files[i]==name.substr(0, fn))
        return tofiles[i]+name.substr(fn);
    }
  }
  return name;
}

void scambia(string& i_str)
{
    int n=i_str.length();
    for (int i=0;i<n/2;i++)
        std::swap(i_str[i],i_str[n-i-1]);
}

bool myreplace(string& i_str, const string& i_from, const string& i_to) 
{
    size_t start_pos = i_str.find(i_from);
    if(start_pos == std::string::npos)
        return false;
    i_str.replace(start_pos, i_from.length(), i_to);
    return true;
}
string getpassword()
{
	string myresult="";
	printf("\nEnter password :");
	char carattere;
	while (1)
	{
		carattere=getch();
		if(carattere=='\r')
			break;
		printf("*");
		myresult+=carattere;
	}
	printf("\n");
	return myresult;

}

FILE* freadopen(const char* i_filename)
{
	std::wstring widename=utow(i_filename);
	FILE* myfp=_wfopen(widename.c_str(), L"rb" );
	if (myfp==NULL)
	{
		printf( "\nfreadopen cannot open:");
		printUTF8(i_filename);
		printf("\n");
		return 0;
	}
	return myfp;
}

bool check_if_password(string i_filename)
{
	FILE* inFile = freadopen(i_filename.c_str());
	if (inFile==NULL) 
	{
		int err=GetLastError();
		printf("\n2057: ERR <%s> kind %d\n",i_filename.c_str(),err); 
		exit(0);
	}
    char s[4]={0};
	if (g_startzpaq)
		fseek(inFile,g_startzpaq,SEEK_SET);
    const int nr=fread(s,1,4,inFile);
///	for (int i=0;i<4;i++)
///		printf("%d  %c  %d\n",i,s[i],s[i]);
	fclose(inFile);
    if (nr>0 && memcmp(s, "7kSt", 4) && (memcmp(s, "zPQ", 3) || s[3]<1))
		return true;
	return false;
}


void explode(string i_string,char i_delimiter,vector<string>& array)
{
	///printf("1\n");
//	printf("Delimiter %c\n",i_delimiter);
//	printf("2\n");
	//printf("String %s\n",i_string.c_str());
	unsigned int i=0;
	while(i<i_string.size())
	{
		string temp="";
///		printf("entro %02d %c %d\n",i,i_string[i],(int)(i_string[i]!=i_delimiter));
		while ((i_string[i]!=i_delimiter) && (i<i_string.size()))
        {
			temp+=i_string[i];
			i++;
		}
		array.push_back(temp);
		i++;
		if (i>=i_string.size())
			break;
    }
}
void Jidac::getpasswordifempty()
{
	if (password==NULL)
		if (check_if_password(archive))
		{
			printf("Archive seems encrypted (or corrupted)");
			string spassword=getpassword();
			if (spassword!="")
			{
				libzpaq::SHA256 sha256;
				for (unsigned int i=0;i<spassword.size();i++)
					sha256.put(spassword[i]);
				memcpy(password_string, sha256.result(), 32);
				password=password_string;
			}
		}
}	

// Parse the command line. Return 1 if error else 0.
int Jidac::doCommand(int argc,  char** argv) 
{
	/// Random 40-bytes long tags. Note: inverting is mandatory
	g_franzo_start="dnE3pipUzeiUo8BMxVKlQTIfLjmskQbhlqBobVVr";
	scambia(g_franzo_start);
	
	g_franzo_end="xzUpA6PsHSE0N5Xe4ctJ2Gz7QTNLDyOXAp4kiEOo";
	scambia(g_franzo_end);
	
	
  // Initialize options to default values
	command=0;
	force=false;
	all=0;
	password=0;  // no password
	noattributes=false;
	summary=0; // detailed: -1
	threads=0; // 0 = auto-detect
	version=DEFAULT_VERSION;
	date=0;

	myoutput="";

#if defined(_WIN64)
	exename="zsfx";
#else
	exename="zsfx32";
#endif

	
	
	/// note: argv[0] does not have path.
	myname=argv[0];
	myname=extractfilename(myname);
	if (!strstr(myname.c_str(), ".exe"))
		myname+=".exe";
	
	///printf("myname is |%s|\n",myname.c_str());
		
	
#if defined(_WIN64)
	printf("zsfx(franz) v" ZSFX_VERSION " by Franco Corbelli - compiled " __DATE__ "\n");
#else
	printf("zsfx32(franz) v" ZSFX_VERSION " by Franco Corbelli - compiled " __DATE__ "\n");
#endif

	
	g_startzpaq=0;
	
	bool 	autoextract=(archive!="");
	bool	flagbuilder=(myname=="zsfx.exe")||(myname=="zsfx32.exe");

	libzpaq::Array<char*> argp(100);
		
	if (!autoextract)
	if (!flagbuilder)
	{
		string comandi=findcommand(g_startzpaq);
		printf("Extracting from EXE with parameters: x %s\n",comandi.c_str());
		//printf("Extracting from EXE\n");
		///printf("Start   %ld\n",startzpaq);
		if (g_startzpaq==0)
		{
			printf("860:Something is WRONG\n");
			exit(0);
		}
/*		
		printf("ARGC prima vale %d\n",argc);
		for (int i=0;i<argc;i++)
		{
			printf("XXXXX %d %s\n",i,argv[i]);
		}
		*/
		vector<string> array_command;
		explode(comandi,' ',array_command);
		argc=array_command.size();
		
#if defined(DEBUG)
		for (unsigned int i=0;i<array_command.size();i++)
			printf("i  %d  %s\n",i,array_command[i].c_str());
#endif

		for (int i=0; i<argc; ++i) 
			argp[i]=(char*)array_command[i].c_str();

#if defined(DEBUG)			
		for (int i=0; i<argc; ++i) 
			printf("|||%s|||\n",argp[i]);
#endif

		argv=&argp[0];
		
	}
#if defined(DEBUG)
	printf("K1\n");
#endif

  // Init archive state
  ht.resize(1);  // element 0 not used
  ver.resize(1); // version 0
  dhsize=dcsize=0;

  // Get date
  time_t now=time(NULL);
  tm* t=gmtime(&now);
  date=(t->tm_year+1900)*10000000000LL+(t->tm_mon+1)*100000000LL
      +t->tm_mday*1000000+t->tm_hour*10000+t->tm_min*100+t->tm_sec;


	string thecommands="";
	
	/// compile a string from all argv (to be stored)
	if (!autoextract)
	if (flagbuilder)
		for (int i=1; i<argc; i++) 
		{
			thecommands+=argv[i];
			if (i<argc-1)
				thecommands+=" ";
		}	
#if defined(DEBUG)
	printf("K2\n");
	printf("Argc vale %d\n",argc);
	for (int i=0; i<argc;i++)
	{
		printf("$$$$%s$$$$\n",argv[i]);
	}
#endif

	for (int i=1; i<argc; ++i) 
	{
		const string opt=argv[i];  // read command
#if defined(DEBUG)
		printf("argc vale %d  i %d  %s\n",argc,i,opt.c_str());
#endif
		if ((!autoextract) && ((opt=="sfx" || opt=="x")
        && i<argc-1 && argv[i+1][0]!='-' && command==0)) 
		{
			command=opt[0];
			archive=argv[++i];  // append ".zpaq" to archive if no extension
			const char* slash=strrchr(argv[i], '/');
			const char* dot=strrchr(slash ? slash : argv[i], '.');
			if (!dot && archive!="") archive+=".zpaq";
			
			///archive='"'+archive+'"';
			
			
			while (++i<argc && argv[i][0]!='-')  // read filename args
				files.push_back(argv[i]);
			--i;
    }
    else if (opt.size()<2 || opt[0]!='-') usage();
    else if (opt=="-all") {
      all=4;
      if (i<argc-1 && isdigit(argv[i+1][0])) all=atoi(argv[++i]);
    }
    else if (opt=="-force" || opt=="-f") force=true;
	
	else if (opt=="-key") 
		{
			if (i<argc-1) // I am not the last parameter
				if (argv[i+1][0]!='-') // -key  -whirlpool
				{
					printf("La decodifico\n");
					libzpaq::SHA256 sha256;
					for (const char* p=argv[++i]; *p; ++p)
						sha256.put(*p);
					memcpy(password_string, sha256.result(), 32);
					password=password_string;
				}
			if (password==NULL)
			{
				string spassword=getpassword();
				///printf("entered <<%s>>\n",spassword.c_str());
				if (spassword!="")
				{
					libzpaq::SHA256 sha256;
					for (unsigned int i=0;i<spassword.size();i++)
						sha256.put(spassword[i]);
					memcpy(password_string, sha256.result(), 32);
					password=password_string;
				}
				
			}
		}
    else if (opt=="-noattributes") noattributes=true;
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
    else if (opt=="-summary" && i<argc-1) summary=atoi(argv[++i]);
    else if (opt[1]=='s') summary=atoi(argv[i]+2);
    else if (opt=="-to") {  // read tofiles
      while (++i<argc && argv[i][0]!='-')
        tofiles.push_back(argv[i]);
      if (tofiles.size()==0) tofiles.push_back("");
      --i;
    }
    else if (opt=="-threads" && i<argc-1) threads=atoi(argv[++i]);
    else if (opt[1]=='t') threads=atoi(argv[i]+2);
	else if (opt=="-output")
		{
			if (myoutput=="")
			{
				if (++i<argc && argv[i][0]!='-')  
					myoutput=argv[i];
				else
					i--;
			}
		}

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
          exit(1);
        }
        date=version;
      }
    }
    else {
      printf("Unknown option ignored: %s\n", argv[i]);
      ///usage();
    }
  }


  // Set threads
  if (threads<1) threads=numberOfProcessors();

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


  // Execute command
	else 
	if (autoextract)
	{
		command='x';
		getpasswordifempty();
		return extract();
	
	}
	if (g_startzpaq>0)
	{
		/// Houston, the magic is into startzpaq
		archive=myname;
		getpasswordifempty();
		return extract();
	}
	else
	if (command=='x')
	{	
		string rimpiazza="dummy.zpaq";
		myreplace(thecommands,archive,rimpiazza);
		rimpiazza="dummy.exe";
		myreplace(thecommands,myoutput,rimpiazza);
		return sfx(thecommands);
	}
	else 
		usage();
	return 0;
}


/////////////////////////// read_archive //////////////////////////////

// Read arc up to -date into ht, dt, ver. Return place to
// append. If errors is not NULL then set it to number of errors found.
int64_t Jidac::read_archive(const char* arc, int *errors) {
  if (errors) *errors=0;
  dcsize=dhsize=0;
  assert(ver.size()==1);
  unsigned files=0;  // count

  // Open archive
  InputArchive in(arc, password);
  if (!in.isopen()) {
    if (command!='a') {
      fflush(stdout);
      printUTF8(arc, stderr);
      fprintf(stderr, " not found.\n");
      if (errors) ++*errors;
    }
    return 0;
  }
  printUTF8(arc);
  if (version==DEFAULT_VERSION) printf(": ");
  else printf(" -until %1.0f: ", version+0.0);
  fflush(stdout);


	in.seek(0,SEEK_SET);	// note: the magic is here
	printf("\n");
	
	if (password)
	{
#if defined(DEBUG)
		printf("HOUston abbiamo una password!\n");
#endif
		in.seek(32,SEEK_SET);
	}
#if defined(DEBUG)
	printf("K0 Inizio a decodificare da %ld\n",in.tell());
	printf("K0 tello                    %ld\n",in.tello());
#endif
	
  // Test password
  
    char s[4]={0};
    int nr=in.read(s, 4);
#if defined(DEBUG)
	printf("\nPrendo 4 byte firma\n");

	for (int i=0;i<4;i++)
		printf("%d %02X %c\n",i,(unsigned char)s[i],s[i]);
#endif
    if (nr>0 && memcmp(s, "7kSt", 4) && (memcmp(s, "zPQ", 3) || s[3]<1))
		error("Password kaputt");
	in.seek(-nr, SEEK_CUR);
#if defined(DEBUG)
	printf("K2 Inizio a decodificare da %ld\n",in.tell());
	printf("K2 tello                    %ld\n",in.tello());
#endif

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

  // Detect archive format and read the filenames, fragment sizes,
  // and hashes. In JIDAC format, these are in the index blocks, allowing
  // data to be skipped. Otherwise the whole archive is scanned to get
  // this information from the segment headers and trailers.
  
#if defined(DEBUG)  
  printf("K3 Inizio a decodificare da %ld\n",in.tell());
  printf("K3 tello                    %ld\n",in.tello());
#endif
  bool done=false;
  while (!done) {
    libzpaq::Decompresser d;
    try {
      d.setInput(&in);
      double mem=0;
      while (d.findBlock(&mem)) {
#if defined(DEBUG)
		  printf("Trovato un blocco\n");
#endif
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
                  string fn=itos(ver.size()-1, all)+"/";
                  if (renamed) fn=rename(fn);
                  if (isselected(fn.c_str(), false))
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
            else if (filename.s[17]=='i') {
              assert(ver.size()>0);
              if (fdate>ver.back().lastdate) ver.back().lastdate=fdate;
              const char* s=os.c_str();
              const char* const end=s+os.size();
              while (s+9<=end) {
                DT dtr;
                dtr.date=btol(s);  // date
                if (dtr.date) ++ver.back().updates;
                else ++ver.back().deletes;
                const int64_t len=strlen(s);
                if (len>65535) error("filename too long");
                string fn=s;  // filename renamed
                if (all) fn=append_path(itos(ver.size()-1, all), fn);
                const bool issel=isselected(fn.c_str(), renamed);
                s+=len+1;  // skip filename
                if (s>end) error("filename too long");
                if (dtr.date) {
                  ++files;
                  if (s+4>end) error("missing attr");
                  unsigned na=btoi(s);  // attr bytes
                  if (s+na>end || na>65535) error("attr too long");
                  for (unsigned i=0; i<na; ++i, ++s)  // read attr
                    if (i<8) dtr.attr+=int64_t(*s&255)<<(i*8);
                  if (noattributes) dtr.attr=0;
                  if (s+4>end) error("missing ptr");
                  unsigned ni=btoi(s);  // ptr list size
                  if (ni>(end-s)/4u) error("ptr list too long");
                  if (issel) dtr.ptr.resize(ni);
                  for (unsigned i=0; i<ni; ++i) {  // read ptr
                    const unsigned j=btoi(s);
                    if (issel) dtr.ptr[i]=j;
                  }
                }
                if (issel) dt[fn]=dtr;
              }  // end while more files
            }  // end if 'i'
            else {
              printf("Skipping %s %s\n",
                  filename.s.c_str(), comment.s.c_str());
              error("Unexpected journaling block");
            }
          }  // end if journaling

          // Streaming format
          else {

            // If previous version does not exist, start a new one
            if (ver.size()==1) {
              if (version<1) {
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
            if (all) fn=append_path(itos(ver.size()-1, all), fn);
            if (isselected(fn.c_str(), renamed)) 
			{
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
      if (errors) ++*errors;
    }
endblock:;
  }  // end while !done
#if defined(DEBUG)
  if (!found_data)
  	printf("no found data\n");
 
#endif
  if (in.tell()>32*(password!=0) && !found_data)
    error("archive contains no data");
  printf("%d versions, %u files, %s bytes\n", 
      int(ver.size()-1), files,
      migliaia(block_offset));

  // Calculate file sizes
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) {
    for (unsigned i=0; i<p->second.ptr.size(); ++i) {
      unsigned j=p->second.ptr[i];
      if (j>0 && j<ht.size() && p->second.size>=0) {
        if (ht[j].usize>=0) p->second.size+=ht[j].usize;
        else p->second.size=-1;  // unknown size
      }
    }
  }
  return block_offset;
}



//////////////////////////////// add //////////////////////////////////

// Append n bytes of x to sb in LSB order
inline void puti(libzpaq::StringBuffer& sb, uint64_t x, int n) {
  for (; n>0; --n) sb.put(x&255), x>>=8;
}

void print_progress(int64_t ts, int64_t td) 
{
  if (td>ts) td=ts;
  if (td>=1000000) 
	printf("%5.2f%%\r", td*100.0/(ts+0.5));
}



/////////////////////////////// extract ///////////////////////////////

// Return true if the internal file p
// and external file contents are equal or neither exists.
// If filename is 0 then return true if it is possible to compare.
bool Jidac::equal(DTMap::const_iterator p, const char* filename) {

  // test if all fragment sizes and hashes exist
  if (filename==0) {
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

  // compare hashes
  fseeko(in, 0, SEEK_SET);
  libzpaq::SHA1 sha1;
  const int BUFSIZE=4096;
  char buf[BUFSIZE];
  for (unsigned i=0; i<p->second.ptr.size(); ++i) {
    unsigned f=p->second.ptr[i];
    if (f<1 || f>=ht.size() || ht[f].usize<0) return fclose(in), false;
    for (int j=0; j<ht[f].usize;) {
      int n=ht[f].usize-j;
      if (n>BUFSIZE) n=BUFSIZE;
      int r=fread(buf, 1, n, in);
      if (r!=n) return fclose(in), false;
      sha1.write(buf, n);
      j+=n;
    }
    if (memcmp(sha1.result(), ht[f].sha1, 20)!=0) return fclose(in), false;
  }
  if (fread(buf, 1, BUFSIZE, in)!=0) return fclose(in), false;
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
        while (out.size()<output_size && d.decompress(1<<14));
        lock(job.mutex);
        print_progress(job.total_size, job.total_done);
		
        release(job.mutex);
        if (out.size()>=output_size) break;
        d.readSegmentEnd();
      }
      if (out.size()<output_size) {
        lock(job.mutex);
        fflush(stdout);
        fprintf(stderr, "output [%d..%d] %d of %d bytes\n",
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
            makepath(filename);
            if (job.jd.summary<=0) {
              lock(job.mutex);
              print_progress(job.total_size, job.total_done);
              release(job.mutex);
            }
			{
              job.outf=fopen(filename.c_str(), WB);
              if (job.outf==FPNULL) {
                lock(job.mutex);
                printerr(filename.c_str());
                release(job.mutex);
              }
              else if ((p->second.attr&0x200ff)==0x20000+'w') {  // sparse?
                DWORD br=0;
                if (!DeviceIoControl(job.outf, FSCTL_SET_SPARSE,
                    NULL, 0, NULL, 0, &br, NULL))  // set sparse attribute
                  printerr(filename.c_str());
              }
            }
          }
          else
            job.outf=fopen(filename.c_str(), RBPLUS);  // update existing file
          if (job.outf==FPNULL) break;  // skip errors
          job.lastdt=p;
          assert(job.outf!=FPNULL);
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
        if ((nz<q+usize || j+1==ptr.size())) {
          fseeko(job.outf, offset, SEEK_SET);
          fwrite(out.c_str()+q, 1, usize, job.outf);
        }
        offset+=usize;
        lock(job.mutex);
        job.total_done+=usize;
        release(job.mutex);

        // Close file. If this is the last fragment then set date and attr.
        // Do not set read-only attribute in Windows yet.
        if (p->second.data==int64_t(ptr.size())) {
          assert(p->second.date);
          assert(job.lastdt!=job.jd.dt.end());
          assert(job.outf!=FPNULL);
          {
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

// Extract files from archive. If force is true then overwrite
// existing files and set the dates and attributes of exising directories.
// Otherwise create only new files and directories. Return 1 if error else 0.

bool yesorno(int i_ok)
{
	int n = getch();
	return (n==i_ok);
    ///printf("%d %c\n", n, n);
}
// This is just equal to ZPAQ 7.15, so can works with zpaqfranz too
int Jidac::extract() 
{
	/// using SFX module, without -to, ask what to do
	if (tofiles.size()==0)
	{
		string where=getline("Path to extract into (empty=default) :");
		if (where!="")
			tofiles.push_back(where);
	}
	
	
  // Read archive
  const int64_t sz=read_archive(archive.c_str());
  if (sz<1) error("archive not found");

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

   // Label files to extract with data=0.
  // Skip existing output files. If force then skip only if equal
  // and set date and attributes.
  ExtractJob job(*this);
  int total_files=0, skipped=0;
  
  bool flagask=true;
  
  
  for (DTMap::iterator p=dt.begin(); p!=dt.end(); ++p) {
    p->second.data=-1;  // skip
    if (p->second.date && p->first!="") {
      const string fn=rename(p->first);
      const bool isdir=p->first[p->first.size()-1]=='/';
	  
	  if (!force && exists(fn)) 
		if (flagask)
		{
			printf("Overwrite ALL already existing files  (y/n)?\n");
			force=yesorno('y');
			flagask=false;
#if defined (DEBUG)
			if (force)
				printf("FORZA\n");
			else
				printf("mantieni\n");
#endif
		}
	  
	  
      if (force && !isdir && equal(p, fn.c_str())) {
        if (summary<=0) {  // identical
          printf("= ");
          printUTF8(fn.c_str());
          printf("\n");
        }
        close(fn.c_str(), p->second.date, p->second.attr);
        ++skipped;
      }
      else if (!force && exists(fn)) 
	  {  // exists, skip
		

	  if (summary<=0) {
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
  if (!force && skipped>0)
    printf("%d ?existing files skipped.\n", skipped);
  if (force && skipped>0)
    printf("%d =identical files skipped.\n", skipped);

  

  // Decompress archive in parallel
  printf("Extracting %s bytes in %d files -threads %d\n",
      migliaia(job.total_size), total_files, threads);
  vector<ThreadID> tid(threads);
  for (unsigned i=0; i<tid.size(); ++i) run(tid[i], decompressThread, &job);

  // Extract streaming files
  unsigned segments=0;  // count
  InputArchive in(archive.c_str(), password);
  if (in.isopen()) {
    FP outf=FPNULL;
    DTMap::iterator dtptr=dt.end();
    for (unsigned i=0; i<block.size(); ++i) {
      if (block[i].usize<0 && block[i].size>0) {
        Block& b=block[i];
        try {
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
                if (summary<=0) {
                  printf("2> ");
                  printUTF8(outname.c_str());
                  printf("\n");
                }
                 {
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

            // Decompress segment
            libzpaq::SHA1 sha1;
            d.setSHA1(&sha1);
            OutputFile o(outf);
            d.setOutput(&o);
            d.decompress();

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

  // Wait for threads to finish
  for (unsigned i=0; i<tid.size(); ++i) join(tid[i]);

  // Create empty directories and set file dates and attributes
   {
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



/////////////////////////////// main //////////////////////////////////

/// N: no argc or argv, we'll work on it
/// Convert argv to UTF-8 and replace \ with /

int main() 
{
	int argc=0;
	LPWSTR* argw=CommandLineToArgvW(GetCommandLine(), &argc);
	vector<string> args(argc);
	libzpaq::Array<char*> argp(argc);
	for (int i=0; i<argc; ++i) 
	{
		args[i]=wtou(argw[i]);
		argp[i]=(char*)args[i].c_str();
	}
	char** argv=&argp[0];

	g_global_start=mtime();  // get start time
	
	int errorcode=0;
	try 
	{
		Jidac jidac;
		jidac.archive=""; // if empty, runs as always
	
		string myfullname	=getmyname();
		string percorso		=extractfilepath(myfullname);
		string nome			=prendinomefileebasta(myfullname);
		string zpaqname		=percorso+nome+".zpaq";
		if (fileexists(zpaqname))
		{
			printf("Found ");
			printUTF8(zpaqname.c_str());
			printf(" => autoextracting\n");
			jidac.archive=zpaqname;
		}
		errorcode=jidac.doCommand(argc, argv);
	}
	catch (std::exception& e) 
	{
		fflush(stdout);
		fprintf(stderr, "zsfx error: %s\n", e.what());
		errorcode=2;
	}
	fflush(stdout);
	fprintf(stderr, "%1.3f seconds %s\n", (mtime()-g_global_start)/1000.0,
		errorcode>1 ? "(with errors)" :
		errorcode>0 ? "(with warnings)" : "(all OK)");
  return errorcode;
}

bool Jidac::isselected(const char* filename, bool rn) {
  bool matched=true;
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
  if (onlyfiles.size()>0) {
    matched=false;
    for (unsigned i=0; i<onlyfiles.size() && !matched; ++i)
      if (ispath(onlyfiles[i].c_str(), filename))
        matched=true;
  }
  if (!matched) return false;
  for (unsigned i=0; i<notfiles.size(); ++i) {
    if (ispath(notfiles[i].c_str(), filename))
      return false;
  }
  return true;
}


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

/*
	Very crude search, no fancy algos needed
*/
unsigned char *memmem(unsigned char *i_haystack,const size_t i_haystack_len,const char *i_needle,const size_t i_needle_len)
{
    int needle_first;
    unsigned char *p = i_haystack;
    size_t plen = i_haystack_len;
    if (!i_needle_len)
        return NULL;
    needle_first = *(unsigned char *)i_needle;
    while (plen >= i_needle_len && (p = (unsigned char*)memchr(p,needle_first,plen-i_needle_len + 1)))
    {
        if (!memcmp(p,i_needle,i_needle_len))
            return (unsigned char *)p;
        p++;
        plen=i_haystack_len-(p-i_haystack);
    }
    return NULL;
}

/*
	The most powerful encryption ever!
*/
string ahahencrypt(string i_string)
{
	///return i_string;
	string risultato="";
	for (unsigned int i=0;i<i_string.size();i++)
        risultato+=i_string[i] ^33;
    return risultato;
}


string Jidac::findcommand(int64_t& o_offset)
{
	
	o_offset=0;
	myname=getmyname();
#if defined(DEBUG)
	printf("My name is %s\n",myname.c_str());
#endif
	FILE* inFile = freadopen(myname.c_str());
	if (inFile==NULL) 
	{
		int err=GetLastError();
		printf("\n2057: ERR <%s> kind %d\n",myname.c_str(),err); 
		return "";
	}
		
	size_t 	readSize;

	/// Assumption: the SFX module is smaller then 3MB
	size_t const blockSize = 3000000;
	unsigned char *buffer=(unsigned char*)malloc(blockSize);
	if (buffer==NULL)
	{
		printf("2281: cannot allocate memory\n");
		exit(0);
	}
		
	string comando	="";
	readSize = fread(buffer, 1, blockSize, inFile);

	bool flagbuilder=(myname=="zsfx.exe")||(myname=="zsfx32.exe");
	if (flagbuilder)
		if (readSize==blockSize)
		{
			printf("2290: SFX module seems huge!\n");
			exit(0);
		}
	
	if (readSize<=0)
	{
		printf("2296: very strange readsize\n");
		exit(0);
	}
	
	unsigned char* startblock=memmem(buffer,readSize,g_franzo_start.c_str(),g_franzo_start.size());
	
	if (startblock!=NULL)
	{
		/// OK start again from 0, it's a small size after all...
		unsigned char* endblock=memmem(buffer,readSize,g_franzo_end.c_str(),g_franzo_end.size());
		if (endblock!=NULL)
		{
			startblock=startblock+g_franzo_start.size()+1;
			o_offset=(int64_t) (endblock+g_franzo_end.size()+1-&buffer[0])-1;
			endblock--;
			if (endblock>startblock)
				while (startblock++!=endblock)
					comando+=(char)*startblock;
		}
		else
		{
			printf("2316: cannot find endblock\n");
			exit(0);
		}
	}
	else
	{
		printf("\n2317: Maybe this is a renamed %s.exe executable without any .zpaq data?\n",exename.c_str());
		exit(0);
	}
	
	fclose(inFile);
	free(buffer);
#if defined(DEBUG)
	printf("|||%s|||\n",comando.c_str());
#endif
	string ahahaencrypted=ahahencrypt(comando);
	return ahahaencrypted;
}

int Jidac::sfx(string i_thecommands)
{
	
	bool	flagbuilder=(myname=="zsfx.exe")||(myname=="zsfx32.exe");
	if (!flagbuilder)
	{
		printf("2351: I need to extract\n");
		return 1;
	}
		
	myname=getmyname(); // this is the FULL name now
		
	printf("Command line  : %s\n",i_thecommands.c_str());
	
	if (flagbuilder)
		if (!fileexists(archive))
		{
			printf("2456: archive does not exists ");
			printUTF8(archive.c_str());
			printf("\n");
			return 2;
		}
		
	printf("My      name  : ");
	printUTF8(myname.c_str());
	printf("\nArchive name  : ");
	printUTF8(archive.c_str());
	printf("\nOutput  name  : ");
	printUTF8(myoutput.c_str());
	printf("\n");
	if (myoutput=="")
		myoutput=archive;
	
	string outfile	=prendinomefileebasta(myoutput);
	string percorso	=extractfilepath(myoutput);
	outfile=percorso+outfile+".exe";

	if (fileexists(outfile))
	{
		printf("2334: output file exists, abort %s\n",outfile.c_str());
		exit(0);
	}
	printf("Working on    : %s\n",outfile.c_str());

///	Take care of non-latin stuff

	std::wstring widename=utow(outfile.c_str());
	FILE* outFile=_wfopen(widename.c_str(), L"wb" );
	if (outFile==NULL)
	{
		printf("2017 :CANNOT OPEN outfile %s\n",outfile.c_str());
		return 2;
	}
	size_t const 	blockSize = 65536;
	unsigned char 	buffer[blockSize];

	FILE* inFile = freadopen(myname.c_str());
	if (inFile==NULL) 
	{
		int err=GetLastError();
		printf("\n2028: ERR <%s> kind %d\n",myname.c_str(),err); 
		return 2;
	}
		
	size_t 	readSize;
	
	while ((readSize = fread(buffer, 1, blockSize, inFile)) > 0) 
		fwrite(buffer,1,readSize,outFile);
	
	fclose(inFile);
	
	/// yes, one byte at time. Why? Because some "too smart" compilers can substitute too much
	/// please note: the strings is already inverted 
	
	for (unsigned int i=0;i<g_franzo_start.size();i++)
		fwrite(&g_franzo_start[i],1,1,outFile);

	string ahahaencrypted=ahahencrypt(i_thecommands);
	for (unsigned int i=0;i<i_thecommands.size();i++)
		///fwrite(&i_thecommands[i],1,1,outFile);
		fwrite(&ahahaencrypted[i],1,1,outFile);
	
	for (unsigned int i=0;i<g_franzo_end.size();i++)
		fwrite(&g_franzo_end[i],1,1,outFile);
	
	///	Now append the .zpaq. Please note: no handling for multipart (in fact, not too hard)
	inFile = freadopen(archive.c_str());
	if (inFile==NULL) 
	{
		int err=GetLastError();
		printf("\n2077: ERR <%s> on archive kind %d\n",archive.c_str(),err); 
		return 2;
	}
	
	while ((readSize = fread(buffer, 1, blockSize, inFile)) > 0) 
		fwrite(buffer,1,readSize,outFile);

	fclose(outFile);
	
	///	Just debug stuff
	printf("SFX written on: %s\n",outfile.c_str());
	printf("\n");
	int64_t sfxsize	=prendidimensionefile(myname.c_str());
	int64_t zpaqsize=prendidimensionefile(archive.c_str());
	int64_t sfxtotal=prendidimensionefile(outfile.c_str());
	
	printf("SFX module    : %19s\n",migliaia(sfxsize));
	printf("Block1        : %19s\n",migliaia(g_franzo_start.size()));
	printf("Command       : %19s\n",migliaia(i_thecommands.size()));
	printf("Block2        : %19s\n",migliaia(g_franzo_end.size()));
	printf("Start         :                      %s\n",migliaia(sfxsize+g_franzo_start.size()+i_thecommands.size()+g_franzo_end.size()));
	
	printf("ZPAQ archive  : %19s\n",migliaia(zpaqsize));

	printf("Expected      : %19s\n",migliaia(sfxsize+zpaqsize+g_franzo_start.size()+g_franzo_end.size()+i_thecommands.size()));
	printf("Written       : %19s\n",migliaia(sfxtotal));
	
	return 0;
}
