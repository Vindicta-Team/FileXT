#pragma once

// Debug logging macros
#ifdef _DEBUG
#define LOG(text) printf text
#else
#define LOG(text)
#endif
