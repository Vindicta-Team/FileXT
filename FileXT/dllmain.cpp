// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "common.h"
#include "errorCodes.h"
#include "filemgr.h"
#include "value.h"


#if !defined(_MSC_VER)
#include <dlfcn.h>
#endif

using namespace std;

// Globals
filext::filemgr gFileMgr;
string dllFolder;
string fileStorageFolder;

#ifdef _DEBUG
FILE* gLogFile;
#endif

bool checkFileName(string& fileName);
string getDllFolder();

static struct FileXTEntryPoint {
	FileXTEntryPoint() {
		// Open log file
#ifdef _DEBUG
		fopen_s(&gLogFile, "filext_log.log", "w");
		fprintf(gLogFile, "Log file print test: %i\n", 123);
#endif
		// Resolve path to this dll
		dllFolder = getDllFolder();
		LOG_1("DLL path: %s\n", dllFolder.c_str());
		fileStorageFolder = dllFolder + string("storage\\");

		// Ensure that there is a folder for filext files
		if (!filesystem::exists(fileStorageFolder)) {
			filesystem::create_directory(fileStorageFolder);
		}
	}

} g_FileXTEntryPoint;

#ifdef _MSC_VER
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	return TRUE;
}
#endif

extern "C"
{
	FILEXT_EXPORT int FILEXT_CALL RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc);
	FILEXT_EXPORT void FILEXT_CALL RVExtensionVersion(char* output, int outputSize);
}

// Macro for asserting correct argument count
#define ASSERT_EXT_ARGC(argc, argcNeeded) if (argc != argcNeeded)	{ \
LOG_2("Wrong arg count, received: %i, expected: %i\n", argc, argcNeeded); \
return FILEXT_ERROR_WRONG_ARG_COUNT; \
}

#define ASSERT_BUFFER_SIZE(maxSize, givenSize) if(givenSize > maxSize) {\
LOG_2("Buffer size is too small. Max size: %i, size: %i.", maxSize, givenSize);\
return FILEXT_ERROR_BUFFER_TOO_SMALL; \
}

/*
We use function argument to pass actual data,
because all strings passed through argv are wrapped into extra quotes - they are stringified strings :/ :/ :/

callExtension arguments are:
[(value), [function, (fileName), (key)]]
*/
FILEXT_EXPORT int FILEXT_CALL RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc)
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
		//LOG_1("Argc: %i\n", argc);

		// Check file name
		// Bail if file name is wrong
		if (!checkFileName(fileName))
			return FILEXT_ERROR_WRONG_FILE_NAME;
		fileName = fileStorageFolder + fileName;
	}

	LOG_3("RVExtensionArgs: function: %s, fileName: %s, outputSize: %i\n", functionName, fileName.c_str(), outputSize);

	// Resolve function name

	// ["", ["open", fileName]]
	if (strcmp(functionName, "\"open\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.open(fileName);
	};

	// ["", ["close", fileName]]
	if (strcmp(functionName, "\"close\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.close(fileName);
	};

	// ["", ["write", fileName]]
	if (strcmp(functionName, "\"write\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.write(fileName);
	};

	// ["", ["read", fileName]]
	if (strcmp(functionName, "\"read\"") == 0) {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.read(fileName);
	};

	// ["", ["get", fileName, key, reset(0/1)]]
	if (strcmp(functionName, "\"get\"") == 0) {
		ASSERT_EXT_ARGC(argc, 4);
		std::string strOut("");
		int reset = 0;
		try {
			reset = std::stoi(argv[3]);
		} catch ( ... ) {
			reset = 0;
		}
		int retInt = gFileMgr.get(fileName, argv[2], strOut, outputSize-4, (bool)reset); // Just to be safe, reduce size a bit
		LOG_1("  Returning string of size: %i\n", (unsigned int)strOut.size());
		ASSERT_BUFFER_SIZE((int)outputSize -1, (int)strOut.size());
		strcpy(output, strOut.c_str());
		return retInt;
	};

	// [value, ["set", fileName, key]]
	if (strcmp(functionName, "\"set\"") == 0) {
		ASSERT_EXT_ARGC(argc, 3);
		return gFileMgr.set(fileName, argv[2], data);
	};

	// ["", ["eraseKey", fileName, key]]
	if (strcmp(functionName, "\"eraseKey\"") == 0) {
		ASSERT_EXT_ARGC(argc, 3);
		return gFileMgr.eraseKey(fileName, argv[2]);
	};

	// ["", ["getFiles"]]
	if (strcmp(functionName, "\"getFiles\"") == 0) {
		ASSERT_EXT_ARGC(argc, 1);

		vector<sqf::value> vectorFileNamesSQF;
		for (const auto& entry : filesystem::directory_iterator(fileStorageFolder)) {
			string fileName = entry.path().filename().string();
			vectorFileNamesSQF.push_back(sqf::value(fileName));
			LOG_1("File: %s\n", fileName.c_str());
		}
		
		sqf::value fileNamesSQFArray(vectorFileNamesSQF);
		string strOut = fileNamesSQFArray.to_string();

		ASSERT_BUFFER_SIZE((int)outputSize - 1, (int)strOut.size());
		strcpy(output, strOut.c_str());
		return 0;
	};

	// ["", ["deleteFile", fileName]]
	if (strcmp(functionName, "\"deleteFile\"") == 0) {
		try {
			filesystem::remove(fileName);
		} catch (...) {
			return FILEXT_ERROR_WRONG_FILE_NAME;
		}
		return FILEXT_SUCCESS;
	};

	// ["", ["fileExists", fileName]]
	if (strcmp(functionName, "\"fileExists\"") == 0) {
		if (filesystem::exists(fileName))
			return FILEXT_SUCCESS;
		else
			return FILEXT_ERROR_WRONG_FILE_NAME;
	};

	return FILEXT_ERROR_WRONG_FUNCTION_NAME;
}

FILEXT_EXPORT void FILEXT_CALL RVExtensionVersion(char* output, int outputSize)
{
	if(outputSize <=0 || output == nullptr)
	{
		LOG("Invalid buffer");
	}
	
	std::string versionString = "Filext 1.1";

	if(versionString.size() > (size_t)outputSize - 1)
	{
		LOG_2("Buffer size is too small. Max size: %i, size: %i.", (int)outputSize - 1, (int)versionString.size());
	}
	else
	{
		strcpy(output, versionString.c_str());
	}
}

// Ensures that the file name is correct:
// Has non-negative length
// Doesn't have / or \ inside it
bool checkFileName(string& fileName) {

	// Check length
	if (fileName.size() == 0)
		return false;

	// Check forbidden characters
	string forbiddenChars("\\/?*:|\"<>,;=");
	if (fileName.find_first_of(forbiddenChars) != string::npos)
		return false;

	// All good so far
	return true;
}


std::string getDllFolder()
{
#ifdef _MSC_VER
	// Borrowed from https://gist.github.com/pwm1234/05280cf2e462853e183d

	char path[FILENAME_MAX];
	HMODULE hm = NULL;

	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)getDllFolder,
		&hm))
	{
		LOG_1("error: GetModuleHandle returned %i", GetLastError());
		return std::string("");
	}

	GetModuleFileNameA(hm, path, sizeof(path));
	std::string p(path);

	// Remove DLL name from the path
	auto pos = p.rfind('\\');
	p.erase(pos + 1, p.size());
	return p;
#else 
	Dl_info dl_info;
	dladdr((void*)getDllFolder, &dl_info);
	return std::string(dl_info.dli_fname);
#endif
}