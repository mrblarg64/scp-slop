#ifdef _WIN32
#include <MessageBoxW.h> //correct the windows header file once I know what it is
#else
#include <stdio.h>
#endif

void errormsg(char *msg)
{
	#ifdef _WIN32
	MessageBoxW(NULL, msg, "SCP Error", 0);
	#else
	printf("%s\n",msg);
	#endif
	return;
}
