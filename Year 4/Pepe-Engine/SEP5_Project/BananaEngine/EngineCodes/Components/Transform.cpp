#include "pch.h"
#include "Transform.h"
#include <Systems/ECS/ECS.h>
#include <Serialisation/BananaSerialisation.h>
namespace BE
{
	Transform::Transform()
		: m_position{}, m_localPosition{},
		m_scale{ 1.0f, 1.0f, 1.0f }, m_localScale{ 1.0f, 1.0f, 1.0f },
		m_rotation{}, m_localRotation{},
		m_eulerRotation{}, m_localEulerRotation{},
		m_children{}, m_parent{ NULL_ENTITY },
		m_modelMatrix{ 1.0f }, m_normalMatrix{ 1.0f }, updateModified{ false }
	{
	}

	Transform::Transform(const Transform& rhs)
		: m_position{ rhs.m_position }, m_localPosition{ rhs.m_position },
		m_scale{ rhs.m_scale }, m_localScale{ rhs.m_scale },
		m_rotation{ rhs.m_rotation }, m_localRotation{ rhs.m_rotation },
		m_eulerRotation{ rhs.m_eulerRotation }, m_localEulerRotation{ rhs.m_eulerRotation },
		m_children{}, m_parent{ NULL_ENTITY },
		m_modelMatrix{ rhs.m_modelMatrix }, m_normalMatrix{ rhs.m_normalMatrix }, updateModified{ false }
	{
		//Do not copy the children and the parent - will result in duplicated UUID
		//Local Pos/Rot/Scale will just be Global Pos/Rot/Scale because no parent
		//Intentionally set local transform as global because there's no parent
	}

	Transform::Transform(Transform&& rhs) noexcept
		: m_position{ rhs.m_position }, m_localPosition{ rhs.m_localPosition },
		m_scale{ rhs.m_scale }, m_localScale{ rhs.m_localScale },
		m_rotation{ rhs.m_rotation }, m_localRotation{ rhs.m_localRotation },
		m_eulerRotation{ rhs.m_eulerRotation }, m_localEulerRotation{ rhs.m_localEulerRotation },
		m_children{ std::move(rhs.m_children) }, m_parent{ rhs.m_parent },
		m_modelMatrix{ rhs.m_modelMatrix }, m_normalMatrix{ rhs.m_normalMatrix }, updateModified{ false }
	{
		rhs.m_position = {};
		rhs.m_localPosition = {};
		rhs.m_scale = {};
		rhs.m_localScale = {};
		rhs.m_rotation = {};
		rhs.m_localRotation = {};
		rhs.m_eulerRotation = {};
		rhs.m_localEulerRotation = {};
		rhs.m_parent = NULL_ENTITY;
		rhs.m_modelMatrix = {};
		rhs.m_normalMatrix = {};
	}

	Transform& Transform::operator=(const Transform& rhs)
	{
		//Do not copy the children and the parent - will result in duplicated UUID
		//Local Pos/Rot/Scale will just be Global Pos/Rot/Scale because no parent
		m_position = rhs.m_position;
		m_localPosition = rhs.m_position;

		m_scale = rhs.m_scale;
		m_localScale = rhs.m_scale;

		m_rotation = rhs.m_rotation;
		m_localRotation = rhs.m_rotation;
		m_eulerRotation = rhs.m_eulerRotation;
		m_localEulerRotation = rhs.m_eulerRotation;

		m_parent = NULL_ENTITY;
		m_children.clear();

		m_modelMatrix = rhs.m_modelMatrix;
		m_normalMatrix = rhs.m_normalMatrix;
		updateModified = false;

		return *this;
	}

