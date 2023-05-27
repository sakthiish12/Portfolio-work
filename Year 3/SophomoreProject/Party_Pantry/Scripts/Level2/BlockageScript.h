// /******************************************************************************/
// /*!
// \file         BushScript.h
// \project name Party_Pantry
// \author(s)    Poh Chun Ren
// // \date        29th Jan, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This is the logic for boss mushroom
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _BLOCKAGE_SCRIPT_H_
#define _BLOCKAGE_SCRIPT_H_

struct scriptBase;
struct BlockageScript : scriptBase
{
	//Hit by bullet related
	float FreeFloatTime = 0.1f;
	float DestructionTime = 0.2f;
	float life = FreeFloatTime + DestructionTime;
	vec2 startingScale{};
	vec2 PushedVel{};
	int NumOfBulletHit = 0; // not used yet,

	//Suction related
	vec2 BushToPlayer{}; //provided runtime in PlayerController, for pushing bush towards player
	vec2 SuctionOriginalPosition{}; //meant to keep track of new position when not in BeingSucked mode
	bool SuctionOriginalPositionSaved = false; //not used yet, intended for updating SuctionOriginalPosition
	bool InSuctionRange = false; //true if it has been detected in PlayerController
	float InSuctionTimerStart = 1.5f; //
	float InSuctionTimerFloating = 1.f; //
	float InSuctionTimer = 1.5f; //identify phase when in suction
	float ShakeLimit = 50.f; // x pos struggle before getting sucked away
	float RotationTarget = 0.f; //calcuated runtime, target to face player from bush pos
	float RotationRequired = 0.f; //Rotation required to reach RotationTarget
	float RotationReqPerSec = 0.f; //Rotation required to reach RotationTarget
	bool RotationInitialised = false;
	float FirstDistanceUponSuction = 0.f; //Rotation required to reach RotationTarget

	//Recovery
	float RecoveryTime = 0.25f;
	float RecoveryTimer = RecoveryTime;
	bool RecoveryInitialised = false;
	float RecoveryRotPerSec = 0.f;
	vec2 RecoveryScalePerSec = { 0.f, 0.f };

	enum class BUSH_STATE
	{
		Untouched,

		//Shot by bullet
		FreeFloat,
		ScalingDown,

		//Sucked by player
		BeingSucked_Shake,
		BeingSucked_Floating,
		BeingSucked_Recovery,

		Dead
	} _state = BUSH_STATE::Untouched;

	//base data
	std::string name{ "BlockageScript" }; //name identifier



	//functions
	BlockageScript(const char* scriptName = "BlockageScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();

	//PlayerData
	int playerID = -1;
	bool playerComTypeexist = false;

};

#endif 