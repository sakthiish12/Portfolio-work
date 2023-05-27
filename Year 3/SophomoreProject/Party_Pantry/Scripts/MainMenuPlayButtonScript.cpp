// /******************************************************************************/
// /*!
// \file         PauseQuitGameScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the pause quit game script 
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
#include "MainMenuPlayButtonScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "../../../Engine/Editor/Editor.h"//editor 
#include "../../Engine/AssetManager/UpgradeManager.h"
#include "../../Engine/Graphics/Source/PostProcessing.h"
#include <iostream>
#include <Sound/SoundManager.h>

using std::cout;
using std::endl;

MainMenuPlayScript::MainMenuPlayScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void MainMenuPlayScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}


void MainMenuPlayScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id)) {
		auto& oTransform = ecs.getComponent<transform2D>(id);
		//mouse picking 
		Vec2 mousePos = MouseManager.getWorldPosition();

		if (active == true) {
			//fade out 
			if (fadeInOut == false) {
				//decrease the alpha of the texture 
				PostProcessing::GetInstance().m_AlphaValue <= 0.f ? PostProcessing::GetInstance().m_AlphaValue = 0.f : PostProcessing::GetInstance().m_AlphaValue -= speedOfTransition * dt;

				//if the player wants to skip this cut scene 
				if (((MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) == true)) || PostProcessing::GetInstance().m_AlphaValue <= 0.f) {
					//click on the button 
					if (UpgradeManager::GetInstance().level1Completed == false) {
						ecs.loadLevel("Resources/SavedLevels/CutsceneLevel.json");
						LogicSystemsPtr->sceneIsChanging = true;
					}
					else {
						ecs.loadLevel("Resources/SavedLevels/ThroneRoomLevel.json");//go to throne room instead 
						PostProcessing::GetInstance().m_AlphaValue = 1.f;//resetting the alpha 
						LogicSystemsPtr->sceneIsChanging = true;
					}
				}
			}
		}


		//std::cout << mousePos.x << ", " << mousePos.y << std::endl;
		if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) &&
			mousePos.x >= (oTransform.pos.x - oTransform.scale.x / 2.0f) &&
			mousePos.x <= (oTransform.pos.x + oTransform.scale.x / 2.0f) &&
			mousePos.y >= (oTransform.pos.y - oTransform.scale.y / 2.0f) &&
			mousePos.y <= (oTransform.pos.y + oTransform.scale.y / 2.0f))
		{
			active = true;
		}
	}
}

void MainMenuPlayScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> MainMenuPlayScript::createCopy()
{
	return std::make_shared <MainMenuPlayScript>();
}

std::string MainMenuPlayScript::getScriptName()
{
	return name;
}






