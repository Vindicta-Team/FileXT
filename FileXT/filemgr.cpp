#include "pch.h"
#include "filemgr.h"
#include "errorCodes.h"
#include "common.h"

using namespace filext;
using namespace std;


// FILEINFO STRUCT

fileInfo::fileInfo(std::string& fileName) :
	m_map(unordered_map<string, string>(512)),
	m_fileName(fileName),
	m_currentGetID(0),
	m_currentGetKey("")
{
	LOG("NEW fileInfo: %s", fileName.c_str());
}

fileInfo::~fileInfo()
{
	LOG("DELETE fileinfo: %s", m_fileName.c_str());
}




// FILEMGR

int filext::filemgr::open(const std::string& fName)
{
	LOG("open(%s)", fName.c_str());

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		LOG("  FOUND in m_fileMap");
	} else {
		LOG("  NOT found in m_fileMap");
		// Create a new entry
		fileInfo* finfoPtr = new fileInfo(fNameStr);
		m_fileMap[fNameStr] = finfoPtr;
	};

	return 0; // Success
}

int filext::filemgr::close(const std::string& fName)
{
	LOG("close(%s)", fName.c_str());

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		LOG("  FOUND in m_fileMap");
		delete (search->second);
		m_fileMap.erase(search);
	}
	else {
		LOG("  NOT found in m_fileMap");
		// Do nothing if it's closed already
	};

	return 0; // Success
}

int filext::filemgr::set(const std::string& fName, const char* key, const char* value)
{
	LOG("set(%s, %s, %s)", fName.c_str(), key, value);

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;
		finfo->m_map[string(key)] = string(value);
		return FILEXT_SUCCESS;
	} else {
		LOG("  File is not open");
		// Return error
		return FILEXT_ERROR_FILE_NOT_OPEN;
	};
}

int filext::filemgr::eraseKey(const std::string& fName, const char* key)
{
	LOG("eraseKey(%s, %s)", fName.c_str(), key);

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;
		finfo->m_map.erase(string(key));
		return FILEXT_SUCCESS;
	} else {
		LOG("  File is not open");
		// Return error
		return FILEXT_ERROR_FILE_NOT_OPEN;
	};
}

int filext::filemgr::get(const std::string& fName, const char* key, string& outValue, unsigned int outputSize, bool reset)
{
	LOG("get(%s, %s, reset: %i, outputSize: %i)", fName.c_str(), key, reset, outputSize);

	unsigned int nBytesToGet = outputSize - 1; // We need to reserve some space for null

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;
		string keyStr(key);
		auto searchKey = finfo->m_map.find(keyStr);
		if (searchKey != finfo->m_map.end()) {
			// Key is found in map
			string* value = &searchKey->second;
			LOG("  Key found, size: %i", (unsigned int) value->size());

			// Check if we were reading same key previously
			if (finfo->m_currentGetKey != keyStr || reset) {
				finfo->m_currentGetKey = keyStr;
				finfo->m_currentGetID = 0;
			}

			// Limit the amount of bytes to read if this read is final
			if (finfo->m_currentGetID + nBytesToGet >= value->size()) {
				nBytesToGet = (unsigned int)(value->size()) - finfo->m_currentGetID;
			}

			LOG("  Returning bytes: %i", nBytesToGet);

			// Read up to outputSize bytes
			outValue = value->substr(finfo->m_currentGetID, nBytesToGet);

			// Increase the currentGetID by the amount of data read
			finfo->m_currentGetID += nBytesToGet;

			if (finfo->m_currentGetID >= value->size()) {
				// We have read everything
				finfo->m_currentGetID = 0;
				return FILEXT_SUCCESS;
			} else {
				// We have read only part of value yet
				return FILEXT_GET_MORE_AVAILABLE;
			}
		} else {
			LOG("  Key %s was not found", key);
			return FILEXT_ERROR_KEY_NOT_FOUND;
		}
	} else {
		LOG("  File is not open");
		// Return error
		return FILEXT_ERROR_FILE_NOT_OPEN;
	}
}

