#pragma once

#include <cstdio>

// Configure
#define ENABLE_LOG_VERBOSE 1

// Debug logging macros
#ifndef NDEBUG
extern FILE* gLogFile;
#define LOG(...) fprintf(gLogFile?gLogFile:gLogFile=stderr, __VA_ARGS__); fflush(gLogFile)
#else
#define LOG(...)
#endif

#if ENABLE_LOG_VERBOSE && !defined(NDEBUG)
#define LOG_VERBOSE(...) fprintf(gLogFile?gLogFile:gLogFile=stderr, "FileXT: "); fprintf(gLogFile, __VA_ARGS__ ); fprintf(gLogFile, "\n"); fflush(gLogFile)
#elif ENABLE_LOG_VERBOSE
#define LOG_VERBOSE(...) fprintf(stderr, "FileXT: "); fprintf(stderr,  __VA_ARGS__ ); fprintf(stderr, "\n"); fflush(stderr)
#else 
#define LOG_VERBOSE(...) 
#endif

#if !defined(NDEBUG)
#define LOG_CRITICAL(...) fprintf(gLogFile?gLogFile:gLogFile=stderr, "FileXT: CRITICAL ERROR: "); fprintf(gLogFile, __VA_ARGS__ ); fprintf(gLogFile, "\n"); fflush(gLogFile)
#else
#define LOG_CRITICAL(...) fprintf(stderr, "FileXT: CRITICAL ERROR: "); fprintf(stderr,  __VA_ARGS__ ); fprintf(stderr, "\n"); fflush(stderr)
#endif