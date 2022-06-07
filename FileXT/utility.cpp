#include "pch.h"
#include "utility.h"

#include "common.h"
#include "platform.h"

#if defined(WIN32)
#include <shellapi.h>
#include <shlobj.h>
#include <direct.h>
#include <Lmcons.h>
#ifndef getcwd
#define getcwd _getcwd
#endif
#endif

#ifdef __linux__
#include <dlfcn.h>
#include <iconv.h>
#include <pwd.h>
#include <unistd.h>
#include <fstream>
#endif

#include <array>

namespace filext
{
	// Because f* you that's why
	std::filesystem::path BuildProfilePath(const std::filesystem::path& folder, const std::string& name) {
		std::filesystem::path path;

		// No Folder, No Name
		// Linux:   "~share/Arma 3 - Other Profiles/Player"
		// Windows: "%UserProfile%\Documents\Arma 3"
		if (folder.empty() && name.empty())
		{
			path = getSystemUserFolder();
#ifdef _WIN32
			path /= "Arma 3";
#elif defined(__linux__)
			path /= "Arma 3 - Other Profiles";
			path.append(getDefaultProfileName());
#endif
		}

		// Absolute Folder, No Name
		// Linux:   "$folder/home/Player"
		// Windows: "$folder\users\%AccountName%"
		else if (!folder.empty() && folder.is_absolute() && name.empty())
		{
			path = folder;
#ifdef _WIN32
			path /= "users";
#elif defined(__linux__)
			path /= "home";
#endif
			path.append(getDefaultProfileName());
		}

		// Relative Folder, No Name
		// Linux:   "%ArmaRoot%/$folder/home/Player"
		// Windows: "%ArmaRoot%\$folder\users\%AccountName%"
		else if (!folder.empty() && folder.is_relative() && name.empty())
		{
			path = process::getArmaRootFolder() / folder;
#ifdef _WIN32
			path /= "users";
#elif defined(__linux__)
			path /= "home";
#endif
			path.append(getDefaultProfileName());
		}

		// No Folder, Name
		// Linux:   "~share/Arma 3 - Other Profiles/$name"
		// Windows: "%UserProfile%\Documents\Arma 3 - Other Profiles\$name"
		else if (folder.empty() && !name.empty())
		{
			path = getSystemUserFolder() / "Arma 3 - Other Profiles";
			path.append(convertName(name));
		}

		// Absolute Folder, Name
		// Linux:   "$folder/home/$name"
		// Windows: "$folder\users\$name"
		else if (!folder.empty() && folder.is_absolute() && !name.empty())
		{
			path = folder;
#ifdef _WIN32
			path /= "users";
#elif defined(__linux__)
			path /= "home";
#endif
			path /= convertName(name);
		}

		// Relative Folder, Name
		// Linux:   "%ArmaRoot%/$folder/home/$name"
		// Windows: "%ArmaRoot%\$folder\users\$name"
		else if (!folder.empty() && folder.is_relative() && !name.empty())
		{
			path = process::getArmaRootFolder() / folder;
#ifdef _WIN32
			path /= "users";
#elif defined(__linux__)
			path /= "home";
#endif
			path /= convertName(name);
		}

		return path;
	}

	std::string convertName(const std::string& name) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		constexpr std::array<char, 16> whitelist {
			'!', '$', '^', '&', '(', ')', '~', '#', '@', '{', '}', '[', ']', '_', '+', '\''
		};

		for (std::string::const_iterator i = name.begin(), n = name.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			// Keep alphanumeric and other accepted characters intact
			if (std::isalnum(c) || std::find(whitelist.cbegin(), whitelist.cend(), c) != whitelist.cend()) {
				escaped << c;
				continue;
			}
			else if (c == '%') {
				escaped << "%%";
				continue;
			}

			// Any other characters are percent-encoded
			escaped << '%' << std::setw(2) << int((unsigned char)c);
		}

		return escaped.str();
	}

	std::filesystem::path findProfileStorage() {
		std::filesystem::path path;

		// Find profile folder and name arguments in Arma process command line
		std::string profileName;
		std::string profileFolder;

		for (auto& arg : process::tryGetCommandLineArgs()) {
			if (profileName.empty() && string::startsWith(arg, "-name=")) {
				profileName = arg.substr(6);
			}
			else if (profileFolder.empty() && string::startsWith(arg, "-profiles=")) {
				profileFolder = arg.substr(10);
			}

			if (!profileName.empty() && !profileFolder.empty()) {
				break;
			}
		}

		std::filesystem::path profilePath = BuildProfilePath(profileFolder, profileName);
		if (std::filesystem::exists(profilePath)) {
			path = profilePath / "storage";
			if (!std::filesystem::exists(path)) {
				std::filesystem::create_directories(path);
			}
		}

		return path;
	}

	std::filesystem::path findModStorage() {
		std::filesystem::path modStorage;

#ifdef _WIN32
		// Borrowed from https://gist.github.com/pwm1234/05280cf2e462853e183d
		char path[1024];
		HMODULE hm = NULL;

		if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)findModStorage,
			&hm)) {
			LOG("error: GetModuleHandle returned %i\n", GetLastError());
		}
		else {
			GetModuleFileNameA(hm, path, sizeof(path));
			modStorage = path;
		}
#elif defined(__linux__) 
		Dl_info dl_info;
		dladdr((void*)findModStorage, &dl_info);
		modStorage = dl_info.dli_fname;
