#include "scp-os.h"

void errormsg(char *msg)
{
	#ifdef _WIN32
	MessageBox(NULL, msg, "SCP Error", MB_SYSTEMMODAL | MB_OK | MB_ICONERROR);
	#else
	printf("%s\n",msg);
	#endif
	return;
}
