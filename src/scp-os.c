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
#define SCP_OS_ERROR_MAX_LENGTH 160


const struct sigaction siga = {.sa_handler = SIG_IGN};

#endif

#ifdef _WIN32

void osint()
{
	DWORD myerrno;

	hh = HeapCreate(0, 0, 0);
	if (!hh)
	{
		myerrno = GetLastError();
		errormsg("HeapCreate() failed!");
		FASTEXIT(myerrno);
	}
	
	myerrno = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (myerrno)
	{
		printf("failed WSAStartup() ecode 0x%lx" NEWLINE, myerrno);
		ExitProcess(myerrno);
	}
}

void *scpmalloc(size_t s)
{
	return HeapAlloc(hh, HEAP_NO_SERIALIZE, s);
}

FILE_T scpopen(char *fname)
{
	FILE_T retval;
	DWORD myerrno;

	retval = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (retval == INVALID_HANDLE_VALUE)
	{
		myerrno = GetLastError();
		errormsg("CreateFile() failed");
		FASTEXIT(myerrno);
	}
}

uint64_t scpgetfsize(FILE_T fd)
{
	LARGE_INTEGER fsize;
	DWORD myerrno;

	if (!GetFileSizeEx(fd, &fsize))
	{
		myerrno = GetLastError();
	        errormsg("GetFileSizeEx()");
		ExitProcess(myerrno);
	}

        return fsize.QuadPart;
}

#else

void osinit()
{
	char *errmsg;
	int myerrno;

	setlocale(LC_ALL, "");

	if (sigaction(SIGPIPE, &siga, NULL))
	{
		myerrno = errno;
		errmsg = __builtin_alloca(SCP_OS_ERROR_MAX_LENGTH);
		__builtin_sprintf(errmsg, "osinit() sigaction(): %s\n", strerror(myerrno));
		errormsg(errmsg);
	        FASTEXIT(myerrno);
	}
}

void *scpmalloc(size_t s)
{
	return malloc(s);
}

FILE_T scpopen(char *fname)
{
	FILE_T retval;
	char *errmsg;
	int myerrno;

	retval = open(fname, O_RDONLY);
	if (retval == -1)
	{
		myerrno = errno;
		errmsg = __builtin_alloca(SCP_OS_ERROR_MAX_LENGTH);
		__builtin_sprintf(errmsg, "scpopen() open(): %s\n", strerror(myerrno));
		errormsg(errmsg);
	        FASTEXIT(myerrno);
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
		myerrno = errno;
		errmsg = __builtin_alloca(SCP_OS_ERROR_MAX_LENGTH);
		__builtin_sprintf(errmsg, "scpgetfsize() fstat(): %s\n", strerror(myerrno));
		errormsg(errmsg);
	        FASTEXIT(myerrno);
	}

	return fst.st_size;
}
#endif
