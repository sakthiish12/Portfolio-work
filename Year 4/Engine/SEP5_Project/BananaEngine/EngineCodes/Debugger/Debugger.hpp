#pragma once

namespace BE
{
	template <typename... Args>
	void Debug::Assertion(bool condition, const char* expToCheck, LogLocation lc, int line, const char* file, const char* format, Args... args)
	{
		//If condition is successful, then return
		if (condition)
			return;

		//What file did the assertion failed?
		std::string fileName(file);
		size_t pos = fileName.find_last_of('\\');
		fileName.erase(0, pos + 1);

		//Appending "Assertion failed in front
		std::string additionalMsg("Assertion failed: ");
		additionalMsg += expToCheck;
		additionalMsg += ". ";
		additionalMsg += format;

		DebugLog(LogLevel::error, lc, line, file, additionalMsg.c_str(), args...);
		assert(false);
	}
}