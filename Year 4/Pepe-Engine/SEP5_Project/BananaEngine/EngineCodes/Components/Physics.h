#pragma once
#ifndef	PHYSICS_H
#define PHYSICS_H

#include <Maths/Vector3D.h>
#include "ECSComponent.h"


using valueType = float;

namespace BE
{	
	inline BE::Vec3<float> JPHtoBE(JPH::Vec3 const& in) {
		BE::Vec3<float> out{ in.GetX(), in.GetY(), in.GetZ() };
		return out;
	}
	inline JPH::Vec3 JPHtoBE(BE::Vec3<float> const& in) {
		JPH::Vec3 out{ in.x, in.y, in.z };
		return out;
	}
	inline Quaternion<float> JPHtoBE(JPH::Quat const& in) {
		Quaternion<float> out{ in.GetW(), in.GetX(), in.GetY(), in.GetZ() };
		return out;
	}
	inline JPH::Quat JPHtoBE(Quaternion<float> const& in) {
		JPH::Quat out{ in.GetI(), in.GetJ(), in.GetK(), in.GetR() };
		return out;
	}
	/// Layer that objects can be in, determines which other objects it can collide with
	namespace PhysicLayers
	{
		static constexpr JPH::uint8 NON_MOVING = 0;
		static constexpr JPH::uint8 MOVING = 1;
		static constexpr JPH::uint8 NUM_LAYERS = 2;
	};

	enum class Shape
	{
		Sphere,
		Box,
		Capsule,
		Cylinder,
		StaticCompound,
		Mesh,
		ShapeNotImplemented
	};

	struct CompoundSubShape
	{
		Vec3<float>			inPosition;
		Quaternion<float>	inRotation;
		Shape				ShapeType;
		Vec3<float>			Scale;

		CompoundSubShape()
		{
			inPosition = { 0.f, 0.f, 0.f };
			inRotation = JPHtoBE(JPH::Quat::sIdentity());
			ShapeType = Shape::Box;
			Scale = { 1.f, 1.f, 1.f };
			shape_reference = nullptr;
		}

		inline Vec3<float> GetPosition() const { return inPosition; }
		inline Quaternion<float> GetRotation() const { return inRotation; }
		inline Shape GetShapeType() const { return ShapeType; }
		inline Vec3<float> GetScale() const { return Scale; }

		inline void SetPosition(Vec3<float> const& in)  { inPosition = in; }
		inline void SetRotation(Quaternion<float> const& in)  { inRotation = in; }
		inline void SetShapeType(Shape in)  { ShapeType = in; }
		inline void SetScale(Vec3<float> const& in)  { Scale = in; }

		uint32_t subKey;
		JPH::Ref<JPH::ScaledShape> shape_reference;
	};

	class Physics : public IECSComponent
	{
	private:
		JPH::Body* body = nullptr;
		std::vector<CompoundSubShape> subshapes;
		JPH::Ref<JPH::ScaledShape> shape_reference;
		JPH::Ref<JPH::StaticCompoundShapeSettings> cmpd_shape_settings;
		Vec3<float> offset;
		uint32_t subShapeKey;
		Shape shape;
		bool bindScale = true;
		bool CanPushCharacter = true;
		bool CanReceiveCharacterImpulse = true;
		bool isVirtualCharacter = false;
	
	public:
		Physics();

		Physics(Shape shapetype, valueType scale_x, valueType scale_y = 0.f, valueType scale_z = 0.f,
			std::vector<CompoundSubShape> const& ShapesToCompound = {});

		~Physics();

		Physics(Physics const& rhs) = default;
		Physics& operator=(Physics const& rhs) = default;
		Physics(Physics&& rhs);
		Physics& operator=(Physics&& rhs);
		/*
			For Physics system access
		*/
		JPH::BodyID GetBodyID() const;

		//void CreateBody(JPH::BodyInterface& system, Vec3<float> Position, Quaternion<float> rotation);

		void SetBodyPointer(JPH::Body* in);

		inline JPH::Shape const* const GetShapeReference()	const	{ return shape_reference; }
		inline JPH::Body* GetBody()									{	return body;													}
		inline JPH::Body const* GetBody() const						{	return body;													}
		inline bool GetCanPushCharacter() const { return CanPushCharacter; }
		inline bool GetCanReceiveCharacterImpulse() const { return CanReceiveCharacterImpulse; }
		inline void SetCanPushCharacter(bool in) { CanPushCharacter = in ; }
		inline void SetCanReceiveCharacterImpulse(bool in) { CanReceiveCharacterImpulse = in; }
		
