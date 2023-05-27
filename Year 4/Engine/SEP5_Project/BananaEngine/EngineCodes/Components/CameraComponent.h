#pragma once
#ifndef	CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include <Maths/Vector3D.h>
#include <Maths/Quaternion.h>
#include <Maths/Matrix3D.h>

#include "ECSComponent.h"

namespace BE
{
	class cameraComponent : public IECSComponent
	{
	public:
		cameraComponent();
		~cameraComponent();
		cameraComponent(const cameraComponent& rhs) noexcept;
		cameraComponent(cameraComponent&& rhs) noexcept;
		cameraComponent& operator=(const cameraComponent& rhs) noexcept;
		cameraComponent& operator=(cameraComponent&& rhs) noexcept;

		void setOrthographicProjection(float left, float right, float top, float bottom, float near1, float far1);
		void setPerspectiveProjection(float fovy, float aspect, float near1, float far1);
		void setInverseView(const glm::mat4& v);
		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
		// void setViewYXZ(glm::vec3 position, glm::vec3 rotation);
		void setViewYXZ(const Vec3f& _pos, const Quatf& _rot);

		const glm::mat4& getProjection() const;
		const glm::mat4& getView() const;
		const glm::mat4& getInverseView() const;
		const glm::vec3 getPosition() const;
		const glm::mat4& getOrtho() const;
		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
			RTTR_REGISTRATION_FRIEND

	private:
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };
		glm::mat4 inverseViewMatrix{ 1.f };
		glm::mat4 orthoMatrix{};
		std::array<float, 16> GetProjectionByArray();
		std::array<float, 16> GetViewDirectionByArray();
		void SetProjectionByArray(std::array<float, 16> in);
		void SetViewDirectionByArray(std::array<float,16> in);
	};
}
#endif