#include "pch.h"
#include "Debugger.h"

namespace BE
{
	Debug::LogMessage::LogMessage(LogLevel _lv, LogLocation _lc, int _ln, std::string&& _file, const char* _msg)
		: level(_lv), location(_lc), line(_ln), file(std::move(_file)), message(_msg)
	{

	}

	bool Debug::LogMessage::operator==(const Debug::LogMessage& lm)
	{
		if (this->level != lm.level)
			return false;

		if (this->line != lm.line)
			return false;

		if (this->location != lm.location)
			return false;

		if (this->file != lm.file)
			return false;

		if (this->message != lm.message)
			return false;

		return true;
	}




	Debug::Debug()
	{
#ifndef USE_EDITOR
		SetDebugFlag(DebugFlags::imguiPause, false);
#else
		SetDebugFlag(DebugFlags::imguiPause, true);
#endif

#ifndef USE_DEBUGGER
		//Release build - don't need to debug anything
		SetDebugFlag(DebugFlags::logToFile, false);
		SetDebugFlag(DebugFlags::enableRecording, false);
		SetDebugFlag(DebugFlags::debugDraw, false);
		SetDebugFlag(DebugFlags::debugStep, false);
#else
		//Debug build - Initialise debugger
		SetDebugFlag(DebugFlags::logToFile, true);
		SetDebugFlag(DebugFlags::enableRecording, true);
		SetDebugFlag(DebugFlags::debugDraw, false);
		SetDebugFlag(DebugFlags::debugStep, false);


		//If going to log to file, then create a file for it
		if (GetDebugFlag(DebugFlags::logToFile))
		{
			std::string filePath;
			filePath += "..\\Logs\\BANANA_ENGINE_LOG-";

			//Gets the current time
			std::time_t now = time(0);
			tm timeData;
			localtime_s(&timeData, &now);

			//Constructing the file path based on time and day
			//If the value is less than 10, we'll append a '0' in front
			filePath += ((timeData.tm_mday < 10) ? '0' + std::to_string(timeData.tm_mday) : std::to_string(timeData.tm_mday)) + '_';
			filePath += ((++timeData.tm_mon < 10) ? '0' + std::to_string(timeData.tm_mon) : std::to_string(timeData.tm_mon)) + '_';
			filePath += std::to_string(1900 + timeData.tm_year) + "-";
			filePath += ((timeData.tm_hour < 10) ? '0' + std::to_string(timeData.tm_hour) : std::to_string(timeData.tm_hour)) + '_';
			filePath += ((timeData.tm_min < 10) ? '0' + std::to_string(timeData.tm_min) : std::to_string(timeData.tm_min)) + '_';
			filePath += ((timeData.tm_sec < 10) ? '0' + std::to_string(timeData.tm_sec) : std::to_string(timeData.tm_sec)) + ".log";

			m_ofs.open(filePath, std::ios::out);

			//If file cannot be opened, then just disable logging to file
			if (!m_ofs.is_open())
				SetDebugFlag(DebugFlags::logToFile, false);
		}
#endif
	}

	Debug::~Debug()
	{

	}

	void Debug::SetEditorProperties(float _windowWidth, float _windowHeight, float _windowPosX, float _windowPosY, float _mousePosX, float _mousePosY)
	{
		m_editorWindowSize = std::make_pair(_windowWidth, _windowHeight);
		m_editorWindowPos = std::make_pair(_windowPosX, _windowPosY);
		m_editorMousePos = std::make_pair(_mousePosX - _windowPosX, _mousePosY - _windowPosY);
	}

	


	void Debug::DebugLog(LogLevel lv, LogLocation lc, int line, const char* file, const char* format, ...)
	{
		//If not recording, then just return
		if (!GetDebugFlag(DebugFlags::enableRecording))
			return;

		std::string finalmessage;
		finalmessage.reserve(MAX_STR_BUFFER);

		//Appends the Log level
		finalmessage += (lv == LogLevel::info) ? "> [LOG|" : (lv == LogLevel::warn) ? "> [WRN|" : "> [ERR|";

		//info from which side - Core? Editor? Application?
		finalmessage += (lc == LogLocation::engine) ? "COR] [" : (lc == LogLocation::editor) ? "EDI] [" : "APP] [";

		//Which line is the log being called
		finalmessage += std::to_string(line) + '|';

		//Which file was the logging at, we will need to truncate the file name
		std::string fileName(file);
		size_t pos = fileName.find_last_of('\\');
		fileName.erase(0, pos + 1);
		finalmessage += fileName + "] ";

		//Printing out the arguments
		char logMessages[MAX_STR_BUFFER];
		va_list args;
		va_start(args, format);
		vsnprintf_s(logMessages, MAX_STR_BUFFER, _TRUNCATE, format, args);
		va_end(args);

		finalmessage += logMessages;

		//Print to console
		std::cout << finalmessage << std::endl;

		//If log to file is enable, then also print to file
		if (GetDebugFlag(DebugFlags::logToFile))
			m_ofs << finalmessage << std::endl;

		//Create a log message for storing into recent messages, used for imgui console output
		LogMessage logMsg{ lv, lc, line, std::move(fileName), logMessages };

		//Looks for same message and just increase the count
		for (size_t i = 0; i < m_recentLogs.size(); ++i)
		{
			if (m_recentLogs[i].first == logMsg)
			{
				++m_recentLogs[i].second;
				return;
			}
		}

		//If too many logs, clear the old ones
		if (m_recentLogs.size() >= MAX_LOG_SIZE)
			m_recentLogs.pop_front();

		m_recentLogs.push_back(std::make_pair(logMsg, 1));
	}
}