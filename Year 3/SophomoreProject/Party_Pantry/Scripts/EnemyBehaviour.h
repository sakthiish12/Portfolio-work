// /******************************************************************************/
// /*!
// \file         EnemyBehaviour.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         27th Feb, 2022
// \brief        This file contains the declarations for the base of any 
//               enemy's logic
// \lines        ??? Lines
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef ENEMYBEHAVIOUR_H
#define ENEMYBEHAVIOUR_H

struct scriptBase;
struct EnemyBehaviour : scriptBase
{
protected:
	Mathf::Vec2 initialScale; //The initial scaling of the enemy. For reference when they are not being sucked in
	enum class facing { up, right, left, down };
	facing currentFacingDirection{ facing::up };
	facing previousFacingDirection{ facing::up }; //Where the enemy faced in the previous frame. For helping with animations to prevent the animation from restarting

	//Helper variables and functions
	//How much time remaining before having to proceed to the next action
	float actionTimeRemaining{ 0.0f };

	//The direction that the enemy faces when acting.
	Mathf::vec2 currentVectorDirection;

	//Helper function
	void updateFacingDirection();
	void changeFacingAnimation
	(spriteCom& sprite,
		std::string const& up,
		std::string const& down,
		std::string const& left,
		std::string const& right);

public:
	//float shotDelay{ 2.0f };

	//enum class action { walking, stunned, death, suction};//the various action animations
	//action currentAction{ action::walking };//tracks the current action of the enemy 
	//bool changedDirection = true;

	bool beingSucked = false; //If the enemy is in the cone of vacuuming
	bool suctionSuccessful = false; //To broadcast to the player's vacuum that the enemy is being sucked in by the player and therefore will be pulled in.
	float stunMultiplier{ 0.0f }; //Multiplier for how long the enemy will be stunned
	float stunTimeRemaining{ 0.0f }; //For how long more in seconds will the enemy be stunned

	//The normal speed at which this enemy moves
	//May be affected by multipliers depending on action
	float normalMovementSpeed{ 300.0f };

	EnemyBehaviour(const char* scriptName = "EnemyBehaviour", float movementSpeed = 200.0f, float stunMultiplier = 1.0f);
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif