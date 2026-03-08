#include "scp-os.h"
#include "errormsg.h"

#include <locale.h>
#include <signal.h>


#ifdef _WIN32

#if (WINVER >= 0x0600)
QOS_FLOWID qosfid = 0;
#endif

HANDLE stdouth;
DWORD bout;

WSADATA wsd;

HANDLE hh;
#else
//linux

const struct sigaction siga = {.sa_handler = SIG_IGN};

#endif

#ifdef _WIN32

void osinit()
{
	DWORD myerrno;

	hh = HeapCreate(0, 0, 0);
	if (!hh)
	{
		WINERROR("HeapCreate()");
	}
	
	myerrno = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (myerrno)
	{
		printf("failed WSAStartup() ecode 0x%lx" NEWLINE, myerrno);
		ExitProcess(myerrno);
	}
}

FILE_T scpopen(char *fname)
{
	FILE_T retval;
	DWORD myerrno;

	retval = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (retval == INVALID_HANDLE_VALUE)
	{
		WINERROR("CreateFile()");
	}
}

uint64_t scpgetfsize(FILE_T fd)
{
	LARGE_INTEGER fsize;
	DWORD myerrno;

	if (!GetFileSizeEx(fd, &fsize))
	{
		WINERROR("GetFileSizeEx()");
	}

        return fsize.QuadPart;
}

unsigned char scpread(FILE_T fd, void *buf, unsigned size)
{
	DWORD bread;

	if (!ReadFile(fd, buf, size, &bread, NULL))
	{
		WINERROR("ReadFile()");
		//todo return 1
	}

	return 0;
}

unsigned char scpwrite(FILE_T fd, void *buf, unsigned size)
{
	DWORD written;

	if (!WriteFile(fd, buf, size, &written, NULL))
	{
		WINERROR("WriteFile()");
		//todo return 1
	}

	return 0;
}

void *scpmapfile(FILE_T fd, uint64_t mapsize)
{
	HANDLE fh;
	HANDLE mh;
	LARGE_INTEGER fsize;
	void *retval;
	ERRNO_T myerrno;

	fsize.quadpart = mapsize
	mh = CreateFileMapping(fh, NULL, PAGE_READONLY, fsize.HighPart, fsize.LowPart, NULL);
	if (!mh)
	{
	        WINERROR("CreateFileMapping() failed");
	}
	retval = MapViewOfFile(mh, FILE_MAP_READ, 0, 0, 0);
	if (!retval)
	{
	        WINERROR("MapViewOfFile() failed");
	}

	CloseHandle(mh);
	CloseHandle(fh);

	return retval;
}

void scpunmapfile(void *map, uint64_t length)
{
	ERRNO_T myerrno;

	(void)length;

	if (!UnmapViewOfFile(map))
	{
		WINERROR("UnmapViewOfFile()");
	}
}

#else

void osinit()
{
	char *errmsg;
	int myerrno;

	setlocale(LC_ALL, "");

	if (sigaction(SIGPIPE, &siga, NULL))
	{
		POSIXERROR("osinit() sigaction()");
	}
}

FILE_T scpopen(char *fname)
{
	FILE_T retval;
	char *errmsg;
	int myerrno;

	retval = open(fname, O_RDONLY);
	if (retval == -1)
	{
		POSIXERROR("scpopen() open()");
	}

	return retval;
}

uint64_t scpgetfsize(FILE_T fd)
{
	struct stat fst;
	char *errmsg;
	int myerrno;

	if (fstat(fd, &fst))
	{
		POSIXERROR("scpgetfsize() fstat()");
	}

	return fst.st_size;
}

unsigned char scpread(FILE_T fd, void *buf, unsigned size)
{
	char *errmsg;
	int myerrno;

	if (read(fd, buf, size) != size)
	{
		POSIXERROR("read()");
		//todo return 1
	}

	return 0;
}

unsigned char scpwrite(FILE_T fd, void *buf, unsigned size)
{
	char *errmsg;
	int myerrno;

	if (write(fd, buf, size) != size)
	{
	        POSIXERROR("write()");
		//todo return 1
	}

	return 0;
}

void *scpmapfile(FILE_T fd, uint64_t mapsize)
{
	char *errmsg;
	int myerrno;
	void *retval;

	retval = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
	if (retval == MAP_FAILED)
	{
		POSIXERROR("mmap()");
	}

	return retval;
}

void scpunmapfile(void *map, uint64_t length)
{
	char *errmsg;
	int myerrno;

	if (munmap(map, length))
	{
		POSIXERROR("munmap()");
	}
}

#endif
