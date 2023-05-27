// /******************************************************************************/
// /*!
// \file         LevelLoaderButton.cpp
// \project name Engine
// \author(s)    Ryan Wang 
// \date         12th Nov, 2021
// \brief        This file contains the definition of the level loader button system
// \lines        23 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "LevelLoaderButton.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Graphics/Source/PostProcessing.h"//post processing 
#include "../../Sound/SoundManagerV2.h"

//extern
std::shared_ptr<LevelLoaderSystem> LevelLoaderSysPtr;

void LevelLoaderSystem::update(float dt)
{
	static_cast<void>(dt); //Presently unused

	for (auto const& e : getEntitiesRegistered())
	{
		auto& transform = ecs.getComponent<transform2D>(e);
		auto& target = ecs.getComponent<levelLoaderButton>(e);
		Vec2 mousePos = MouseManager.getWorldPosition();
		//std::cout << mousePos.x << ", " << mousePos.y << std::endl;
		if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) &&
			mousePos.x >= (transform.pos.x - transform.scale.x / 2.0f) &&
			mousePos.x <= (transform.pos.x + transform.scale.x / 2.0f) &&
			mousePos.y >= (transform.pos.y - transform.scale.y / 2.0f) &&
			mousePos.y <= (transform.pos.y + transform.scale.y / 2.0f) && InputManager.mouseClickEnabled)
		{
			//Cannot directly pass reference to targetLevelFile here because it gets deleted when clearing all entities while loading level
			std::string targetFile = target.targetLevelFile;

			//Play button SFX
			audio.playPannedSoundEffect(audio.sfxClick2);

			//PostProcessing::GetInstance().m_AlphaValue = 1.0f;//reset the alpha value 
			ecs.loadLevel(targetFile);
			break; //(29 Nov) Break out of the loop, because the list of entities has changed as a result of level changing. Thanks Jerlyn
		}
	}
}