	Transform& Transform::operator=(Transform&& rhs) noexcept
	{
		m_position = rhs.m_position;
		m_localPosition = rhs.m_localPosition;

		m_scale = rhs.m_scale;
		m_localScale = rhs.m_localScale;

		m_rotation = rhs.m_rotation;
		m_localRotation = rhs.m_localRotation;
		m_eulerRotation = rhs.m_eulerRotation;
		m_localEulerRotation = rhs.m_localEulerRotation;

		m_parent = rhs.m_parent;
		m_children = std::move(rhs.m_children);

		m_modelMatrix = rhs.m_modelMatrix;
		m_normalMatrix = rhs.m_normalMatrix;
		updateModified = false;

		rhs.m_position = {};
		rhs.m_localPosition = {};
		rhs.m_scale = {};
		rhs.m_localScale = {};
		rhs.m_rotation = {};
		rhs.m_localRotation = {};
		rhs.m_eulerRotation = {};
		rhs.m_localEulerRotation = {};
		rhs.m_parent = NULL_ENTITY;
		rhs.m_modelMatrix = {};
		rhs.m_normalMatrix = {};

		return *this;
	}




	//Set world position for this entity
	void Transform::SetPosition(const Vec3f& _worldPos, bool _forceUpdate)
	{		
		if (!_forceUpdate && _worldPos == m_position)
			return;

		m_position = _worldPos;

		//If entity does not have parent, then set local position as world position as well
		if (!this->HasParent())
		{
			m_localPosition = _worldPos;
		}
		else
		{
			//Get the parent's transform component
			const Vec3f& parentScale = ECS->GetComponent<Transform>(m_parent).GetScale();
			const Vec3f& parentPos = ECS->GetComponent<Transform>(m_parent).GetPosition();
			const Quatf& parentRot = ECS->GetComponent<Transform>(m_parent).GetRotation();

			//Reverse scale -> rotation -> translation
			// (TRS)^-1 = S^-1 * R^-1 * T^-1

			//Directly find the inverse of each transform component
			Mat3f invScale = Mat3f{}.SetScale(1 / parentScale.x, 1 / parentScale.y, 1 / parentScale.z);
			Mat3f invRot = Mat3f{}.SetRotation(parentRot).SetTranspose();
			//For invRot, because columns in rotation matrix is orthonormal, we can just transpose to find inverse

			//Find the offset between this object and parent. Parent's position will be the local origin
			Vec3f offset = parentPos.VecTowards(this->m_position);

			//Result is the entity's local position using parent as the coordinate system (accounts for scale and rotation)
			m_localPosition = invScale * (invRot * offset);
		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}

		updateModified = true;
		UpdateMatrices();
	}

	//Set local position relative to the parent
	void Transform::SetLocalPosition(const Vec3f& _localPos, bool _forceUpdate)
	{
		if (!_forceUpdate && _localPos == m_localPosition)
			return;

		m_localPosition = _localPos;

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local position as world position
			m_position = _localPos;
		}
		else
		{
			//Get the parent's transform component
			const Vec3f& parentScale = ECS->GetComponent<Transform>(m_parent).GetScale();
			const Vec3f& parentPos = ECS->GetComponent<Transform>(m_parent).GetPosition();
			const Quatf& parentRot = ECS->GetComponent<Transform>(m_parent).GetRotation();

			//Get the transformation matrices
			Mat3f scale = Mat3f{}.SetScale(parentScale.x, parentScale.y, parentScale.z);
			Mat3f rot = Mat3f{}.SetRotation(parentRot);

			//Result is the conversion of the given local position relative to it's parent to global position
			m_position = parentPos + (rot * (scale * _localPos));
		}

