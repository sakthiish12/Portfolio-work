#include "pch.h"
#include "lightComponent.h"
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	void lightComponent::SetColorByArray(std::array<float, 3> in)
	{
		for (unsigned i = 0; i < 3; i++)
		{
			color[i] = in[i];
		}
	}
	std::array<float, 3> lightComponent::GetColorByArray()
	{
		std::array<float, 3> temp;
		for (unsigned i = 0; i < 3; i++)
		{
			temp[i] = color[i];
		}
		return temp;
	}
	lightComponent::lightComponent()
	{
	}

	lightComponent::~lightComponent()
	{

	}

	lightComponent::lightComponent(const lightComponent& rhs ) noexcept
	{
		color = rhs.color;
		intensity = rhs.intensity;
		radius = rhs.radius;
	}

	lightComponent::lightComponent(lightComponent&& rhs) noexcept
	{
		color = rhs.color;
		intensity = rhs.intensity;
		radius = rhs.radius;
	}

	lightComponent& lightComponent::operator=(const lightComponent& rhs) noexcept
	{
		color = rhs.color;
		intensity = rhs.intensity;
		radius = rhs.radius;
		return *this;
	}

	lightComponent& lightComponent::operator=(lightComponent&& rhs) noexcept
	{
		color = rhs.color;
		intensity = rhs.intensity;
		radius = rhs.radius;
		return *this;
	}

	glm::vec3 const& lightComponent::getColor()
	{
		return color;
	}

	float lightComponent::getIntensity()
	{
		return intensity;
	}

	float lightComponent::getRadius()
	{
		return radius;
	}

	void lightComponent::setColor(glm::vec3 const& v)
	{
		color = v;
	}

	void lightComponent::setIntensity(float f)
	{
		intensity = f;
	}

	void lightComponent::setRadius(float f)
	{
		radius = f;
		if (radius < 0.0f)
		{
			radius = 0.0f;
		}
	}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<lightComponent>(RegistrationNameCreator<lightComponent>())
		.constructor()
		.constructor<const lightComponent&>()
		.constructor<lightComponent&&>()
		.property("Colour", &lightComponent::GetColorByArray, &lightComponent::SetColorByArray)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Intensity", &lightComponent::getIntensity, &lightComponent::setIntensity)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Radius", &lightComponent::getRadius, &lightComponent::setRadius)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);

}