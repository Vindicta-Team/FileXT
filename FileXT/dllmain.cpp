// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "common.h"
#include "errorCodes.h"
#include "filemgr.h"
#include "value.h"


#ifndef _MSC_VER
#include <dlfcn.h>
#endif

using namespace std;

// Forward Declerations
bool checkFileName(string& fileName);
string getDllFolder();
std::filesystem::path& getLogPath();

// Globals
filext::filemgr gFileMgr;
std::unique_ptr<FILE, decltype(&std::fclose)> gpFile(std::fopen(getLogPath().u8string().c_str(), "w"), &std::fclose);

const std::string& GetAndEnsureStorageDir()
{
	// keep this static path internal so it doesn't get used too early or modified externally.
	static string fileStorageFolder;

	if(fileStorageFolder.empty() || !filesystem::exists(fileStorageFolder))
	{
		LOG_VERBOSE("File storage directory isn't set or doesn't exist. Attempting to find or create it now.");
		std::string dllFolder = getDllFolder();
		LOG_VERBOSE("DLL Folder: %s", dllFolder.c_str());

		if(dllFolder.empty() || !filesystem::exists(dllFolder))
		{
			// Use LOG_VERBOSE here because LOG depends on this function completing.
			LOG_CRITICAL("The DLL folder was not found: \"%s\"", dllFolder.c_str());
		}
		fileStorageFolder = dllFolder + string("storage/");

		if(!filesystem::exists(fileStorageFolder))
		{
			LOG_VERBOSE("fileStorageFolder not found, creating it now: \"%s\"", fileStorageFolder.c_str());
			filesystem::create_directory(fileStorageFolder);
		}

		if(!filesystem::exists(fileStorageFolder))
		{
			LOG_CRITICAL("The file storage directory could not be created.");
			fileStorageFolder.clear();
		}

	}
	return fileStorageFolder;
}

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void Entry()
{
		LOG_VERBOSE("FileXT Dll entry");
		std::string const& storageDirectory = GetAndEnsureStorageDir();
}

#ifndef _MSC_VER
__attribute__((destructor))
#endif
static void Cleanup()
{
		LOG_VERBOSE("FileXT Dll cleanup");
}


#ifdef _MSC_VER
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch( ul_reason_for_call ) 
    { 
        case DLL_PROCESS_ATTACH:
			Entry();
		break;
		case DLL_PROCESS_DETACH:
			Cleanup();
		break;
	}
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
LOG("Wrong arg count, received: %i, expected: %i", argc, argcNeeded); \
return FILEXT_ERROR_WRONG_ARG_COUNT; \
}

#define ASSERT_BUFFER_SIZE(maxSize, givenSize) if(givenSize > maxSize) {\
LOG("Buffer size is too small. Max size: %i, size: %i.", maxSize, givenSize);\
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

	std::string const& storageDirectory = GetAndEnsureStorageDir();

	// Exctract file name: remove leading and trailing "
	string fileName("");
	if (argc >= 2) {
		fileName = string(argv[1]);
		fileName.erase(0, 1);
		fileName.pop_back();
		//LOG("Argc: %i", argc);

		// Check file name
		// Bail if file name is wrong
		if (!checkFileName(fileName))
			return FILEXT_ERROR_WRONG_FILE_NAME;
		fileName = storageDirectory + fileName;
	}

	LOG_VERBOSE("RVExtensionArgs: function: %s, fileName: %s, outputSize: %i", functionName, fileName.c_str(), outputSize);

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
		LOG("  Returning string of size: %i", (unsigned int)strOut.size());
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

		// prevent filesystem exception on directory_iterator
		if (!filesystem::exists(storageDirectory)) {
			LOG_CRITICAL("failed to getFiles");
			return 0;
		}

		vector<sqf::value> vectorFileNamesSQF;
		for (const auto& entry : filesystem::directory_iterator(storageDirectory)) {
			string fileName = entry.path().filename().string();
			vectorFileNamesSQF.push_back(sqf::value(fileName));
			LOG("File: %s", fileName.c_str());
		}
		
		sqf::value fileNamesSQFArray(vectorFileNamesSQF);
		string strOut = fileNamesSQFArray.to_string();

		ASSERT_BUFFER_SIZE((int)outputSize - 1, (int)strOut.size());
		strcpy(output, strOut.c_str());
		return 0;
	};

	// ["", ["deleteFile", fileName]]
	if (strcmp(functionName, "\"deleteFile\"") == 0) {
		if(filesystem::exists(fileName))
		{
			try {
				filesystem::remove(fileName);
			} catch (...) {
				return FILEXT_ERROR_WRONG_FILE_NAME;
			}
		}
		else
		{
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
	
	std::string versionString = "FileXT 1.2";

	if(versionString.size() > (size_t)outputSize - 1)
	{
		LOG("Buffer size is too small. Max size: %i, size: %i.", (int)outputSize - 1, (int)versionString.size());
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
		LOG_G("Fatal Error: GetModuleHandle returned %i", GetLastError());
		throw new std::runtime_error("Fatal Error: GetModuleHandle failed.");
	}

	GetModuleFileNameA(hm, path, sizeof(path));
	std::string p(path);

	// Remove DLL name from the path
	auto pos = p.rfind('\\');
	p.erase(pos + 1, p.size());
	return p;
#else 
	Dl_info dl_info;
	if (dladdr((void*)getDllFolder, &dl_info) == 0) {
		LOG_G("Fatal Error: dladdr returned 0");
		throw new std::runtime_error("Fatal Error: dladdr returned 0");
    }
	std::string p = std::string(dl_info.dli_fname);

	// Remove DLL name from the path
	auto pos = p.rfind('/');
	p.erase(pos + 1, p.size());
	return p;
#endif
}

std::filesystem::path& getLogPath() {
	static std::filesystem::path logPath;

	if (logPath.empty()) {
		logPath = std::filesystem::path(getDllFolder()) / "filext_log.log";
	}

	return logPath;
}