#pragma once

/*
Class which represents whole extension.
*/

#include <map>
#include <string>
#include <stdint.h>

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

#pragma pack(push)
#pragma pack(4)
	// File header, written at the top of all files
#define FILEXT_HEADER_MAGIC_NUMBER 0x00420666
#define FILEXT_HEADER_VERSION 1
	struct fileHeader
	{
		// These three should always stay same:
		uint32_t magicNumber;
		uint32_t size;
		uint32_t version;
	};
#pragma pack(pop)

	class filemgr
	{
	public:
		int open(const std::string& fName);
		int close(const std::string& fName);
		int set(const std::string& fName, const char* key, const char* value);
		int get(const std::string& fName, const char* key, std::string& outValue, unsigned int outputSize, bool reset);
		int eraseKey(const std::string& fName, const char* key);
		int write(const std::string& fName);
		int read(const std::string& fName);

	private:
		std::map<std::string, fileInfo*> m_fileMap;
	};
};

