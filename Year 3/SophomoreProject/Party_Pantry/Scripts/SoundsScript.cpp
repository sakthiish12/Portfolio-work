// /******************************************************************************/
// /*!
// \file         SoundsScript.cpp
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

#include "SoundsScript.h"
#include <iostream>
#include <Sound/SoundManagerV2.h>
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"
#include "../../Engine/InputMgr/InputMgr.h"
using std::cout;
using std::endl;

SoundScript::SoundScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void SoundScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);
	const float pos[] = { 0.f,0.f,0.f };
	//musicState m_musicState = musicState::Play;
	Log log;
	//SoundClass SoundMade;
	
	id = idToBeSet;

	//Change to relevant BGM once (only for those whose Com_Font is fixed)
	// Note: Move those that Com_Font can change within a single scene to update, and handle it properly

	//if endgame play this 
	if (ecs.entityHas<Com_Font>(id))
	{
		std::string text = "endgame";
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == text)
		{
			//Insert Sound Here #EndgameMusic
			audio.changeBGM(audio.bgmWin2);
		}
		//if (KeyManager.isKeyReleased(VK_ESCAPE) || KeyManager.isKeyReleased('P'))
		if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON))
		{
			text = "ass";
			//SoundClass::getInstance()->Pause(ENDGAME);
		}
	}

	if (ecs.entityHas<Com_Font>(id))
	{
		std::string text = "Quitgame";
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == text)
		{
			//Insert Sound Here #EndgameMusic

		}
		//if (KeyManager.isKeyReleased(VK_ESCAPE) || KeyManager.isKeyReleased('P'))
		if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON))
		{
			text = "ass";
			//SoundClass::getInstance()->Pause(ENDGAME);
		}
	}

	//if endgame play this 
	if (ecs.entityHas<Com_Font>(id))
	{
		std::string text = "Win";
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == text)
		{
			//Insert Sound Here #WinSound
			audio.changeBGM(audio.bgmWin1);
			/*SoundClass::getInstance()->setArgsAndPlay(BACKGROUND, pos);
			SoundClass::getInstance()->Pause(WALKING);
			SoundClass::getInstance()->Pause(SUCTION);*/
		}
		if (KeyManager.isKeyPressed(VK_ESCAPE) && KeyManager.isKeyPressed(VK_CONTROL, true))
		{
			//text = "ass";
			//SoundClass::getInstance()->Pause(BACKGROUND);
		}


	}


	//if Main Menu play this
	if (ecs.entityHas<Com_Font>(id))
	{
		std::string text = "MainMenu";
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == text)
		{
			audio.changeBGM(audio.bgmMainMenu);
		}
	}

	//SoundClass::getInstance()->updatesystem();

	//state = scriptBase::ScriptState::END;

	if (ecs.entityHas<Com_Font>(id))
	{
		std::string text = "ThroneRoom";
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == text)
		{
			//Insert Sound Here #ThroneRoom
			audio.changeBGM(audio.bgmThroneRoom);

		}
	}
	if (ecs.entityHas<Com_Font>(id))
	{
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == "level1")
		{
			audio.changeBGM(audio.bgmNewLevel1);
		}
	}

	if (ecs.entityHas<Com_Font>(id))
	{
		std::string text = "level2";
		auto& oFont = ecs.getComponent<Com_Font>(id);//getting the component font
		if (oFont.text == text)
		{

			//SoundClass::getInstance()->channels[background]->setPaused(false);
		}
	}
	state = scriptBase::ScriptState::UPDATE;
}

void SoundScript::update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	//cout << "update(float dt)" << " called by : " << id << " count " << testCounter++ << endl;
	float pos[] = { 0,0,0 };
	//if (ecs.entityHas<transform2D>(id))
	//{
		//if ((KeyManager.isKeyPressed('W') || KeyManager.isKeyPressed('A') || KeyManager.isKeyPressed('S') || KeyManager.isKeyPressed('D')))
		//{
			//std::cout << "ass" << std::endl;
			//SoundClass::getInstance()->setArgsAndPlay(WALKING, pos);
			//SoundClass::getInstance()->PlayAgain(WALKING);
		//}

		//if (KeyManager.isKeyReleased('W') || KeyManager.isKeyReleased('A') || KeyManager.isKeyReleased('S') || KeyManager.isKeyReleased('D'))
		//{
			//SoundClass::getInstance()->Pause(WALKING);
		//	SoundClass::getInstance()->setArgsAndPlay(WALKING, pos);
		//}

	/*	if (KeyManager.isKeyReleased('Q') || KeyManager.isKeyReleased('E')) 
		{
			SoundClass::getInstance()->CreateSound("skills.mp3", false);
			SoundClass::getInstance()->CreateChannel(Toggle, false);
			SoundClass::getInstance()->setVolume(Toggle, (float)0.3);
			SoundClass::getInstance()->setArgsAndPlay(Toggle, pos);
		}*/

		//if (KeyManager.isKeyReleased(VK_ESCAPE))
		//{
			//SoundClass::getInstance()->Pause(WALKING);
			//SoundClass::getInstance()->Pause(BACKGROUND);
			//SoundClass::getInstance()->Pause(SUCTION);
			//SoundClass::getInstance()->Pause(ENDGAME);
		//}

		//if(MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON)&& UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Play)
		//{
			
		//}
		//if(MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON))
		//{
			//SoundClass::getInstance()->Pause(SUCTION);
		//}




	//}

	//if (ecs.entityHas<AABB>(id))
	//{
	//	std::vector<unsigned int> collidedContainer = collisionSystemsPtr->getEntityCollidedWith(id);
	//	//cout << "update(float dt)" << " called by : " << id << " count " << testCounter++ << endl;
	//	for (unsigned int x : collidedContainer)
	//	{
	//		cout << "entity " << id << " is colliding with: " << x << endl;

	//		if (ecs.entityHas<Com_type>(x) == 1)
	//		{
	//			float position[] = { 0,0,0 };

	//		}
	//		else
	//		{
	//			//SoundClass::getInstance()->Pause(SUCTION);
	//		}

	//	}
	//	if (collidedContainer.size() == 0)
	//	{
	//		//cout << "entity " << id << " is NOT colliding" << endl;
	//	}
	//}
	// 
	
	
	

	
}

void SoundScript::end(float dt)
{
	//SoundClass::getInstance()->Pause(BACKGROUND);
	UNREFERENCED_PARAMETER(dt);
	//cout << "end()" << endl;
}

std::shared_ptr<scriptBase> SoundScript::createCopy()
{
	return std::make_shared <SoundScript>();
}

std::string SoundScript::getScriptName()
{
	return name;
}






