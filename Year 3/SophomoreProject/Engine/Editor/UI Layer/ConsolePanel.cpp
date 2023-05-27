// /******************************************************************************/
// /*!
// \file         ConsolePanel.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         8th Oct, 2021
// \brief        Drawing collision boundary using graphic debugging
// \lines        45 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"
#include"../../Debugging/Log.h"
#include "../../Debugging/Assert.h"
#include "../../Mono/Mono.h"
#include "../../Sound/SoundManager.h"
#include <functional>
#include <thread>
#include <iostream>

//#include "../../ECS/Systems/CSharpScriptSystem.h"

std::vector<std::string> Log::m_consoleLog;

void UiLayer::DrawPanel(bool* p_open)
{
	UNREFERENCED_PARAMETER(p_open);
	ImGui::Begin("Debug");

	if (ImGui::Button("Crash"))
	{
		/*Log logger;
		logger.Error(false,"Oo no");*/
		PartyAssert(false,"what's going on");
	}

	//if (ImGui::Button("Scipting Test"))
	//{
	//	/*Log logger;
	//	logger.Error(false,"Oo no");*/
	//	//PartyAssert(false, "what's going on");

	//	//Mono::GetInstanced();
	//	Log logger;
	//	logger.Info(true, "Scripting loaded");
	//}

	//if (ImGui::Button("Sound Test Boom"))
	//{
	//	

	//	//SoundManager::GetInstance().BoomSound();
	//	
	//	
	//	//soundsystem->release();
	//	Log logger;
	//	logger.Info(true, "Boom Sound");
	//}
	//if (ImGui::Button("Release"))
	//{
	///*	SoundClass* a{};
	//	a->release();*/
	//	

	//	//SoundManager::GetInstance().playPlayerWalking();
	//	Log logger;
	//	logger.Info(true, "Play Player Walking");
	//}
	//if (ImGui::Button("Sound Test Clicking Sound"))
	//{
	//	//SoundManager::GetInstance().playClickSound();
	//	Log logger;
	//	logger.Info(true, "Play Clicking Sound");
	//}

	for (auto it = Log::m_consoleLog.rbegin(); it != Log::m_consoleLog.rend(); ++it)
	{
		ImGui::Text("%s", it->c_str());
	}
	ImGui::End();
}
