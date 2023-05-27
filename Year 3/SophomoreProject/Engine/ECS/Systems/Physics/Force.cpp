// /******************************************************************************/
// /*!
// \file         Force.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Force class for force manager
// \lines        80 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Force.h"


using namespace Mathf;

Force::Force(ForceType type, vec2 force, float life_time, bool active) :
	_forceType{ type },
	_lifetime{ life_time },
	_isActive{ active },
	_dir{},
	_mag{},
	_torque{},
	_age{ 0.f },
	_directionalDrag{},
	_rotationalDrag{}
{
	if (type == ForceType::LINEAR_FORCE)
	{
		_dir = Normalize(force);
		_mag = Length(force);
	}
	else if (type == ForceType::ROTATIONAL_FORCE)
	{
		//_torque = force;
	}
	/*else if (type == ForceType::DRAG_FORCE)
	{
		_directionalDrag = force.x;
		_rotationalDrag = force.y;
	}*/
}

bool Force::ValidateAge(float dt)
{
	static_cast<void>(dt);//unreferenced variable 
	/*if (_forceType == ForceType::DRAG_FORCE)
		return true;
	else
	{*/

		if (_age < _lifetime)
			return true;
		else
			return false;
	//}
}

//GETTER
float Force::getLifetime() const
{
	return _lifetime;
}

bool Force::getIsActive() const
{
	return _isActive;
}

Force::ForceType Force::getType() const
{
	return _forceType;
}

vec2 Force::GetDirection() const
{
	return _dir;
}

float Force::GetMagnitude() const
{
	return _mag;
}

float Force::getAge() const
{
	return _age;
}

float Force::GetTorque() const
{
	return _torque;
}

//float Force::GetDirectionalDrag() const
//{
//	return _directionalDrag;
//}
//
//float Force::GetRotationalDrag() const
//{
//	return _rotationalDrag;
//}


//SETTER
void Force::SetLifetime(const float lifetime)
{
	_lifetime = lifetime;
}

void Force::SetIsActive(const bool& state)
{
	_isActive = state;
}

void Force::setLinearForce(const vec2& v)
{
	_dir = Normalize(v);
	_mag = Length(v);
}

void Force::SetAge(const float a)
{
	_age = a;
}

void Force::SetTorque(const float& t)
{
	_torque = t;
}

//void Force::SetDirectionalDrag(const float d)
//{
//	_directionalDrag = d;
//}
//
//void Force::SetRotationalDrag(const float d)
//{
//	_rotationalDrag = d;
//}
