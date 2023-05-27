#include "pch.h"
#include "AnimationComponent.h"
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	void AnimationComponent::SetAnimation(unsigned idx)
	{
		currentAnimationID = idx;
		completedOnce = false;
		m_CurrentTime = 0.0f;
	}
	unsigned AnimationComponent::GetAnimation()
	{
		return currentAnimationID;
	}
	AnimationComponent::AnimationComponent()
	{
		m_FinalBoneMatrices.reserve(40);
		m_LocalBoneMatrices.reserve(40);
		for (int i{}; i < 40; ++i)
		{
			m_FinalBoneMatrices.emplace_back(glm::mat4(1.0f));
			m_LocalBoneMatrices.emplace_back(glm::mat4(1.0f));
		}
	}

	AnimationComponent::~AnimationComponent()
	{
	}



	void AnimationComponent::PlayAnimation(unsigned int id,const std::string& name)
	{
		currentAnimationID = id;
		animationName = name;
		m_CurrentTime = 0.0f;
		completedOnce = false;
	}

	float& AnimationComponent::getCurrentTime()
	{
		return m_CurrentTime;
	}

	void AnimationComponent::setCurrentTime(float& time)
	{
		m_CurrentTime = time;
	}

	unsigned AnimationComponent::GetCurrentAnimationID()
	{
		return currentAnimationID;
	}

	std::vector<glm::mat4>& AnimationComponent::GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
	std::string& AnimationComponent::getAnimationName()
	{
		return animationName;
	}
	bool AnimationComponent::getCompletedOnce()
	{
		return completedOnce;
	}
	float AnimationComponent::getTickFactor()
	{
		return tickFactor;
	}
	bool AnimationComponent::getFreezeAtLastFrame()
	{
		return freezeAtLastFrame;
	}
	void AnimationComponent::SetFinalBoneMatrices(std::vector<glm::mat4> v)
	{
		m_FinalBoneMatrices = v;
	}
	void AnimationComponent::setAnimationName(std::string& s)
	{
		animationName = s;
	}
	void AnimationComponent::setCompletedOnce(bool b)
	{
		completedOnce = b;
	}
	void AnimationComponent::setTickFactor(float i)
	{
		tickFactor = i;

		if (i < 0.0f)
		{
			tickFactor = 0.0f;
		}
	}
	void AnimationComponent::setFreezeAtLastFrame(bool b)
	{
		freezeAtLastFrame = b;
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<AnimationComponent>(RegistrationNameCreator<AnimationComponent>())
		.constructor()
		.property("Time", &AnimationComponent::getCurrentTime, &AnimationComponent::setCurrentTime)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("AnimationName", &AnimationComponent::getAnimationName, &AnimationComponent::setAnimationName)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("AnimationIdx", &AnimationComponent::GetAnimation, &AnimationComponent::SetAnimation)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("TickOffset", &AnimationComponent::getTickFactor, &AnimationComponent::setTickFactor)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			);
}