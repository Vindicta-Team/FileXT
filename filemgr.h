#pragma once

/*
Class which represents whole extension.
*/

#include <map>
#include <string>

namespace filext
{
	struct fileInfo
	{
		fileInfo(std::string& fileName);
		~fileInfo();
		std::string m_fileName;
		std::unordered_map<std::string, std::string> m_map;
		unsigned int m_currentGetID;
		std::string m_currentGetKey;
	};

	class filemgr
	{
	public:
		int open(const std::string& fName);
		int close(const std::string& fName);
		int set(const std::string& fName, const char* key, const char* value);
		int get(const std::string& fName, const char* key, std::string& outValue, unsigned int outputSize, bool reset);
		int write(const std::string& fName);
		int read(const std::string& fName);

	private:
		std::map<std::string, fileInfo*> m_fileMap;
	};
};

