#include "pch.h"
#include "filemgr.h"
#include "errorCodes.h"
#include "common.h"

using namespace filext;
using namespace std;

int filext::filemgr::open(const std::string& fName)
{
	LOG(("open(%s)\n", fName.c_str()));

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		LOG(("  FOUND in m_fileMap\n"));
	} else {
		LOG(("  NOT found in m_fileMap\n"));
		// Create a new entry
		fileInfo* finfoPtr = new fileInfo(fNameStr);
		m_fileMap[fNameStr] = finfoPtr;
	};

	return 0; // Success
}

int filext::filemgr::close(const std::string& fName)
{
	LOG(("close(%s)\n", fName.c_str()));

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		LOG(("  FOUND in m_fileMap\n"));
		delete (search->second);
		m_fileMap.erase(search);
	}
	else {
		LOG(("  NOT found in m_fileMap\n"));
		// Do nothing if it's closed already
	};

	return 0; // Success
}

int filext::filemgr::set(const std::string& fName, const char* key, const char* value)
{
	LOG(("set(%s, %s, %s)\n", fName.c_str(), key, value));

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;
		finfo->m_map[string(key)] = string(value);
		return FILEXT_SUCCESS;
	} else {
		LOG(("  File is not open\n"));
		// Return error
		return FILEXT_FILE_NOT_OPEN;
	};
}

int filext::filemgr::get(const std::string& fName, const char* key, string& outValue)
{
	LOG(("get(%s, %s)\n", fName.c_str(), key));

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;
		auto searchKey = finfo->m_map.find(string(key));
		if (searchKey != finfo->m_map.end()) {
			outValue = searchKey->second;
			return FILEXT_SUCCESS;
		} else {
			LOG(("  Key %s was not found\n", key));
			return FILEXT_KEY_NOT_FOUND;
		}
	} else {
		LOG(("  File is not open\n"));
		// Return error
		return FILEXT_FILE_NOT_OPEN;
	}
}

int filext::filemgr::write(const std::string& fName)
{
	LOG(("write(%s)\n", fName.c_str()));

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;
		finfo->write();
	}
	else {
		LOG(("  File is not open\n"));
		// Return error
		return FILEXT_FILE_NOT_OPEN;
	}
	return 0;
}