int filext::filemgr::write(const std::string& fName)
{
	LOG("write(%s)", fName.c_str());

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;

		ofstream f(finfo->m_fileName, ios_base::out | ios_base::binary);
		char endKey[] = { 0 };
		char endVal[] = { 0, '\n' };
		if (f.is_open()) {
			// Write header
			fileHeader header;
			header.version = FILEXT_HEADER_VERSION;
			header.size = sizeof(fileHeader);
			header.magicNumber = FILEXT_HEADER_MAGIC_NUMBER;
			f.write((const char*)&header, sizeof(fileHeader));

			// Write data
			for (auto iter = finfo->m_map.begin(); iter != finfo->m_map.end(); ++iter) {
				const string* key = &(iter->first);
				const string* val = &(iter->second);

				LOG("    writing %s, size: %i", iter->first.c_str(), (int)val->size());

				f.write(key->data(), key->size());
				f.write(endKey, sizeof(endKey));
				f.write(val->data(), val->size());
				f.write(endVal, sizeof(endVal));
			}
			return FILEXT_SUCCESS;
		}
		else {
			LOG("    Error writing file: file is not open");
			return FILEXT_ERROR_WRITE;
		}

	} else {
		LOG("  File is not open");
		// Return error
		return FILEXT_ERROR_FILE_NOT_OPEN;
	}
	return 0;
}

int filext::filemgr::read(const std::string& fName)
{
	LOG("read(%s)", fName.c_str());

	// Check if this file is already open
	string fNameStr(fName);
	auto search = m_fileMap.find(fNameStr);
	if (search != m_fileMap.end()) {
		fileInfo* finfo = search->second;

		ifstream f(finfo->m_fileName, ios_base::in | ios_base::binary);
		if (f.is_open()) {
			// Read header
			LOG("  Reading header: ");
			fileHeader header;
			f.seekg(0);
			f.read((char*)&header, sizeof(fileHeader));
			bool magicNumberOk = header.magicNumber == FILEXT_HEADER_MAGIC_NUMBER;
			LOG("size: %i, version: %i, magic number ok: %i", header.size, header.version, (int)magicNumberOk);

			// Bail if header is wrong
			if (!magicNumberOk) {
				LOG("Error: Magic number mismatch!");
				return FILEXT_ERROR_WRONG_FILE_FORMAT;
			}

			// Bail if version is not supported
			if (header.version != 1) {
				LOG("Error: version is not supported!");
				return FILEXT_ERROR_WRONG_FILE_FORMAT;
			}

			// Read content into RAM
			LOG("  File was found in file system. Reading content... ");
			f.seekg(0, std::ios::end);
			unsigned int contentSize = (unsigned int)f.tellg() - header.size;
			char* fContent = new char[contentSize];
			f.seekg(header.size);
			f.read(fContent, contentSize);
			f.close();

			// Move data to hashmap
			unsigned int i = 0;
			while (i < contentSize) {
				// Find end of key
				unsigned int iKeyStart = i;
				while (fContent[i] != 0)
					i++;
				i++;	// End of key is always {0}

				// Find end of value
				unsigned int iValStart = i;
				while (fContent[i] != 0)
					i++;
				i += 2; // End of value is always {0, '\n'}

				string key(&fContent[iKeyStart]);
				string val(&fContent[iValStart]);
				finfo->m_map[key] = val;

				LOG("    Added %s: size: %i", key.c_str(), (int)val.size());
			};

			delete[]fContent;

			LOG("    Ended reading content");

			return FILEXT_SUCCESS;
		}
		else {
			LOG("    Error reading file: file is not open");
			return FILEXT_ERROR_READ;
		};


	} else {
		LOG("  File is not open");
		// Return error
		return FILEXT_ERROR_FILE_NOT_OPEN;
	}
}