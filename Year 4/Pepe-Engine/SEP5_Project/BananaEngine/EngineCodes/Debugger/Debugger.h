#pragma once
#ifndef	DEBUGGER_H
#define DEBUGGER_H

#include <Precompiled/pch.h>
#include <Utilities/Utility.h>
#include <Utilities/Singleton/Singleton.h>

namespace BE
{
	class Debug : public Singleton<Debug>
	{
	public:
		//Warning level of the log
		enum class LogLevel
		{
			info = 0,
			warn,
			error
		};

		//Which log is it for?
		enum class LogLocation
		{
			engine = 0,
			editor,
			app
		};

		//Debug states
		enum class DebugFlags
		{
			logToFile = 0,		//Whether to log content to file
			enableRecording,	//Whether to display new logs
			debugDraw,			//Whether to draw debug lines (collision/physics)
			debugStep,			//Whether to step through the physics/collision simulation
			imguiPause,			//Whether to pause the simulation for IMGUI

			debugFlagsLast
		};

		//A single log message, for the debug console use
		struct LogMessage
		{
			LogLevel level;
			LogLocation location;
			int line;
			std::string file;
			std::string message;

			LogMessage(LogLevel _lv, LogLocation _lc, int _ln, std::string&& _file, const char* _msg);
			bool operator==(const LogMessage& lm);
		};



	private:
		//All the debug flags
		std::bitset<(size_t)DebugFlags::debugFlagsLast> m_debugFlags;

		//Variables Debug logging 
		std::ofstream m_ofs;										//The output file for the logs
		std::deque<std::pair<LogMessage, size_t>> m_recentLogs;		//Recent logs
		static constexpr int MAX_LOG_SIZE = 128;					//Maximum number of logs can be stored

		//Editor screen properties
		std::pair<float, float> m_editorWindowSize;		//Screen size of the editor
		std::pair<float, float> m_editorWindowPos;		//Top left position of the editor
		std::pair<float, float> m_editorMousePos;		//Mouse pos for IMGUI

		Debug();
		~Debug();

	public:
		//Editor Screen properties
		void SetEditorProperties(float _windowWidth, float _windowHeight, float _windowPosX, float _windowPosY, float _mousePosX, float _mousePosY);
		const auto& GetEditorWindowSize() const				{ return m_editorWindowSize; }
		const auto& GetEditorWindowPos() const				{ return m_editorWindowPos; }
		const auto& GetEditorMousePos() const				{ return m_editorMousePos; }


		//Debug controls
		inline void SetDebugFlag(DebugFlags _f, bool _b)	{ m_debugFlags[(size_t)_f] = _b;   }
		inline bool GetDebugFlag(DebugFlags _f)				{ return m_debugFlags[(size_t)_f]; }

		//Debug logging
		void DebugLog(LogLevel lv, LogLocation lc, int line, const char* file, const char* format, ...);
		inline const std::deque<std::pair<LogMessage, size_t>>& GetRecentLogs()	{ return m_recentLogs; }
		inline void ClearConsole()												{ m_recentLogs.clear(); }

		//Asserts
		template <typename... Args>
		void Assertion(bool condition, const char* expToCheck, LogLocation lc, int line, const char* file, const char* format, Args... args);
		
		//Debug drawing
		// ...



		friend class Singleton<Debug>;

	};

}

#include "Debugger.hpp"

//Accessing debugger
#define	Debugger Debug::GetInstance()

//Debug logging macro
#ifdef USE_DEBUGGER
//Core engine logging macro
#define LogCore(...)				BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::info,  BE::Debug::LogLocation::engine, __LINE__, __FILE__, __VA_ARGS__)
#define LogCoreWarning(...)			BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::warn,  BE::Debug::LogLocation::engine, __LINE__, __FILE__, __VA_ARGS__)
#define LogCoreError(...)			BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::error, BE::Debug::LogLocation::engine, __LINE__, __FILE__, __VA_ARGS__)

//Core engine logging macro					  						     
#define LogEditor(...)				BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::info,  BE::Debug::LogLocation::editor, __LINE__, __FILE__, __VA_ARGS__)
#define LogEditorWarning(...)		BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::warn,  BE::Debug::LogLocation::editor, __LINE__, __FILE__, __VA_ARGS__)
#define LogEditorError(...)			BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::error, BE::Debug::LogLocation::editor, __LINE__, __FILE__, __VA_ARGS__)

//client logging macro						  						     
#define Log(...)					BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::info,  BE::Debug::LogLocation::app, __LINE__, __FILE__, __VA_ARGS__)
#define LogWarning(...)				BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::warn,  BE::Debug::LogLocation::app, __LINE__, __FILE__, __VA_ARGS__)
#define LogError(...)				BE::Debug::GetInstance()->DebugLog(BE::Debug::LogLevel::error, BE::Debug::LogLocation::app, __LINE__, __FILE__, __VA_ARGS__)

#else
//No debugging for release builds
#define LogCore(...)				(void)(__VA_ARGS__)
#define LogCoreWarning(...)			(void)(__VA_ARGS__)
#define LogCoreError(...)			(void)(__VA_ARGS__)
#define LogEditor(...)				(void)(__VA_ARGS__)
#define LogEditorWarning(...)		(void)(__VA_ARGS__)
#define LogEditorError(...)			(void)(__VA_ARGS__)
#define Log(...)					(void)(__VA_ARGS__)
#define LogWarning(...)				(void)(__VA_ARGS__)
#define LogError(...)				(void)(__VA_ARGS__)
#endif

//Assertion macro
#ifdef USE_DEBUGGER
#define AssertCore(expression, ...)		BE::Debug::GetInstance()->Assertion(expression, _CRT_STRINGIZE_(#expression), BE::Debug::LogLocation::engine, __LINE__, __FILE__, __VA_ARGS__)
#define AssertEditor(expression, ...)	BE::Debug::GetInstance()->Assertion(expression, _CRT_STRINGIZE_(#expression), BE::Debug::LogLocation::editor, __LINE__, __FILE__, __VA_ARGS__)
#define Assert(expression, ...)			BE::Debug::GetInstance()->Assertion(expression, _CRT_STRINGIZE_(#expression), BE::Debug::LogLocation::app, __LINE__, __FILE__, __VA_ARGS__)
#else
#define	AssertCore(x, ...)		(void)(x), (void)(__VA_ARGS__)
#define	AssertEditor(x, ...)	(void)(x), (void)(__VA_ARGS__)
#define	Assert(x, ...)			(void)(x), (void)(__VA_ARGS__)
#endif

#endif