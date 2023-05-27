#pragma once
#include "pch.h"
#include "AnimationSystem.h"
#include "../../Engine/PauseControl.h"
#include "../../Engine/SpeedControl.h"
#include <Systems/Graphics/Be_bone.h>
#include "../RendererSystem/RendererSystem.h"
#include "../ECS/ECS.h"
#include <AssetManager/AssetManager.h>

namespace BE
{
	void AnimationSystem::Init()
	{

	}

	void AnimationSystem::Awake()
	{

	}

	void AnimationSystem::Update(double dt)
	{
#ifdef USE_EDITOR
		//If IMGUI is pausing, then don't update
		if (Debug::GetInstance()->GetDebugFlag(Debug::DebugFlags::imguiPause))
			return;
#endif

		for (EntityID entity : ECS->GetTracker<AnimationComponentTracker>()->GetTrackedEntities())
		{
			//If entity is inactive, then skip
			if (!ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::active))
				continue;

			//Don't update animation when pausing
			if (PauseControl::GetPause() && !ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::updateWhenPaused))
				continue;

			//Don't update animation when too far away
			if (!ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::updateWhenNotVisible) &&
				ECS->GetComponent<Transform>(entity).GetPosition().SqDistBetween(RendererSystem::GetInstance()->GameCamera.tf.GetPosition()) > MAX_UPDATE_SQ_DIST)
				continue;

			float final_dt = ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::updateWhenPaused) ?
				static_cast<float>(dt) : static_cast<float>(dt) * EngineSpeed->GetEngineSpeedMultiplier();

			if ((ECS->GetComponent<Renderer>(entity).getModelID()))
			{
				auto& modelAsset = AssetMgr->GetAssetData<BeModel>(ECS->GetComponent<Renderer>(entity).getModelID());
				auto& animationAsset = modelAsset.GetSpecificAnimation(ECS->GetComponent<AnimationComponent>(entity).GetCurrentAnimationID());
				auto& animComp = ECS->GetComponent<AnimationComponent>(entity);

				if (animationAsset.GetDuration())
				{
					currentTime = &animComp.getCurrentTime();
					
					if ((animComp.getFreezeAtLastFrame() &&
						*currentTime + (static_cast<float>(animationAsset.GetTicksPerSecond()) * animComp.getTickFactor()) * final_dt < animationAsset.GetDuration()))
					{
						*currentTime += (static_cast<float>(animationAsset.GetTicksPerSecond()) * animComp.getTickFactor()) * final_dt;
					}
					else if (!animComp.getFreezeAtLastFrame())
					{
						*currentTime += (static_cast<float>(animationAsset.GetTicksPerSecond()) * animComp.getTickFactor()) * final_dt;
					}

					if (*currentTime >= animationAsset.GetDuration())
					{
						animComp.setCompletedOnce(true);
					}

					*currentTime = fmod(*currentTime, animationAsset.GetDuration());
					CalculateBoneTransformation(animationAsset, ECS->GetComponent<AnimationComponent>(entity));
				}
			}
		}
	}

	void AnimationSystem::Reload()
	{

	}

	void AnimationSystem::Shutdown()
	{

	}

	AnimationSystem::AnimationSystem()
	{
	}

	AnimationSystem::~AnimationSystem()
	{
	}

	void AnimationSystem::CalculateBoneTransformation(Animation& anim, AnimationComponent& component)
	{
		auto& allBones = anim.GetBoneIDMap();

		BeBone* bone{};

		for (int i{}; i < allBones.size(); ++i)
		{
			//find if a bone is engaged in this animation
			bone = anim.FindBone(allBones[i].idOfBone);
			parentTransform = glm::mat4(1.0f);

			if (i > 0)
			{
				parentTransform = component.GetLocalBoneMatrices()[allBones[i].idOfParentBone];
			}

			//interpolate bone data of this animation
			bone->Update(component.getCurrentTime());
			nodeTransform = bone->GetLocalTransform(); //move bone around origin, also called animTransform


			component.GetLocalBoneMatrices()[allBones[i].idOfBone] = parentTransform * nodeTransform;
			component.GetFinalBoneMatrices()[allBones[i].idOfBone] = 
				component.GetLocalBoneMatrices()[allBones[i].idOfBone] * allBones[i].offset;
		}
	}
}