		inline bool GetInvalidateCollide() const { return (bool)body->GetUserData(); }
		inline void SetInvalidateCollide(bool in) {  body->SetUserData(static_cast<JPH::uint64>(in)); }
		inline bool GetIsVirtualCharacter() const { return isVirtualCharacter; }
		inline void SetIsVirtualCharacter(bool in) { isVirtualCharacter = in; }
		/*
			Collider Body Getters
		*/
		inline bool IsStatic() const								{	return body->IsStatic();										}
		inline JPH::MotionProperties* GetMotionProperties()	const	{	if (IsStatic()) return nullptr; else return body->GetMotionProperties(); }
		inline bool IsCompoundShape() const							{	return body->GetShape()->GetSubType() == JPH::EShapeSubType::StaticCompound; }
		inline Vec3<float> GetAngularVelocity() const				{	return JPHtoBE(body->GetAngularVelocity());						}
		inline Vec3<float> GetLinearVelocity() const				{	return JPHtoBE(body->GetLinearVelocity());						}
		inline float GetFriction() const							{	return body->GetFriction();										}
		inline float GetRestitution() const							{	return body->GetRestitution() * 100.f;							}
		inline float GetInnerRadius() const							{	return body->GetShape()->GetInnerRadius();						}
		inline float GetVolume() const								{	return body->GetShape()->GetVolume();							}
		inline Shape GetShapeType() const							{	return shape;													}
		inline uint64_t GetShapeTypeUInt() const					{	return static_cast<uint64_t>(shape);							}
		inline JPH::EMotionType GetMotionType() const				{	return body->GetMotionType();									}
		inline uint64_t GetMotionTypeUInt() const					{	return static_cast<uint64_t>(GetMotionType());					}
		inline bool GetIsTrigger() const							{	return body->IsSensor();										}
		inline const JPH::Shape* GetShape() const					{	return body->GetShape();										}
		inline Vec3<float> const& GetOffset() const					{	return offset; 													}
		inline Vec3<float> GetShapeSize() const						{	return GetShapeSizeInternal(body->GetShape());					}
		inline bool GetIsScaleBinded() const						{	return bindScale;												}
		// Motion data
		inline JPH::EMotionQuality GetMotionQuality() const			{ if (IsStatic()) return JPH::EMotionQuality::Discrete; return GetMotionProperties()->GetMotionQuality(); }
		inline uint64_t GetMotionQualityUInt() const				{ return static_cast<uint64_t>(GetMotionQuality()); }
		inline float GetLinearDamping() const						{ if (IsStatic()) return 0.f; return GetMotionProperties()->GetLinearDamping();			}
		inline float GetMaxLinearVelocity() const					{ if (IsStatic()) return 0.f; return GetMotionProperties()->GetMaxLinearVelocity();		}
		inline float GetAngularDamping() const						{ if (IsStatic()) return 0.f; return GetMotionProperties()->GetAngularDamping();		}
		inline float GetMaxAngularVelocity() const					{ if (IsStatic()) return 0.f; return GetMotionProperties()->GetMaxAngularVelocity();	}
		inline float GetGravityFactor() const						{ if (IsStatic()) return 0.f; return GetMotionProperties()->GetGravityFactor();			}
		inline bool BodyIsValid() const								{ return !body->GetID().IsInvalid(); }

