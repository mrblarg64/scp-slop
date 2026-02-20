// Made by me
#ifndef SCP_OS_H
#define SCP_OS_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

//#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#if (WINVER >= 0x0600)
#include <qos2.h>
#else
#include <mstcpip.h>
#include <ntdef.h>
#include <ntstatus.h>
#endif

#define ERRNO_T DWORD
#define SOCKET_T SOCKET
#define FILE_T HANDLE
#define NEWLINE "\r\n"
#define U64_PF "%llu"
#define SSO_CAST (const char*)
#define B_SHUT_RD SD_RECEIVE
#define B_SHUT_WR SD_SEND

#if (WINVER >= 0x0600)
extern QOS_FLOWID qosfid = 0;
#endif

extern HANDLE stdouth;
extern DWORD bout;
#define FASTPRINT(str) (WriteFile(stdouth, str, sizeof(str) - 1, &bout, NULL))

#define SOCKERROR(str) do {			\
wsaerrno = WSAGetLastError();	\
printf(str " 0x%x" NEWLINE, wsaerrno);	\
ExitProcess(wsaerrno);\
} while (0)

#define SEND_MSG_MORE(inbuf, size) do {\
		wsab.len = size;\
		wsab.buf = (char *)inbuf;				\
		if (WSASend(sock, &wsab, 1, &sentbytes, MSG_PARTIAL, NULL, NULL))\
		{\
			wsaerrno = WSAGetLastError();\
			if (wsaerrno != WSA_IO_PENDING)\
			{\
				printf("WSASend() 0x%x" NEWLINE, wsaerrno);	\
				ExitProcess(wsaerrno);			\
			}						\
		}							\
	} while (0)

#define CLOSESOCK(fd) (closesocket(fd))

#define FASTEXIT(code) (ExitProcess(code))

#define CLOSEFILE(fd) (CloseHandle(fd))

#define BERNTRNSFR_LINUX_SENDFILE_MAX 2147483646

extern WSADATA wsd;

extern HANDLE hh;


#else

#define _GNU_SOURCE

#include <fcntl.h>
#include <linux/sched.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <signal.h>
#include <unistd.h>
#include <locale.h>
#include <sched.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERRNO_T int
#define SOCKET_T int
#define FILE_T int
#define NEWLINE "\n"
#define U64_PF "%'lu"
#define SSO_CAST (void*)
#define B_SHUT_RD SHUT_RD
#define B_SHUT_WR SHUT_WR

#define FASTPRINT(str) ((void)!write(STDOUT_FILENO, str, sizeof(str) - 1))

#define SOCKERROR(str) do {			\
myerrno = errno;\
perror(str);\
_exit(myerrno);\
} while (0)

#define SEND_MSG_MORE(buf, size) do {\
if (send(sock, buf, size, MSG_MORE ) != sizeof(uint64_t))\
{\
	SOCKERROR("send()");\
}\
} while (0)

#define CLOSESOCK(fd) (close(fd))

#define FASTEXIT(code) (_exit(code))

#define CLOSEFILE(fd) (close(fd))

#define BERNTRNSFR_LINUX_SENDFILE_MAX 0x7ffff000

extern const struct sigaction siga;


#endif

void osinit();
void *scpmalloc(size_t s);
FILE_T scpopen(char *fname);
uint64_t scpgetfsize(FILE_T fd);

#endif
