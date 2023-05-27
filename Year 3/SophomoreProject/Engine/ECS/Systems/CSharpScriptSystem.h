// /******************************************************************************/
// /*!
// \file         ScriptScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        57
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _CSHARPSCRIPTSYSTEM_H_
#define _CSHARPSCRIPTSYSTEM_H_

//#ifndef MONO
//#define MONO_EXPORT __declspec(dllexport)
//#else
//#define MONO_EXPORT __declspec(dllimport)
//#endif
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>

#include <map>
#include <memory>
#include <string>

#include <xlocale>
#include <iostream>

#include "../system.h"

#include "MonoHelpStruct.h"
//#include"../Dependencies/Mono/bin/"

//#include "../../Mono/Mono.h"

//Holds the different classes from csharp
class MonoClassManager
{
	//contains wrapper around csharp CLASS
	std::unordered_map<std::string, std::shared_ptr<MonoClassData> > _MonoClassDatabase;
	std::vector<std::string> _scriptToBeRemoved;
	MonoClassManager();
public:
	~MonoClassManager();
	
	

	//Add Csharp CLASS, not instance
	bool addCSharpClass(std::string ClassName, std::string ClassNamespace = "EngineScript");

	void deleteScript(std::shared_ptr<MonoClassData> script);
	void deleteScript(std::string script);
	static MonoClassManager& getInstance();

	//Add Csharp SCRIPT(or class instance) wrapped in our own struct
	MonoClassObject createFromScriptDatabase(std::string ClassName, std::string ClassNamespace = "EngineScript");

	//get a specific script pointer
	std::shared_ptr<MonoClassData> getCSharpClass(std::string name);

	std::unordered_map<std::string, std::shared_ptr<MonoClassData> > getCSharpClassDatabase();

	//For savings of the different csharp script
	//saving
	void Serialise();
	//Loading
	void DeSerialise();
};

#define MonoScriptManager ::MonoClassManager::getInstance()


class CSharpSystem : public systemBase
{
	MonoDomain* m_pMonoDomain = nullptr; //dir to mono.exe 
	MonoAssembly* m_pMonoAssembly = nullptr;//dir to dll
	MonoImage* m_pMonoImage = nullptr; // dir to file

	uint32_t m_MonoHandler;
	MonoObject* m_pMainObj = nullptr; //instances
	MonoClass* m_pMainClass = nullptr; //class
	MonoMethod* m_pMainFn = nullptr; //fn
	MonoMethod* m_pMainFn2 = nullptr;

	//MonoClassManager _classDataBase;
public:
	CSharpSystem(bool requireComponentMatch = false);
	~CSharpSystem();

	void init();
	void update(float dt);
	void test();

	void OpenAssembly();

	MonoClass* ImportClass(const char* _namespace, const char* _class);

	MonoMethod* ImportFunction(MonoClass* m_pClass, MonoObject* m_pObj, const char* m_pFnDesc);

	static MonoObject* GetClassInstance(const char* m_pFnDesc, MonoClass* m_pClass);


	void addInternalCall(std::string CSharpNamespace, std::string CSharpClassName, std::string CSharpFunctionName, std::string CSharpParameter, const void* functionAddress);
	//Parameter sample: ("EngineScript.Script::callCPP_function()", &PartyPantry::CSharpScriptSample::cppcallCPP_function)
	//Parameter sample: ("EngineScript.Script::callCPP_function_withPrimitiveParam(int)", &PartyPantry::CSharpScriptSample::cppcallCPP_function_withPrimitiveParam)
	void addInternalCall(std::string FullCSharpFunctionName, const void* functionAddress);

	void addInternalsTest();

	static void MonoException(MonoObject* exception);

	MonoObject* RunImportFn(MonoObject* m_pObj, MonoMethod* m_pFn);

	template <typename T>
	T ExtractResult(MonoObject* m_pResult);

	bool UpdateDLL();

	void Unload();

	void ReleaseDomain();

	//for use of GetClassInstance etc
	friend class MonoClassManager; 
private:
	MonoObject* ThreadCallback(MonoMethod* method, MonoObject* exception);

};

extern std::shared_ptr<CSharpSystem> CSharpSystemPtr;

#endif //