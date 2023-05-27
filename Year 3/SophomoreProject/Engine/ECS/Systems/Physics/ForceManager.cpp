// /******************************************************************************/
// /*!
// \file         ForceManager.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        ForceManager class for rigidbody
// \lines        119 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "ForceManager.h"

//TODO: probably need a way to modify gravity as bool option or something
//constexpr vec2 GRAVITY = { 0.f, -9.8f };
//constexpr vec2 GRAVITY = { 0.f, 0.f };
constexpr vec2 GRAVITY = { 0.f, -100.f };

ForceManager::ForceManager() :
	forceContainer{},
	_gravity{ GRAVITY},
	_force{},
	_acceleration{}
{
}

void ForceManager::ComputeForce(float& dt)
{
	for (auto& it : forceContainer)
	{
		switch (it.second.first.getType())
		{
		case Force::ForceType::LINEAR_FORCE:
			(void)dt; // probably need to increase age of any force
			//NEED TO CHECK IF ACTIVE AND VALIDATED!!!!!!!!!!!!!!!!
			_force += it.second.first.GetMagnitude() * it.second.first.GetDirection();
			break;
		case Force::ForceType::ROTATIONAL_FORCE:
			_torque += it.second.first.GetTorque();
			break;
		//case Force::ForceType::DRAG_FORCE:
		default:
			break;
		}
	}
}

void ForceManager::ApplyConstaintOnForce(bool freezeX, bool freezeY, bool freezeR)
{

	_force = {
		freezeX ? 0.f : _force.x,
		freezeY ? 0.f : _force.y
	};

	_torque = {
		freezeR ? 0.f : _torque,
	};
}

void ForceManager::IntegrateVelocity(vec2& vel, float& inverseMass, float& dt, bool Gravity, float& rot_vel, float& inverseInertiaMass)
{

	//Simulate drag resistance, limits
	vel *= (1.0f - _linear_drag);

	//vel += (_force * inverseMass + _gravity * dt);
	//F = MA
	//A = F / M or F * InverseMass
	//The inverse mass is something like inertia that is the tendancy to resist change dependant on mass
	_acceleration = _force * inverseMass;

	//include gravity if true
	if (Gravity && inverseMass > 0.f)
	{
		_acceleration += _gravity;
	}

	//A = D vel / D Time
	//D vel = A * dt
	//Vel = old vel + D vel
	vel += _acceleration * dt;

	

	_speed = Length(vel);
	if (_speed < _min_vel)
	{
		vel = { 0.f, 0.f }; // clamp to 0 if near 0
	}
	else if (_speed > _max_vel)
	{
		vel = _max_vel * Normalize(vel);
	}


	//ROTATION HERE
	//Torque = Moment of inertia * angular acceleration
	//moment of inertia uses mass as a simplied one.
	_angular_acceleration = _torque * inverseInertiaMass;
	rot_vel += _angular_acceleration * dt;
	rot_vel *= (1.0f - _angular_linear_drag);

	_angular_speed = rot_vel;
	float absolute_angular_speed = _angular_speed;
	if (absolute_angular_speed < 0)
	{
		absolute_angular_speed *= -1;
	}
	if (absolute_angular_speed < _angular_min_vel)
	{
		rot_vel = 0.f; // clamp to 0 if near 0
	}
	else if (absolute_angular_speed > _angular_max_vel)
	{
		rot_vel = _angular_max_vel;
	}
}

void ForceManager::AddLinearForce(const std::string name, const Force force, bool alwaysActive)
{
	std::pair<std::map< std::string, std::pair<Force, bool> >::iterator, bool> ret_val = forceContainer.try_emplace(name, std::make_pair(force, alwaysActive));
	if (!ret_val.second) // no insertion happened
	{
		ret_val.first->second.first = force;
	}
}

void ForceManager::AddRotationalForce(const std::string& name, const Force& force, bool alwaysActive)
{
	std::pair<std::map< std::string, std::pair<Force, bool> >::iterator, bool> ret_val = forceContainer.try_emplace(name, std::make_pair(force, alwaysActive));
	if (!ret_val.second) // no insertion happened
	{
		ret_val.first->second.first = force;
	}
}

void ForceManager::setAlwaysActive(const std::string name, bool newState)
{
	auto it = forceContainer.find(name);
	if (it != forceContainer.end())
		it->second.second = newState;
}

void ForceManager::DeleteForce(const std::string& name)
{
	std::map< std::string, std::pair<Force, bool> >::iterator end = forceContainer.end();
	for (std::map< std::string, std::pair<Force, bool> >::iterator it = forceContainer.begin(); it != end; ++it)
	{
		if (it->first == name)
		{
			forceContainer.erase(it);
			return; 
		}
	}
}

void ForceManager::Update(float dt)
{

	std::map< std::string, std::pair<Force, bool> >::iterator end = forceContainer.end();
	// loop through all forces in the system
	for (std::map< std::string, std::pair<Force, bool> >::iterator it = forceContainer.begin(); it != end; ++it)
	{
		if (it->second.second == false)
		{
			//not always active
			// update age and active status of forces
			it->second.first.SetAge(it->second.first.getAge() + dt);
			it->second.first.ValidateAge();
		}
	}
}

void ForceManager::Clear()
{
	//temporary changing this to erase those that is not forever on
	//forceContainer.clear();
	auto end = forceContainer.end();
	for (auto it = forceContainer.begin(); it != forceContainer.end(); )
	{
		if (it->second.second == false)
		{
			//not forever
			it = forceContainer.erase(it);
		}
		else
			++it;
	}

}

void ForceManager::setLinearDrag(float newVal)
{
	_linear_drag = newVal;
}

void ForceManager::setMinVel(float newVal)
{
	_min_vel = newVal;
}

void ForceManager::setMaxVel(float newVal)
{
	_max_vel = newVal;
}

vec2 ForceManager::getForce()
{
	return _force;
}

void ForceManager::clearForce()
{
	_force = { 0.f, 0.f };
	Clear();
}

float ForceManager::getLienarDrag()
{
	return _linear_drag;
}

float ForceManager::getMinVel()
{
	return _min_vel;
}

float ForceManager::getMaxVel()
{
	return _max_vel;
}
