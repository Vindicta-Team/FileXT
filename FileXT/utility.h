#pragma once

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

namespace string
{
	/// <summary>
	/// Trims the string of all /tokens/ from the start of the string to the first character that isn't in /tokens/.
	/// This functions modifies the original string.
	/// </summary>
	/// <param name="s">String to trim</param>
	/// <param name="tokens">String of individual characters to trim</param>
	inline void ltrim(std::string& s, std::string tokens) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&tokens](unsigned char ch) {
			return tokens.find(ch) == std::string::npos;
			}));
	}

	/// <summary>
	/// Trims the string of all /tokens/ from the end of the string to the first character that isn't in /tokens/.
	/// This functions modifies the original string.
	/// </summary>
	/// <param name="s">String to trim</param>
	/// <param name="tokens">String of individual characters to trim</param>
	inline void rtrim(std::string& s, std::string tokens) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [&tokens](unsigned char ch) {
			return tokens.find(ch) == std::string::npos;
			}).base(), s.end());
	}

	/// <summary>
	/// Trims the string of all /tokens/ from the start and end of the string to the first character on each side that isn't in /tokens/.
	/// This functions modifies the original string.
	/// </summary>
	/// <param name="s">String to trim</param>
	/// <param name="tokens">String of individual characters to trim</param>
	inline void trim(std::string& s, std::string tokens) {
		ltrim(s, tokens);
		rtrim(s, tokens);
	}

	/// <summary>
	/// Trims the string of all /tokens/ from the start of the string to the first character that isn't in /tokens/.
	/// This function creates a new string.
	/// </summary>
	/// <param name="s">String to trim</param>
	/// <param name="tokens">String of individual characters to trim</param>
	inline std::string trim(const char* szInput, std::string tokens) {
		std::string s(szInput);
		ltrim(s, tokens);
		rtrim(s, tokens);
		return s;
	}

	/// <summary>
	/// Checks if /token/ appears at the immediate start of /str/.
	/// This check is case-sensitive.
	/// </summary>
	/// <param name="str">The string to check for /token/.</param>
	/// <param name="token">The string to find.</param>
	/// <returns>True if /token/ appears at the immediate start of /str/. False otherwise.</returns>
	inline bool startsWith(const std::string& str, const std::string& token) {
		if (str.size() < token.size()) {
			return false;
		}

		for (int i = 0; i < token.size(); i++) {
			if (str.at(i) != token.at(i)) {
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Replaces occurrences of token with replacement in subject
	/// </summary>
	/// <param name="subject">The subject to execute replacements on</param>
	/// <param name="token">The token to replace</param>
	/// <param name="replacement">The replacement to insert</param>
	inline void replaceAll(std::string& subject, const std::string& token, const std::string& replacement) {
		if (token.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = subject.find(token, start_pos)) != std::string::npos) {
			subject.replace(start_pos, token.length(), replacement);
			start_pos += replacement.length();
		}
	}
}

namespace filext
{
	/// <summary>
	/// Depending on platform and profile related command-line arguments, this 
	/// function builds a path to the current profile folder.
	/// </summary>
	/// <returns>Profile folder path.</returns>
	std::filesystem::path BuildProfilePath(const std::filesystem::path& folder, const std::string& name);

	/// <summary>
	/// Converts characters of the -name argument the same as the Arma executable does
	/// </summary>
	/// <param name="name">The string of the -name argument</param>
	/// <returns>A converted string that should match Arma's conversion</returns>
	std::string convertName(const std::string& name);

	/// <summary>
	/// Attempts to get the folder that holds the Arma Server profile data.
	/// If it cannot find the Profile folder, it will fall-back to the mod folder.
	/// On total failure, this function raises a std::runtime_error.
	/// </summary>
	/// <returns>Profile folder path.</returns>
	std::filesystem::path findProfileStorage();

	/// <summary>
	/// Attempts to get the parent folder of the FileXT DLL. (FileXT mod folder).
	/// </summary>
	/// <returns>Folder path of FileXT mod. Empty path on failure.</returns>
	std::filesystem::path findModStorage();

	/// <summary>
	/// Attempts to get the profile name that would be used by default for the 
	/// server if no profile name was specified on the command-line.
	/// </summary>
	/// <returns>Profile name</returns>
	std::string getDefaultProfileName();

	/// <summary>
	/// Gets (and stores internally) a suitable path for the log file. (Stored in storage directory).
	/// </summary>
	/// <returns></returns>
	const std::filesystem::path& getLogPath();

	/// <summary>
	/// Gets (and stores internally) a suitable location for storage data.
	/// This will either be the server profile folder, or the FileXT mod folder.
	/// </summary>
	/// <returns>Path to storage location.</returns>
	const std::filesystem::path& getStorage();

	/// <summary>
	/// Returns the path to the default user folder for the host platform.
	/// </summary>
	/// <returns>Path to the user folder.</returns>
	std::filesystem::path getSystemUserFolder();

	/// <summary>
	/// Tries to migrate any files from the mod folder to the new profile folder.
	/// </summary>
	void tryMigrateStorageFolder();
}

namespace process
{
#ifdef _WIN32
	/// <summary>
	/// Converts a string the same way Arma does (an incorrect Unicode conversion)
	/// </summary>
	/// <param name="name">The wide string as provided to the application (original)</param>
	/// <param name="out">Reference to string for the converted output</param>
	/// <returns>True if the argument was converted successfully</returns>
	bool convertWinArg(const wchar_t* in, std::string& out);
#endif

#ifdef __linux__
	/// <summary>
	/// Converts a string the same way Arma does (an incorrect Unicode conversion)
	/// </summary>
	/// <param name="name">The string as provided to the application (original)</param>
	/// <param name="out">Reference to string for the converted output</param>
	/// <returns>True if the argument was converted successfully</returns>
	bool convertLinuxArg(const std::string& in, std::string& out);
#endif

	/// <summary>
	/// Returns the full path of the current executing process. (eg, arma3server_x64.exe)
	/// </summary>
	/// <returns>Path to the current process executable.</returns>
	std::filesystem::path getProcessExecutable();

	/// <summary>
	/// Returns the parent path of the current executing process. (Arma root folder).
	/// </summary>
	/// <returns>Path to parent folder of current process.</returns>
	std::filesystem::path getArmaRootFolder();

	/// <summary>
	/// Attempts to get the command line arguments used to call this process.
	/// </summary>
	/// <returns>Contiguous list of arguments used to start this process</returns>
	std::vector<std::string> tryGetCommandLineArgs();
}