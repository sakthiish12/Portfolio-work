// /******************************************************************************/
// /*!
// \file         GUISystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the class declaration for the GUI system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef GUI_SYS
#define GUI_SYS

#include "../system.h"
#include "../../ECS/ECS.h"//ecs 

class GUISys : public systemBase
{

public:

	GUISys(bool requireComponentMatch = false);
	~GUISys() {}

	//delete fncs
	GUISys(GUISys& rhs) = delete;
	GUISys(const GUISys& rhs) = delete;
	GUISys& operator=(const GUISys& rhs) = delete;
	GUISys& operator=(const GUISys& rhs) const = delete;

	//update 
	void update(float dt);

private:

};

extern std::shared_ptr<GUISys> GUISysPtr;

#endif //GUI_SYS