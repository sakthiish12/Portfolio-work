// /******************************************************************************/
// /*!
// \file         PauseScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions of the pause script  
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../../Engine/ECS/component.h"
#include "../../../Engine/ECS/ECS.h"

#include "PauseScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../WinMgr/WinMgr.h"
#include <iostream>
#include <Sound/SoundManager.h>
using std::cout;
using std::endl;

PauseScript::PauseScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void PauseScript::update(float dt)
{
	//if ((KeyManager.isKeyTriggered(VK_LCONTROL) || KeyManager.isKeyTriggered(VK_RCONTROL)) && KeyManager.isKeyTriggered(VK_MENU) && KeyManager.isKeyTriggered(VK_DELETE))
	if (WindowsManager.getIsWindowActive() == false)
	{
		UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;

		if (ecs.entityHas<spriteCom>(id)) {
			auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite

			//if the scene state is pause, toggle visibility
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause) {
				oSprite.isVisible = true;
			}
			else {
				oSprite.isVisible = false;
			}
		}
	}

	if ((KeyManager.isKeyTriggered((unsigned)VK_LCONTROL) || KeyManager.isKeyTriggered((unsigned )VK_RCONTROL)) && KeyManager.isKeyTriggered(VK_MENU) && KeyManager.isKeyTriggered(VK_DELETE))
	{
		UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;

		if (ecs.entityHas<spriteCom>(id)) {
			auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite

			//if the scene state is pause, toggle visibility
			if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause) {
				oSprite.isVisible = true;
			}
			else {
				oSprite.isVisible = false;
			}
		}
	}

	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)){
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		
		//if the scene state is pause, toggle visibility
		if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause && UiLayer::GetInstance().dialoguePause == false) {
			oSprite.isVisible = true;
			for(int i = 0; i < 37; i++)
			{
				//SoundClass::getInstance()->channels[i]->setPaused(true);
			}
		}
		else {
			oSprite.isVisible = false;
		}
	}


}

void PauseScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PauseScript::createCopy()
{
	return std::make_shared <PauseScript>();
}

std::string PauseScript::getScriptName()
{
	return name;
}






