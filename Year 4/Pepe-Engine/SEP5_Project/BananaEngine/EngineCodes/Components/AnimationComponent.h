#pragma once
#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include "ECSComponent.h"

namespace BE
{
	class AnimationComponent : public IECSComponent
	{
	public:
		AnimationComponent();
		~AnimationComponent();

		inline void SetAnimation(unsigned idx);
		inline unsigned GetAnimation();
		void PlayAnimation(unsigned int id,const std::string&);

		inline float& getCurrentTime();
		unsigned GetCurrentAnimationID();
		std::vector<glm::mat4>& GetFinalBoneMatrices();
		std::vector<glm::mat4>& GetLocalBoneMatrices() { return m_LocalBoneMatrices; };
		inline std::string& getAnimationName();
		bool getCompletedOnce();
		inline float getTickFactor();
		bool getFreezeAtLastFrame();

		inline void setCurrentTime(float& time);
		inline void SetFinalBoneMatrices(std::vector<glm::mat4>);
		inline void setAnimationName(std::string&);
		void setCompletedOnce(bool b);
		inline void setTickFactor(float i);
		void setFreezeAtLastFrame(bool b);
	private:
		float tickFactor{1.0f};
		float m_CurrentTime{};
		unsigned currentAnimationID{};
		std::string animationName{};
		std::vector<glm::mat4> m_FinalBoneMatrices;
		std::vector<glm::mat4> m_LocalBoneMatrices;
		bool completedOnce{};
		bool freezeAtLastFrame{};
		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
			RTTR_REGISTRATION_FRIEND
	};
}
#endif