		// For accessing a compound shape
		std::vector<Vec3<float>> GetCompoundShapeSize() const;
		std::vector<CompoundSubShape>const& GetCompoundSubShapes() const;
		/*
			Collider Body Setters
		*/
		void SetAngularVelocity(Vec3<float> in);
		void SetLinearVelocity(Vec3<float>in);
		inline void SetFriction(float in)									{	if (in < 0.0f) in = 0.f;									body->SetFriction(in);								}
		inline void SetRestitution(float in)								{	if (in < 0.0f) in = 0.f; else if (in > 100.f) in = 100.f;	body->SetRestitution(in * 0.01f);					}
		inline void SetOffset(Vec3<float> const& in);
		inline void SetScaleBinded(bool in);
		void SetMotionType(JPH::EMotionType in);
		inline void SetMotionTypeUInt(uint64_t in)							{ SetMotionType(static_cast<JPH::EMotionType>(in));}
		void SetShape(Shape inShape);
		inline void SetShapeEnum(Shape in)									{ SetShape(in); }
		inline void SetShapeTypeUInt(uint64_t in)							{ SetShape(static_cast<Shape>(in)); }
		inline void SetShapeSize(Vec3<float> in);
		inline void SetIsTrigger(bool in);
		// Motion data
		inline void SetMotionQuality(JPH::EMotionQuality in) const			{ if (IsStatic()) return; GetMotionProperties()->SetMotionQuality(in); }
		inline void SetMotionQualityUInt(uint64_t in) const					{ SetMotionQuality(static_cast<JPH::EMotionQuality>(in)); }
		inline void SetLinearDamping(float in) const						{ if (IsStatic()) return; if (in < 0.0f) in = 0.f; GetMotionProperties()->SetLinearDamping(in);				}
		inline void SetAngularDamping(float in) const						{ if (IsStatic()) return; if (in < 0.0f) in = 0.f; GetMotionProperties()->SetAngularDamping(in);			}
		inline void SetMaxLinearVelocity(float in) const					{ if (IsStatic()) return; if (in < 0.0f) in = 0.f; GetMotionProperties()->SetMaxLinearVelocity(in);			}
		inline void SetMaxAngularVelocity(float in) const					{ if (IsStatic()) return; if (in < 0.0f) in = 0.f; GetMotionProperties()->SetMaxAngularVelocity(in);		}
		inline void SetGravityFactor(float in)								{ if (IsStatic()) return; GetMotionProperties()->SetGravityFactor(in);										}
		void SetShapeAndSize(Shape inShape, Vec3<float> const& size);

		// For editing a compound shape
		void AddSubShape(CompoundSubShape const& in);
		void RemoveSubShape(uint32_t key);
		void SetSubShapeRotation(uint32_t key, Quaternion<float> const& in);
		void SetSubShapeRotation(uint32_t key, Vec3<float> const& euler_in);
		void SetSubShapeLocalPosition(uint32_t key, Vec3<float> const& in);
		void SetSubShapeShapeType(uint32_t key, Shape inShape);
		void SetSubShapeScale(uint32_t key, Vec3<float> const& in);
		/*
			Adding Body Forces
		*/
		void AddForce(Vec3<float> const& in);
		void AddImpulse(Vec3<float> const& in);
		void AddTorque(Vec3<float> const& in);
		void AddAngularImpulse(Vec3<float> const& in);
		void MoveKinematic(Vec3<float> const& targetpos, Quaternion<float> const& targetRot, float inDeltaTime); 
		/*
			Utilities
		*/
		inline bool BoundingBoxContainsPoint(Vec3<float> const& in) const	 { return body->GetWorldSpaceBounds().Contains(JPHtoBE(in)); }
		//Serilisation
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND
	private:
		bool CompoundShapes(JPH::StaticCompoundShapeSettings* shapeRef, std::vector<CompoundSubShape> const& ShapesToCompound);
		void CompoundEditInternal(std::vector<CompoundSubShape> const& subshapes);
		Vec3<float> GetShapeSizeInternal(const JPH::Shape* shape) const;
		
		CompoundSubShape CompoundSingleSubShape(const JPH::Shape* inshape) const;
		void CompoundSubShapesUpdate();
		inline void UpdateScaledShape(JPH::Ref<JPH::Shape> const& _shape, Vec3f const& scale, bool updateMass);
		inline bool HasUniformSubShape();
	};

	inline Shape JPHtoBE(JPH::EShapeSubType in) {
		switch (in) {
		case JPH::EShapeSubType::Sphere: {
			return Shape::Sphere;
		}
		case JPH::EShapeSubType::Box: {
			return Shape::Box;
		}
		case JPH::EShapeSubType::Capsule: {
			return Shape::Capsule;
		}
		case JPH::EShapeSubType::Cylinder: {
			return Shape::Cylinder;
		}
		case JPH::EShapeSubType::StaticCompound: {
			return Shape::StaticCompound;
		}
		case JPH::EShapeSubType::Mesh: {
			return Shape::Mesh;
		}
		default: {
			return Shape::ShapeNotImplemented;
		}
		}
	}
}
#endif