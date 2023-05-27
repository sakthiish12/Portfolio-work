#pragma once
#ifndef	LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "ECSComponent.h"

namespace BE
{
	class lightComponent : public IECSComponent
	{
		glm::vec3 color{1.0f,1.0f,1.0f};
		float intensity{ 1.0f };
		float radius{ 0.75f };
		void SetColorByArray(std::array<float, 3> in);
		std::array<float, 3> GetColorByArray();
	public:
		lightComponent();
		~lightComponent();
		lightComponent(const lightComponent& rhs) noexcept;
		lightComponent(lightComponent&& rhs) noexcept;
		lightComponent& operator=(const lightComponent& rhs) noexcept;
		lightComponent& operator=(lightComponent&& rhs) noexcept;

		glm::vec3 const& getColor();
		inline float getIntensity();
		inline float getRadius();

		void setColor(glm::vec3 const&);
		inline void setIntensity(float);
		inline void setRadius(float);
		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND
	};
}
#endif