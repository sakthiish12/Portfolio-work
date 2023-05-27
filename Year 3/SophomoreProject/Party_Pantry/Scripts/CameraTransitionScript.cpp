// /******************************************************************************/
// /*!
// \file         CameraTransitionScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the camera transition
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
#include "CameraTransitionScript.h"//camera transition
#include "../../Engine/ECS/Systems/Logic.h"//logic
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/AbilitiesSystem.h"//abilities system

CameraTransitionScript::CameraTransitionScript(const char* scriptName) :
	scriptBase(scriptName)
{

}

void CameraTransitionScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	currentTime = timeToChange;
}

void CameraTransitionScript::update(float dt)
{
	if (ecs.entityHas<transform2D>(id))
	{
		//fade in out 
		if (fadeInOut == true) {
			//fade in
			PostProcessing::GetInstance().m_AlphaValue >= 1.f ? PostProcessing::GetInstance().m_AlphaValue = 1.f : PostProcessing::GetInstance().m_AlphaValue += speedOfTransition * dt;
		}



		//if the toggle is true and the panning has not been activated before
		if (toggleActive == true && panningActivated == false) {
			activeCameraData->isFixedToPlayer = false;//disable follow player 

			//to skip
			if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON) && InputManager.mouseClickEnabled) {
				skippedPanning = true;
				activeCameraTransform->pos.y = endingPosY;
				activeCameraTransform->pos.x = endingPosX;
			}

			//move the camera 
			if (firstMovement == true) {
				activeCameraTransform->pos.x += 150.f * dt;//increasing the pos 
				//if reached second position, activate second movement 
				if (activeCameraTransform->pos.x >= endingPosX) secondMovement = true, firstMovement = false;
			}
			
			if (secondMovement == true) {
				activeCameraTransform->pos.y -= 150.f * dt;//decreasing the pos 
			}

			//if reached, deactivate everything 
			if (activeCameraTransform->pos.y <= endingPosY) {
				secondMovement = false;
				currentTime -= dt;
				//wait for a few seconds 
				if (currentTime <= 0.f) {
					secondMovement = false, toggleActive = false;
					activeCameraData->isFixedToPlayer = true;
					panningActivated = true;
					PostProcessing::GetInstance().m_AlphaValue = 1.f;
					ecs.loadLevel("Resources/SavedLevels/NewLevel1Level.json");//loading of the next level 
					LogicSystemsPtr->sceneIsChanging = true;//it is changing scenes 
				}
			}

		}
	}

}

void CameraTransitionScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> CameraTransitionScript::createCopy()
{
	return std::make_shared <CameraTransitionScript>();
}

std::string CameraTransitionScript::getScriptName()
{
	return name;
}






