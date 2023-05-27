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

#include "PauseQuitGameScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../../Engine/ECS/Systems/Logic.h"
#include "../../../Engine/Editor/Editor.h"//editor 
#include <iostream>
//#include <Sound/SoundManager.h>
#include <Sound/SoundManagerV2.h>

using std::cout;
using std::endl;

PauseQuitGameScript::PauseQuitGameScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void PauseQuitGameScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;

	//getting the player entity id 
	for (auto const& e : ecs.getEntityNames())
	{
		if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
		{
			playerID = e.first;//player is found 
			break;
		}
	}

	UiLayer::GetInstance().isGamePlayLevel = true;//TODO : think of a better way to do this 
}

void PauseQuitGameScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (ecs.entityHas<spriteCom>(id)) {
		auto& oSprite = ecs.getComponent<spriteCom>(id);//getting the component sprite
		auto& oTransform = ecs.getComponent<transform2D>(id);//getting the component transform

		//if the scene state is pause, toggle visibility
		if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause && UiLayer::GetInstance().dialoguePause == false) {

			
			oSprite.isVisible = true;

			//mouse picking 
			Vec2 mousePos = MouseManager.getWorldPosition();

			if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON) &&
				mousePos.x >= (oTransform.worldMatrix[3][0] - oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.x <= (oTransform.worldMatrix[3][0] + oTransform.worldMatrix[0][0] / 2.0f) &&
				mousePos.y >= (oTransform.worldMatrix[3][1] - oTransform.worldMatrix[1][1] / 2.0f) &&
				mousePos.y <= (oTransform.worldMatrix[3][1] + oTransform.worldMatrix[1][1] / 2.0f) &&
				UiLayer::GetInstance().m_InstructionsState == UiLayer::InstructionsState::InstructionsOff && InputManager.mouseClickEnabled)
			{

				//if the entity is not loaded and is not showing 
				if (isEntityLoaded == false && isEntityShowing == false) {
					//creating the entity 
					backgroundID = ecs.createEntityFromFile("Resources/PresetEntities/PauseQuitBG.json");
					noButtonID = ecs.createEntityFromFile("Resources/PresetEntities/PauseQuitNo.json");
					yesButtonID = ecs.createEntityFromFile("Resources/PresetEntities/PauseQuitYes.json");


					//setting to the parent which is the player 
					ecs.setEntityParent(backgroundID, id);//setting the parent to the player 
					ecs.setEntityParent(noButtonID, id);//setting the parent to the player 
					ecs.setEntityParent(yesButtonID, id);//setting the parent to the player 


					isEntityLoaded = true;
					isEntityShowing = true;
				}
				else {
					isEntityShowing = true;//make all the entities visible 
				}

			}

			//if the entity is to be shown 
			if (isEntityShowing == true) {

				UiLayer::GetInstance().pauseQuitConfirmation = true;//setting the pause confirmation to true 

				//making it all visible 
				ecs.getComponent<spriteCom>(backgroundID).isVisible = true;
				ecs.getComponent<spriteCom>(noButtonID).isVisible = true;
				ecs.getComponent<spriteCom>(yesButtonID).isVisible = true;

				//checking if the button is pressed with the no button 
				auto& noButtonTransform = ecs.getComponent<transform2D>(noButtonID);
				//checking if the button is being hovered 
				if (
					mousePos.x >= (noButtonTransform.worldMatrix[3][0] - noButtonTransform.worldMatrix[0][0] / 2.0f) &&
					mousePos.x <= (noButtonTransform.worldMatrix[3][0] + noButtonTransform.worldMatrix[0][0] / 2.0f) &&
					mousePos.y >= (noButtonTransform.worldMatrix[3][1] - noButtonTransform.worldMatrix[1][1] / 2.0f) &&
					mousePos.y <= (noButtonTransform.worldMatrix[3][1] + noButtonTransform.worldMatrix[1][1] / 2.0f))
				{
					ecs.getComponent<material>(noButtonID).color.z = 0.5f;//change the alpha of the button 

					//if the no button is triggered
					if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON)) {
						isEntityShowing = false;//make it not showing 
					}
				}
				else {
					ecs.getComponent<material>(noButtonID).color.z = 1.f;//change the alpha of the button 
				}

				//checking if the button is pressed with the yes button 
				auto& yesButtonTransform = ecs.getComponent<transform2D>(yesButtonID);
				//checking if the button is being hovered 
				if (
					mousePos.x >= (yesButtonTransform.worldMatrix[3][0] - yesButtonTransform.worldMatrix[0][0] / 2.0f) &&
					mousePos.x <= (yesButtonTransform.worldMatrix[3][0] + yesButtonTransform.worldMatrix[0][0] / 2.0f) &&
					mousePos.y >= (yesButtonTransform.worldMatrix[3][1] - yesButtonTransform.worldMatrix[1][1] / 2.0f) &&
					mousePos.y <= (yesButtonTransform.worldMatrix[3][1] + yesButtonTransform.worldMatrix[1][1] / 2.0f))
				{
					ecs.getComponent<material>(yesButtonID).color.z = 0.5f;//change the alpha of the button 

					//if the yes button is triggered
					if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON)) {

						//doing the necessary stuff to transition back to the editor mode 						
						audio.chBGM->stop();
						audio.chSFX->stop();
						
						UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Edit;//setting the scene state to edit 
						Editor::GetInstance().isPlaying = false;//is playing false 
						UiLayer::GetInstance().isGamePlayLevel = false;//TODO : think of a better way to do this 
						UiLayer::GetInstance().pauseQuitConfirmation = false;

						//Reload temporary level
						ecs.loadLevel("Resources/TemporaryLevel.json");
						//Delete temporary level
						std::filesystem::remove("Resources/TemporaryLevel.json");
						LogicSystemsPtr->sceneIsChanging = true;
					}
				}
				else {
					ecs.getComponent<material>(yesButtonID).color.z = 1.f;//change the alpha of the button 
				}


			}
		}
		else {
			oSprite.isVisible = false;
			isEntityShowing = false;
		}

		//when the entity is not showing and it is already loaded 
		if (isEntityShowing == false && isEntityLoaded == true) {
			//making it all visible 
			ecs.getComponent<spriteCom>(backgroundID).isVisible = false;
			ecs.getComponent<spriteCom>(noButtonID).isVisible = false;
			ecs.getComponent<spriteCom>(yesButtonID).isVisible = false;

			UiLayer::GetInstance().pauseQuitConfirmation = false;
		}
	}

}

void PauseQuitGameScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	cout << "end()" << endl;
}

std::shared_ptr<scriptBase> PauseQuitGameScript::createCopy()
{
	return std::make_shared <PauseQuitGameScript>();
}

std::string PauseQuitGameScript::getScriptName()
{
	return name;
}






