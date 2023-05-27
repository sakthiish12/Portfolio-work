/******************************************************************************/
/*!
\file       UndoSystem.cpp
\par        Project:
\author     Pham Thao Mien
\date       12 November 2021
\brief      This header file contains the handling of changing of scene state
			when the tool bar functions are being clicked.

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "EntityCommand.h"

#ifdef USE_EDITOR

#include "../Hierarchy_Window/Hierarchy_Window.h"

namespace BE
{
	void CreateEntityCommand::execute()
	{
		newEntity = BE::ECS->CreateEntity();
	}

	void CreateEntityCommand::undo()
	{
		Hierarchy_Window::isusing = false;
		BE::ECS->DestroyEntity(newEntity);
	}

	void CloneEntityCommand::setCloneID(BE::EntityID id)
	{
		CloneID = id;
	}

	void CloneEntityCommand::execute()
	{
		newEntity = BE::ECS->CloneEntity(CloneID);
	}

	void CloneEntityCommand::undo()
	{
		Hierarchy_Window::isusing = false;
		BE::ECS->DestroyEntity(newEntity);
	}

	void DestroyEntityCommand::execute()
	{
		BE::ECS->DestroyEntity(DestroyID);

		newEntity = DestroyID;
	}

	void DestroyEntityCommand::setDestroyID(BE::EntityID id)
	{
		DestroyID = id;
	}

	void DestroyEntityCommand::undo()
	{
		//Hierarchy_Window::isusing = false;
		BE::ECS->CreateEntity(newEntity);
	}

	void AttachEntityCommand::execute()
	{
		BE::Transform& temp = dynamic_cast<BE::Transform&>(BE::ECS->GetComponent("Transform", ParentEntity));
		temp.AttachChild(ChildEntity);
	}

	void AttachEntityCommand::setChildID(BE::EntityID id)
	{
		ChildEntity = id;
	}

	void AttachEntityCommand::setParentID(BE::EntityID id)
	{
		ParentEntity = id;
	}

	void AttachEntityCommand::undo()
	{
		BE::Transform& temp = dynamic_cast<BE::Transform&>(BE::ECS->GetComponent("Transform", ParentEntity));
		temp.DetachChild(ChildEntity);
	}

	void DetachEntityCommand::execute()
	{
		BE::Transform& temp = dynamic_cast<BE::Transform&>(BE::ECS->GetComponent("Transform", ParentEntity));
		temp.DetachChild(ChildEntity);
	}

	void DetachEntityCommand::setChildID(BE::EntityID id)
	{
		ChildEntity = id;
	}

	void DetachEntityCommand::setParentID(BE::EntityID id)
	{
		ParentEntity = id;
	}

	void DetachEntityCommand::undo()
	{
		BE::Transform& temp = dynamic_cast<BE::Transform&>(BE::ECS->GetComponent("Transform", ParentEntity));
		temp.AttachChild(ChildEntity);
	}
}
#endif