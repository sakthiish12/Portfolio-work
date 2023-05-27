#pragma once
#ifndef	ISYSTEM_H
#define ISYSTEM_H

#include "../Precompiled/pch.h"

//Miscellaneous systems
#include <Debugger/Debugger.h>
#include <EventDispatcher/EventDispatcher.h>
//#include <AssetManager/AssetManager.h>

namespace BE
{
	//Base class for all engine systems
	class ISystem
	{
		//For profiling
		double m_frameStartTime;
		double m_systemDt;

	protected:
		ISystem() : m_frameStartTime(0.0), m_systemDt(0.0) {}

	public:
		virtual ~ISystem() = 0 {}
		virtual void Init() = 0;						//1 time initialisation of the system after constructor is called
		virtual void Awake() = 0;						//To be called when editor goes into play mode
		virtual void Update(double dt) = 0;				//The usual update loop
		virtual void Reload() = 0;						//To be called when editor goes into pause mode
		virtual void Shutdown() = 0;					//1 time uninitalisation of the system before destructor is called

		//For system profiling - called in game engine class
		inline void StartSystemDt() { m_frameStartTime = glfwGetTime(); }
		inline void EndSystemDt() { m_systemDt = glfwGetTime() - m_frameStartTime; }
		inline double GetSystemDt() { return m_systemDt; }
	};
}
#define ISYSTEM_FUNCTIONS	void Init() override; \
							void Awake() override; \
							void Update(double dt) override; \
							void Reload() override; \
							void Shutdown() override
#endif