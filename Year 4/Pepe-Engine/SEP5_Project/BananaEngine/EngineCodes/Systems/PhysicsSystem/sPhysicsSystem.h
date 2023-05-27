#pragma once
#ifndef	PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include "EntityTagFilter.h"
#include "../../Components/Physics.h"
#include "../ISystem.h"

namespace BE
{
	// forward declare
	class EntityTagFilter;
	class sPhysicsSystem;

	// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
	const unsigned int cMaxBodies = 10240;
	// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
	const unsigned int cNumBodyMutexes = 0;
	// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
	// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
	// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
	const unsigned int cMaxBodyPairs = 65536;
	// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
	// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
	const unsigned int cMaxContactConstraints = 10240;

	#define GRAVITY -12.0f
	/// Function that determines if two object layers can collide
	inline bool ObjectCanCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2)
	{
		switch (inObject1)
		{
		case PhysicLayers::NON_MOVING:
			return inObject2 == PhysicLayers::MOVING; // Non moving only collides with moving
		case PhysicLayers::MOVING:
			return true; // Moving collides with everything
		default:
			AssertCore(false, "Unrecognised physics layer number");
			return false;
		}
	};

	/// Broadphase layers
	namespace BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr JPH::uint NUM_LAYERS(2);
	};

	/// BroadPhaseLayerInterface implementation
	class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
	{
	public:
		BPLayerInterfaceImpl()
		{
			// Create a mapping table from object to broad phase layer
			mObjectToBroadPhase[PhysicLayers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[PhysicLayers::MOVING] = BroadPhaseLayers::MOVING;
		}

		virtual JPH::uint GetNumBroadPhaseLayers() const override
		{
			return BroadPhaseLayers::NUM_LAYERS;
		}

		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
		{
			AssertCore(inLayer < PhysicLayers::NUM_LAYERS, "Getting broad phase layer number more than total layer number");
			return mObjectToBroadPhase[inLayer];
		}


#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
		{
			switch ((JPH::BroadPhaseLayer::Type)inLayer)
			{
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
			default:	AssertCore(false, "Unrecognised physics layer number"); return "INVALID";
			}
		}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

	private:
		JPH::BroadPhaseLayer					mObjectToBroadPhase[PhysicLayers::NUM_LAYERS];
	};

	/// Function that determines if two broadphase layers can collide
	inline bool BroadPhaseCanCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2)
	{
		switch (inLayer1)
		{
		case PhysicLayers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case PhysicLayers::MOVING:
			return true;
		default:
			AssertCore(false, "Unrecognised physics layer number");
			return false;
		}
	}

	// Collision listener
	class BECollisionListener : public JPH::ContactListener
	{
	public:
		// See: ContactListener
		virtual JPH::ValidateResult	OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::CollideShapeResult& inCollisionResult) override;

		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

		virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
	};


	// Body activation listener
	class BEBodyActivationListener : public JPH::BodyActivationListener
	{
	public:
		virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
		{
			std::cout << "A body got activated" << std::endl;
		}

		virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override
		{
			std::cout << "A body went to sleep" << std::endl;
		}
	};


	class sPhysicsSystem : public Singleton<sPhysicsSystem>, public ISystem
	{
	protected:
		

	public:
		sPhysicsSystem();

		~sPhysicsSystem();

		ISYSTEM_FUNCTIONS;

		// get JPH Physics Body interface to control objects
		const JPH::BodyInterface& GetBodyInterface() const;
		JPH::BodyInterface& GetBodyInterface();

		/// Set gravity value
		void SetGravity(Vec3<float> const& inGravity);
		Vec3<float>	GetGravity() const;

		// get broadphase to simulate collision with a given ray/shape.
		JPH::BroadPhaseQuery const& GetBroadPhaseQuery() const;

		void CreateBody(EntityID id, Physics& PhysicsComponent, Vec3<float> const& Position, Quaternion<float> const& rotation);

		void DeleteBody(EntityID id);
		void RemoveBody(EntityID id);

		EntityID GetCorrespondingID(JPH::BodyID find) const;

		JPH::BodyID GetCorrespondingID(EntityID find) const;

		bool GetHadInit() const;
		bool HasUpdatedThisFrame() const;
		bool IsInBodyInterface(JPH::BodyID const& find) const;

		inline JPH::PhysicsSystem* GetJoltSystem() { return &system; }
		inline JPH::PhysicsSystem const* GetJoltSystem() const { return &system; }

		inline auto const& GetDefaultShapes() { return default_shapes; }
		inline auto const& GetMeshShapeSettings() { return mesh_shapes; }
		inline void AddMeshShape(std::pair<BEID, JPH::Ref<JPH::Shape>> pair) { mesh_shapes.insert(pair); }
		inline auto& GetAllocator() { return temp_allocator; }

		inline void AddCharacterToSystem(JPH::Character* charac, EntityID id , JPH::EActivation activation = JPH::EActivation::Activate) {
			charac->AddToPhysicsSystem(activation);
			bodyID_map.insert({ charac->GetBodyID(), id });
		}
		size_t GetMapSize() { return bodyID_map.size(); }

		// ray cast
		void RayCast(std::vector<EntityID>& CollidedEntities, std::vector<Vec3<valueType>>& ContactPoints, Vec3<valueType> const& ray_origin, Vec3<valueType> const& ray_dir, EntityTagFilter const& filter = {}) const;

		// ray cast for only hitting first object
		EntityID RayCastSingleHit(Vec3<valueType>& ContactPoint, Vec3<valueType>& contactNormal, Vec3<valueType> const& ray_origin, Vec3<valueType> const& ray_dir, EntityTagFilter const& filter = {}) const;
		EntityID RayCastSingleHit(Vec3<valueType> const& ray_origin, Vec3<valueType> const& ray_dir, EntityTagFilter const& filter = {}) const;
		// collide point
		void CollidePoint(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& point, EntityTagFilter const& filter = {}) const;

		// Sphere Cast
		void SphereCollide(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& sphere_origin, valueType radius,
						std::set<TagID> const& Filter_tags = {}, FilterType filterset = FilterType::NoFilter, short OnlyTrigger = -1) const;
		// AAbox Cast
		void AABoxCollide(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& box_origin, valueType radius,
						std::set<TagID> const& Filter_tags = {}, FilterType filterset = FilterType::NoFilter, short OnlyTrigger = -1) const;
		void AABoxCollide(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& min, Vec3<valueType> const& max,
			std::set<TagID> const& Filter_tags = {}, FilterType filterset = FilterType::NoFilter, short OnlyTrigger = -1) const;
		
		//@donavern
		bool getUpdatedThisFrame();
	private:
		void AABoxCollideInternal(std::vector<EntityID>& CollidedEntities, JPH::AABox const& box, std::set<TagID> const& Filter_tags = {}, FilterType filterset = FilterType::NoFilter, short OnlyTrigger = -1)  const;
	private:
		JPH::TempAllocatorImpl temp_allocator;
		JPH::JobSystemThreadPool job_system;
		JPH::PhysicsSystem system;
		// Create mapping table from object layer to broadphase layer
		BPLayerInterfaceImpl broad_phase_layer_interface;
		BECollisionListener CollisionListener;

		std::unordered_map<JPH::BodyID, EntityID> bodyID_map;
		std::array<JPH::Ref<JPH::Shape>, static_cast<size_t>(Shape::ShapeNotImplemented)> default_shapes;
		std::unordered_map<BEID, JPH::Ref<JPH::Shape>> mesh_shapes;
		bool HadInit = false;
		bool updatedThisFrame{}; //@donavern used to ensure camera updates only when character updates
	};

	#define PhySys sPhysicsSystem::GetInstance()

	class BECharacterVirtual : public JPH::CharacterVirtual
	{
	public:
		BECharacterVirtual(const JPH::CharacterVirtualSettings* inSettings, Vec3f inPosition, Quatf inRotation)
			:CharacterVirtual(inSettings, JPHtoBE(inPosition), JPHtoBE(inRotation), PhySys->GetJoltSystem())
		{}
		BEID id = NULL_ENTITY;
	};

	class BECharacterListener : public JPH::CharacterContactListener
	{
	public:
		/// Checks if a character can collide with specified body. Return true if the contact is valid.
		bool OnContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2);

		/// Called whenever the character collides with a body. Returns true if the contact can push the character.
		void OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::Vec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings) override;
	};
}

#endif