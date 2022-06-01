#pragma once

#include <cstdio>
#include <vector>

// Configure this when building locally to enable more verbose logging.
// By default verbose logging is enabled in debug builds.
#if !defined(NDEBUG)
#define ENABLE_LOG_VERBOSE
#endif

// Debug logging macros
#ifndef NDEBUG
#define LOG(...) log(__VA_ARGS__)
#else
#define LOG(...)
#endif

#ifdef ENABLE_LOG_VERBOSE
#define LOG_VERBOSE(...) log("FileXT: ", __VA_ARGS__);
#else 
#define LOG_VERBOSE(...) 
#endif

#define LOG_CRITICAL(...) logTrace(__LINE__, __FILE__, "FileXT: CRITICAL ERROR: ", __VA_ARGS__);

// Forward Decls
extern std::unique_ptr<FILE, decltype(&std::fclose)> gpFile;
std::filesystem::path& getLogPath();

template <typename ...Args>
void log(const char* format, Args&&...args) {
#if defined(__linux__) && !defined(NDEBUG)
	FILE* pFile = stderr;
#else
	FILE* pFile = gpFile.get();
#endif

	if (pFile != nullptr)
	{
		std::fprintf(pFile, format, args...);
		std::fflush(pFile);
	}
}

template <typename ...Args>
inline void logTrace(int line, const char* fileName, const char* format, Args&&...args) {
	size_t bufSize = std::snprintf(NULL, 0, "%s [%s] [line: %d]\n", format, fileName, line);
	std::vector<char> buf(bufSize + 1, '\0');
	std::snprintf(buf.data(), bufSize, "%s [%s] [line: %d]\n", format, fileName, line);
	log(buf.data(), args...);
}