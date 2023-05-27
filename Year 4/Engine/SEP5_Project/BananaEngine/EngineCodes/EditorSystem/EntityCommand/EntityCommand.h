/******************************************************************************/
/*!
\file       UndoSystem.h
\par        Project: 
\author     Pham Thao Mien
\date       12 November 2021
\brief      

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef ENTITY_COMMAND_H
#define ENTITY_COMMAND_H

#include "../UndoSystem/UndoSystem.h"
#ifdef USE_EDITOR


namespace BE
{
	template <typename T> 
	class InspectorCommand : public ICommand
	{
		T _oldVal;

		T* _object;
	public:
		void setnewVal(T newval)
		{
			*_object = newval;
		}
		
		InspectorCommand() = default;
		InspectorCommand(T oldVal, T* object  ) : _oldVal{oldVal} , _object{object} {}

		void execute() override {}

		void undo() override
		{
			if (_object)
			{
				*_object = _oldVal;
			}		
		}
	};

	class TransformPosCommand : public ICommand
	{
		std::string _valType;
		BE::Vec3f _newVal;
		BE::Vec3f _oldVal;
		BE::EntityID _entityID;
	public:
		void setVal(std::string valType, BE::Vec3f oldVal, BE::Vec3f newval, BE::EntityID entityID)
		{
			_valType = valType;
			_oldVal = oldVal;
			_newVal = newval;
			_entityID = entityID;
		}

		void execute() override
		{
			BE::Transform& TransformRef = BE::ECS->GetComponent<BE::Transform>(_entityID);

			if (_valType == "Pos")
			{
				TransformRef.SetPosition(BE::Vec3f{ _newVal[0], _newVal[1], _newVal[2] });
			}
			if (_valType == "Scale")
			{
				TransformRef.SetScale(BE::Vec3f{ _newVal[0], _newVal[1], _newVal[2] });
			}
			if (_valType == "Rot")
			{
				TransformRef.SetEulerRotation(BE::Vec3f{ _newVal[0], _newVal[1], _newVal[2] });
			}
		}
		void undo() override
		{
			BE::Transform& TransformRef = BE::ECS->GetComponent<BE::Transform>(_entityID);
			if (_valType == "Pos")
			{
				TransformRef.SetPosition(BE::Vec3f{ _oldVal[0], _oldVal[1], _oldVal[2] });
			}
			if (_valType == "Scale")
			{
				TransformRef.SetScale(BE::Vec3f{ _oldVal[0], _oldVal[1], _oldVal[2] });
			}
			if (_valType == "Rot")
			{
				TransformRef.SetEulerRotation(BE::Vec3f{ _oldVal[0], _oldVal[1], _oldVal[2] });
			}
		}
	};

	class CreateEntityCommand : public ICommand
	{
		BE::EntityID newEntity;
	public:
		void execute() override;
		void undo() override;
	};

	class CloneEntityCommand : public ICommand
	{
		BE::EntityID CloneID;
		BE::EntityID newEntity;
	public:
		void setCloneID(BE::EntityID);
		void execute() override;
		void undo() override;
	};

	class DestroyEntityCommand : public ICommand
	{
		BE::EntityID DestroyID;
		BE::EntityID newEntity;
	public:
		void setDestroyID(BE::EntityID);
		void execute() override;
		void undo() override;
	};

	class AttachEntityCommand : public ICommand
	{
		BE::EntityID ChildEntity;
		BE::EntityID ParentEntity;
	public:
		void setChildID(BE::EntityID);
		void setParentID(BE::EntityID);
		void execute() override;
		void undo() override;
	};

	class DetachEntityCommand : public ICommand
	{
		BE::EntityID ChildEntity;
		BE::EntityID ParentEntity;
	public:
		void setChildID(BE::EntityID);
		void setParentID(BE::EntityID);
		void execute() override;
		void undo() override;
	};

	template<class T>
	class AddComponentCommand : public ICommand
	{
		BE::EntityID entityID;
		//T component;
	public:
		
		void setID(BE::EntityID id)
		{
			entityID = id;
		}

		void execute() override
		{
			BE::ECS->AddComponent(entityID, T{});
		}
	
		void undo() override
		{
			BE::ECS->RemoveComponent<T>(entityID);
		}
	};

	template<class T>
	class RemoveComponentCommand : public ICommand
	{
		BE::EntityID entityID;
		//T component;
	public:

		void setID(BE::EntityID id)
		{
			entityID = id;
		}

		void execute() override
		{
			BE::ECS->RemoveComponent<T>(entityID);
		}

		void undo() override
		{
			BE::ECS->AddComponent(entityID, T{});
		}
	};

	//class ChangeTransformComponent : public ICommand
	//{
	//public:
	//	void execute() override;
	//	void undo() override;
	//};
};
#endif
#endif