// /******************************************************************************/
// /*!
// \file         Force.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Force class for force manager
// \lines        39 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _FORCE_H_
#define _FORCE_H_

#include "../../Math/Vector/Vector2.h"

using Mathf::vec2;

class Force
{
public:
	enum class ForceType
	{
		LINEAR_FORCE,
		ROTATIONAL_FORCE
		//DRAG_FORCE
	} _forceType;

private:
	//COMMON VARIABLE
	float _lifetime; //life expentancy
	bool _isActive;
	//LINEAR
	vec2 _dir; //direction
	float _mag; //magnitude
	float _age; //age: current age, for linear and rotational
	//ROTATIONAL
	float _torque; //WHATS THE TYPE? probably has angular vel etc related
	//DRAG
	float _directionalDrag;
	float _rotationalDrag;


public:
	/*
	type: linear, rotational, or drag
	force: linear force, rotional force, or (directionaldrag as x, and rotational drag as y)
	lifetime: how many second to live for
	*/
	Force(ForceType type, vec2 force, float life_time, bool active = true);
	~Force() = default;
	
	//ValidateAge() to check, add dt to decrease the age
	bool ValidateAge(float dt = 0.f);

	//getter
	float getLifetime() const;
	bool getIsActive() const;
	ForceType getType() const;
	//LINEAR
	vec2 GetDirection() const;
	float GetMagnitude() const;
	float getAge() const;
	//ROTATIONAL
	float GetTorque() const;
	//DRAG
	//float GetDirectionalDrag() const;
	//float GetRotationalDrag() const;

	//setter
	void SetLifetime(const float lifetime);
	void SetIsActive(const bool& state);
	//LINEAR
	void setLinearForce(const vec2& v);
	void SetAge(const float a);
	//ROTATIONAL
	void SetTorque(const float& t);
	//DRAG
	//void SetDirectionalDrag(const float d);
	//void SetRotationalDrag(const float d);
};

#endif