// /******************************************************************************/
// /*!
// \file         ScriptScript.cpp
// \project name Engine
// \author(s)    Sakthiish, Poh Chun Ren
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        150
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <iostream>
#include "../ECS.h"
#include "CSharpScriptSystem.h"
#include "../component.h"
#include "../../InputMgr/InputMgr.h"
#include "../../ScriptWrapper/ScriptWrappers.h"

MonoClassManager::MonoClassManager()
{
	DeSerialise();
}

MonoClassManager::~MonoClassManager()
{
	Serialise();
}

bool MonoClassManager::addCSharpClass(std::string ClassName, std::string ClassNamespace)
{
	//Adding of a csharp CLASS into database
	MonoClassData testClass;
	testClass.classNamespace = ClassNamespace;
	testClass.name = ClassName;
	testClass.classPtr = CSharpSystemPtr->ImportClass(testClass.classNamespace.c_str(), testClass.name.c_str());

	_MonoClassDatabase[testClass.name] = std::make_shared<MonoClassData>(testClass);
	if (_MonoClassDatabase[testClass.name])
		return true;
	else
		return false;
}

void MonoClassManager::deleteScript(std::shared_ptr<MonoClassData> script)
{
	deleteScript(script->name);
}

void MonoClassManager::deleteScript(std::string script)
{
	_MonoClassDatabase.erase(script);
}

MonoClassManager& MonoClassManager::getInstance()
{
	static MonoClassManager inst;
	return inst;
}

MonoClassObject MonoClassManager::createFromScriptDatabase(std::string ClassName, std::string ClassNamespace)
{
	//if class has not been registered
	if (_MonoClassDatabase.find(ClassName) == _MonoClassDatabase.end())
	{
		//attempt to register, return nullptr class obj if fails
		if (!addCSharpClass(ClassName, ClassNamespace))
		{
			return MonoClassObject();
		}
		else
		{
			if (_MonoClassDatabase[ClassName]->classPtr)
				return MonoClassObject();
		}
	}

	MonoClassObject newObject;
	newObject.name = ClassName;
	std::string classGetInstance = { "." };
	classGetInstance += newObject.name;
	classGetInstance += ":GetInstance()";//".Script:GetInstance()"
	newObject.objPtr = CSharpSystem::GetClassInstance(classGetInstance.c_str(), _MonoClassDatabase[ClassName]->classPtr);
	newObject.classNamespace = _MonoClassDatabase[ClassName]->classNamespace;
	newObject.functionsToCall;

	std::string fn_desc = ".";
	fn_desc += ClassName;

	std::string fn_desc_update{ fn_desc };
	fn_desc_update += ":update()";

	std::string fn_desc_init{ fn_desc };
	fn_desc_init += ":init()";

	//MonoMethod* fnToAdd = CSharpSystemPtr->ImportFunction(getCSharpClass(name)->classPtr, newObject.objPtr, ".Script:update()");
	MonoMethod* fnToAdd = CSharpSystemPtr->ImportFunction(getCSharpClass(ClassName)->classPtr, newObject.objPtr, fn_desc_update.c_str());
	newObject.functionsToCall.insert(std::make_pair(std::string("Update"), fnToAdd));
	//fnToAdd = CSharpSystemPtr->ImportFunction(getCSharpClass(name)->classPtr, newObject.objPtr, ".Script:init()");
	fnToAdd = CSharpSystemPtr->ImportFunction(getCSharpClass(ClassName)->classPtr, newObject.objPtr, fn_desc_init.c_str());
	newObject.functionsToCall.insert(std::make_pair(std::string("Init"), fnToAdd));

	return newObject;
}

//get a specific script pointer
std::shared_ptr<MonoClassData> MonoClassManager::getCSharpClass(std::string name)
{
	return _MonoClassDatabase[name];
}

std::unordered_map<std::string, std::shared_ptr<MonoClassData> > MonoClassManager::getCSharpClassDatabase()
{
	return _MonoClassDatabase;
}

