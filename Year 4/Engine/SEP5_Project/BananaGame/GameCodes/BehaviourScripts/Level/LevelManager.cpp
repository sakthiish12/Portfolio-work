
#include "LevelManager.h"
#include "../Character/CharacterScript.h"
#include "../Player/PlayerScript.h"

namespace TH
{
	using namespace BE;

	void LevelManager::ShowGameOver()
	{
		if (GameOverIsShowing) 
			return;
		
		GameOverIsShowing = true;
			
		//Show the canvas
		BE::UI->GetCanvas(gameoverCanvas)->SetIsActive(true);
		BE::UI->SaveFontState();
		BE::Input->SetEnableCursor(true);
	}

	void LevelManager::ShowLevelComplete()
	{
		if (LevelCompleteIsShowing)
			return;

		LevelCompleteIsShowing = true;

		//Show the canvas
		BE::UI->GetCanvas(levelCompleteCanvas)->SetIsActive(true);
		BE::UI->SaveFontState();
		BE::Input->SetEnableCursor(true);
	}

	void LevelManager::ToggleAbilityMenu(bool on)
	{
		if (AbilityMenuIsShowing == on) 
			return;
		
		AbilityMenuIsShowing = on;

		// show canvas
		BE::UI->GetCanvas(abilityMenuCanvas)->SetIsActive(on);

		// disable player attacks by left clicks
		Assert(ECS->HasComponent<GameLogic>(GLS->GetPlayerID()), "Player is missing gamelogic component");
		auto pptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScriptDerived<PlayerCharacter>();
		pptr->SetCanAttack(!on);
		pptr->SetCanSwitchMode(!on);
		
		//Enable or disable cursor
		BE::Input->SetEnableCursor(on);
	}

	void LevelManager::Awake()
	{
		PhySys->SetGravity(Gravity);

		gameoverCanvas = BE::UI->FindUICanvasesByName("GameOverScreen", 1).front();
		levelCompleteCanvas = BE::UI->FindUICanvasesByName("LevelCompleteScreen", 1).front();
		abilityMenuCanvas = BE::UI->FindUICanvasesByName("AbilityMenu", 1).front();

	}

	void LevelManager::Init()
	{
		mWallCount = ECS->FindEntitiesByTag("Wall").size();

		EntityID pid = GLS->GetPlayerID();
		if (ECS->HasComponent<GameLogic>(pid)) {
			auto ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<CharacterScript>();
			if (ptr != nullptr) {
				CharacterScript* charptr = dynamic_cast<CharacterScript*>(ptr.get());
				if (charptr)
					charptr->cLevelMgr = this;
			}
		}
	}

	void LevelManager::Update(double dt)
	{
#if 0
		//Add this back code if need to add new walls
		size_t currentwalls = ECS->FindEntitiesByTag("Wall").size();
		if (mWallCount != currentwalls) {
			mWallCount = currentwalls;
			auto ptr = ECS->GetComponent<GameLogic>(id).GetScript<PathFinder>();
			if (ptr) {
				PathFinder* pather = dynamic_cast<PathFinder*>(ptr.get());
				if (pather)
					pather->UpdateGrid();
			}
		}
#endif

		if (Input->IsKeyPressed(Key::TAB) && !AbilityMenuIsShowing) {
			ToggleAbilityMenu(true);
			EngineSpeed->SetEngineSpeedMultiplier(0.05f);
			AudioMgr->SetGlobalSoundPitch(MIN_AUDIO_PITCH);
		}
		else if (Input->IsKeyReleased(Key::TAB) && AbilityMenuIsShowing) {
			ToggleAbilityMenu(false);
			EngineSpeed->ResetEngineSpeedMultiplier();
			AudioMgr->SetGlobalSoundPitch(DEF_AUDIO_PITCH);
		}
		
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<LevelManager>(RegistrationNameCreator<LevelManager>())
		.constructor();
}