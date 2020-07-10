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

void filext::fileInfo::write()
{
	ofstream f(m_fileName, ios_base::out | ios_base::binary);
	char endKey[] = {0};
	char endVal[] = {0, '\n'};
	for (auto iter = m_map.begin(); iter != m_map.end(); ++iter) {
		const string* key = &(iter->first);
		const string* val = &(iter->second);

		LOG(("    writing %s, size: %i\n", iter->first.c_str(), (int)val->size()));

		f.write(key->data(), key->size());
		f.write(endKey, sizeof(endKey));
		f.write(val->data(), val->size());
		f.write(endVal, sizeof(endVal));
	}
	f.close();
}
