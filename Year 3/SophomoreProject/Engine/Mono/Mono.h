//// /******************************************************************************/
//// /*!
//// \file         Mono.h
//// \project name Engine
//// \author(s)    Sakthiish
//// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
//// \brief        This file contains the function definitions for the BatchRenderer 
//// \lines        150
//// 
//// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
//// reserved.
//// Copyright (C) 2021 DigiPen Institute of Technology.
//// Reproduction or disclosure of this file or its contents without the
//// prior written consent of DigiPen Institute of Technology is prohibited.
////  */
////  /******************************************************************************/
//#pragma once
//
//#ifndef MONO
//#define MONO_EXPORT __declspec(dllexport)
//#else
//#define MONO_EXPORT __declspec(dllimport)
//#endif
//
//#include <mono/jit/jit.h>
//#include <mono/metadata/assembly.h>
//#include <mono/metadata/debug-helpers.h>
//#include <mono/metadata/appdomain.h>
////#include"../Dependencies/Mono/bin/"
//#include <xlocale>
//#include <iostream>
//#include "../ECS/Systems/CSharpScriptSystem.h"
///*
//class Mono 
//{
//	MonoDomain* m_pMonoDomain = nullptr;
//	MonoAssembly* m_pMonoAssembly = nullptr;
//	MonoImage* m_pMonoImage = nullptr;
//	uint32_t m_MonoHandler;
//	MonoObject* m_pMainObj;
//	MonoClass* m_pMainClass = nullptr;
//	MonoMethod* m_pMainFn = nullptr;
//	MonoMethod* m_pNextFn2 = nullptr;
//
//public:
//
//
//	std::string ModuleName;
//
//	MonoClass* ImportClass(const char* _namespace, const char* _class)
//	{
//		return mono_class_from_name(m_pMonoImage, _namespace, _class);
//	}
//
//	MonoMethod* ImportFunction(MonoClass* m_pClass, MonoObject* m_pObj, const char* m_pFnDesc)
//	{
//		MonoMethod* fn = nullptr;
//		// description of Start function
//		MonoMethodDesc* mono_extern_methoddesc = mono_method_desc_new(m_pFnDesc, false);
//		MonoMethod* vitrualMethod = mono_method_desc_search_in_class(mono_extern_methoddesc, m_pClass);
//
//		if (vitrualMethod)
//			fn = mono_object_get_virtual_method(m_pObj, vitrualMethod);
//
//		// Free
//		mono_method_desc_free(mono_extern_methoddesc);
//
//		return fn;
//	}
//
//	MonoObject* GetClassInstance(const char* m_pFnDesc, MonoClass* m_pClass)
//	{
//		MonoObject* m_pMonoObj = nullptr;
//
//		MonoMethodDesc* mono_main_desc = mono_method_desc_new(m_pFnDesc, false);
//		if (mono_main_desc) {
//			MonoMethod* mono_main_method = mono_method_desc_search_in_class(mono_main_desc, m_pClass);
//			if (mono_main_method) {
//				// Call main method
//				MonoObject* mono_exception = nullptr;
//				// Reference object for specified class
//				m_pMonoObj = mono_runtime_invoke(mono_main_method, nullptr, nullptr, &mono_exception);
//				//ThreadCallback(mono_main_method, mono_exception);
//				// Exception Handling
//				MonoException(mono_exception);
//			}
//			// Free Desc
//			mono_method_desc_free(mono_main_desc);
//		}
//		return m_pMonoObj;
//	}
//
//	MonoObject* ThreadCallback(MonoMethod* method, MonoObject* exception)
//	{
//		mono_jit_thread_attach(m_pMonoDomain);
//		return mono_runtime_invoke(method, nullptr, nullptr, &exception);
//	}
//
//	void MonoException(MonoObject* exception)
//	{
//		if (exception) {
//			MonoString* exString = mono_object_to_string(exception, nullptr);
//			//const char* exCString = mono_string_to_utf8(exString);
//			static_cast<void>(exString);
//			// Print Error
//			//PPB_ASSERT(exCString);
//		}
//	}
//
//	MonoObject* RunImportFn(MonoObject* m_pObj, MonoMethod* m_pFn)
//	{
//		MonoObject* fn = nullptr;
//		if (m_pFn)
//		{
//			MonoObject* exception = nullptr;
//			// Get function
//			fn = mono_runtime_invoke(m_pFn, m_pObj, nullptr, &exception);
//
//			// Exception Handling
//			MonoException(exception);
//		}
//		return fn;
//	}
//
//	template <typename T>
//	T ExtractResult(MonoObject* m_pResult)
//	{
//		return *(T*)mono_object_unbox(m_pResult);
//	}
//
//	//Mono()
//	//{
//	//	// Set Directory for lib and etc
//	//	mono_set_dirs("../Engine/Dependencies/mono/lib", "../Engine/Dependencies/mono/etc");
//
//	//	// Create domain (exits if unsuccessful)
//	//	m_pMonoDomain = mono_jit_init("Mono.exe");
//
//	//	if (m_pMonoDomain)
//	//		std::cout << "check" << std::endl;
//
//	//	m_pMonoAssembly = mono_domain_assembly_open(m_pMonoDomain, "../EngineScript/bin/Debug/EngineScript.dll");
//	//	if (m_pMonoAssembly) {
//	//		m_pMonoImage = mono_assembly_get_image(m_pMonoAssembly);
//	//		if (m_pMonoImage) {
//
//	//			// Add classes
//	//			m_pMainClass = ImportClass("EngineScript", "Script");
//
//	//			if (m_pMainClass) {
//	//				// Describe Method for main
//	//				m_pMainObj = GetClassInstance(".Script:GetInstance()", m_pMainClass);
//
//	//				if (m_pMainObj) {
//	//					// Reference handler for specified class
//	//					m_MonoHandler = mono_gchandle_new(m_pMainObj, false);
//
//	//					// Add External Calls
//	//					m_pMainFn = ImportFunction(m_pMainClass, m_pMainObj, ".Script:Main()");
//
//	//					RunImportFn(m_pMainObj, m_pMainFn);
//	//				}
//	//			}
//	//		}
//	//		//}
//	//	}
//	//}
//	bool UpdateDLL()
//	{
//		// Load binary file as an assembly
//		m_pMonoAssembly = mono_domain_assembly_open(m_pMonoDomain, "bin/CSScript.dll");
//		if (m_pMonoAssembly) {
//			//	 Load mono image
//			m_pMonoImage = mono_assembly_get_image(m_pMonoAssembly);
//			if (m_pMonoImage)
//				return true;
//		}
//		return false;
//	}
//
//	// Trying to template params for external use in c#
//	//template <typename... Args>
//	//MonoObject* RunImportFn(MonoObject* m_pObj, MonoMethod* m_pFn, Args... m_args)
//	//{
//	//	void** m_param;
//	//	MonoObject* fn = nullptr;
//	//	// Retrieve function params 
//	//	std::tuple param = std::make_tuple(m_args...);
//	//	m_param = (void**)malloc(sizeof(param));
//	//	std::apply([](auto&&... args) {((m_param[0] = (void*)(args)), ...); }, param);
//	//	
//	//	if (m_pFn)
//	//	{
//	//		MonoObject* exception = nullptr;
//	//		// Get function
//	//		fn = mono_runtime_invoke(m_pFn, m_pObj, nullptr, &exception);
//	//		// Exception Handling
//	//		MonoException(exception);
//	//	}
//	//	return fn;
//	//}
//
//	void Unload() 
//	{
//		if (m_pMonoDomain)
//		{
//			mono_domain_unload(m_pMonoDomain);
//			
//			//m_pMainFn
//			//m_pMainClass
//			//m_pMonoAssembly
//		}
//	}
//
//	void ReleaseDomain()
//	{
//		if (m_MonoHandler)
//			mono_gchandle_free(m_MonoHandler);
//
//		if (m_pMonoDomain)
//			mono_jit_cleanup(m_pMonoDomain);
//	}
//
//
//	static Mono& GetInstanced()
//	{
//		static Mono mono;
//		return mono;
//	}
//
//	private:
//		MonoDomain* m_pMonoDomain;
//		MonoAssembly* m_pMonoAssembly;
//		MonoImage* m_pMonoImage;
//		uint32_t m_MonoHandler;
//		MonoObject* m_pMainObj;
//		MonoClass* m_pMainClass;
//		MonoMethod* m_pMainFn;
//		MonoMethod* m_pNextFn2;
//}; */