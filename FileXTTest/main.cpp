
#include "platform.h"


extern "C"
{
	FILEXT_IMPORT int FILEXT_CALL RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc);
	FILEXT_IMPORT void FILEXT_CALL RVExtensionVersion(char* output, int outputSize);
}

int main()
{
    char buff[256];
	const char* args[] = {
		"a",
		"b",
		"c"
	};
	RVExtensionArgs(buff, sizeof(buff), "invalid", (const char**)args, 3);
}