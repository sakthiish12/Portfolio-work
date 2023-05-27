
#include "InteractableConsole.h"
#include "../../DialogueCutsceneObjectivesControl/CutsceneController.h"
#include "../../DialogueCutsceneObjectivesControl/ObjectivesController.h"
#include "Door.h"
namespace TH
{
	using namespace BE;

	void InteractableConsole::Awake()
	{
		m_cdoController = BE::ECS->FindEntitiesByName("Dialogue_Cutscene_Objectives_Controller", 1).front();

	}

	void InteractableConsole::Init() {

		// Get pointer to interactable base
		auto script_ptr = BE::ECS->GetComponent<BE::GameLogic>(id).GetScript<Interactable>();
		if (script_ptr.get() == nullptr) {
			ECS->GetComponent<GameLogic>(id).AddScript<Interactable>();
			script_ptr = ECS->GetComponent<GameLogic>(id).GetScript<Interactable>();
		}
		interact_base = dynamic_cast<Interactable*>(script_ptr.get());

		const auto targets = ECS->FindEntitiesByTag(Tagtotarget);
		for (auto eid : targets) {
			tTargetIDs.insert(eid);
		}
	}

	void InteractableConsole::Update(double dt)
	{
		if (interact_base->GetInteracted() && !ActivatedOnce)
		{
			ActivatedOnce = true;

			// start cutscene
			BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
			std::shared_ptr<CutsceneController> ptr1 = gl.GetScriptDerived<CutsceneController>();
			ptr1->StartCutscene(CutsceneIndex);
			

			// unlock door
			for (auto eid : tTargetIDs) {
				auto scrptptr = ECS->GetComponent<GameLogic>(eid).GetScript<DoorScript>();
				if (scrptptr) {
					DoorScript* door_ptr = dynamic_cast<DoorScript*>(scrptptr.get());
					if (door_ptr)
						door_ptr->SetDoorLock(false);
				}
				for (auto cid : ECS->GetComponent<Transform>(eid).GetChildren()) {
					if (ECS->HasComponent<lightComponent>(cid)) {
						ECS->GetComponent<lightComponent>(cid).setColor({ 1.f, 0.f, 0.f });
					}
				}
			}
			const EntityID targetDoor = *tTargetIDs.begin();
			const auto CameraCallback = [targetDoor, this](float t)
			{
				if (t < 0.5f)
				{
					for (auto cid : ECS->GetComponent<Transform>(targetDoor).GetChildren()) {
						if (ECS->HasComponent<lightComponent>(cid)) {
							ECS->GetComponent<lightComponent>(cid).setColor(Vec3f(1.f, 0.f, 0.f).toGLM() * (0.5f - t) * 2.0f);
						}
					}
				}
				else
				{
					for (auto cid : ECS->GetComponent<Transform>(targetDoor).GetChildren()) {
						if (ECS->HasComponent<lightComponent>(cid)) {
							ECS->GetComponent<lightComponent>(cid).setColor(Vec3f(0.f, 1.f, 0.f).toGLM() * (t - 0.5f) * 2.0f);
						}
					}
				}

				//Objective 6
				BE::GameLogic& gl = BE::ECS->GetComponent<BE::GameLogic>(m_cdoController);
				std::shared_ptr<ObjectivesController> ptr1 = gl.GetScriptDerived<ObjectivesController>();
				ptr1->StartObjectives(5);
			};
			const EntityID cameraTarget = ECS->FindEntitiesByName("ConsoleUnlockTargeting").front();
			CameraController::inst->SetTarget(
				ECS->GetComponent<Transform>(targetDoor).GetPosition(),
				ECS->GetComponent<Transform>(cameraTarget).GetPosition(),
				ECS->GetComponent<Transform>(cameraTarget).GetEulerRotation(),
				1.0f,
				CameraCallback
			);
		}
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<InteractableConsole>(RegistrationNameCreator<InteractableConsole>())
		.constructor()
		.property("Target tag: ", &InteractableConsole::Tagtotarget)
		.property("Cutscene to play: ", &InteractableConsole::CutsceneIndex);
}