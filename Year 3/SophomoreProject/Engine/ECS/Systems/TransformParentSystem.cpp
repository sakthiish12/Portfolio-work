// /******************************************************************************/
// /*!
// \file         TransformParentSystem.cpp
// \project name Engine
// \author(s)    Ryan Wang 
// \date         25th Nov, 2021
// \brief        This file contains the definition of the tranform parent system
//				 that calculates world matrix of entity based off its parents
// \lines        22 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "TransformParentSystem.h"

//extern
std::shared_ptr<TransformParentSystem> TransformParentSysPtr;

//Local helper function
//Normally for the initial call, appliedOn should be passed in as identity
void applyTransform(entityID e, glm::mat4& appliedOn)
{
	//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
	if (ecs.entityHas<transform2D>(e))
	{
		auto& transform = ecs.getComponent<transform2D>(e);

		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(transform.pos.x, transform.pos.y, 0.0f)); //Z 'positioning' only determines layering, so is set at 0.0
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(transform.scale.x, transform.scale.y, 1.0f)); //There is no Z scaling, so it's set as 1.0
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f)); //Rotation about Z axis, TURN INTO RADIANS

		appliedOn = translate * rotation * scale * appliedOn;

		//Do it again if the entity has a parent
		entityID parentID{ 0 };
		if (ecs.doesEntityHaveParent(e, parentID))
			applyTransform(parentID, appliedOn);
	}
}

void TransformParentSystem::update()
{
	for (auto const& e : getEntitiesRegistered())
	{
		auto& transform = ecs.getComponent<transform2D>(e);
		transform.worldMatrix = glm::mat4(1.0f); //Start from Identity Matrix
		applyTransform(e, transform.worldMatrix);
	}
}