// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "common.h"
#include "errorCodes.h"
#include "filemgr.h"

using namespace std;

// Globals
filext::filemgr gFileMgr;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		// Allocate console
#ifdef _DEBUG
		::AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		LOG(("DLL Attached\n"));
#endif
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C"
{
	__declspec(dllexport) void __stdcall RVExtension(char* output, int outputSize, const char* function);
	__declspec(dllexport) int __stdcall RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc);
	__declspec(dllexport) void __stdcall RVExtensionVersion(char* output, int outputSize);
}


void RVExtension(char* output, int outputSize, const char* function)
{
	strcpy_s(output, outputSize-1, function);
	LOG(("Function called: %s\n", function));
}

// Macro for asserting correct argument count
#define ASSERT_EXT_ARGC(argc, argcNeeded) if (argc != argcNeeded)	{ \
LOG(("Wrong arg count, received: %i, expected: %i\n", argc, argcNeeded)); \
return FILEXT_WRONG_ARG_COUNT; \
}

/*
We use function argument to pass actual data,
because all strings passed through argv are wrapped into extra quotes - they are stringified strings :/ :/ :/

callExtension arguments are:
[(value), [function, (fileName), (key)]]
*/
int RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc)
{
	// Extract function name
	const char* functionName = argv[0];
	const char* data = function; // It doesn't have the quotes!

	// Exctract file name: remove leading and trailing "
	string fileName("");
	if (argc >= 2) {
		fileName = string(argv[1]);
		fileName.erase(0, 1);
		fileName.pop_back();
		//LOG(("Argc: %i\n", argc));
	}

	LOG(("RVExtensionArgs: function: %s, fileName: %s, outputSize: %i\n", functionName, fileName.c_str(), outputSize));

	// Resolve function name

	// ["", ["open", fileName]]
	if (strcmp(functionName, "\"open\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2)
		return gFileMgr.open(fileName);
	};

	// ["", ["close", fileName]]
	if (strcmp(functionName, "\"close\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2)
		return gFileMgr.close(fileName);
	};

	// ["", ["write", fileName]]
	if (strcmp(functionName, "\"write\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2)
		return gFileMgr.write(fileName);
	};

	// ["", ["get", fileName, key]]
	if (strcmp(functionName, "\"get\"") == 0) {
		ASSERT_EXT_ARGC(argc, 3)
		std::string strOut("");
		int retInt = gFileMgr.get(fileName, argv[2], strOut);
		strcpy_s(output, outputSize - 1, strOut.c_str());
		return retInt;
	};

	// [value, ["set", fileName, key]]
	if (strcmp(functionName, "\"set\"") == 0) {
		ASSERT_EXT_ARGC(argc, 3)
		return gFileMgr.set(fileName, argv[2], data);
	};

	// ["testDataToCopy", ["loopback"]]
	if (strcmp(functionName, "\"loopback\"") == 0) {
		ASSERT_EXT_ARGC(argc, 1)
		strcpy_s(output, outputSize-1, data);
		return 0;
	};

	// ["testData", ["dummy"]]
	if (strcmp(functionName, "\"dummy\"") == 0) {
		ASSERT_EXT_ARGC(argc, 1)
		LOG(("Data length: %i", (int)strlen(data)));
		return (int)strlen(data);
	};

	return FILEXT_WRONG_FUNCTION_NAME;
}

void RVExtensionVersion(char* output, int outputSize)
{
	strcpy_s(output, outputSize-1, "Filext 1.0");
}


/*
open("mysave.vin")
setv("mysave.vin", "varName", "value")
setv("mysave.vin", "varName", "value")
setv("mysave.vin", "varName", "value")
setv("mysave.vin", "varName", "value")
write("mysave.vin")

listFiles()

//keys = getKeys("mysave.vin")
open("mysave.vin")
getv("mysave.vin", "varName")
getv("mysave.vin", "varName")
close("mysave.vin")

*/