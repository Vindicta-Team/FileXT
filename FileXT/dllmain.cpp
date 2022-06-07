// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "common.h"
#include "errorCodes.h"
#include "filemgr.h"
#include "utility.h"
#include "value.h"

// Globals
filext::filemgr gFileMgr;
std::unique_ptr<FILE, decltype(&std::fclose)> gpFile(std::fopen(filext::getLogPath().u8string().c_str(), "w"), &std::fclose);

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void Entry()
{
		LOG_VERBOSE("FileXT Dll entry\n");
		filext::getStorage();
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
LOG("Wrong arg count, received: %i, expected: %i\n", argc, argcNeeded); \
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
	filext::tryMigrateStorageFolder();
	static const std::filesystem::path& storage = filext::getStorage();

	// Extract function name
	const std::string functionName = string::trim(argv[0], "\"");
	const char* data = function; // It doesn't have the quotes!

	// Resolve function name

	// ["", ["getFiles"]]
	if (functionName == "getFiles") {
		ASSERT_EXT_ARGC(argc, 1);

		std::vector<std::string> filenames;
		if (!std::filesystem::exists(storage)) {
			LOG_CRITICAL("failed to getFiles\n");
			return 0;
		}

		// Collect list of all files in storage folder
		for (const auto& dirEntry : std::filesystem::directory_iterator(storage)) {
			LOG("File: %s\n", dirEntry.path().c_str());
			std::string filename = dirEntry.path().filename().string();
			filenames.emplace_back(filename);
		}

		sqf::value filenamesSQF(filenames);
		std::string strOut = filenamesSQF.to_string();

		ASSERT_BUFFER_SIZE((int)outputSize - 1, (int)strOut.size());
		std::strcpy(output, strOut.c_str());
		return 0;
	};

	// Extract file name: remove leading and trailing "
	std::filesystem::path file;
	if (argc >= 2) {
		file = string::trim(argv[1], "\"");
	}

	// Check file name
	// Bail if file name is wrong
	if (file.empty())
	{
		return FILEXT_ERROR_WRONG_FILE_NAME;
	}

	file = storage / file;

	LOG_VERBOSE("RVExtensionArgs: function: %s, fileName: %s, outputSize: %i\n", functionName, file.string().c_str(), outputSize);

	// ["", ["open", fileName]]
	if (functionName == "open") {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.open(file);
	};

	// ["", ["close", fileName]]
	if (functionName == "close") {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.close(file);
	};

	// ["", ["write", fileName]]
	if (functionName == "write") {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.write(file);
	};

	// ["", ["read", fileName]]
	if (functionName == "read") {
		ASSERT_EXT_ARGC(argc, 2);
		return gFileMgr.read(file);
	};

	// ["", ["get", fileName, key, reset(0/1)]]
	if (functionName == "get") {
		ASSERT_EXT_ARGC(argc, 4);
		std::string strOut("");
		int reset = 0;
		try {
			reset = std::stoi(argv[3]);
		} catch ( ... ) {
			reset = 0;
		}
		int retInt = gFileMgr.get(file, argv[2], strOut, outputSize-4, (bool)reset); // Just to be safe, reduce size a bit
		LOG("  Returning string of size: %i\n", (unsigned int)strOut.size());
		ASSERT_BUFFER_SIZE((int)outputSize -1, (int)strOut.size());
		std::strcpy(output, strOut.c_str());
		return retInt;
	};

	// [value, ["set", fileName, key]]
	if (functionName == "set") {
		ASSERT_EXT_ARGC(argc, 3);
		return gFileMgr.set(file, argv[2], data);
	};

	// ["", ["eraseKey", fileName, key]]
	if (functionName == "eraseKey") {
		ASSERT_EXT_ARGC(argc, 3);
		return gFileMgr.eraseKey(file, argv[2]);
	};

	// ["", ["deleteFile", fileName]]
	if (functionName == "deleteFile") {
		if (std::filesystem::exists(file))
		{
			try {
				std::filesystem::remove(file);
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
	if (functionName == "fileExists") {
		if (std::filesystem::exists(file))
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