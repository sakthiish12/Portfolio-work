#include "pch.h"
#include "sPhysicsSystem.h"
#include "../../Engine/PauseControl.h"
#include "../../Engine/SpeedControl.h"
#include "../../Engine/GameEngine.h"
#include "../../Components/GameLogic.h"

namespace BE
{

	JPH::ValidateResult	BECollisionListener::OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::CollideShapeResult& inCollisionResult)
	{
		UNUSED(inCollisionResult);

		if (inBody1.GetUserData() || inBody2.GetUserData()) {
			return  JPH::ValidateResult::RejectAllContactsForThisBodyPair;
		}
		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	void BECollisionListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
		// Do behaviour on collision here
		if (inBody1.GetID() == inBody2.GetID()) return;
		EntityID Body1EID = PhySys->GetCorrespondingID(inBody1.GetID());
		EntityID Body2EID = PhySys->GetCorrespondingID(inBody2.GetID());
		if (ECS->GetComponent<EntityInfo>(Body1EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body1EID)) {
			for (auto& script : ECS->GetComponent<GameLogic>(Body1EID).GetAllScripts()) {
				if (script->GetAwake())
				script->OnCollision(Body2EID);
			}
		}
		if (ECS->GetComponent<EntityInfo>(Body2EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body2EID)) {
			for (auto& script : ECS->GetComponent<GameLogic>(Body2EID).GetAllScripts()) {
				if (script->GetAwake())
					script->OnCollision(Body1EID);
			}
		}
	}

	void BECollisionListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
	{
		// Do behaviour on persisted collision here
		EntityID Body1EID = PhySys->GetCorrespondingID(inBody1.GetID());
		EntityID Body2EID = PhySys->GetCorrespondingID(inBody2.GetID());
		if (ECS->GetComponent<EntityInfo>(Body1EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body1EID)) {
			for (auto& script : ECS->GetComponent<GameLogic>(Body1EID).GetAllScripts()) {
				if (script->GetAwake())
					script->OnPersistedCollision(Body2EID);
			}
		}
		if (ECS->GetComponent<EntityInfo>(Body2EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body2EID)) {
			for (auto& script : ECS->GetComponent<GameLogic>(Body2EID).GetAllScripts()) {
				if (script->GetAwake())
					script->OnPersistedCollision(Body1EID);
			}
		}
	}

	void BECollisionListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
	{
		if (PhySys->IsInBodyInterface(inSubShapePair.GetBody2ID()) && PhySys->IsInBodyInterface(inSubShapePair.GetBody1ID())) {
			EntityID Body1EID = PhySys->GetCorrespondingID(inSubShapePair.GetBody1ID());
			EntityID Body2EID = PhySys->GetCorrespondingID(inSubShapePair.GetBody2ID());
			if (ECS->GetComponent<EntityInfo>(Body1EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body1EID)) {
				for (auto& script : ECS->GetComponent<GameLogic>(Body1EID).GetAllScripts()) {
					if (script->GetAwake())
						script->OnCollisionRemoved(Body2EID);
				}
			}
			if (ECS->GetComponent<EntityInfo>(Body2EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body2EID)) {
				for (auto& script : ECS->GetComponent<GameLogic>(Body2EID).GetAllScripts()) {
					if (script->GetAwake())
						script->OnCollisionRemoved(Body1EID);
				}
			}
		}
	}

	bool BECharacterListener::OnContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
	{
		if (PhySys->GetBodyInterface().GetUserData(inBodyID2)) {
			return false;
		}
		BECharacterVirtual const* BEcharacter = dynamic_cast<BECharacterVirtual const*>(inCharacter);
		if (BEcharacter) {
			EntityID Body2EID = PhySys->GetCorrespondingID(inBodyID2);
			if (BEcharacter->id == Body2EID)
				return false;
			if (ECS->HasComponent<Physics>(Body2EID)) {
				Physics& physics = ECS->GetComponent<Physics>(Body2EID);
				if (physics.GetIsTrigger()) {
					if (ECS->GetComponent<EntityInfo>(Body2EID).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(Body2EID)) {
						for (auto& script : ECS->GetComponent<GameLogic>(Body2EID).GetAllScripts()) {
							if (script->GetAwake())
								script->OnCollision(BEcharacter->id);
						}
					}
					if (ECS->GetComponent<EntityInfo>(BEcharacter->id).HasFlag(EntityFlag::active) && ECS->HasComponent<GameLogic>(BEcharacter->id)) {
						for (auto& script : ECS->GetComponent<GameLogic>(BEcharacter->id).GetAllScripts()) {
							if (script->GetAwake())
								script->OnCollision(Body2EID);
						}
					}
					return false;
				}
			}	
		}
		return true;
	}

	void BECharacterListener::OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::Vec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
	{
		BECharacterVirtual const* BEcharacter = dynamic_cast<BECharacterVirtual const*>(inCharacter);
		if (BEcharacter) {
			EntityID Body2EID = PhySys->GetCorrespondingID(inBodyID2);
			if (ECS->HasComponent<Physics>(Body2EID)) {
				Physics& physics = ECS->GetComponent<Physics>(Body2EID);
				ioSettings.mCanPushCharacter = physics.GetCanPushCharacter();
				ioSettings.mCanReceiveImpulses = physics.GetCanReceiveCharacterImpulse();
			}
			/*if (ECS->HasComponent<GameLogic>(Body2EID)) {
				for (auto& script : ECS->GetComponent<GameLogic>(Body2EID).GetAllScripts()) {
					if (script->GetAwake())
						script->OnCollision(BEcharacter->id);
				}
			}
			if (ECS->HasComponent<GameLogic>(BEcharacter->id)) {
				for (auto& script : ECS->GetComponent<GameLogic>(BEcharacter->id).GetAllScripts()) {
					if (script->GetAwake())
						script->OnCollision(Body2EID);
				}
			}*/
		}
	}

	sPhysicsSystem::sPhysicsSystem() : temp_allocator{ JPH::TempAllocatorImpl(10 * 1024 * 1024) }
		, job_system{ JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1) }
		, system{ JPH::PhysicsSystem() }
		, broad_phase_layer_interface{ BPLayerInterfaceImpl() }
	{
		JPH::Factory::sInstance = new JPH::Factory();
		// Register all Jolt physics types
		JPH::RegisterTypes();
		// you would implement the JobSystem interface yourself and let Jolt Physics run on top
		// of your own job scheduler. JobSystemThreadPool is an example implementation.
	}

	sPhysicsSystem::~sPhysicsSystem()
	{
		// Destroy the factory

		if (JPH::Factory::sInstance)
		{
			delete JPH::Factory::sInstance;
			JPH::Factory::sInstance = nullptr;
		}
	}

	void sPhysicsSystem::Init()
	{
		system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, BroadPhaseCanCollide, ObjectCanCollide);
		system.SetContactListener(&CollisionListener);

		// (create all entity's physics settings)
		JPH::BodyInterface& body_interface = system.GetBodyInterface();
		for (EntityID entity : ECS->GetTracker<PhysicsTracker>()->GetTrackedEntities())
		{
			Physics& rPhysics = ECS->GetComponent<Physics>(entity);
			Transform& rTransform = ECS->GetComponent<Transform>(entity);
			if (!body_interface.IsAdded(rPhysics.GetBodyID()))
			{
				// Create the actual rigid body
				//rPhysics.CreateBody(body_interface, rTransform.GetPositon(), rTransform.GetRotation());
				CreateBody(entity, rPhysics, rTransform.GetPosition() + rPhysics.GetOffset(), rTransform.GetRotation());
				// Add it to the world
				body_interface.AddBody(rPhysics.GetBodyID(), JPH::EActivation::Activate);
			}
		}
		system.SetGravity(JPH::Vec3(0.f, -9.8f, 0.f));
		// Insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		system.OptimizeBroadPhase();

		// Init all basic shapes
		default_shapes[static_cast<size_t>(Shape::Sphere)] = new JPH::SphereShape(0.5f);
		default_shapes[static_cast<size_t>(Shape::Box)] = new JPH::BoxShape(JPH::Vec3(0.5f, 0.5f, 0.5f));
		default_shapes[static_cast<size_t>(Shape::Capsule)] = new JPH::CapsuleShape(0.5f, 0.5f);
		default_shapes[static_cast<size_t>(Shape::Cylinder)] = new JPH::CylinderShape(0.5f, 0.5f);

		HadInit = true;
	}

	void sPhysicsSystem::Awake()
	{

	}

	void sPhysicsSystem::Update(double dt)
	{
		UNUSED(dt);
#ifdef USE_EDITOR
		//If IMGUI is pausing, then don't update
		if (Debug::GetInstance()->GetDebugFlag(Debug::DebugFlags::imguiPause))
			return;
#endif
		if(PauseControl::GetPause())
			return;

		int numberOfSteps = Engine->GetPhysSteps();
		updatedThisFrame = false;

		//Loop used in systems that have time-based formula
		for (int step = 0; step < numberOfSteps; ++step)
		{
			JPH::BodyInterface& body_interface = system.GetBodyInterface();

			for (EntityID entity : ECS->GetTracker<PhysicsTracker>()->GetTrackedEntities()) 
			{
				//If entity is inactive, then skip
				if (!ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::active)) 
					continue;
				
				Transform& rTransform = ECS->GetComponent<Transform>(entity);

				if (rTransform.GetPhysicsUpdated())
				{
					rTransform.SetPhysicsUpdated(false);
					Physics& rPhysics = ECS->GetComponent<Physics>(entity);
					if (rPhysics.GetMotionType() == JPH::EMotionType::Static || rPhysics.GetMotionQuality() == JPH::EMotionQuality::LinearCast) body_interface.DeactivateBody(rPhysics.GetBodyID());
					body_interface.SetPositionAndRotation(rPhysics.GetBodyID(), JPHtoBE(rTransform.GetPosition() + rPhysics.GetOffset()), JPHtoBE(rTransform.GetRotation()), JPH::EActivation::Activate);
					if(rPhysics.GetIsScaleBinded()) rPhysics.SetShapeSize(rTransform.GetScale() /** 0.5f*/);
				}
			}
			// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
			const int cCollisionSteps = 1;

			// If you want more accurate step results you can do multiple sub steps within a collision step. Usually you would set this to 1.
			const int cIntegrationSubSteps = 1;

			// Step the world
			system.Update(static_cast<float>(Engine->GetFixedDeltaTime()) * EngineSpeed->GetEngineSpeedMultiplier(), cCollisionSteps, cIntegrationSubSteps, &temp_allocator, &job_system);
		
			//std::queue<std::pair<EntityID, Vec3f>> child_translations;

			for (EntityID entity : ECS->GetTracker<PhysicsTracker>()->GetTrackedEntities())
			{
				//If entity is inactive, then skip
				if (!ECS->GetComponent<EntityInfo>(entity).HasFlag(EntityFlag::active))
					continue;

				Physics& rPhysics = ECS->GetComponent<Physics>(entity);
				Transform& rTransform = ECS->GetComponent<Transform>(entity);

				//if (rTransform.HasParent()) {
					/*Vec3f translate = JPHtoBE(body_interface.GetPosition(rPhysics.GetBodyID())) - rTransform.GetPositon();
					child_translations.push(std::make_pair(entity, translate));*/
				/*	continue;
				}*/

				rTransform.PhysicsUpdatePosition(JPHtoBE(body_interface.GetPosition(rPhysics.GetBodyID())) - rPhysics.GetOffset());
				rTransform.PhysicsUpdateRotation(JPHtoBE(body_interface.GetRotation(rPhysics.GetBodyID())));

				if(rPhysics.GetIsScaleBinded()) rTransform.PhysicsUpdateScale(rPhysics.GetShapeSize() /** 2.f*/);
			}

			//while (!child_translations.empty()) {
			//	auto& pair = child_translations.front();
			//	Transform& rTransform = ECS->GetComponent<Transform>(pair.first);
			//	rTransform.SetPosition(rTransform.GetPositon() + pair.second);
			//	rTransform.SetPhysicsUpdated(true);
			//	child_translations.pop();
			//}
			updatedThisFrame = true;
		}

	}

	void sPhysicsSystem::Reload() 
	{
		//Physics bodies are cleared from ECS 
	}

	void sPhysicsSystem::Shutdown()
	{
		// Remove and destroy all bodies from the physics system.
		JPH::BodyInterface& body_interface = system.GetBodyInterface();
		for (EntityID entity : ECS->GetTracker<PhysicsTracker>()->GetTrackedEntities()) {
			Physics& rPhysics = ECS->GetComponent<Physics>(entity);
			if (body_interface.IsAdded(rPhysics.GetBodyID()))
			{
				// Remove it from the physics system. Note that the object itself keeps all of its state and can be re-added at any time.
				body_interface.RemoveBody(rPhysics.GetBodyID());
				// Destroy the object. After this the ID is no longer valid.
				body_interface.DestroyBody(rPhysics.GetBodyID());
			}
		}
		//JPH::Factory::sInstance = nullptr;
	}

	const JPH::BodyInterface& sPhysicsSystem::GetBodyInterface() const
	{
		return system.GetBodyInterface();
	}

	JPH::BodyInterface& sPhysicsSystem::GetBodyInterface()
	{
		return system.GetBodyInterface();
	}

	void sPhysicsSystem::SetGravity(Vec3<float> const& inGravity)
	{
		JPH::Vec3 set = { inGravity.x, inGravity.y, inGravity.z };
		system.SetGravity(set);
	}
	Vec3<float>	sPhysicsSystem::GetGravity() const
	{
		return JPHtoBE(system.GetGravity());
	}

	JPH::BroadPhaseQuery const& sPhysicsSystem::GetBroadPhaseQuery() const
	{
		return system.GetBroadPhaseQuery();
	}
	void sPhysicsSystem::CreateBody(EntityID id, Physics& PhysicsComponent, Vec3<float> const& Position, Quaternion<float> const& rotation)
	{
		JPH::BodyInterface& body_interface = system.GetBodyInterface();

		// create the settings
		JPH::ObjectLayer layer = PhysicLayers::NON_MOVING;
		JPH::BodyCreationSettings body_settings = JPH::BodyCreationSettings(PhysicsComponent.GetShapeReference(), JPHtoBE(Position), JPHtoBE(rotation), JPH::EMotionType::Static, layer);
		body_settings.mAllowDynamicOrKinematic = true;
		// create body in Jolt physics body interface
		JPH::Body* body = body_interface.CreateBody(body_settings);
		//body->SetPositionAndRotationInternal(, JPH::Quat(rotation.GetI(), rotation.GetJ(), rotation.GetK(), rotation.GetR()));
		// pass the body pointer to physics component
		PhysicsComponent.SetBodyPointer(body);
		// add body ID into EntityID-BodyID map
		bodyID_map.insert({  body->GetID(),id });
	}

	void sPhysicsSystem::DeleteBody(EntityID id)
	{
		JPH::BodyID bodyID = GetCorrespondingID(id);
		if (bodyID == JPH::BodyID()) return;
		bodyID_map.erase(bodyID);
		JPH::BodyInterface& body_interface = system.GetBodyInterface();
		if (body_interface.IsAdded(bodyID)) {
			body_interface.RemoveBody(bodyID);
			body_interface.DestroyBody(bodyID);
			return;
		}
		if(body_interface.IsActive(bodyID))
			body_interface.DestroyBody(bodyID);
	}

	void sPhysicsSystem::RemoveBody(EntityID id)
	{
		JPH::BodyID bodyID = GetCorrespondingID(id);
		if (bodyID == JPH::BodyID()) return;
		JPH::BodyInterface& body_interface = system.GetBodyInterface();
		if (body_interface.IsAdded(bodyID)) {
			body_interface.RemoveBody(bodyID);
		}
	}

	EntityID sPhysicsSystem::GetCorrespondingID(JPH::BodyID toFind) const
	{
		if(bodyID_map.find(toFind) == bodyID_map.end())
			return NULL_ENTITY;
		return bodyID_map.at(toFind);
	}

	JPH::BodyID sPhysicsSystem::GetCorrespondingID(EntityID toFind) const
	{
		for (auto it = bodyID_map.begin(); it != bodyID_map.end(); ++it) {
			if (it->second == toFind)
				return it->first;
		}
		//AssertCore(false, "Could not find a valid corresponding JoltPhysics BodyID with given EntityID");
		return JPH::BodyID();
	}

	bool sPhysicsSystem::IsInBodyInterface(JPH::BodyID const& find) const
	{
		auto itr = bodyID_map.find(find);
		if (itr == bodyID_map.end()) return false;
		return true;
	}

	bool sPhysicsSystem::GetHadInit() const
	{
		return HadInit;
	}
	bool sPhysicsSystem::HasUpdatedThisFrame() const
	{
		return static_cast<bool>(Engine->GetPhysSteps());
	}
	void sPhysicsSystem::RayCast(std::vector<EntityID>& CollidedEntities, std::vector<Vec3<valueType>>& ContactPoints, Vec3<valueType> const& ray_origin, Vec3<valueType> const& ray_dir, EntityTagFilter const& filter) const
	{
		// create ray
		JPH::Vec3 from = { ray_origin.x, ray_origin.y, ray_origin.z };
		JPH::Vec3 dir = { ray_dir.x, ray_dir.y, ray_dir.z };
		JPH::RayCast ray{ from, dir };

		// create a collector to contain collision data
			//JPH::AllHitCollisionCollector<JPH::RayCastBodyCollector> collector;
			//system.GetBroadPhaseQuery().CastRay(ray, collector);
		JPH::AllHitCollisionCollector<JPH::CastRayCollector> collector;
		JPH::RayCastSettings cast_settings;
		system.GetNarrowPhaseQuery().CastRay(ray, cast_settings, collector, {}, {}, filter);
		size_t num_hits = collector.mHits.size();
		JPH::RayCastResult* results = collector.mHits.data();

		// add the data of collided entities and contact poitns to vector containers
		for (size_t i = 0; i < num_hits; ++i)
		{
			// Filter tags
			EntityID eid = PhySys->GetCorrespondingID(results[i].mBodyID);
			CollidedEntities.emplace_back(eid);
			ContactPoints.emplace_back(JPHtoBE(ray.mOrigin + results[i].mFraction * ray.mDirection));
		}
	}

	EntityID sPhysicsSystem::RayCastSingleHit(Vec3<valueType>& contactPoint, Vec3<valueType>& contactNormal, Vec3<valueType> const& ray_origin, Vec3<valueType> const& ray_dir, EntityTagFilter const& filter) const
	{
		// create ray
		JPH::Vec3 from = { ray_origin.x, ray_origin.y, ray_origin.z };
		JPH::Vec3 dir = { ray_dir.x, ray_dir.y, ray_dir.z };
		JPH::RayCast ray{ from, dir };

		JPH::RayCastResult results;
		JPH::RayCastSettings cast_settings;
		system.GetNarrowPhaseQuery().CastRay(ray, results, {}, {}, filter);

		contactPoint = JPHtoBE(ray.mOrigin + results.mFraction * ray.mDirection);

		EntityID eid = PhySys->GetCorrespondingID(results.mBodyID);
		if (eid != NULL_ENTITY) {
			contactNormal = JPHtoBE(ECS->GetComponent<Physics>(eid).GetBody()->
				GetWorldSpaceSurfaceNormal(results.mSubShapeID2, ray.GetPointOnRay(results.mFraction)));
		}

		return eid;
	}

	EntityID sPhysicsSystem::RayCastSingleHit(Vec3<valueType> const& ray_origin, Vec3<valueType> const& ray_dir, EntityTagFilter const& filter) const
	{
		// create ray
		JPH::Vec3 from = { ray_origin.x, ray_origin.y, ray_origin.z };
		JPH::Vec3 dir = { ray_dir.x, ray_dir.y, ray_dir.z };
		JPH::RayCast ray{ from, dir };

		JPH::RayCastResult results;
		JPH::RayCastSettings cast_settings;
		system.GetNarrowPhaseQuery().CastRay(ray, results, {}, {}, filter);

		EntityID eid = PhySys->GetCorrespondingID(results.mBodyID);
		return eid;
	}

	void sPhysicsSystem::CollidePoint(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& point, EntityTagFilter const& filter) const
	{
		JPH::AllHitCollisionCollector<JPH::CollidePointCollector> collector;
		system.GetNarrowPhaseQuery().CollidePoint(JPHtoBE(point), collector, {}, {}, filter);
		size_t num_hits = collector.mHits.size();
		JPH::CollidePointResult* results = collector.mHits.data();
		for (size_t i = 0; i < num_hits; ++i)
		{
			EntityID eid = PhySys->GetCorrespondingID(results[i].mBodyID);
			CollidedEntities.emplace_back(eid);
		}
	}

	void sPhysicsSystem::SphereCollide(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& sphere_origin, valueType radius, std::set<TagID> const& Filter_tags, FilterType filterset, short OnlyTrigger)  const
	{
		JPH::AllHitCollisionCollector<JPH::CollideShapeBodyCollector> collector;
		system.GetBroadPhaseQuery().CollideSphere(JPHtoBE(sphere_origin), radius, collector);
		JPH::BodyIDVector results = collector.mHits;
		for (auto const& bodyid: results)
		{
			// Filter tags
			EntityID eid = PhySys->GetCorrespondingID(bodyid);

			bool istrigger = ECS->GetComponent<Physics>(eid).GetIsTrigger();
			if ((OnlyTrigger > 0 && !istrigger) || (OnlyTrigger == 0 && istrigger)) continue;

			EntityInfo& eInfo = ECS->GetComponent<EntityInfo>(eid);
			auto& eAllTags = eInfo.GetAllTags();
			if (CheckForFilter(eAllTags, Filter_tags, filterset))
			{
				CollidedEntities.emplace_back(eid);
			}
		}
	}
	void sPhysicsSystem::AABoxCollide(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& box_origin, valueType radius, std::set<TagID> const& Filter_tags, FilterType filterset, short OnlyTrigger) const
	{
		JPH::AABox box(JPHtoBE(box_origin), radius);
		AABoxCollideInternal(CollidedEntities, box, Filter_tags, filterset, OnlyTrigger);
	}

	void sPhysicsSystem::AABoxCollide(std::vector<EntityID>& CollidedEntities, Vec3<valueType> const& min, Vec3<valueType> const& max, std::set<TagID> const& Filter_tags, FilterType filterset, short OnlyTrigger) const
	{
		JPH::AABox box(JPHtoBE(min), JPHtoBE(max));
		AABoxCollideInternal(CollidedEntities, box, Filter_tags, filterset, OnlyTrigger);
	}

	bool sPhysicsSystem::getUpdatedThisFrame()
	{
		return updatedThisFrame;
	}

	void sPhysicsSystem::AABoxCollideInternal(std::vector<EntityID>& CollidedEntities, JPH::AABox const& box, std::set<TagID> const& Filter_tags, FilterType filterset, short OnlyTrigger) const
	{
		JPH::AllHitCollisionCollector<JPH::CollideShapeBodyCollector> collector;
		system.GetBroadPhaseQuery().CollideAABox(box, collector);
		JPH::BodyIDVector results = collector.mHits;
		for (auto const& bodyid : results)
		{
			// Filter tags
			EntityID eid = PhySys->GetCorrespondingID(bodyid);
			if (eid == NULL_ENTITY) continue;

			bool istrigger = ECS->GetComponent<Physics>(eid).GetIsTrigger();
			if ((OnlyTrigger > 0 && !istrigger) || (OnlyTrigger == 0 && istrigger)) continue;

			EntityInfo& eInfo = ECS->GetComponent<EntityInfo>(eid);
			auto& eAllTags = eInfo.GetAllTags();
			if (CheckForFilter(eAllTags, Filter_tags, filterset))
			{
				CollidedEntities.emplace_back(eid);
			}
		}
	}
}