#endif

		if (!modStorage.empty()) {
			modStorage = modStorage.parent_path() / "storage"; // Strip binary name
		}

		if (!std::filesystem::exists(modStorage)) {
			std::filesystem::create_directories(modStorage);
		}

		return modStorage;
	}

	std::string getDefaultProfileName() {
		std::string name;
#ifdef _WIN32
		char username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		GetUserName(username, &username_len);
		name = username;
#elif defined(__linux__)
		name = "Player";
#endif
		return name;
	}

	const std::filesystem::path& getLogPath() {
		static std::filesystem::path logPath;

		if (logPath.empty()) {
			logPath = getStorage().parent_path(); // Storage is always a sub-directory
		}

		return logPath;
	}

	const std::filesystem::path& getStorage() {
		static std::filesystem::path storage;

		if (storage.empty()) {
			// If locally hosted (hosted via client) - don't try to find profile storage
			const std::string executable = process::getProcessExecutable().filename().string();
			if (string::startsWith(executable, "arma3server"))
			{
				auto profileFolder = findProfileStorage();
				if (!profileFolder.empty()) {
					storage = profileFolder;
					return storage;
				}
			}

			auto modFolder = findModStorage();
			if (!modFolder.empty()) {
				storage = modFolder;
				return storage;
			}

			if (!std::filesystem::exists(storage)) {
				throw new std::runtime_error("Failed to find a suitable storage folder.");
			}
		}

		return storage;
	}

	std::filesystem::path getSystemUserFolder() {
		std::filesystem::path userDir;
#ifdef _WIN32
		// Folder = Documents
		PWSTR documentPath = nullptr;
		HRESULT hresult = SHGetKnownFolderPath(FOLDERID_Documents, NULL, NULL, &documentPath);
		if (SUCCEEDED(hresult)) {
			std::string temp;
			size_t len = wcstombs(nullptr, documentPath, 0);
			temp.resize(len);
			std::wcstombs(temp.data(), documentPath, len);
			userDir = temp;
		}
		CoTaskMemFree(documentPath);
#elif defined(__linux__)
		// Folder = /home/~user/.local/share
		struct passwd* record = getpwuid(geteuid());
		userDir.append(record->pw_dir)
			.append(".local")
			.append("share");
#endif
		return userDir;
	}

	void tryMigrateStorageFolder() {
		static bool isMigrated = false;

		if (!isMigrated) {
			isMigrated = true;

			static const std::filesystem::path& oldStorage = findModStorage();
			static const std::filesystem::path& storage = getStorage();

			// Don't migrate if we fell back to mod storage
			if (oldStorage != storage) {

				// Migrate any files from deprecated storage
				if (std::filesystem::exists(oldStorage)) {
					for (const auto& dirEntry : std::filesystem::directory_iterator(oldStorage)) {
						const std::filesystem::path storageFile = storage / dirEntry.path().filename();

						// Don't overwrite any files that may already exist
						if (!std::filesystem::exists(storageFile)) {
							std::filesystem::copy(dirEntry, storageFile);
						}
					}
				}
			}
		}
	}
}

namespace process
{
#if defined(_WIN32)
	bool convertWinArg(const wchar_t* in, std::string& out) {
		int clen = WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, NULL) - 1; // -1 to not write the null terminator
		out.resize(clen);

		if (WideCharToMultiByte(CP_UTF8, 0, in, clen, out.data(), clen, NULL, NULL) != clen) {
			return false;
		}

		return true;
	}
#endif

#if defined(__linux__)
	bool convertLinuxArg(const std::string& in, std::string& out)
	{
		iconv_t conv = iconv_open("UTF-8", "WINDOWS-1252");
		if (conv != (iconv_t)-1) {
			out.resize(in.size() * 4);

			// Mutable 'cursors' for iconv
			char* pIn = const_cast<char*>(in.data()); // inbuffer isn't mutated
			size_t inLen = in.size();
			char* pOut = out.data();
			size_t outLen = out.size();

			while (true) {
				size_t rc = iconv(conv, &pIn, &inLen, &pOut, &outLen);
				if (rc == (size_t)-1) {
					if (inLen > 0) {
						// HACK: Attempt direct copy with ex code
						*pOut = 0xC2; ++pOut; --outLen;
						*pOut = *pIn; ++pOut; --outLen;
						++pIn; --inLen;
						continue;
					}
				}

				break;
			}

			iconv_close(conv);
			return true;
		}

		return false;
	}
#endif

	std::filesystem::path getProcessExecutable() {
		char szPath[1024];
#ifdef _WIN32
		GetModuleFileNameA(NULL, szPath, 1024);
#elif defined(__linux__)
		readlink("/proc/self/exe", szPath, 1024);
#endif
		return szPath;
	}

	std::filesystem::path getArmaRootFolder() {
		return getProcessExecutable().parent_path();
	}

	std::vector<std::string> tryGetCommandLineArgs() {
		std::vector<std::string> args;

#ifdef _WIN32
		int n_args;
		wchar_t** argv16 = CommandLineToArgvW(GetCommandLineW(), &n_args);

		if (argv16 != NULL) {
			for (int i = 0; i < n_args; i++) {
				wchar_t* arg = argv16[i];
				std::string converted;
				if (convertWinArg(arg, converted)) {
					args.emplace_back(converted);
				}
			}
		}
		LocalFree(argv16);

#elif defined(__linux__)
		// Taken from DTool library in Panda3D
		std::ifstream proc("/proc/self/cmdline");
		if (!proc.fail()) {
			int ch = proc.get();
			int index = 0;
			while (!proc.eof() && !proc.fail()) {
				std::string arg;

				while (!proc.eof() && !proc.fail() && ch != '\0') {
					arg += (char)ch;
					ch = proc.get();
				}

				if (index > 0) {
					std::string converted;
					if (convertLinuxArg(arg.c_str(), converted)) {
						args.push_back(converted);
					}
				}

				index++;

				ch = proc.get();
			}
		}
#endif
		return args;
	}
}