//save
void MonoClassManager::Serialise()
{
	std::string targetFilePath{"./Config/CSharpClassList.json"};
	//SerializeManager serializeManager;
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	std::string ClassNamespace{ "classNamespace" };
	std::string ClassName{ "className" };
	writer.StartObject(); //File level braces
	{
		//writer.Key("CSharp script class list");
		writer.Key("script class size");
		writer.Uint(static_cast<unsigned int>(_MonoClassDatabase.size()));
		int i = 0;
		for (auto& cscript : _MonoClassDatabase)
		{
			if (cscript.second->classPtr)
			{
				writer.Key(("Csharp Class " + std::to_string(i)).c_str());
				{
					writer.StartObject();
					writer.Key((ClassNamespace + std::to_string(i)).c_str());
					writer.String(cscript.second->classNamespace.c_str());
					writer.Key((ClassName + std::to_string(i)).c_str());
					writer.String(cscript.second->name.c_str());
					writer.EndObject();
				}
			}
			++i;
		}
		writer.EndObject(); 
	}
	
	//Open the file and write to it
	std::ofstream f(targetFilePath);
	f << sb.GetString();
	f.flush();
	f.close();
}

//load
void MonoClassManager::DeSerialise()
{
	std::string targetFilePath{ "./Config/CSharpClassList.json" };
	SerializeManager sm;
	std::stringstream fileContents;
	sm.DeserializeFromFile(targetFilePath, fileContents);
	rapidjson::Document doc;
	if (doc.Parse(fileContents.str().c_str()).HasParseError())
		PartyAssert(false, "Cannot parse JSON properly");

	//const rapidjson::Value& detail = doc["CSharp script class list"];
	const rapidjson::Value& detail = doc;

	unsigned int size;
	sm.DeserializeUInt("script class size", size, detail);

	std::string classNamespace{ "classNamespace" };
	std::string className{ "className" };

	for (unsigned int i = 0; i < size; ++i)
	{
		if (doc.HasMember(("Csharp Class " + std::to_string(i)).c_str()) &&
			doc[("Csharp Class " + std::to_string(i)).c_str()].IsObject())
		{
			const rapidjson::Value& detail2 = doc[("Csharp Class " + std::to_string(i)).c_str()];
			std::string classNamespaceResult{};
			std::string classNameResult{};

			sm.DeserializeString((classNamespace + std::to_string(i)).c_str(), classNamespaceResult, detail2);
			sm.DeserializeString((className + std::to_string(i)).c_str(), classNameResult, detail2);
			

			//if check to ensure classNamespaceResult and classNameResult is not nullptr
			
			//Need take note, will cause loop when not found
			addCSharpClass(classNameResult, classNamespaceResult);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<CSharpSystem> CSharpSystemPtr;
//CSharpScriptComponent cs;
CSharpScriptComponent* CSharpCode = nullptr;
CSharpSystem::CSharpSystem(bool requireComponentMatch) :
	systemBase(requireComponentMatch)
{
}

CSharpSystem::~CSharpSystem()
{
	//ReleaseDomain();
}

void CSharpSystem::init() 
{
	OpenAssembly();

	//Testing
	MonoScriptManager.addCSharpClass("Script", "EngineScript");
	//test();

	CSharpScriptComponent ass;
	ecs.addComponent<CSharpScriptComponent>(0, ass);
	ecs.getComponent<CSharpScriptComponent>(0)._scriptMap.insert(
		std::make_pair(
			std::string("Script"), MonoScriptManager.createFromScriptDatabase("Script"))
	);
	
	//Need to organise the adding of cpp function to csharp
	addInternalsTest();

	
}

void CSharpSystem::test()
{
	////temp adding of script
	//CSharpScriptComponent ass;
	//ecs.addComponent<CSharpScriptComponent>(0, ass);

	////init for instance
	//for (auto const& e : getEntitiesRegistered()) {
	//	//CSharpCode = &ecs.getComponent<CSharpScriptComponent>(e);
	//	CSharpScriptComponent& CS = ecs.getComponent<CSharpScriptComponent>(e);
	//	//assume serialised 
	//	MonoClassObject obj1;
	//	CS._scriptMap.insert(std::make_pair(std::string("Script"), obj1));
	//	for (auto& s : CS._scriptMap)
	//	{
	//		s.second.name = s.first;
	//		s.second.objPtr = GetClassInstance(".Script:GetInstance()", _classDataBase.getCSharpClass(s.second.name)->classPtr);

	//		MonoMethod* fnToAdd = ImportFunction(_classDataBase.getCSharpClass(s.second.name)->classPtr, s.second.objPtr, ".Script:update()");
	//		s.second.functionsToCall.insert(std::make_pair(std::string("Update"), fnToAdd));
	//		fnToAdd = ImportFunction(_classDataBase.getCSharpClass(s.second.name)->classPtr, s.second.objPtr, ".Script:init()");
	//		s.second.functionsToCall.insert(std::make_pair(std::string("Init"), fnToAdd));
	//	}
	//}
}


void CSharpSystem::update(float dt) 
{
	static_cast<void>(dt);
	//RunImportFn(m_pMainObj, m_pMainFn);
	for (auto const& e : getEntitiesRegistered()) {
		//CSharpCode = &ecs.getComponent<CSharpScriptComponent>(e);
		CSharpScriptComponent& CS = ecs.getComponent<CSharpScriptComponent>(e);
		for (auto& s : CS._scriptMap)
		{
			MonoObject* result = nullptr;

			if (s.second.initialised == true)
			{
				result = RunImportFn(s.second.objPtr, s.second.functionsToCall["Update"]);
			}
			else
			{
				result = RunImportFn(s.second.objPtr, s.second.functionsToCall["Init"]);
				//initialised need  to be link to each script TODO 
				s.second.initialised = true;
			}
		}
	}
}

void CSharpSystem::OpenAssembly() {
	mono_set_dirs("../Engine/Dependencies/mono/lib", "../Engine/Dependencies/mono/etc");

	// Create domain (exits if unsuccessful)
	m_pMonoDomain = mono_jit_init("Mono.exe");
	
	if (m_pMonoDomain)
	{
		m_pMonoAssembly = mono_domain_assembly_open(m_pMonoDomain, "../EngineScript/bin/Debug/EngineScript.dll");
		if (m_pMonoAssembly) {
			m_pMonoImage = mono_assembly_get_image(m_pMonoAssembly);


			/*
			/// <summary>
			/// Importing of classes, functions
			/// </summary>
			if (m_pMonoImage) {

				// Add classes
				m_pMainClass = ImportClass("EngineScript", "Script");

				if (m_pMainClass) {
					// Describe Method for main
					m_pMainObj = GetClassInstance(".Script:GetInstance()", m_pMainClass);

					if (m_pMainObj) {
						// Reference handler for specified class
						m_MonoHandler = mono_gchandle_new(m_pMainObj, false);

						// Add External Calls
						m_pMainFn = ImportFunction(m_pMainClass, m_pMainObj, ".Script:Main()");
						//	m_pMainFn2 = ImportFunction(m_pMainClass, m_pMainObj, ".Script:Input::IsKeyPressed()");
					}
				}
			}
			*/
		}
	}
}

MonoClass* CSharpSystem::ImportClass(const char* _namespace, const char* _class)
{
	return mono_class_from_name(m_pMonoImage, _namespace, _class);
}

MonoMethod* CSharpSystem::ImportFunction(MonoClass* m_pClass, MonoObject* m_pObj, const char* m_pFnDesc)
{
	MonoMethod* fn = nullptr;
	// description of Start function
	MonoMethodDesc* mono_extern_methoddesc = mono_method_desc_new(m_pFnDesc, false);
	MonoMethod* vitrualMethod = mono_method_desc_search_in_class(mono_extern_methoddesc, m_pClass);

	if (vitrualMethod)
		fn = mono_object_get_virtual_method(m_pObj, vitrualMethod);

	// Free
	mono_method_desc_free(mono_extern_methoddesc);

	return fn;
}

MonoObject* CSharpSystem::GetClassInstance(const char* m_pFnDesc, MonoClass* m_pClass)
{
	MonoObject* m_pMonoObj = nullptr;

	MonoMethodDesc* mono_main_desc = mono_method_desc_new(m_pFnDesc, false);
	if (mono_main_desc) {
		MonoMethod* mono_main_method = mono_method_desc_search_in_class(mono_main_desc, m_pClass);
		if (mono_main_method) {
			// Call main method
			MonoObject* mono_exception = nullptr;
			// Reference object for specified class
			m_pMonoObj = mono_runtime_invoke(mono_main_method, nullptr, nullptr, &mono_exception);
			//ThreadCallback(mono_main_method, mono_exception);
			// Exception Handling
			MonoException(mono_exception);
		}
		// Free Desc
		mono_method_desc_free(mono_main_desc);
	}
	return m_pMonoObj;
}

void CSharpSystem::addInternalCall(std::string Namespace, std::string ClassName, std::string FunctionName, std::string Parameter, const void* functionAddress)
{
	//mono_add_internal_call((Namespace + "." + ClassName + "::" + FunctionName + Parameter).c_str(), functionAddress);
	addInternalCall((Namespace + "." + ClassName + "::" + FunctionName + Parameter).c_str(), functionAddress);
}

void CSharpSystem::addInternalCall(std::string FullCSharpFunctionName, const void* functionAddress)
{
	mono_add_internal_call(FullCSharpFunctionName.c_str(), functionAddress);
}

void CSharpSystem::addInternalsTest()
{
	//format -> csharp externed function, then address of cpp function
	//mono_add_internal_call("EngineScript.Script::callCPP_function()", &PartyPantry::CSharpScriptSample::cppcallCPP_function);
	//mono_add_internal_call("EngineScript.Script::callCPP_function_withPrimitiveParam(int)", &PartyPantry::CSharpScriptSample::cppcallCPP_function_withPrimitiveParam);

	addInternalCall("EngineScript.Script::callCPP_function()", &PartyPantry::CSharpScriptSample::cppcallCPP_function);
	addInternalCall("EngineScript.Script::callCPP_function_withPrimitiveParam(int)", &PartyPantry::CSharpScriptSample::cppcallCPP_function_withPrimitiveParam);
}

//MONO_EXPORT void CSharp_Input_IsKeyPressed() 
//{
//}

MonoObject* CSharpSystem::ThreadCallback(MonoMethod* method, MonoObject* exception)
{
	mono_jit_thread_attach(m_pMonoDomain);
	return mono_runtime_invoke(method, nullptr, nullptr, &exception);
}

void CSharpSystem::MonoException(MonoObject* exception)
{
	if (exception) {
		MonoString* exString = mono_object_to_string(exception, nullptr);
		//const char* exCString = mono_string_to_utf8(exString);
		static_cast<void>(exString);
		// Print Error
		//PPB_ASSERT(exCString);
	}
}

MonoObject* CSharpSystem::RunImportFn(MonoObject* m_pObj, MonoMethod* m_pFn)
{
	MonoObject* fn = nullptr;
	if (m_pFn)
	{
		MonoObject* exception = nullptr;
		// Get function
		fn = mono_runtime_invoke(m_pFn, m_pObj, nullptr, &exception);

		// Exception Handling
		MonoException(exception);
	}
	return fn;
}

template <typename T>
T CSharpSystem::ExtractResult(MonoObject* m_pResult)
{
	return *(T*)mono_object_unbox(m_pResult);
}

bool CSharpSystem::UpdateDLL()
{
	// Load binary file as an assembly
	m_pMonoAssembly = mono_domain_assembly_open(m_pMonoDomain, "bin/CSScript.dll");
	if (m_pMonoAssembly) {
		//	 Load mono image
		m_pMonoImage = mono_assembly_get_image(m_pMonoAssembly);
		if (m_pMonoImage)
			return true;
	}
	return false;
}

void CSharpSystem::Unload()
{
	if (m_pMonoDomain)
	{
		mono_domain_unload(m_pMonoDomain);
		//m_pMainFn
		//m_pMainClass
		//m_pMonoAssembly
	}
}

void CSharpSystem::ReleaseDomain()
{
	if (m_MonoHandler)
		mono_gchandle_free(m_MonoHandler);

	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);
}