		//Also update children transform
		for (EntityID& e : m_children)
		{
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}
		updateModified = true;
		UpdateMatrices();
	}

	//Set world scale for this entity
	void Transform::SetScale(const Vec3f& _worldScale, bool _forceUpdate)
	{
		if (!_forceUpdate && _worldScale == m_scale)
			return;

		m_scale = _worldScale;

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local scale as world scale
			m_localScale = _worldScale;
		}
		else
		{
			//Get the parent's global scale
			const Vec3f& parentScale = ECS->GetComponent<Transform>(m_parent).GetScale();

			//This entity's local scale relative to the parent's scale (Parent's scale is like Vec3(1,1,1))
			m_localScale = Vec3f(m_scale.x / parentScale.x,
				m_scale.y / parentScale.y,
				m_scale.z / parentScale.z);
		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			ECS->GetComponent<Transform>(e).SetLocalScale(ECS->GetComponent<Transform>(e).GetLocalScale(), true);
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}

		updateModified = true;
		UpdateMatrices();
	}

	//Set local scale relative to the parent
	void Transform::SetLocalScale(const Vec3f& _localScale, bool _forceUpdate)
	{
		if (!_forceUpdate && m_localScale == _localScale)
			return;

		m_localScale = _localScale;

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local scale as world scale
			m_scale = _localScale;
		}
		else
		{
			//Get the parent's global scale
			const Vec3f& parentScale = ECS->GetComponent<Transform>(m_parent).GetScale();

			//This entity's local scale relative to the parent's scale (Parent's scale is like Vec3(1,1,1))
			m_scale = Vec3f(_localScale.x * parentScale.x,
				_localScale.y * parentScale.y,
				_localScale.z * parentScale.z);
		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			ECS->GetComponent<Transform>(e).SetLocalScale(ECS->GetComponent<Transform>(e).GetLocalScale(), true);
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}
		updateModified = true;
		UpdateMatrices();
	}

	//Set world rotation for this entity
	void Transform::SetRotation(const Quatf& _worldRot, bool _forceUpdate)
	{
		if (!_forceUpdate && _worldRot == m_rotation)
			return;

		m_rotation = _worldRot;
		m_eulerRotation = _worldRot.GetEulerAngles();

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local rotation as world rotation
			m_localRotation = _worldRot;
			m_localEulerRotation = m_eulerRotation;

		}
		else
		{
			//This entity's local rotation relative to the parent's rotation (Parent's rotation is like no rotation)
			m_localRotation = ECS->GetComponent<Transform>(m_parent).GetRotation().Inverse() * m_rotation;

			m_localEulerRotation = m_eulerRotation - ECS->GetComponent<Transform>(m_parent).GetEulerRotation();
			m_localEulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.x);
			m_localEulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.y);
			m_localEulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.z);

		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			AssertCore(e != ECS->FindOwnerOfComponentData(*this), "Detected current entity being children to itself: %llu", e);
			ECS->GetComponent<Transform>(e).SetLocalRotation(ECS->GetComponent<Transform>(e).GetLocalRotation(), true);
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}

		updateModified = true;
		UpdateMatrices();
	}

	//Set local rotation relative to the parent
	void Transform::SetLocalRotation(const Quatf& _localRotation, bool _forceUpdate)
	{
		if (!_forceUpdate && _localRotation == m_localRotation)
			return;

		m_localRotation = _localRotation;
		m_localEulerRotation = m_localRotation.GetEulerAngles();

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local rotation as world rotation
			m_rotation = _localRotation;
			m_eulerRotation = m_localEulerRotation;

		}
		else
		{
			//This entity's local rotation relative to the parent's rotation (Parent's rotation is like no rotation)
			m_rotation = ECS->GetComponent<Transform>(m_parent).GetRotation() * m_localRotation;

			m_eulerRotation = m_localEulerRotation + ECS->GetComponent<Transform>(m_parent).GetEulerRotation();
			m_eulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.x);
			m_eulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.y);
			m_eulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.z);

		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			AssertCore(e != ECS->FindOwnerOfComponentData(*this), "Detected current entity being children to itself: %llu", e);
			ECS->GetComponent<Transform>(e).SetLocalRotation(ECS->GetComponent<Transform>(e).GetLocalRotation(), true);
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}
		updateModified = true;
		UpdateMatrices();
	}

	//Set rotation by euler angles for thie entity
	void Transform::SetEulerRotation(const Vec3f& _worldRot, bool _forceUpdate)
	{
		if (!_forceUpdate && _worldRot == m_eulerRotation)
			return;

		m_eulerRotation = _worldRot;

		m_eulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.x);
		m_eulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.y);
		m_eulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.z);

		m_rotation = Quatf(m_eulerRotation.x, m_eulerRotation.y, m_eulerRotation.z);

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local rotation as world rotation
			m_localRotation = m_rotation;
			m_localEulerRotation = m_eulerRotation;
		}
		else
		{
			//This entity's local rotation relative to the parent's rotation (Parent's rotation is like no rotation)
			m_localRotation = ECS->GetComponent<Transform>(m_parent).GetRotation().Inverse() * m_rotation;
			m_localEulerRotation = m_eulerRotation - ECS->GetComponent<Transform>(m_parent).GetEulerRotation();

			m_localEulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_localEulerRotation.x);
			m_localEulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_localEulerRotation.y);
			m_localEulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_localEulerRotation.z);
		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			AssertCore(e != ECS->FindOwnerOfComponentData(*this), "Detected current entity being children to itself: %llu", e);
			ECS->GetComponent<Transform>(e).SetLocalEulerRotation(ECS->GetComponent<Transform>(e).GetLocalEulerRotation(), true);
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}

		updateModified = true;
		UpdateMatrices();
	}

	//Set local rotation by euler angles relative to the parent
	void Transform::SetLocalEulerRotation(const Vec3f& _localRotation, bool _forceUpdate)
	{
		if (!_forceUpdate && _localRotation == m_localEulerRotation)
			return;

		m_localEulerRotation = _localRotation;

		m_localEulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_localEulerRotation.x);
		m_localEulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_localEulerRotation.y);
		m_localEulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_localEulerRotation.z);

		m_localRotation = Quatf(m_localEulerRotation.x, m_localEulerRotation.y, m_localEulerRotation.z);

		if (!this->HasParent())
		{
			//If this entity has no parent, then set local rotation as world rotation
			m_rotation = m_localRotation;
			m_eulerRotation = m_localEulerRotation;
		}
		else
		{
			//This entity's local rotation relative to the parent's rotation (Parent's rotation is like no rotation)
			m_rotation = ECS->GetComponent<Transform>(m_parent).GetRotation() * m_localRotation;
			m_eulerRotation = ECS->GetComponent<Transform>(m_parent).GetEulerRotation() + m_localEulerRotation;

			m_eulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.x);
			m_eulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.y);
			m_eulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.z);
		}

		//Also update children's transform
		for (EntityID& e : m_children)
		{
			AssertCore(e != ECS->FindOwnerOfComponentData(*this), "Detected current entity being children to itself: %llu", e);
			ECS->GetComponent<Transform>(e).SetLocalEulerRotation(ECS->GetComponent<Transform>(e).GetLocalEulerRotation(), true);
			ECS->GetComponent<Transform>(e).SetLocalPosition(ECS->GetComponent<Transform>(e).GetLocalPosition(), true);
		}
		updateModified = true;
		UpdateMatrices();
	}


	void Transform::AttachChild(EntityID _childID)
	{
		EntityID _thisEntityID = ECS->FindOwnerOfComponentData(*this);

		//Cannot attach child to itself
		if (_thisEntityID == _childID)
		{
			LogCoreError("Cannot attach child to itself");
			return;
		}

		//If down the hierachy, thisEntity is found in the _childID, then dont do anything

		EntityID childsParent = ECS->GetComponent<Transform>(_thisEntityID).GetParent();

		while (childsParent != NULL_ENTITY)
		{
			LogCore("Searching for childID, if too many calls then there is infinite loop");

			if (childsParent == _childID)
			{
				LogCoreError("Attempting to attach a child that is in the parent.");
				return;
			}

			childsParent = ECS->GetComponent<Transform>(childsParent).GetParent();
		}

		//If _thisEntityID already has _childID in the children list, then don't do anything
		if (HasChild(_childID) && ECS->GetComponent<Transform>(_childID).GetParent() == _thisEntityID)
			return;

		//If child contains a parent, then detach it first
		if (ECS->GetComponent<Transform>(_childID).HasParent())
		{
			//Get the old parent
			EntityID oldParentID = ECS->GetComponent<Transform>(_childID).GetParent();

			//Make the old parent detach the child
			ECS->GetComponent<Transform>(oldParentID).DetachChild(_childID);
		}

		//Attach this child to the new parent
		m_children.push_back(_childID);

		//Update the child's parent value
		ECS->GetComponent<Transform>(_childID).m_parent = _thisEntityID;

		//Now need to update the local transform of the child
		//The global transform shouldn't change, so we'll update local transform via the global functions
		ECS->GetComponent<Transform>(_childID).SetPosition(ECS->GetComponent<Transform>(_childID).GetPosition(), true);
		ECS->GetComponent<Transform>(_childID).SetScale(ECS->GetComponent<Transform>(_childID).GetScale(), true);
		ECS->GetComponent<Transform>(_childID).SetRotation(ECS->GetComponent<Transform>(_childID).GetRotation(), true);

		//Copy the entity's flag
		ECS->GetComponent<EntityInfo>(_childID).SetAllFlags(ECS->GetComponent<EntityInfo>(_thisEntityID).GetAllFlags());
	}

	void Transform::DetachChild(EntityID _childID)
	{
		auto it = std::find(m_children.begin(), m_children.end(), _childID);

		//If this entity contains this child
		if (it != m_children.end())
		{
			//Set this child's parent to be NULL
			ECS->GetComponent<Transform>(_childID).m_parent = NULL_ENTITY;

			//Remove this child from this entity's children list
			m_children.erase(it);

			//Now need to update the local transform of the child
			//Because child is parentless, the local transform is now global transform
			ECS->GetComponent<Transform>(_childID).SetPosition(ECS->GetComponent<Transform>(_childID).GetPosition(), true);
			ECS->GetComponent<Transform>(_childID).SetScale(ECS->GetComponent<Transform>(_childID).GetScale(), true);
			ECS->GetComponent<Transform>(_childID).SetRotation(ECS->GetComponent<Transform>(_childID).GetRotation(), true);

		}
		else
		{
			LogCoreError("Parent does not have child with ID %llu", *_childID);
		}
	}

	//Set an entity to have a parent
	void Transform::AttachParent(EntityID _parentID)
	{
		ECS->GetComponent<Transform>(_parentID).AttachChild(ECS->FindOwnerOfComponentData(*this));
	}

	//Detaches the parent from this entity
	void Transform::DetachParent()
	{
		//If no parent then just return
		if (m_parent == NULL_ENTITY)
		{
			LogCoreError("Entity %llu has no parent, cannot detach parent", *ECS->FindOwnerOfComponentData(*this));
			return;
		}

		EntityID thisComponentOwner = ECS->FindOwnerOfComponentData(*this);
		ECS->GetComponent<Transform>(m_parent).DetachChild(thisComponentOwner);
	}

	void Transform::UpdateMatrices()
	{
		Mat3f rotMtx{};
		rotMtx.SetRotation(m_rotation);

		m_modelMatrix = glm::mat4{ {m_scale.x * rotMtx[0][0], m_scale.x * rotMtx[1][0], m_scale.x * rotMtx[2][0], 0.0f},
								   {m_scale.y * rotMtx[0][1], m_scale.y * rotMtx[1][1], m_scale.y * rotMtx[2][1], 0.0f},
								   {m_scale.z * rotMtx[0][2], m_scale.z * rotMtx[1][2], m_scale.z * rotMtx[2][2], 0.0f},
								   {m_position.x,			  m_position.y,			    m_position.z,			  1.0f} };

		m_normalMatrix = glm::transpose(glm::inverse(m_modelMatrix));
	}

	void Transform::SerialisePosition(const Vec3f& _worldPos)
	{
		m_position = _worldPos;
		UpdateMatrices();
	}

	void Transform::SerialiseScale(const Vec3f& _worldScale)
	{
		m_scale = _worldScale;
		UpdateMatrices();
	}

	void Transform::SerialiseRotation(const Quatf& _worldRot)
	{
		m_rotation = _worldRot;
		m_eulerRotation = m_rotation.GetEulerAngles();
		UpdateMatrices();
	}


	void Transform::SerialiseLocalPosition(const Vec3f& _localPos)
	{
		m_localPosition = _localPos;
	}

	void Transform::SerialiseLocalScale(const Vec3f& _localScale)
	{
		m_localScale = _localScale;
	}

	void Transform::SerialiseLocalRotation(const Quatf& _localRot)
	{
		m_localRotation = _localRot;
		m_localEulerRotation = m_localRotation.GetEulerAngles();
	}

