#pragma once

// Debug logging macros
#ifdef _DEBUG
#include <cstdio>
extern FILE* gLogFile;
#define LOG(text) fprintf(gLogFile, text); fflush(gLogFile)
#define LOG_0(text) fprintf(gLogFile, text); fflush(gLogFile)
#define LOG_1(text, a) fprintf(gLogFile, text, a); fflush(gLogFile)
#define LOG_2(text, a, b) fprintf(gLogFile, text, a, b); fflush(gLogFile)
#define LOG_3(text, a, b, c) fprintf(gLogFile, text, a, b, c); fflush(gLogFile)
#define LOG_4(text, a, b, c, d) fprintf(gLogFile, text, a, b, c, d); fflush(gLogFile)
#else
#define LOG(text)
#define LOG_0(text)
#define LOG_1(text, a)
#define LOG_2(text, a, b)
#define LOG_3(text, a, b, c)
#define LOG_4(text, a, b, c, d)
#endif
