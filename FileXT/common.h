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
#define LOG(format, ...) log("FileXT: " format "\n", ##__VA_ARGS__)
#else
#define LOG(...)
#endif

#ifdef ENABLE_LOG_VERBOSE
#define LOG_VERBOSE(format, ...) log("FileXT [VERBOSE]: " format "\n", ##__VA_ARGS__);
#else 
#define LOG_VERBOSE(...) 
#endif

#define LOG_CRITICAL(format, ...) logTrace(__LINE__, __FILE__, "FileXT [CRITICAL ERROR]: " format " [%s] [line: %d]\n", ##__VA_ARGS__);

#ifndef NDEBUG
#define LOG_G(...) log_g(__VA_ARGS__)
#else
#define LOG_G(...)
#endif

// Forward Decls
extern std::unique_ptr<FILE, decltype(&std::fclose)> gpFile;

template <typename ...Args>
void log(const char* format, Args&&...args) {
#if defined(__linux__) && !defined(NDEBUG)
	FILE* pFile = stderr;
#else
	FILE* pFile = gpFile.get();
#endif

	if (pFile != nullptr) {
		std::fprintf(pFile, format, args...);
		std::fflush(pFile);
	}
}

template <typename ...Args>
inline void logTrace(int line, const char* fileName, const char* format, Args&&...args) {
	log(format, args..., fileName, line);
}

// Guaranteed to print somewhere, even without a log file.
// Windows: Visual Studio output window
// Linux: Terminal window
#ifndef NDEBUG
template <typename ...Args>
inline void log_g(const char* format, Args...args) {
#if defined(_WIN32)
	std::vector<char> buf;

	size_t bufSize = std::snprintf(nullptr, 0, format, args...);
	buf.resize(++bufSize);

	std::snprintf(buf.data(), bufSize, format, args...);
	::OutputDebugStringA(buf.data());
#else
	fprintf(stderr, format, args...);
	fflush(stderr);
#endif
}
#endif