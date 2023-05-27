
#include "../Player/PlayerScript.h"
#include "PickUps.h"
#include "../../UserInterface/InGameUIScripts/TutorialUIController.h"
namespace TH
{
	using namespace BE;

	void BananaPickUp::Init() {

		// Get pointer to interactable base
		auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<Interactable>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<Interactable>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<Interactable>();
		}
		interact_base = dynamic_cast<Interactable*>(script_ptr.get());

		auto scrpt_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<PlayerCharacter>();
		PlayerCharacter* char_ptr = dynamic_cast<PlayerCharacter*>(scrpt_ptr.get());
		if (char_ptr != nullptr) {
			player = char_ptr;
		}
	}

	void BananaPickUp::Update(double dt)
	{
		if (interact_base->GetInteracted()) {
			if (player != nullptr) {
				if (!HasCalledPlayerAnim) {
					player->SetAnimState(PlayerAnim::InteractCrate);

					// Emit particles
					BE::ParticleSystem particleSystem;
					glm::vec3 position{ 0, 0, 0 };
					glm::vec3 direction{ 0, 1, 0 };
					float speed = 2.0f;
					float spread = 180.0f;
					int count =10;
					particleSystem.Emit(position, direction, speed, spread, count);


					HasCalledPlayerAnim = true;
					AudioSource& as = ECS->GetComponent<AudioSource>(GLS->GetPlayerID());
					as.StopAllAudio();
					as.Play(AssetID(4054102081384738114));	//Banana!.wav
					AudioSource& as2 = ECS->GetComponent<AudioSource>(id);
					as2.Play(AssetID(1958017969286768020));	//MediumObjectMoved.wav
				}
				if (HasCalledPlayerAnim && !player->IsPlayingOneShotAnim()) {
					player->AddBananas(Bananas_quantity);
					const auto idGameManager =ECS->FindEntitiesByName("GameManager").front();
					TutorialUIController& uiController = *dynamic_cast<TutorialUIController*>(ECS->GetComponent<GameLogic>(idGameManager).GetScript<TutorialUIController>().get());
					uiController.SetTutorialComplete(TutorialUIController::TutorialIndex::CrateInteractCheckbox);
					ECS->DestroyEntity(id);
				}
			}
		}
	}



	void MilkBottlePickUp::Init() {

		// Get pointer to interactable base
		auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<Interactable>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<Interactable>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<Interactable>();
		}
		interact_base = dynamic_cast<Interactable*>(script_ptr.get());

		script_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<MilkBottleUIController>();
		MilkBottleUIController* mb_ptr = dynamic_cast<MilkBottleUIController*>(script_ptr.get());
		if (mb_ptr != nullptr) {
			player_milkbottles = mb_ptr;
		}
	}

	void MilkBottlePickUp::Update(double dt)
	{
		if (interact_base->GetInteracted()) {
			if (player_milkbottles != nullptr){
				player_milkbottles->AddMilkBottleCount();
				BE::ECS->GetComponent<BE::AudioSource>(BE::GLS->GetPlayerID()).Play(BE::AssetID(10210675003471560139));		//MilkForBaby.wav
				ECS->DestroyEntity(id);
			}
		}
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<BananaPickUp>(RegistrationNameCreator<BananaPickUp>())
		.constructor()
		.property("Bananas Quantity", &BananaPickUp::Bananas_quantity);
	registration::class_<MilkBottlePickUp>(RegistrationNameCreator<MilkBottlePickUp>())
		.constructor();
}