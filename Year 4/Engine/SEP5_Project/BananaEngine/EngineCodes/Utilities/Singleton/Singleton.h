#pragma once
#ifndef	SINGLETON_H
#define SINGLETON_H

#include <Precompiled/pch.h>

namespace BE
{
	
	template <typename T>
	class Singleton
	{
	public:
		using SingletonPtr = std::unique_ptr<T, std::function<void(T*)>>;

	private:
		static SingletonPtr m_instance;

	protected:
		explicit Singleton();
		virtual ~Singleton();

	public:
		//Creates an instance of T, Args is the constructor parameter of T.
		template <typename... Args>
		static SingletonPtr& CreateInstance(Args&& ...args);
		
		//Destroys the instance of T.
		virtual void DeleteInstance();
		
		//Checks if the instance does not exist
		static bool IsNull();

		//Gets the instance of T. Does not create 1 if instance does not exist.
		inline static SingletonPtr& GetInstance() { return m_instance; }

		//Delete copy ctor/assignment
		Singleton<T>(const Singleton<T>&) = delete;
		Singleton<T>& operator=(const Singleton<T>&) = delete;
	};
}

#include "Singleton.hpp"

/*
	To make a class singleton
	 1: Add friend class Singleton<T> in the class T itself
	 2: Add public inheritance to Singleton<T> in class T
	 3: Make T class constructor and destructor private


	Sample usage:

	struct SomeStruct : public BE::Singleton<SomeStruct>
	{
	private:
		SomeStruct();
		~SomeStruct();

		friend class Singleton<T>;
	}
*/


#endif