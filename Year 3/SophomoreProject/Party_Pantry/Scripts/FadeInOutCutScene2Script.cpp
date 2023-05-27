// /******************************************************************************/
// /*!
// \file         FadeInOutCutScene2Script.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the fading in and out of the scene to the cutscene 2 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include "FadeInOutCutScene2Script.h"//cut scene 2
#include "../../Engine/InputMgr/InputMgr.h"//key manager 
#include "../../Engine/ECS/Systems/Logic.h"//logic

FadeInOutCustScene2Script::FadeInOutCustScene2Script(const char* scriptName) :
	scriptBase(scriptName)
{

}

void FadeInOutCustScene2Script::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	PostProcessing::GetInstance().m_AlphaValue = 0.f;//start from total black 
}

void FadeInOutCustScene2Script::update(float dt)
{
	//if it's activated 
	if (active == true) {
		//fade out 
		if (fadeInOut == false) {
			//decrease the alpha of the texture 
			PostProcessing::GetInstance().m_AlphaValue <= 0.f ? PostProcessing::GetInstance().m_AlphaValue = 0.f : PostProcessing::GetInstance().m_AlphaValue -= speedOfTransition * dt;

			//if the player wants to skip this cut scene 
			if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) == true || PostProcessing::GetInstance().m_AlphaValue <= 0.f) {
				//PostProcessing::GetInstance().m_AlphaValue = 1.f;//resetting the alpha 
				InputManager.mouseClickDisabled = true;
				InputManager.mouseClickEnabled = false;
				//change scene to the cut scene 2
				ecs.loadLevel("Resources/SavedLevels/NewLevel1PanningLevel.json");//load the blender level 
				LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes
			}
		}
		else {
			//fade in
			PostProcessing::GetInstance().m_AlphaValue >= 1.f ? PostProcessing::GetInstance().m_AlphaValue = 1.f : PostProcessing::GetInstance().m_AlphaValue += speedOfTransition * dt;

			//if (PostProcessing::GetInstance().m_AlphaValue >= 1.0f) fadeInOut = false;//switching 

			if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) == true) {
				PostProcessing::GetInstance().m_AlphaValue = 1.f;//immediately set to 1.0 just in case 
				fadeInOut = false;//switching to the fade out 
			}

		}
	}
}

void FadeInOutCustScene2Script::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> FadeInOutCustScene2Script::createCopy()
{
	return std::make_shared <FadeInOutCustScene2Script>();
}

std::string FadeInOutCustScene2Script::getScriptName()
{
	return name;
}






