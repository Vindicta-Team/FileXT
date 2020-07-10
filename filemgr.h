#pragma once

/*
Class which represents whole extension.
*/

#include <map>
#include <string>
#include "fileInfo.h"

namespace filext
{
	class filemgr
	{
	public:
		int open(const std::string & fName);
		int close(const std::string & fName);
		int set(const std::string& fName, const char* key, const char* value);
		int get(const std::string& fName, const char* key, std::string& outValue);

	private:
		std::map<std::string, fileInfo*> m_fileMap;
	};
};