#if 0
	void Transform::LookAt(const Transform& _dest, const Vec3f& _axis)
	{
		Vec3f x, y, z;

		z = _dest.GetPositon().DirTowards(this->GetPositon()).Normalised();
		y = _axis.Normalised();
		x = y.Cross(z).Normalised();
		y = z.Cross(x).Normalised();

		//Construct the final matrix
		glm::mat4 transform{ x.x, y.x, z.x, 0.0f,
							 x.y, y.y, z.y, 0.0f,
							 x.z, y.z, z.z, 0.0f,
							 -x.Dot(this->GetPositon()), -y.Dot(this->GetPositon()), -z.Dot(this->GetPositon()), 1.0f };

		//Decompose to get the rotation quaterion
		glm::vec3 translation, scale, skew;
		glm::vec4 pers;

		//only using this
		glm::quat rotation;
		glm::decompose(transform, scale, rotation, translation, skew, pers);

		//set the final rotation
		SetRotation(Quatf{ rotation.w, rotation.x, rotation.y, rotation.z });
		//Compute the destination vector
		Vec3f forward = m_position.DirTowards(_dest.GetPositon());

		//Cross product between axis and destDir
		Vec3f left = forward.Cross(_axis);
		left.Normalise();

		Vec3f up = left.Cross(forward);
		up.Normalise();

		//Construct the final matrix
		glm::mat4 transform{ left.x, left.y, left.z, 0.0f,
							 up.x, up.y, up.z, 0.0f,
							 forward.x, forward.y, forward.z, 0.0f,
							 0.0f, 0.0f, 0.0f, 1.0f };

		//Decompose to get the rotation quaterion
		glm::vec3 translation, scale, skew;
		glm::vec4 pers;

		//only using this
		glm::quat rotation;
		glm::decompose(transform, scale, rotation, translation, skew, pers);

		//set the final rotation
		SetRotation(Quatf{ rotation.w, rotation.x, rotation.y, rotation.z });
	}
