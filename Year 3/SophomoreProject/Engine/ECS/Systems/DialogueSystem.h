// /******************************************************************************/
// /*!
// \file         DialogueSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the class declaration of the dialogue system 
// \lines        12
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef DIALOGUE_SYS
#define DIALOGUE_SYS

#include "../../ECS/ECS.h"//ecs

class DialogueSystem : public systemBase
{
public:
	DialogueSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt);
};
extern std::shared_ptr<DialogueSystem> DialogueSysPtr;



#endif // !DIALOGUE_SYS
