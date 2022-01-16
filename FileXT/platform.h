#pragma once

#if defined(_MSC_VER)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>

#define FILEXT_EXPORT __declspec(dllexport)
#define FILEXT_IMPORT __declspec(dllimport)
#define FILEXT_CALL __stdcall

#else

#define FILEXT_EXPORT __attribute__((visibility("default")))
#define FILEXT_IMPORT
#define FILEXT_CALL

#endif