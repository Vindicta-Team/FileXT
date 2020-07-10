#pragma once

/*
fileInfo stores information about a file and all file content data in form accessible by SQF.
*/

#include <string>
#include <unordered_map>
#include <fstream>

namespace filext
{
	struct fileInfo
	{
		fileInfo(std::string &fileName);
		~fileInfo();
		std::string m_fileName;
		std::unordered_map<std::string, std::string> m_map;
	};
};