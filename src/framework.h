#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
// Windows Header Files
#include <windows.h>
#endif

// STD header files
#include <string>
//#include <cstring>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#ifdef _DEBUG
#include <stdio.h>
#endif
