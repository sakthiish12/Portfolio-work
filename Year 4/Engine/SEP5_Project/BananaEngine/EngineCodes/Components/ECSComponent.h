#pragma once
#ifndef ECSCOMPONENT_H
#define ECSCOMPONENT_H
#include <Precompiled/pch.h>
#include <Systems/ECS/ECSConfig.h>


namespace BE
{
	//Base class that all components need to inherit
	class IECSComponent
	{
	public:
		IECSComponent() = default;
		virtual ~IECSComponent() = default;
		RTTR_ENABLE()
	};
}
#endif