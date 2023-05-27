// /******************************************************************************/
// /*!
// \file         FontRenderingSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the class declarations for the font rendering system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef FONTRENDER_SYS
#define FONTRENDER_SYS

#include "../system.h"
#include "../../ECS/ECS.h"//ecs 
#include "../../Graphics/Source/FontRendering.h"//rendering of the font 

//try to have entity 0 as a player controlled obj
class FontRenderingSys : public systemBase
{

public:

	FontRenderingSys(bool requireComponentMatch = false);
	~FontRenderingSys() {}

	//delete fncs
	FontRenderingSys(FontRenderingSys& rhs) = delete;
	FontRenderingSys(const FontRenderingSys& rhs) = delete;
	FontRenderingSys& operator=(const FontRenderingSys& rhs) = delete;
	FontRenderingSys& operator=(const FontRenderingSys& rhs) const = delete;


	//update 
	void update(float dt);

private:

};

extern std::shared_ptr<FontRenderingSys> FontRenderingSysPtr;

#endif //FONTRENDER_SYS