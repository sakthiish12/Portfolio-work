// /******************************************************************************/
// /*!
// \file         controllerSystem.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _PLAYERCONTROLLERSYSTEM_H_
#define _PLAYERCONTROLLERSYSTEM_H_

#include "../system.h"

//try to have entity 0 as a player controlled obj
class controllerSystem : public systemBase
{
public:
	/*Stats of suction*/
	bool suctionActive = false; // if the suction is activated 
	float arc_angle = 3.14f * 0.125f; //this var is one side only, but will have two side
	float suctionLimit = 200.f; //suction limit
	float suctionKillLimit = 80.f; //suction limit
	float suctionStrenth = 50.f; //strength of pull
	/*Stats of suction*/

	controllerSystem(bool requireComponentMatch = false);
	~controllerSystem() = default;
	controllerSystem(controllerSystem& rhs) = delete;
	controllerSystem(const controllerSystem& rhs) = delete;
	controllerSystem& operator=(const controllerSystem& rhs) = delete;
	controllerSystem& operator=(const controllerSystem& rhs) const = delete;

	void init();
	void inputUpdate(float dt);
	void update(float dt);
private:
	void applySuction(float dt);
};

extern std::shared_ptr<controllerSystem> controllerSystemPtr;

#define ControllerSystem controllerSystemPtr

#endif //_PLAYERCONTROLLERSYSTEM_H_
