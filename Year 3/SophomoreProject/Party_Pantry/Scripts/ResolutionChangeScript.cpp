// /******************************************************************************/
// /*!
// \file         ResolutionChangeScript.cpp
// \project name Engine
// \author(s)    Sakthiish
// \date         18th March, 2022 <Date of last SIGNIFICANT CHANGE>
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
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"

#include "ResolutionChangeScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Logic.h"
#include "../../Engine/Editor/Editor.h"//editor 
#include <iostream>
#include <Sound/SoundManager.h>
#include <Graphics/Source/PostProcessing.h>

using std::cout;
using std::endl;

ResolutionChangeScript::ResolutionChangeScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void ResolutionChangeScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	if (WindowsManager.getFullscreenState() == false) {
		ecs.getComponent<transform2D>(id).pos.z = 0.f;
	}
	else {
		ecs.getComponent<transform2D>(id).pos.z = 1.5f;
	}
	state = scriptBase::ScriptState::UPDATE;
}

void ResolutionChangeScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);

	int counter = 1;
	if (ecs.entityHas<spriteCom>(id)) {
		//auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform
		//mouse picking 
		Vec2 mousePos = MouseManager.getWorldPosition();
		if ((
			mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
			mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
			mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f)) && MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON) && counter == 1)
		{
			
				if (WindowsManager.getFullscreenState()) {
					WindowsManager.setFullscreenState(false);
					//updating the frame buffer size
					//resize the post processing frame 
					PostProcessing::GetInstance().m_PostProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
					//resize the pre processing frame 
					PostProcessing::GetInstance().m_PreProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
					ecs.getComponent<transform2D>(id).pos.z = 0;
				}
				else {
					WindowsManager.setFullscreenState(true);
					//updating the frame buffer size
					//resize the post processing frame 
					PostProcessing::GetInstance().m_PostProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
					//resize the pre processing frame 
					PostProcessing::GetInstance().m_PreProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
					ecs.getComponent<transform2D>(id).pos.z = 1.5;
				}
		
			//CHANGE TEXTURE
			//ecs.getComponent<transform2D>(id).pos.z = 0;
			counter = 0;
		}

	}

}

void ResolutionChangeScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);

}

std::shared_ptr<scriptBase> ResolutionChangeScript::createCopy()
{
	return std::make_shared <ResolutionChangeScript>();
}

std::string ResolutionChangeScript::getScriptName()
{
	return name;
}






