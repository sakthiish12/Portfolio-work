// /******************************************************************************/
// /*!
// \file         .cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         10th Apr, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <stdlib.h>
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include "GameIconScript.h"//Logo
#include <ECS/Systems/Logic.h>

GameIconScript::GameIconScript(const char* scriptName) :
	scriptBase(scriptName)
{

}

void GameIconScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

void GameIconScript::update(float dt)
{
	//fade out 
	if (fadeInOut == false) {
		//decrease the alpha of the texture 
		PostProcessing::GetInstance().m_AlphaValue <= 0.f ? PostProcessing::GetInstance().m_AlphaValue = 0.f : PostProcessing::GetInstance().m_AlphaValue -= speedOfTransition * dt;

		//if the player wants to skip this cut scene 
		if (PostProcessing::GetInstance().m_AlphaValue <= 0.f) {
			//change scene to the cut scene 2
			ecs.loadLevel("Resources/SavedLevels/MainMenuLevel.json");//mainmenu
			PostProcessing::GetInstance().m_AlphaValue = 1.f;
			LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes
		}
	}
	else {
		//fade in
		PostProcessing::GetInstance().m_AlphaValue >= 1.f ? PostProcessing::GetInstance().m_AlphaValue = 1.f : PostProcessing::GetInstance().m_AlphaValue += speedOfTransition * dt;
		if (PostProcessing::GetInstance().m_AlphaValue >= 1.f) {
			fadeInOut = false;
		}
	}
}

void GameIconScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> GameIconScript::createCopy()
{
	return std::make_shared <GameIconScript>();
}

std::string GameIconScript::getScriptName()
{
	return name;
}