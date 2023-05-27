// /******************************************************************************/
// /*!
// \file         ScriptScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

#ifndef MONO
#define MONO_EXPORT __declspec(dllexport)
#else
#define MONO_EXPORT __declspec(dllimport)
#endif
#include <mono/jit/jit.h>

struct MonoClassObject
{
	MonoObject* objPtr = nullptr;
	std::string name{}; //req
	std::string classNamespace{};//req
	//std::vector<std::string> functionsToCall{};//fn to call per entity
	std::map<std::string, MonoMethod* > functionsToCall{};
	bool initialised = false; //Should change this into bool for each script
};

struct MonoClassData
{
	//Pointer to CSharpScriptComponent Class
	MonoClass* classPtr = nullptr;
	std::string name{}; //req
	std::string classNamespace{};//req
	std::vector<std::string> classFunctions{};
};