#endif

	//void Transform::StartFacing(const Transform& _dest, float angularSpeed, const Vec3f& _axis)
	//{
	//	//to do
	//}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<Transform>(RegistrationNameCreator<Transform>())
		.constructor()
		.constructor<const Transform&>()
		.property("Position", &Transform::GetPosition, &Transform::SerialisePosition)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true),
			metadata(MetaDataKey::IsLocal, false),
			metadata(MetaDataKey::HiddenFromEditor, true)
			)
		.property("Scale", &Transform::GetScale, &Transform::SerialiseScale)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true),
			metadata(MetaDataKey::IsLocal, false),
			metadata(MetaDataKey::HiddenFromEditor, true)
			)
		.property("Rotation", &Transform::GetRotation, &Transform::SerialiseRotation)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true),
			metadata(MetaDataKey::IsLocal, false),
			metadata(MetaDataKey::HiddenFromEditor, true)
			)
		.property("Local Position", &Transform::GetLocalPosition, &Transform::SerialiseLocalPosition)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, false),
			metadata(MetaDataKey::IsLocal, true),
			metadata(MetaDataKey::HiddenFromEditor, true)
			)
		.property("Local Scale", &Transform::GetLocalScale, &Transform::SerialiseLocalScale)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, false),
			metadata(MetaDataKey::IsLocal, true),
			metadata(MetaDataKey::HiddenFromEditor, true)
			)
		.property("Local Rotation", &Transform::GetLocalRotation, &Transform::SerialiseLocalRotation)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, false),
			metadata(MetaDataKey::IsLocal, true),
			metadata(MetaDataKey::HiddenFromEditor, true)
			)
		.property("Children", &Transform::m_children)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, false),
			metadata(MetaDataKey::HiddenFromEditor, true)
		)
		.property("Parent", &Transform::m_parent)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, false),
			metadata(MetaDataKey::HiddenFromEditor, true)
		);


}
