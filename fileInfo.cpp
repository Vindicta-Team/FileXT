#include "pch.h"
#include "fileInfo.h"
#include "common.h"

using namespace std;
using namespace filext;

fileInfo::fileInfo(std::string& fileName) :
	m_map(unordered_map<string, string>(512)),
	m_fileName(fileName)
{
	LOG(("NEW fileInfo: %s\n", fileName.c_str()));

	// Attempt to read the file
	ifstream f(fileName, ios_base::in | ios_base::binary);
	if (f.is_open()) {
		// Read content
		LOG(("  File was found in file system. Reading content... NYI.\n"));
	}
	else {
		LOG(("  File was not found in file system.\n"));
	}
	f.close();
}

fileInfo::~fileInfo()
{
	LOG(("DELETE fileinfo: %s\n", m_fileName.c_str()));
}