#include "Interactable.h"
namespace TH
{
	LevelManager* Interactable::iLevelMgr = nullptr;

	void Interactable::Awake()
	{
		if(iLevelMgr) iLevelMgr = nullptr;
	}
	void Interactable::Init()
	{
		if (!iLevelMgr) {
			const auto game = ECS->FindEntitiesByName("GameManager", 1);
			if (game.size()) {
				auto scrpt_ptr = ECS->GetComponent<GameLogic>(game.front()).GetScript<LevelManager>();
				LevelManager* level = dynamic_cast<LevelManager*>(scrpt_ptr.get());
				if (level) {
					iLevelMgr = level;
				}
			}
		}
	}

	void Interactable::InteractObject()
	{
		if (!IsInteracted && !FirstTimeInteracted) {
			iLevelMgr->ObjectInteractedFirstTime(id);
			FirstTimeInteracted = true;
		}
		IsInteracted = true;
	}
	void Interactable::ResetInteracted()
	{
		IsInteracted = false;
	}
	bool Interactable::GetInteracted() const
	{
		return IsInteracted;
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<Interactable>(RegistrationNameCreator<Interactable>())
		.constructor();
}
