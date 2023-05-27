#pragma once
#ifndef	TRANSFORM_H
#define TRANSFORM_H

#include <Maths/Matrix3D.h>
#include "ECSComponent.h"

namespace BE
{
	class Transform : public IECSComponent
	{
		//World position, rotation, scale of object
		//Local position, rotation, scale of object relative to the parent, or world if no parent
		
		Vec3f m_position;
		Vec3f m_localPosition;
		
		Vec3f m_scale;
		Vec3f m_localScale;
		
		Quatf m_rotation;
		Quatf m_localRotation;
		Vec3f m_eulerRotation;
		Vec3f m_localEulerRotation;

		//Model and inverse matrices

		//Children entities under this entity
		std::vector<EntityID> m_children;

		//The parent entity of this entity
		EntityID m_parent;

		//Matrices
		glm::mat4 m_modelMatrix;
		glm::mat3 m_normalMatrix;
		
		//Whether transform is updated
		bool updateModified = false;


			
	public:
		Transform();
		Transform(const Transform& rhs);
		Transform(Transform&& rhs) noexcept;
		Transform& operator=(const Transform& rhs);
		Transform& operator=(Transform&& rhs) noexcept;


		/*
			Position
		*/
		//Get world position of the entity
		inline const Vec3f& GetPosition() const						{ return m_position; }

		//Get local position relative to the parent
		inline const Vec3f& GetLocalPosition() const				{ return m_localPosition; }

		//Set world position for this entity
		void SetPosition(const Vec3f& _worldPos, bool _forceUpdate = false);

		//Set local position relative to the parent
		void SetLocalPosition(const Vec3f& _localPos, bool _forceUpdate = false);

		//Set world position for this entity(for physics call)
		inline void PhysicsUpdatePosition(const Vec3f& _worldPos)	{ SetPosition(_worldPos); updateModified = true; }

		/*
			Scale
		*/
		//Get world scale of the entity
		inline const Vec3f& GetScale() const						{ return m_scale; }

		//Get local scale relative to the parent
		inline const Vec3f& GetLocalScale() const					{ return m_localScale; }

		//Set world scale for this entity
		void SetScale(const Vec3f& _worldScale, bool _forceUpdate = false);

		//Set local scale relative to the parent
		void SetLocalScale(const Vec3f& _localScale, bool _forceUpdate = false);

		//Set world scale for this entity(for physics call)
		inline void PhysicsUpdateScale(const Vec3f& _worldScale)	{ SetScale(_worldScale); updateModified = false; }

		/*
			Rotation
		*/
		//Get world rotation of the entity
		inline const Quatf& GetRotation() const									{ return m_rotation; }

		//Get local rotation relative to the parent
		inline const Quatf& GetLocalRotation() const							{ return m_localRotation; }

		//Set world rotation for this entity
		void SetRotation(const Quatf& _worldRot, bool _forceUpdate = false);

		//Set local rotation relative to the parent
		void SetLocalRotation(const Quatf& _localRotation, bool _forceUpdate = false);


		//Get world rotation of the entity as euler angles (Degrees)
		inline const Vec3f& GetEulerRotation() const							{ return m_eulerRotation; }

		//Get local rotation relative to the parent as euler angles
		inline const Vec3f& GetLocalEulerRotation() const						{ return m_localEulerRotation; }
		
		//Set world rotation for this entity using euler angles (Degrees)
		void SetEulerRotation(const Vec3f& _worldRot, bool _forceUpdate = false);

		//Set local rotation relative to the parent using euler angles
		void SetLocalEulerRotation(const Vec3f& _localRotation, bool _forceUpdate = false);

		//Set world position for this entity(for physics call)
		inline void PhysicsUpdateRotation(const Quatf& _worldRot)				{ SetRotation(_worldRot); updateModified = false; }

		/*
			Parent-child hierachy
		*/
		//Attaches a child to this entity. 
		void AttachChild(EntityID _childID);

		//Detaches a child from this entity
		void DetachChild(EntityID _childID);

		//Set an entity to have a parent
		void AttachParent(EntityID _parentID);

		//Detaches the parent from this entity
		void DetachParent();

		//Get all the children attached to this entity
		inline const auto& GetChildren() const							{ return m_children; }

		//Number of children this entity has
		inline size_t GetChildrenCount() const							{ return m_children.size(); }

		//Check if this entity has a specified child
		inline bool HasChild(EntityID _childID) const					{ return std::find(m_children.begin(), m_children.end(), _childID) != m_children.end(); }

		//Finds the parent of this entity, NULL_ENTITY if no parent
		inline EntityID GetParent() const								{ return m_parent; }

		//Whether this entity has parents
		inline bool HasParent() const									{ return m_parent != NULL_ENTITY; }



		/*
			Getters
		*/
		inline glm::mat4 getModelMatrix() const							{ return m_modelMatrix; }
		inline glm::mat3 getNormalMatrix() const						{ return m_normalMatrix; }

		inline bool GetPhysicsUpdated() const							{ return updateModified; }
		inline void SetPhysicsUpdated(bool b)							{ updateModified = b; }


		/*
			Additional helper functions
		*/
		//Make the caller transform's forward direction point to _dest's position (instant)
		//void LookAt(const Transform& _dest, const Vec3f& _axis = Vec3f::Up());

		//Make the caller transform's forward direction point to _dest's position (instant) at certain angular speed
		//void StartFacing(const Transform& _dest, float angularSpeed, const Vec3f& _axis = Vec3f::Up());



	private:
		void UpdateMatrices();

		//For serialisation
		void SerialisePosition(const Vec3f& _worldPos);
		void SerialiseScale(const Vec3f& _worldScale);
		void SerialiseRotation(const Quatf& _worldRot);
		void SerialiseLocalPosition(const Vec3f& _localPos);
		void SerialiseLocalScale(const Vec3f& _localScale);
		void SerialiseLocalRotation(const Quatf& _localRot);

	public:
		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND


	};
}


#endif