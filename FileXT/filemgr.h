#pragma once

/*
Class which represents whole extension.
*/

#include <map>
#include <string>
#include <filesystem>
#include <stdint.h>

namespace filext
{
	struct fileInfo
	{
		fileInfo(const std::filesystem::path& filepath);
		~fileInfo();
		std::filesystem::path m_filepath;
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
		int open(const std::filesystem::path& filepath);
		int close(const std::filesystem::path& filepath);
		int set(const std::filesystem::path& filepath, const char* key, const char* value);
		int get(const std::filesystem::path& filepath, const char* key, std::string& outValue, unsigned int outputSize, bool reset);
		int eraseKey(const std::filesystem::path& filepath, const char* key);
		int write(const std::filesystem::path& filepath);
		int read(const std::filesystem::path& filepath);

	private:
		std::map<std::filesystem::path, fileInfo*> m_fileMap;
	};
};

