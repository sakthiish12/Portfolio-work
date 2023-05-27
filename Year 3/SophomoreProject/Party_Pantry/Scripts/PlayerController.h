// /******************************************************************************/
// /*!
// \file         PlayerController.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         12th Apr, 2022
// \brief        This file contains the declarations for player's controller logic
// \lines        45
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2022 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

struct scriptBase;
struct PlayerController : scriptBase
{
	//Animation variables
	int tomatoesCaptured{ 0 };	
	enum class facing { up, right, left, down };

	/*enum class action { walking, idle, death };//the various action animations
	action currentAction{ action::walking };//tracks the current action of the player 
	action prevAction{ action::walking };//tracks the previous action 
	bool death = false;//Marker if the defeat animation began playing to prevent replaying it*/

	//GAMEPLAY VARIABLES

	//Keybinds:
	//LMB    = Vacuum in. Gain 1 Pressure
	//         (Upgrades from 1 to 4 Pressure)
	//V      = Melee attack. No Pressure change
	//RMB(E) = Airblast out. Use all Pressure
	//         (1 Pressure = Weak gust of wind directly ahead, short stun)
	//         (2 Pressure = Medium gust of wind directly ahead, med stun)
	//         (3 Pressure = Strong gust of wind ahead, long stun)
	//         (4 Pressure = A tornado that lasts for a few seconds)
	//LShift = Super Speed. Use all Pressure
	//         (1 Pressure = 0.5s, weak burst of speed)
	//         (2 Pressure = 1.2s, weak burst of speed)
	//         (3 Pressure = 2.0s, weak burst of speed)
	//         (4 Pressure = 0.2s, very strong burst of speed, pass through enemies)
	//Q      = Energy Shield. Use all Pressure
	// 	       (1 Pressure = 0.5s, deflects contacted projectiles & enemies)
	// 	       (2 Pressure = 1.2s, deflects contacted projectiles & enemies)
	// 	       (3 Pressure = 2.0s, deflects contacted projectiles & enemies)
	// 	       (4 Pressure = No shield, vaporises NEARBY projectiles & stuns NEARBY enemies)
	//RMB(E) = Flamethrower. Use all Pressure
	//         (1 Pressure = 3 shot of flames)
	//         (2 Pressure = 8 shots of flames)
	//         (3 Pressure = 15 shots of flames)
	//         (4 Pressure = A firebomb that bursts into 36 flames around)

	//The pressure limit of the vacuum gun
	//1 to 4
	int pressureLimit{ 4 };

	//The current pressure of the vacuum gun
	int pressureCurrent{ 0 };

	//List of skills
	//none - No skill presently being used. Movement as per normal
	//vacuum - Stopping and suctioning in, pulling in stunned enemies
	//melee - Stopping and doing a melee attack, damaging nearby enemies
	//airblast - Stopping and shooting a violent gust, stunning nearby enemies
	//speed - Doing a super speed charge, moving at doubled speed
	//        Collision will stop super speed, damaging enemy collided if any
	//shield - Stopping and shielding self with an energy shield
	// 	       Deflecting incoming projectiles and stunning enemies collided
	//flamethrower - Stopping and shooting a burst of 5(?) flames
	//               1 flame deals 1(?) damage
	enum class skills {none, vacuum, melee, airblast, speed, shield, flamethrower, death};
	//Current skill being used
	skills currentSkill{ skills::none };

	//Upgradables
	//NOTE
	//THESE VARIABLES HAVE BEEN PHASED OUT IN FAVOUR OF UPGRADEMANAGER

	//If super speed charging is unlocked or not
	//Requires 5(That little?) tomatoes to unlock
	//Kettle Knight will move a lot faster
	//Colliding into anything will make him stop dashing
	//Should the thing collided into be an enemy, damage AND stun them
	//bool superSpeedUnlocked{ true };

	//If energy shielding is unlocked or not
	//Requires 10 mushrooms (the boss is worth 5) and 3 tomatoes to unlock
	//Kettle Knight stops and deploys a shield,
	//Deflecting incoming projectiles and
	//enemies, temporarily stunning them in the process
	//bool shieldUnlocked{ true };

	//If flamethrower is unlocked or not
	//Requires 6 chillis and 6 pumpkins to unlock
	//Kettle Knight stops and deploys a burst of flames in a certain direction
	//One flame deals some damage, but not enough to take out an enemy
	//bool flamethrowerUnlocked{ true };

	//The maximum normal speed at which Kettle Knight moves by his legs
	//When super speed is active, the speed is double of this
	//When ULTRA speed is active, the speed is TEN fold of this
	//Upgrades:
	//Base:                 200
	//Level 1 (3 Tomatoes): 240
	//Level 2 (5 Tomatoes): 300
	//Level 3 (7 Tomatoes): 400
	float maximumNormalSpeed{ 200.0f };

	



	PlayerController(const char* scriptName = "PlayerController");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

private:

	facing currentFacingDirection{ facing::up };
	facing previousFacingDirection{ facing::up };

	//Melee weapon entity id
	entityID meleeWeaponID{ 0 };

	//Shield entity ID
	entityID shieldID{ 0 };
	
	//If the shield has been loaded into the game or not
	bool shieldLoaded{ false };

	//Toggle between air blast and flamethrower
	bool trueAirblastFalseFlamethrower{ true };
	//Time remaining for switching flash
	const float flashTime{ 0.2f };
	float flashTimeRemaining{ 0.0f };

	//Time remaining before next flamethrowerShot
	float timeTillNextFlamethrowerShot{ 0.0f };
	//How many shots of fire remain for flamethrower
	int flamethrowerShotsRemaining{ 0 };

	//How much more time the player has to stop to do these skills:
	//Vacuuming in (GAINS pressure)
	//Airblasting out (loses pressure)
	//Doing a melee attack (NO CHANGE to pressure)
	//Super Speed Dash charging (loses pressure)
	//Shielding (loses pressure)
	//Flamethrower (loses pressure)
	float skillTimeRemaining{ 0.0f };

	//Delay when using the free dash
	const float freeDashDelay{ 1.0f };
	float freeDashCooldownRemaining{ 0.0f };

	//Health at the last frame.
	//Whenever the player gets hurt, their skill gets stopped.
	int lastFrameHealth{ 0 };

	//Mouse Angle wrt player (in radians (0 to 2 pi, CW, 0 being to top)
	//To convert into "CCW, 0 being right" system, subtract this from pi/2
	float mouseAngle{ 0.0f };

	//Time after defeat to transition to lose screen
	float defeatDuration{ 3.0f };

	//For controlling footstep sound effect
	//Only plays when ENTERING into a particular frame in the walk cycle
	bool footstepSounded{ false };

	//helper functions

	//Damage and stun a certain enemy by a certain amount of health and time
	//Stun time addition formula is this:
	//stunTime * stunMultiplier / max(enemyHealth after damage, 1)
	void damageAndStun(entityID targetEntity, int damage, float stunTime);

	//Shoot a single airblast
	//To help shootAirblasts()
	void singleAirblast(float direction, float initialSpeed, int damage = 1, float stunMultiplier = 1.0f);

	//Shoot n airblasts in a certain spread (radians) and initial speed
	void shootAirblasts(size_t n, float spread, float initialSpeed, int damage = 1, float stunMultiplier = 1.0f);

	//Shoot a tornado
	void shootTornado();

	//Shoot a firebomb
	void shootFirebomb();

	//Do a melee attack
	void melee();

	//Shoot a fireball
	//Direction is altered by variance
	void shootFlame(float variance, float initialSpeed = 300.0f);
};

#endif;