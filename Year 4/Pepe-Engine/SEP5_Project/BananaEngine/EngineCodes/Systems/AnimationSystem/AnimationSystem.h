#pragma once
#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H
#include <Precompiled/pch.h>
#include <Utilities/Singleton/Singleton.h>
#include "../../AssetManager/AssetTypes/Animation.h"
#include "../../Components/AnimationComponent.h"
#include "../ISystem.h"

namespace BE
{
	class Animation;
	class AnimationComponent;
	//This class goes through all animationComponent and updates the animation, calculates bone transformation and sets it there
	class AnimationSystem : public Singleton<AnimationSystem>, public ISystem
	{
	public:
		AnimationSystem();
		~AnimationSystem();
		void CalculateBoneTransformation(Animation& anim, AnimationComponent& component);

	private:
		ISYSTEM_FUNCTIONS;
		glm::mat4 parentTransform{ glm::mat4(1.0f) };
		glm::mat4 nodeTransform{};
		glm::mat4 globalTransformation{};
		float* currentTime{};
		friend class Singleton<AnimationSystem>;
	};
#define Animationsystem AnimationSystem::GetInstance()
}
#endif