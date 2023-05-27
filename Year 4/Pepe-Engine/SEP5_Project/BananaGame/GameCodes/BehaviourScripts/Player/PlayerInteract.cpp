#include "PlayerInteract.h"
#include "../Environment/Interactable.h"
#include "BabyMonkey.h"

namespace TH
{
	using namespace BE;

	void PlayerInteract::Awake()
	{
		const auto results = ECS->FindEntitiesByName("InteractUI", 1);
		if (results.empty())
		{
			interactUI = ECS->CreateEntity();
			ECS->GetComponent<EntityInfo>(interactUI).SetName("InteractUI");
			ECS->GetComponent<Transform>(interactUI).SetScale({ 1.4f, 0.3f, 0.01f });
			ECS->GetComponent<Transform>(interactUI).SetRotation({ 0.0f,-180.0f,0.0f });
			ECS->AddComponent<Renderer>(interactUI, Renderer());
			ECS->GetComponent<Renderer>(interactUI).setModelIDwithName("../Assets/Models/Primitives/flatPlane.bananaModel");
			ECS->GetComponent<Renderer>(interactUI).setAlbedoAlphaMapwithName("../Assets/Textures/UI_Assets/Game/PressE.dds", 0);
			ECS->GetComponent<Renderer>(interactUI).setBrightnessFactor(10.f, 0);
			ECS->GetComponent<Renderer>(interactUI).setOpacity(0.8f);
			ECS->GetComponent<EntityInfo>(interactUI).SetFlag(EntityFlag::active, false);
		}
		else
			interactUI = results.front();

		auto script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<VirtualCharacter>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<VirtualCharacter>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<VirtualCharacter>();
		}
		charac_body = dynamic_cast<VirtualCharacter*>(script_ptr.get());
	}

	void PlayerInteract::Update(double dt)
	{
		if (enabled) 
		{
			//Vec3f sight = (RendererSystem::GetInstance()->GameCamera.tf.GetRotation() * Vec3f::Forward()) * sightDistance;
			//Vec3f single_contact, single_normal;
			//EntityID hit_id = PhySys->RayCastSingleHit( RendererSystem::GetInstance()->GameCamera.tf.GetPosition(), sight, EntityTagFilter(0));

			std::vector<EntityID> entities_in_AOE;
			bool hit_interactable = false;
			float Box_HalfZ = 0.8f;
			Vec3f center = charac_body->GetColliderFrontalPos(Box_HalfZ);
			Vec3f box_halfExtent = Vec3f(Box_HalfZ, 1.2f, Box_HalfZ);
			PhySys->AABoxCollide(entities_in_AOE, center - box_halfExtent, center + box_halfExtent);

			for (size_t i = 0; i < entities_in_AOE.size(); i++)
			{
				if (ECS->HasComponent<GameLogic>(entities_in_AOE[i]))
				{
					auto scrpt_ptr = ECS->GetComponent<GameLogic>(entities_in_AOE[i]).GetScript<Interactable>();
					Interactable* intrct_ptr = dynamic_cast<Interactable*>(scrpt_ptr.get());

					if (intrct_ptr && !intrct_ptr->GetInteracted() && !intrct_ptr->disableInteraction) {
						
						hit_interactable = true;
						ECS->GetComponent<EntityInfo>(interactUI).SetFlag(EntityFlag::active, true);
						// set UI position above the interatacble
						Vec3f UIposition = ECS->GetComponent<Transform>(entities_in_AOE[i]).GetPosition();
						UIposition.y += ECS->HasComponent<Renderer>(entities_in_AOE[i]) * 0.5f + UIAboveOffset;
						ECS->GetComponent<Transform>(interactUI).SetPosition(UIposition);
						// set UI rotation to face the camera
						Vec3f position_to_face = Vec3f(RendererSystem::GetInstance()->GameCamera.tf.GetPosition().x, UIposition.y, RendererSystem::GetInstance()->GameCamera.tf.GetPosition().z);
						Quatf dir_rot = Vec3f::Forward().get_rotation_between(position_to_face - UIposition);
						ECS->GetComponent<Transform>(interactUI).SetRotation(dir_rot);

						if(InteractThisFrame)
						intrct_ptr->InteractObject();

					}
				}
			}

			//if (hit_id != NULL_ENTITY) {
			//	if (ECS->HasComponent<GameLogic>(hit_id)) {
			//		auto ptr = ECS->GetComponent<GameLogic>(hit_id).GetScript<Interactable>();

			//		// if sight hit interactable
			//		if (ptr && ECS->DoesEntityExists(interactUI) && ECS->HasComponent<Renderer>(hit_id))
			//		{			
			//			hit_interactable = true;
			//			ECS->GetComponent<EntityInfo>(interactUI).SetFlag(EntityFlag::active, true);
			//			// set UI position above the interatacble
			//			Vec3f UIposition = ECS->GetComponent<Transform>(hit_id).GetPosition();
			//			UIposition.y += ECS->HasComponent<Renderer>(hit_id) * 0.5f + UIAboveOffset;
			//			ECS->GetComponent<Transform>(interactUI).SetPosition(UIposition);
			//			// set UI rotation to face the camera
			//			Vec3f position_to_face = Vec3f(RendererSystem::GetInstance()->GameCamera.tf.GetPosition().x, UIposition.y, RendererSystem::GetInstance()->GameCamera.tf.GetPosition().z);
			//			Quatf dir_rot = Vec3f::Forward().get_rotation_between(position_to_face - UIposition);
			//			ECS->GetComponent<Transform>(interactUI).SetRotation(dir_rot);
			//		}
			//	}
			//}
			if (!hit_interactable && ECS->DoesEntityExists(interactUI)) {
				ECS->GetComponent<EntityInfo>(interactUI).SetFlag(EntityFlag::active, false);
			}
			InteractThisFrame = false;
		}
		
	}

}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<PlayerInteract>(RegistrationNameCreator<PlayerInteract>())
		.constructor()
		.property("Sight Distance", &PlayerInteract::sightDistance)
		.property("UI Above Offset", &PlayerInteract::UIAboveOffset);
}