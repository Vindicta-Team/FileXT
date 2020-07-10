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
		// Read content into RAM
		LOG(("  File was found in file system. Reading content... \n"));
		f.seekg(0, std::ios::end);
		auto fSize = f.tellg();
		char* fContent = new char[fSize];
		f.seekg(0, std::ios::beg);
		f.read(fContent, fSize);
		f.close();

		// Move data to hashmap
		unsigned int i = 0;
		while (i < fSize) {
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
			m_map[key] = val;

			LOG(("    Added %s: size: %i\n", key.c_str(), (int)val.size()));
		};

		delete []fContent;
	}
	else {
		LOG(("  File was not found in file system.\n"));
	}
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
