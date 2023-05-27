#include "pch.h"
#include "Physics.h"
#include <Systems/PhysicsSystem/sPhysicsSystem.h>
#include <AssetManager/AssetManager.h>
#include <Serialisation/BananaSerialisation.h>
namespace BE
{
	Physics::Physics()
	{
		shape = Shape::Box;
		shape_reference = new JPH::ScaledShape(PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Box)], JPH::Vec3(1.f, 1.f, 1.f));
	}

	Physics::Physics(Shape shapetype, valueType scale_x, valueType scale_y, valueType scale_z, 
					std::vector<CompoundSubShape> const& ShapesToCompound) {
			if (ShapesToCompound.empty() && shapetype == Shape::StaticCompound) {
				shapetype = Shape::Box;
			}
			// Create the shape
			JPH::Ref<JPH::Shape> new_shape;
			Vec3f size = Vec3f(scale_x, scale_y, scale_z);
			switch (shapetype) {
			case Shape::Sphere: {
				size.y = size.x; size.z = size.x;
				new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Sphere)];
				break;
			}
			case Shape::Box: {
				new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Box)];
				break;
			}
			case Shape::Capsule: {
				size.y = size.x; size.z = size.x;
				new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Capsule)];
				break;
			}
			case Shape::Cylinder: {
				size.y = size.x; size.z = size.x;
				new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Cylinder)];
				break;
			}
			case Shape::StaticCompound: {
				if (cmpd_shape_settings != nullptr) cmpd_shape_settings->Release();
				cmpd_shape_settings = new JPH::StaticCompoundShapeSettings();
				bool Has_Uniform_Shape = CompoundShapes(cmpd_shape_settings, ShapesToCompound);
				if (Has_Uniform_Shape) {
					size.y = size.x; size.z = size.x;
				}
				new_shape = cmpd_shape_settings->Create().Get();
				break;
			}			
			default: {
				new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Box)];
				break;
			}
			}
			shape = shapetype;
			//if (shape_reference != nullptr) shape_reference->Release();
			shape_reference = new JPH::ScaledShape(new_shape, JPHtoBE(size));
	}

	Physics::~Physics()
	{
		//body_settings.SetShapeSettings(nullptr);
		//body_settings.SetShape(nullptr);
		shape_reference = nullptr;
		cmpd_shape_settings = nullptr;
		for (size_t i = 0; i < subshapes.size(); i++)
		{
			subshapes[i].shape_reference = nullptr;
		}
	}

	Physics::Physics(Physics&& rhs)
	{
		body = std::move(rhs.body);
		subshapes = std::move(rhs.subshapes);
		shape_reference = std::move(rhs.shape_reference);
		cmpd_shape_settings = std::move(rhs.cmpd_shape_settings);
		offset = std::move(rhs.offset);
		subShapeKey = std::move(rhs.subShapeKey);
		shape = std::move(rhs.shape);
		bindScale = std::move(rhs.bindScale);
		CanPushCharacter = std::move(rhs.CanPushCharacter);
		CanReceiveCharacterImpulse = std::move(rhs.CanReceiveCharacterImpulse);
		rhs.body = nullptr;
	}

	Physics& Physics::operator=(Physics&& rhs)
	{
		body = std::move(rhs.body);
		subshapes = std::move(rhs.subshapes);
		shape_reference = std::move(rhs.shape_reference);
		cmpd_shape_settings = std::move(rhs.cmpd_shape_settings);
		offset = std::move(rhs.offset);
		subShapeKey = std::move(rhs.subShapeKey);
		shape = std::move(rhs.shape);
		bindScale = std::move(rhs.bindScale);
		CanPushCharacter = std::move(rhs.CanPushCharacter);
		CanReceiveCharacterImpulse = std::move(rhs.CanReceiveCharacterImpulse);
		rhs.body = nullptr;
		return *this;
	}

	JPH::BodyID Physics::GetBodyID() const {
		if (body)
			return body->GetID();
		else {
			return JPH::BodyID();
		}
	}

	//void Physics::CreateBody(JPH::BodyInterface& system, Vec3<float> Position, Quaternion<float> rotation) {
	//	body = system.CreateBody(body_settings);
	//	body->SetPositionAndRotationInternal(JPH::Vec3(Position.x, Position.y, Position.z), JPH::Quat(rotation.GetI(), rotation.GetJ(), rotation.GetK(), rotation.GetR()));
	//}


	void Physics::SetBodyPointer(JPH::Body* in)
	{
		if (body == nullptr) {
			body = in;
		}
		else {
			AssertCore(false, "Overwriting an existing physics body pointer");
		}
	}

	inline void Physics::SetAngularVelocity(Vec3<float> in)
	{ 
		if (IsStatic()) return; PhySys->GetBodyInterface().SetAngularVelocity(body->GetID(), JPHtoBE(in));
	}

	inline void Physics::SetLinearVelocity(Vec3<float>in)
	{
		if (IsStatic()) return; PhySys->GetBodyInterface().SetLinearVelocity(body->GetID(), JPHtoBE(in));
	}

	inline void Physics::SetOffset(Vec3<float> const& in)
	{
		if (in == offset) return;
		offset = in;
		BEID id = BE::ECS->FindOwnerOfComponentData(*this);
		PhySys->GetBodyInterface().SetPosition(body->GetID(), JPHtoBE(ECS->GetComponent<Transform>(id).GetPosition() + in), JPH::EActivation::Activate);
	}

	inline void Physics::SetScaleBinded(bool in)
	{
		bindScale = in;
		if (in == true) {
			BEID id = BE::ECS->FindOwnerOfComponentData(*this);
			SetShapeSize(ECS->GetComponent<Transform>(id).GetScale() /** 0.5f*/);
		}
	}

	inline void Physics::SetShapeSize(Vec3<float> in)
	{
		if (in == GetShapeSize()) return;
		Vec3f size = in;
		if (size.x == 0.f) size.x = 0.01f;
		if (size.y == 0.f) size.y = 0.01f;
		if (size.z == 0.f) size.z = 0.01f;
		if (shape == Shape::StaticCompound) {
			if (HasUniformSubShape()) {
				size.y = size.x; size.z = size.x;
			}
			JPH::Shape::ShapeResult CachedResult;
			JPH::Ref<JPH::Shape> temp_shape = new JPH::StaticCompoundShape(*cmpd_shape_settings, PhySys->GetAllocator(), CachedResult);
			UpdateScaledShape(temp_shape, size, false);
		}
		else SetShapeAndSize(shape, size);
	}

	inline void Physics::SetIsTrigger(bool in)
	{
		body->SetIsSensor(in);
	}

	void Physics::SetMotionType(JPH::EMotionType in)
	{
		if (in == JPH::EMotionType::Static )
			PhySys->GetBodyInterface().DeactivateBody(body->GetID());
			
		//body->SetMotionType(in);
		JPH::ObjectLayer layer = in == JPH::EMotionType::Static ? PhysicLayers::NON_MOVING : PhysicLayers::MOVING;
		PhySys->GetBodyInterface().SetMotionType(body->GetID(), in, JPH::EActivation::DontActivate);
		PhySys->GetBodyInterface().SetObjectLayer(body->GetID(), layer);
		PhySys->GetBodyInterface().ActivateBody(body->GetID());	
		//if (in == JPH::EMotionType::Static)
	}

	void Physics::SetShape(Shape inShape)
	{
		if (inShape == shape) return;
		SetShapeAndSize(inShape, GetShapeSize());
	}

	void Physics::AddSubShape(CompoundSubShape const& in)
	{
		if (shape != Shape::StaticCompound) return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		subshapes_new.emplace_back(in);

		CompoundEditInternal(subshapes_new);
	}

	void Physics::RemoveSubShape(uint32_t key)
	{
		if (!IsCompoundShape()) return;

		auto itr = std::find_if(subshapes.begin(), subshapes.end(), [=](CompoundSubShape const& a) {return a.subKey == key; });
		if (itr != subshapes.end()) {
			subshapes.erase(itr);
		}
		else return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		CompoundEditInternal(subshapes_new);
	}
	void Physics::SetSubShapeRotation(uint32_t key, Quaternion<float> const& in)
	{
		if (shape != Shape::StaticCompound) return;

		auto itr = std::find_if(subshapes.begin(), subshapes.end(), [=](CompoundSubShape const& a) {return a.subKey == key; });
		if (itr != subshapes.end()) {
			itr->inRotation = in;
		}
		else return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		CompoundEditInternal(subshapes_new);
	}

	void Physics::SetSubShapeRotation(uint32_t key, Vec3<float> const& euler_in)
	{
		if (shape != Shape::StaticCompound) return;

		Vec3<float> m_eulerRotation = euler_in;

		m_eulerRotation.x = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.x);
		m_eulerRotation.y = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.y);
		m_eulerRotation.z = BEMath::Wrap(-180.0f, 180.0f, m_eulerRotation.z);

		Quaternion<float> m_rotation = Quaternion<float>(m_eulerRotation.x, m_eulerRotation.y, m_eulerRotation.z);

		auto itr = std::find_if(subshapes.begin(), subshapes.end(), [=](CompoundSubShape const& a) {return a.subKey == key; });
		if (itr != subshapes.end()) {
			itr->inRotation = m_rotation;
		}
		else return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		CompoundEditInternal(subshapes_new);
	}


	void Physics::SetSubShapeLocalPosition(uint32_t key, Vec3<float> const& in)
	{
		if (shape != Shape::StaticCompound) return;

		auto itr = std::find_if(subshapes.begin(), subshapes.end(), [=](CompoundSubShape const& a) {return a.subKey == key; });
		if (itr != subshapes.end()) {
			itr->inPosition = in;
		}
		else return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		CompoundEditInternal(subshapes_new);
	}
	void Physics::SetSubShapeShapeType(uint32_t key, Shape inShape)
	{
		if (shape != Shape::StaticCompound) return;

		auto itr = std::find_if(subshapes.begin(), subshapes.end(), [=](CompoundSubShape const& a) {return a.subKey == key; });
		if (itr != subshapes.end()) {
			if (itr->ShapeType == inShape) return;
			itr->ShapeType = inShape;
		}
		else return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		CompoundEditInternal(subshapes_new);
	}
	void Physics::SetSubShapeScale(uint32_t key, Vec3<float> const& in)
	{
		if (shape != Shape::StaticCompound) return;

		auto itr = std::find_if(subshapes.begin(), subshapes.end(), [=](CompoundSubShape const& a) {return a.subKey == key; });
		if (itr != subshapes.end()) {
			itr->Scale = in;
		}
		else return;
		std::vector<CompoundSubShape> subshapes_new = subshapes;
		CompoundEditInternal(subshapes_new);
	}

	void Physics::SetShapeAndSize(Shape inShape, Vec3<float> const& _size)
	{
		bool updateMass = true;
		JPH::Ref<JPH::Shape> new_shape;
		Vec3f size = _size;
		switch (inShape) {
		case Shape::Sphere: {
			size.y = size.x; size.z = size.x;
			new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Sphere)];
			break;
		}
		case Shape::Box: {
			new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Box)];
			break;
		}
		case Shape::Capsule: {
			size.y = size.x; size.z = size.x;
			new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Capsule)];
			break;
		}
		case Shape::Cylinder: {
			size.y = size.x; size.z = size.x;
			new_shape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Cylinder)];
			break;
		}
		case Shape::StaticCompound: {
			if (cmpd_shape_settings != nullptr) cmpd_shape_settings->Release();
			cmpd_shape_settings = new JPH::StaticCompoundShapeSettings();
			CompoundSubShape toAdd;
			toAdd.inRotation = JPHtoBE(body->GetRotation());
			toAdd.Scale = { 1.0f, 1.0f, 1.0f };
			toAdd.ShapeType = shape;
			toAdd.inPosition = { 0.f, 0.f, 0.f };
			bool Has_Uniform_Shape = CompoundShapes(cmpd_shape_settings, {toAdd });
			new_shape = cmpd_shape_settings->Create().Get();

			if (Has_Uniform_Shape) {
				size.y = size.x; size.z = size.x;
			}
			updateMass = false;
			break;
		}
		case Shape::Mesh:{
			const BEID& id = BE::ECS->FindOwnerOfComponentData(*this);
			if (BE::ECS->HasComponent<Renderer>(id) &&
				BE::ECS->GetComponent<Renderer>(id).getModelID() != BE::NULL_ASSET)
			{
				AssetID aID = BE::ECS->GetComponent<Renderer>(id).getModelID();
				auto itr = PhySys->GetMeshShapeSettings().find(aID);

				if (itr == PhySys->GetMeshShapeSettings().end())
				{
					BeModel& model = BE::AssetManager::GetInstance()->GetAssetData<BeModel>(aID);
					JPH::Ref<JPH::MeshShapeSettings> meshSettings = new JPH::MeshShapeSettings();
					for (const auto& mesh : model.getMeshes())
					{
						meshSettings->mTriangleVertices.reserve(meshSettings->mTriangleVertices.size() + mesh.vertices.size());
						for (const auto& vtx : mesh.vertices)
						{
							JPH::Float3 temp;
							temp.x = vtx.position.x;
							temp.y = vtx.position.y;
							temp.z = vtx.position.z;
							meshSettings->mTriangleVertices.emplace_back(std::move(temp));
						}
						meshSettings->mIndexedTriangles.reserve(meshSettings->mIndexedTriangles.size() + mesh.indices.size() / 3);
						for (size_t i = 0; i < mesh.indices.size() / 3; i += 3)
						{
							JPH::IndexedTriangle temp;
							temp.mIdx[0] = mesh.indices[i];
							temp.mIdx[1] = mesh.indices[i + 1];
							temp.mIdx[2] = mesh.indices[i + 2];
							meshSettings->mIndexedTriangles.emplace_back(std::move(temp));
						}
					}
					new_shape = meshSettings->Create().Get();
					PhySys->AddMeshShape(std::make_pair(aID, new_shape));
				}
				else {
					new_shape = itr->second;
				}
				updateMass = false;

			}
			else
			{
				LogError("Entity with Id %u tried to set mesh collider with no mesh", *id);
				return;
			}
		}
		}
		shape = inShape;

		UpdateScaledShape(new_shape, size, updateMass);
	}

	Vec3<float> Physics::GetShapeSizeInternal(const JPH::Shape* inshape) const
	{
		auto scaled_shape = static_cast<JPH::ScaledShape const*>(inshape);
		if (scaled_shape)
			return JPHtoBE(scaled_shape->GetScale());
		AssertCore(false, "Invalid shape pointer used to get shape size");
		return Vec3<float>(0.f, 0.f, 0.f);
	}

	std::vector<Vec3<float>> Physics::GetCompoundShapeSize() const
	{
		std::vector<Vec3<float>> toreturn;

		if (body->GetShape()->GetSubType() != JPH::EShapeSubType::StaticCompound) return toreturn;

		auto compound = static_cast<JPH::StaticCompoundShape const*>(body->GetShape());
		if (compound) {
			for (auto& sshape : compound->GetSubShapes()) {
				toreturn.emplace_back(GetShapeSizeInternal(sshape.mShape));
			}
		}
		return toreturn;
	}

	std::vector<CompoundSubShape> const& Physics::GetCompoundSubShapes() const
	{
		return subshapes;
	}

	CompoundSubShape Physics::CompoundSingleSubShape(const JPH::Shape* inshape) const
	{
		auto decorated = static_cast<JPH::RotatedTranslatedShape const*>(inshape);
		CompoundSubShape toRet;
		if (decorated)
		{
			toRet.Scale = GetShapeSizeInternal(decorated->GetInnerShape());
			toRet.inRotation = JPHtoBE(decorated->GetRotation());
			toRet.inPosition = JPHtoBE(decorated->GetPosition());
			toRet.ShapeType = JPHtoBE(decorated->GetInnerShape()->GetSubType());
			toRet.subKey = 1;
		}
		return toRet;
	}
	bool Physics::CompoundShapes(JPH::StaticCompoundShapeSettings* shapeRef, std::vector<CompoundSubShape> const& ShapesToCompound)
	{
		bool HasUniformShape = false;
		subShapeKey = 1;
		for (size_t i = 0; i < subshapes.size(); i++)
		{
			if (subshapes[i].shape_reference != nullptr)
				subshapes[i].shape_reference->Release();
		}
		subshapes.clear();
		subshapes.reserve(ShapesToCompound.size());
		for (size_t i = 0; i < ShapesToCompound.size(); i++)
		{
			auto const& sshape = ShapesToCompound[i];
			JPH::Ref<JPH::Shape> newshape;
			Vec3<float> size = sshape.Scale;
			if (size.x == 0.f) size.x = 0.01f; 
			if (size.y == 0.f) size.y = 0.01f;
			if (size.z == 0.f) size.z = 0.01f;
			switch (sshape.ShapeType) {
			case Shape::Sphere: {
				size.y = size.x; size.z = size.x;
				newshape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Sphere)];
				HasUniformShape = true;
				break;
			}
			case Shape::Box: {
				newshape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Box)];
				break;
			}
			case Shape::Capsule: {
				size.y = size.x; size.z = size.x;
				newshape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Capsule)];
				HasUniformShape = true;
				break;
			}
			case Shape::Cylinder: {
				size.y = size.x; size.z = size.x;
				newshape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Cylinder)];
				break;
			}
			default: {
				newshape = PhySys->GetDefaultShapes()[static_cast<size_t>(Shape::Box)];
				break;
			}
			}
			CompoundSubShape subs = sshape;
			subs.Scale = size;
			subs.subKey = subShapeKey;
			subs.shape_reference = new JPH::ScaledShape(newshape, JPHtoBE(size));
			shapeRef->AddShape(JPHtoBE(sshape.inPosition), JPHtoBE(sshape.inRotation), subs.shape_reference, subShapeKey++);
			subshapes.emplace_back(std::move(subs));
		}
		return HasUniformShape;
	}

	void Physics::CompoundEditInternal(std::vector<CompoundSubShape> const& insubshapes)
	{
		if (cmpd_shape_settings != nullptr) cmpd_shape_settings->Release();
		cmpd_shape_settings = new JPH::StaticCompoundShapeSettings();
		bool Has_Uniform_Shape = CompoundShapes(cmpd_shape_settings, insubshapes);
		JPH::Shape::ShapeResult CachedResult;
		JPH::Ref<JPH::Shape> temp_shape = new JPH::StaticCompoundShape(*cmpd_shape_settings, PhySys->GetAllocator(), CachedResult);

		// fix scale if sub shape is uniform shape type (sphere/capsule/etc.)
		Vec3f scale = GetShapeSize();
		if (Has_Uniform_Shape) {
			scale.y = scale.x;
			scale.z = scale.x;
		}
		UpdateScaledShape(temp_shape, scale, false);
		shape = Shape::StaticCompound;
		//CompoundSubShapesUpdate();
	}

	inline void Physics::UpdateScaledShape(JPH::Ref<JPH::Shape> const& _shape, Vec3f const& scale, bool updateMass)
	{
		JPH::Ref<JPH::ScaledShape> temp_scaled = new JPH::ScaledShape(_shape, JPHtoBE(scale));
		PhySys->GetBodyInterface().SetShape(body->GetID(), temp_scaled, updateMass, JPH::EActivation::Activate);

		if (shape_reference != nullptr)
			shape_reference->Release();

		shape_reference = temp_scaled;
	}

	inline bool Physics::HasUniformSubShape() {
		return subshapes.end() != std::find_if(subshapes.begin(), subshapes.end(), [](CompoundSubShape const& a) { return a.ShapeType == Shape::Sphere || a.ShapeType == Shape::Capsule; });
	}

	void Physics::CompoundSubShapesUpdate()
	{
		if (!IsCompoundShape()) return ;
		subshapes.clear();
		auto compound = static_cast<JPH::StaticCompoundShape const*>(body->GetShape());
		if (compound) {
			for (auto& sshape : compound->GetSubShapes()) {
				CompoundSubShape toAdd;
				toAdd.inPosition = JPHtoBE(sshape.GetPositionCOM() + body->GetShape()->GetCenterOfMass() - sshape.GetRotation() * sshape.mShape->GetCenterOfMass());
				toAdd.inRotation = JPHtoBE(sshape.GetRotation());
				toAdd.Scale = GetShapeSizeInternal(sshape.mShape);
				toAdd.ShapeType = JPHtoBE(sshape.mShape->GetSubType());
				toAdd.subKey = sshape.mUserData;
				subshapes.emplace_back(toAdd);
			}
		}
	}

	void Physics::AddForce(Vec3<float> const& in)
	{ 
		if (body->GetMotionType() != JPH::EMotionType::Dynamic) return;
		PhySys->GetBodyInterface().AddForce(body->GetID(), JPHtoBE(in));
		body->AddForce(JPHtoBE(in));
	}
	void Physics::AddImpulse(Vec3<float> const& in)
	{ 
		if (body->GetMotionType() != JPH::EMotionType::Dynamic) return; 
		PhySys->GetBodyInterface().AddImpulse(body->GetID(), JPHtoBE(in));
	}
	void Physics::AddTorque(Vec3<float> const& in)
	{ 
		if (body->GetMotionType() != JPH::EMotionType::Dynamic) return;
		PhySys->GetBodyInterface().AddTorque(body->GetID(), JPHtoBE(in));
	}
	void Physics::AddAngularImpulse(Vec3<float> const& in)
	{
		if (body->GetMotionType() != JPH::EMotionType::Dynamic) return; 
		PhySys->GetBodyInterface().AddAngularImpulse(body->GetID(), JPHtoBE(in));
	}
	void Physics::MoveKinematic(Vec3<float> const& targetpos, Quaternion<float> const& targetRot, float inDeltaTime)
	{ 
		if (body->GetMotionType() == JPH::EMotionType::Static) return; 
		PhySys->GetBodyInterface().MoveKinematic(body->GetID(), JPHtoBE(targetpos), JPHtoBE(targetRot), inDeltaTime);
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	rttr::registration::enumeration<Shape>(RegistrationNameCreator<Shape>())
	(
		value("Box", Shape::Box),
		value("Capsule", Shape::Capsule),
		value("Cylinder", Shape::Cylinder),
		value("ShapeNotImplemented", Shape::ShapeNotImplemented),
		value("Sphere", Shape::Sphere),
		value("StaticCompound", Shape::StaticCompound),
		value("Mesh", Shape::Mesh)
	);
	rttr::registration::enumeration<JPH::EMotionType>(RegistrationNameCreator<JPH::EMotionType>())
	(
		value("Dynamic", JPH::EMotionType::Dynamic),
		value("Kinematic", JPH::EMotionType::Kinematic),
		value("Static", JPH::EMotionType::Static)
	);
	rttr::registration::enumeration<JPH::EMotionQuality>(RegistrationNameCreator<JPH::EMotionQuality>())
	(
		value("Discrete", JPH::EMotionQuality::Discrete),
		value("LinearCast", JPH::EMotionQuality::LinearCast)
	);
	registration::class_<Physics>(RegistrationNameCreator<Physics>())
		.constructor()
		.property("Shape Enum", &Physics::GetShapeType, &Physics::SetShapeEnum)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Shape Size", &Physics::GetShapeSize, &Physics::SetShapeSize)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Motion Type Enum", &Physics::GetMotionType, &Physics::SetMotionType)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Scale Binded", &Physics::GetIsScaleBinded, &Physics::SetScaleBinded)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Motion Quality Enum", &Physics::GetMotionQuality, &Physics::SetMotionQuality)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Angular Velocity", &Physics::GetAngularVelocity, &Physics::SetAngularVelocity)
		(
			metadata(MetaDataKey::Serialize, false),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Max Angular Velocity", &Physics::GetMaxAngularVelocity, &Physics::SetMaxAngularVelocity)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Angular Damping", &Physics::GetAngularDamping, &Physics::SetAngularDamping)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Linear Velocity", &Physics::GetLinearVelocity, &Physics::SetLinearVelocity)
		(
			metadata(MetaDataKey::Serialize, false),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Max Linear Velocity", &Physics::GetMaxLinearVelocity, &Physics::SetMaxLinearVelocity)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Linear Damping", &Physics::GetLinearDamping, &Physics::SetLinearDamping)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Friction", &Physics::GetFriction, &Physics::SetFriction)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Restitution", &Physics::GetRestitution, &Physics::SetRestitution)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Gravity", &Physics::GetGravityFactor, &Physics::SetGravityFactor)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Offset", &Physics::GetOffset, &Physics::SetOffset)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Is Trigger", &Physics::GetIsTrigger, &Physics::SetIsTrigger)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Can Push Character", &Physics::GetCanPushCharacter, &Physics::SetCanPushCharacter)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Can Receive Character Impulse", &Physics::GetCanReceiveCharacterImpulse, &Physics::SetCanReceiveCharacterImpulse)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);

}