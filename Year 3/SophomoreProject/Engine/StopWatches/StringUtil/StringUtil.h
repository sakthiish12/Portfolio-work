// /******************************************************************************/
// /*!
// \file         StringUtil.h
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        92
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <string>

namespace StringUtil
{

	// Checks if a string contains a given substring
	[[nodiscard]] inline bool Contains(const std::string& mainStr, const std::string& subStr)
	{
		// Search for the substring in string
		return  mainStr.find(subStr) != std::string::npos;
	}

	// Erases the first occurrence of a substring from a string
	[[nodiscard]] inline std::string EraseSubStr(const std::string& mainStr, const std::string& subStr)
	{
		std::string result = mainStr;

		// Search for the substring in string
		const size_t pos = result.find(subStr);
		if (pos != std::string::npos)
		{
			// If found then erase it from string
			result.erase(pos, subStr.length());
		}

		return result;
	}

	// Trim trailing spaces from start (Modifies the String)
	inline void TrimLeft(std::string& s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
			{
				return !std::isspace(ch);
			}));
	}

	// Trim trailing spaces from end (Modifies the String)
	inline void TrimRight(std::string& s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
			{
				return !std::isspace(ch);
			}).base(), s.end());
	}

	// Trim trailing spaces from both ends (Modifies the String)
	inline void Trim(std::string& s)
	{
		TrimLeft(s);
		TrimRight(s);
	}

	// Trim trailing spaces from start (Returns a copy)
	[[nodiscard]] inline std::string TrimLeftCopy(std::string s)
	{
		TrimLeft(s);
		return s;
	}

	// Trim trailing spaces from end (Returns a copy)
	[[nodiscard]] inline std::string TrimRightCopy(std::string s)
	{
		TrimRight(s);
		return s;
	}

	// Trim trailing spaces from both ends (Returns a copy)
	[[nodiscard]] inline std::string TrimCopy(std::string s)
	{
		Trim(s);
		return s;
	}

	// Sanitize the string generated from typeid.name()
	[[nodiscard]] inline std::string SanitizeTypeIdName(const std::string& typeidName)
	{
		std::string result = typeidName;

		// Erase 'struct ' and 'class ' from string
		result = EraseSubStr(result, "struct ");
		return EraseSubStr(result, "class ");
	}

	// Returns a string with its file extension removed
	// E.g: "C:\Windows\system.ini" -> "C:\Windows\system"
	[[nodiscard]] inline std::string TrimFileExtension(const std::string& filePath)
	{
		const std::filesystem::path fullPath(filePath); // "C:\Windows\system.ini"
		const std::string fileParentPath = fullPath.parent_path().string() + static_cast<char>(fullPath.preferred_separator); // "C:\Windows\"
		const std::string fileStem = fullPath.stem().string(); // "system"
		std::string filePathWithoutExtension = fileParentPath + fileStem; // "C:\Windows\system"
		return filePathWithoutExtension;
	}

	// Returns the given string in lowercase form
	[[nodiscard]] inline std::string ToLowerCase(const std::string& str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(),
			[](const unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return result;
	}

	// Returns the given string in uppercase form
	[[nodiscard]] inline std::string ToUpperCase(const std::string& str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(),
			[](const unsigned char c) { return static_cast<char>(std::toupper(c)); });
		return result;
	}

	// Returns the given string in lowercase but sets the first character uppercase
	// E.g: "helloWORLD" -> "Helloworld"
	[[nodiscard]] inline std::string ToLowerCaseExceptFirst(const std::string& str)
	{
		std::string result = str;
		if (!result.empty())
		{
			result[0] = static_cast<char>(std::toupper(static_cast<int>(result[0])));
			for (size_t i = 1; i < result.length(); i++)
				result[i] = static_cast<char>(std::tolower(static_cast<int>(result[i])));
		}
		return result;
	}

}