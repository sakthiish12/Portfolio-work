#include "pch.h"
#include "cameraComponent.h"
#include <Serialisation/BananaSerialisation.h>
namespace BE
{
	cameraComponent::cameraComponent()
	{
	}

	cameraComponent::~cameraComponent()
	{

	}

	cameraComponent::cameraComponent(const cameraComponent& rhs) noexcept
	{
		projectionMatrix = rhs.projectionMatrix;
		viewMatrix = rhs.viewMatrix;
		inverseViewMatrix = rhs.inverseViewMatrix;
	}

	cameraComponent::cameraComponent(cameraComponent&& rhs) noexcept
	{
		projectionMatrix = rhs.projectionMatrix;
		viewMatrix = rhs.viewMatrix;
		inverseViewMatrix = rhs.inverseViewMatrix;
	}

	cameraComponent& cameraComponent::operator=(const cameraComponent& rhs) noexcept
	{
		projectionMatrix = rhs.projectionMatrix;
		viewMatrix = rhs.viewMatrix;
		inverseViewMatrix = rhs.inverseViewMatrix;
		return *this;
	}

	cameraComponent& cameraComponent::operator=(cameraComponent&& rhs) noexcept
	{
		projectionMatrix = rhs.projectionMatrix;
		viewMatrix = rhs.viewMatrix;
		inverseViewMatrix = rhs.inverseViewMatrix;
		return *this;
	}

	void cameraComponent::setOrthographicProjection(
		float left, float right, float top, float bottom, float near1, float far1) {
		orthoMatrix = glm::mat4{};
		orthoMatrix[0][0] = 2.f / (right - left);
		orthoMatrix[1][1] = 2.f / (top - bottom);
		orthoMatrix[2][2] = -1.0f;//-2.0f / (far1 - near1);
		orthoMatrix[3][0] = -(right + left) / (right - left);
		orthoMatrix[3][1] = -(bottom + top) / (top - bottom);
		orthoMatrix[3][2] = -(far1+near1) / (far1 - near1);
		orthoMatrix[3][3] = 1.0f;
	}

	void cameraComponent::setPerspectiveProjection(float fovy, float aspect, float near1, float far1) {
		//assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		AssertCore(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f, "Assertcore Camera");
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix = glm::mat4{ 0.0f };
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		projectionMatrix[2][2] = far1 / (far1 - near1);
		projectionMatrix[2][3] = 1.f;
		projectionMatrix[3][2] = -(far1 * near1) / (far1 - near1);
	}

	void cameraComponent::setInverseView(const glm::mat4& v)
	{
		inverseViewMatrix = v;
	}

	std::array<float, 16> cameraComponent::GetProjectionByArray()
	{
		std::array<float, 16> temp;
		for (glm::mat4::length_type i = 0; i < 16; i++)
		{
			temp[i] = projectionMatrix[i / 4][i % 4];
		}
		return temp;
	}

	std::array<float, 16> cameraComponent::GetViewDirectionByArray()
	{
		std::array<float, 16> temp;
		for (glm::mat4::length_type i = 0; i < 16; i++)
		{
			temp[i] = viewMatrix[i / 4][i % 4];
		}
		return temp;
	}

	void cameraComponent::SetProjectionByArray(std::array<float, 16> in)
	{
		for (glm::mat4::length_type i = 0; i < 16; i++)
		{
			projectionMatrix[i / 4][i % 4] = in[i];
		}
	}

	void cameraComponent::SetViewDirectionByArray(std::array<float, 16> in)
	{
		for (glm::mat4::length_type i = 0; i < 16; i++)
		{
			viewMatrix[i/4][i%4] = in[i];
		}
		inverseViewMatrix = glm::inverse(viewMatrix);
	}

	void cameraComponent::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);

		inverseViewMatrix = glm::inverse(viewMatrix);
	}

	void cameraComponent::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
		setViewDirection(position, target - position, up);
	}

#if 0
	void cameraComponent::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
		//clamp
		rotation.x = glm::clamp(rotation.x, -180.0f, 180.0f);
		rotation.y = glm::clamp(rotation.y, -180.0f, 180.0f);
		rotation.z = glm::clamp(rotation.z, -180.0f, 180.0f);

		//convert
		rotation.x *= glm::pi<float>()/180.0f;
		rotation.y *= glm::pi<float>()/180.0f;
		rotation.z *= glm::pi<float>()/180.0f;

		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);

		inverseViewMatrix = glm::inverse(viewMatrix);
	}
#endif

	void cameraComponent::setViewYXZ(const Vec3f& _pos, const Quatf& _rot)
	{
		Mat3f rotationMtx{};
		rotationMtx.SetRotation(_rot);

		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = rotationMtx[0][0];
		viewMatrix[1][0] = rotationMtx[1][0];
		viewMatrix[2][0] = rotationMtx[2][0];
		viewMatrix[0][1] = rotationMtx[0][1];
		viewMatrix[1][1] = rotationMtx[1][1];
		viewMatrix[2][1] = rotationMtx[2][1];
		viewMatrix[0][2] = rotationMtx[0][2];
		viewMatrix[1][2] = rotationMtx[1][2];
		viewMatrix[2][2] = rotationMtx[2][2];
		viewMatrix[3][0] = -_pos.Dot(Vec3f(rotationMtx[0][0], rotationMtx[1][0], rotationMtx[2][0]));
		viewMatrix[3][1] = -_pos.Dot(Vec3f(rotationMtx[0][1], rotationMtx[1][1], rotationMtx[2][1]));
		viewMatrix[3][2] = -_pos.Dot(Vec3f(rotationMtx[0][2], rotationMtx[1][2], rotationMtx[2][2]));
			
		inverseViewMatrix = glm::inverse(viewMatrix);
	}

	const glm::mat4& cameraComponent::getProjection() const
	{
		return projectionMatrix;
	}
	const glm::mat4& cameraComponent::getView() const
	{
		return viewMatrix;
	}
	const glm::mat4& cameraComponent::getInverseView() const
	{
		return inverseViewMatrix;
	}
	const glm::vec3 cameraComponent::getPosition() const
	{
		return glm::vec3(inverseViewMatrix[3]);
	}
	const glm::mat4& cameraComponent::getOrtho() const
	{
		return orthoMatrix;
	}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<cameraComponent>(RegistrationNameCreator<cameraComponent>())
		.constructor()
		.constructor<const cameraComponent&>()
		.constructor<cameraComponent&&>()
		.property("projectionMatrix", &cameraComponent::GetProjectionByArray, &cameraComponent::SetProjectionByArray)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true),
			metadata(MetaDataKey::HiddenFromEditor, true)
		)
		.property("viewMatrix", &cameraComponent::GetViewDirectionByArray, &cameraComponent::SetViewDirectionByArray)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true),
			metadata(MetaDataKey::HiddenFromEditor, true)
		);
}