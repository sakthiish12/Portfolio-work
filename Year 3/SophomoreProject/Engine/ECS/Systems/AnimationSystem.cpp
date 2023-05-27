// /******************************************************************************/
// /*!
// \file         AnimationSystem.cpp
// \project name Engine
// \author(s)    Ryan Wang 
// \date         1st Mar, 2022
// \brief        This file contains the definition of the tranform parent system
//				 that calculates world matrix of entity based off its parents
// \lines        22 Lines
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "AnimationSystem.h"

//extern
std::shared_ptr<AnimationSystem> AnimationSysPtr;

//To facilitate with animations of sprites
void AnimationSystem::update(float dt)
{
	for (auto const& e : getEntitiesRegistered())
	{
		auto& sprite = ecs.getComponent<spriteCom>(e);
		//Only animate if the sprite is animated
		//AND has the frame data
		if (sprite.loop && 
			sprite.frameData.size() > 0 &&
			sprite.currentAnimation.size() > 0)
		{
			//Reduce (or increase) lifetime of frame by framerate
			sprite.frameLifetimeRemaining -= sprite.speed * dt;
			//Proceed to next frame
			if (sprite.frameLifetimeRemaining < 0.0f)
			{
				++sprite.currentFrame;
				//Loop back to the beginning
				if (sprite.currentFrame == sprite.currentAnimation.size())
					sprite.currentFrame = 0;

				//Setting the corresponding sprite current texture coordinates
				//Remember that currentFrame is an index within the current animation
				sprite.currentTexCoords = sprite.frameData[sprite.currentAnimation[sprite.currentFrame]];

				//Reset frame lifetime
				//+= operator to compensate for if frameLifetimeRemaining goes negative
				sprite.frameLifetimeRemaining += 1.0f;
			}

			//Proceed to previous frame (if going in reverse, i.e. speed is negative)
			if (sprite.frameLifetimeRemaining > 1.0f)
			{
				if (sprite.currentFrame > 0)
					--sprite.currentFrame;
				else //Loop back to the end
					sprite.currentFrame = sprite.currentAnimation.size() - 1;

				//Setting the corresponding sprite current texture coordinates
				//Remember that currentFrame is an index within the current animation
				sprite.currentTexCoords = sprite.frameData[sprite.currentAnimation[sprite.currentFrame]];

				//Reset frame lifetime
				//-= operator to compensate for if frameLifetimeRemaining goes beyond 1
				sprite.frameLifetimeRemaining -= 1.0f;
			}
		}
	}
}