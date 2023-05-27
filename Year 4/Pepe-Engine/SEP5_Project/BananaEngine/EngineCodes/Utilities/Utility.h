#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include "../Precompiled/pch.h"

namespace BE
{
	//Unreferenced parameters
	#define UNUSED(x) (void)(x)


	//Compilation macros
	#if !_GAMEPLAY
	#define USE_EDITOR			//Build with editor
	#define USE_DEBUGGER		//Build with debugger
	#endif


	//Sizes
	constexpr int MAX_STR_BUFFER = 256;

	//Max update distance
	constexpr float MAX_UPDATE_DISTANCE = 125.0f;
	constexpr float MAX_UPDATE_SQ_DIST = MAX_UPDATE_DISTANCE * MAX_UPDATE_DISTANCE;


	//Helper function to convert filepath slash from \\ to /
	void ConvertFilePathSlash(std::string& _fp);

	//Helper function to truncate filepath to the last part
	void TruncateFilePath(std::string& _fp);
}

#endif