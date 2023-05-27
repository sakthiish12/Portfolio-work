// /******************************************************************************/
// /*!
// \file         ForceManager.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        ForceManager class for rigidbody
// \lines        47 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _FORCEMANAGER_H_
#define _FORCEMANAGER_H_

#include <memory>
#include <map>
#include <string>
#include <utility>
#include "../../Math/Vector/Vector2.h"
#include "Force.h"

/*
FLOW:
Loop:
	Steps:
		Apply Forces
		Update Pos and Vel
		Detect collision
		Solve constraints
		Display results;
*/

/*
Note: if you are looking to have physic such that the character stop moving say Left, set the vel in rigidbody to 0
thats what i can think of for now.


TODO: Make into singleton and manages forces for other entity insted of every entitiy holding this. 
*/

using Mathf::vec2;
//An entity that uses force will have a force manager to handle all its forces
class ForceManager
{
public:
	vec2 _gravity;
private:
	//the bool will determine if the force is added everyframe
	std::map< std::string, std::pair<Force, bool> > forceContainer;
	//Force _dragForce;
	vec2 _force;
	vec2 _acceleration;

	float _linear_drag = 0.005f;//set to 0.1 or 0.01 for it to stop faster
	float _min_vel = 0.9f;
	float _max_vel = 100.f;
	float _speed = 0.f;


	//For rotation Center of mass, assume at center of obj for now.
	float _torque = 0.f; //rot force
	float _angular_acceleration = 0.f; // rot acc
	float _angular_velocity = 0.f; // rot vel, aka omega?

	/*TODO: Rotation*/
	float _angular_linear_drag = 0.05f;
	float _angular_min_vel = 0.1f;
	float _angular_max_vel = 100.f;
	float _angular_speed = 0.f;
public:
	ForceManager();
	~ForceManager() = default;

	void ComputeForce(float& dt);
	void ApplyConstaintOnForce(bool freezeX, bool freezeY, bool freezeR);

	void IntegrateVelocity(vec2& vel, float& inverseMass, float& dt, bool Gravity, float& rot_vel, float& inverseInertiaMass);

	//Name has to be unique
	void AddLinearForce(const std::string name, const Force force, bool alwaysActive = false);
	void AddRotationalForce(const std::string& name, const Force& force, bool alwaysActive = false);

	void setAlwaysActive(const std::string name, bool newState);
	void DeleteForce(const std::string& name);

	void Update(float dt);
	void Clear();



	//To be modified in the future after changed to singleton
	// activates a force that is being managed
	void ActivateForce(const std::string& name);
	// deactivates a force that is being managed
	void DeactivateForce(const std::string& name);

	void ApplyForce();
	void RemoveForce();

	//getter
	float getLienarDrag();
	float getMinVel();
	float getMaxVel();

	//setter
	void setLinearDrag(float newVal);
	void setMinVel(float newVal);
	void setMaxVel(float newVal);

	vec2 getForce();
	void clearForce();
private:
	//ForceManager(ForceManager& lhs) = delete;
	//ForceManager(const ForceManager& lhs) = delete;
	//ForceManager& operator=(ForceManager& lhs) = delete;
	//ForceManager& operator=(const ForceManager& lhs) = delete;
	
};

#endif
