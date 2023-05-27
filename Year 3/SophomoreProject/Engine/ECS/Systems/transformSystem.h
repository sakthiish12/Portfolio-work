// /******************************************************************************/
// /*!
// \file         transformSystem.h 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the transform system 
// \lines        32 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#ifndef TRANSFORM_SYSTEM
#define TRANSFORM_SYSTEM

#include <iostream>
#include <random>
#include <chrono>
#include "../../WinMgr/WinMgr.h"
#include "../../Dependencies/Vendor/imgui/imgui_impl_win32.h"
#include "../../Dependencies/Vendor/imgui/imgui_impl_opengl3.h"
#include "../../EditorStateManager/gameStateManager.h"
#include "../ECS.h"

/***************************************
* @Brief : this is the transform system for the ecs
****************************************/
class testTransformSystem : public systemBase
{
public:
	testTransformSystem(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt)
	{
		//static_cast<void>(dt);//unreferenced variable 

		//registering the entities 
		for (auto const& e : getEntitiesRegistered())
		{
			//std::cout << "Getting transform of #";
			auto& objTransform = ecs.getComponent<transform>(e);

			objTransform.rotationInDegrees += 20000.f * dt;//for testing

			glm::mat4 transXscale = objTransform.translate * objTransform.scale; //transformation * scale 
			glm::mat4 model = glm::rotate(transXscale, objTransform.rotationInDegrees, glm::vec3(0, 0, 1)); // where x, y, z is axis of rotation (e.g. 0 0 1)

			//model view project in reverse order due to opengl -> now calculating model matrix and mvp everyframe 
			glm::mat4 mvp = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * model;

			objTransform.mvp = mvp;
		}
	}
};
std::shared_ptr<testTransformSystem> testTransformSys;



#endif // !TRANSFORM_SYSTEM
