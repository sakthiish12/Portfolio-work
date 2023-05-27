// /******************************************************************************/
// /*!
// \file         AbilitiesSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains class declaration of the abilities system
// \lines        151
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef ABILITIES_SYS
#define ABILITIES_SYS

#include "../../ECS/ECS.h"//ecs

class AbilitiesSystem : public systemBase
{
public:
	AbilitiesSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<AbilitiesSystem> AbilitiesSysPtr;

#endif // !ABILITIES_SYS
