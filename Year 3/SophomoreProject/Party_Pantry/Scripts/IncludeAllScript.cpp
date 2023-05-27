#include "../Engine/ECS/Systems/Logic.h"
#include "IncludeAllScript.h"

//Script to be added
#include "ScriptTest.h"
#include "TaskListScript.h"
#include "PlayerCollisionEnemyTest.h"
#include "HealthUIScript.h"
#include "BlenderProximityScript.h"
#include "PlayerController.h"
#include "SoundsScript.h"
#include "LoseQuitGameScript.h"
#include "KetchapGunScript.h"
#include "WaterShotScript.h"
#include "FireShotScript.h"
#include "ChiliShotScript.h"
#include "EnemyBehaviour.h"
#include "MouseHoverDarkenScript.h"
#include "DamagedScript.h"
#include "SpaceToSkipScript.h"
#include "MoveableBounder.h"
#include "MushroomBoss.h"
#include "CameraTransitionScript.h"
#include "FadeInOutScript.h"
#include "BridgeScript.h"
#include "LeverActivationScript.h"
#include "FadeInScript.h"
#include "TeleporterScript.h"
#include "FadeInOutCutScene2Script.h"
#include "BushScript.h"
#include "OptionsScrollerScriptV2.h"
#include "SoundLevel2Script.h"
#include "MainMenuPlayButtonScript.h"
#include "TeleporterLevel1Script.h"
#include "TomoSplashScript.h"
#include "DigipenLogoScript.h"
#include "GameIconScript.h"
#include "Tornado.h"
#include "Firebomb.h"
#include "MeleeSpatula.h"
#include "SpaceToSkipEndSceneScript.h"

//HUD
#include "HUD/AbilitiesUIScript.h"
#include "HUD/HUDUIScript.h"
#include "HUD/HUDSpeedOverlayScript.h"
#include "HUD/HUDShieldOverlayScript.h"
#include "HUD/HUDFireballOverlayScript.h"
#include "HUD/TomatoCounterScript.h"
#include "HUD/MushroomCounterScript.h"
#include "HUD/ChilliesCounterScript.h"
#include "HUD/CounterUIScript.h"

//pause
#include "Pause/PauseScript.h"
#include "Pause/PauseResumeScript.h"
#include "Pause/PauseMainMenuScript.h"
#include "Pause/PauseQuitGameScript.h"
#include "Pause/PauseRestartScript.h"
#include "Pause/PauseInstructionsScript.h"
#include "Pause/PauseInstructionFirstScript.h"
#include "Pause/PauseInstructionsSecondScript.h"
#include "Pause/PauseRestart2Script.h"
#include "Pause/PauseInstructionThirdScript.h"

//upgrade 
#include "Upgrades/UpgradeBodyScript.h"
#include "Upgrades/UpgradeSkillsScript.h"
#include "Upgrades/UpgradeButtonScript.h"
#include "Upgrades/UpgradeButton3FireBallScript.h"
#include "Upgrades/UpgradeButton2Shield.h"


//throne room
#include "ThroneRoom/ThroneDialoguesScript.h"
#include "Pause/PauseRestartThroneScript.h"
#include "ThroneRoom/ThroneRoomStairsScript.h"
#include "ThroneRoom/ThroneRoomTeleporterScript.h"

//upgrade dialogues
#include "UpgradeDialogues/UpgradeDialogue1Script.h"
#include "UpgradeDialogues/UpgradeDialogue2Script.h"
#include "UpgradeDialogues/UpgradeDialogue3Script.h"
#include "UpgradeDialogues/UpgradeDialogueSpeedUnlockedScript.h"
#include "UpgradeDialogues/UpgradeDialogueUnlocked2Script.h"
#include "UpgradeDialogues/UpgradeDialogueUnlocked3Script.h"
#include "UpgradeDialogues/UpgradeDialogueShield1Script.h"
#include "UpgradeDialogues/UpgradeDialogueShield2Script.h"
#include "UpgradeDialogues/UpgradeDialogueFlame1Script.h"
#include "UpgradeDialogues/UpgradeDialogueFlame2Script.h"


//blender
#include "Blender/MushroomTextCountScript.h"
#include "Blender/BlenderSceneTextScript.h"
#include "Blender/BlenderOutputScript.h"
#include "Blender/BlenderTextCountScript.h"
#include "Blender/BlenderCapDragScript.h"
#include "Blender/ChilliesTextCountScript.h"
#include "Blender/BlenderWarningScript.h"

//menu level
#include "MenuSelector/MenuSelectorLevel1Script.h"
#include "MenuSelector/MenuSelectorLevel2Script.h"

//main menu 
#include "CreditSceneScript.h"


//level 2 scripts
#include "Level2/BoulderScript.h"
#include "Level2/CaveTeleportingScript.h"
#include "Level2/FallingStepScript.h"
#include "Level2/BlockageScript.h"
#include "PauseOptionGameScript.h"
#include "ResolutionChangeScript.h"
#include "OptionsScrollerScript.h"
#include "PauseOptionSoundScript.h"
#include "Level2/MushroomDynamicCameraScript.h"
#include "Level2/DynamicHUDUIScript.h"
#include "Level2/BoulderCameraTransitionScript.h"
#include "Level2/SpeedUpgradeDialogue.h"
#include "Level2/BridgeCutSceneScript.h"
#include "Level2/KingCoordSplashScript.h"
#include "Level2/LevelTwoTasklistScript.h"

//enemy scripts
#include "TomatoTrooper.h"
#include "MushroomMole.h"
#include "MushroomBoss.h"
#include "TomatoTimebomb.h"
#include "TomatoBoss.h"
#include "ChilliCrackler.h"

//Your scripts here
void includeAllScript()
{

	//throne room
	ScriptManager.addScript(std::make_shared<ThroneDialogueScript>());
	ScriptManager.addScript(std::make_shared<DigipenLogoScript>());
	ScriptManager.addScript(std::make_shared<ThroneRoomStairsScript>());
	ScriptManager.addScript(std::make_shared<ThroneRoomTeleporterScript>());

	ScriptManager.addScript(std::make_shared<TestingScriptCPP>());
	ScriptManager.addScript(std::make_shared<TaskListScript>());
	ScriptManager.addScript(std::make_shared<PlayerCollisionScript>());
	ScriptManager.addScript(std::make_shared<AbilitiesUIScript>());
	ScriptManager.addScript(std::make_shared<HealthUIScript>());
	ScriptManager.addScript(std::make_shared<BlenderProximityScript>());
	ScriptManager.addScript(std::make_shared<BlenderOutputScript>());
	ScriptManager.addScript(std::make_shared<BlenderTextScript>());
	ScriptManager.addScript(std::make_shared<BlenderCapDragScript>());
	ScriptManager.addScript(std::make_shared<SoundScript>());
	ScriptManager.addScript(std::make_shared<PauseOptionGameScript>());
	ScriptManager.addScript(std::make_shared<ResolutionChangeScript>());
	ScriptManager.addScript(std::make_shared<OptionsScrollerScript>());
	ScriptManager.addScript(std::make_shared<PauseOptionSoundScript>());
	ScriptManager.addScript(std::make_shared<UpgradeBodyScript>());
	ScriptManager.addScript(std::make_shared<UpgradeSkillsScript>());
	ScriptManager.addScript(std::make_shared<UpgradeButtonScript>());
	ScriptManager.addScript(std::make_shared<OptionsScrollerScriptV2>());
	ScriptManager.addScript(std::make_shared<SoundLevel2Script>());

	ScriptManager.addScript(std::make_shared<LoseQuitGameScript>());
	ScriptManager.addScript(std::make_shared<KetchapGunScript>());
	ScriptManager.addScript(std::make_shared<BlenderSceneTextScript>());
	ScriptManager.addScript(std::make_shared<MouseHoverDarkScript>());
	ScriptManager.addScript(std::make_shared<DamagedScript>());
	ScriptManager.addScript(std::make_shared<SpaceToSkipScript>());
	ScriptManager.addScript(std::make_shared<CameraTransitionScript>());
	ScriptManager.addScript(std::make_shared<FadeInOutScript>());
	ScriptManager.addScript(std::make_shared<FadeInOutCustScene2Script>());
	ScriptManager.addScript(std::make_shared<FadeInScript>());
	ScriptManager.addScript(std::make_shared<LeverActivationScript>());
	ScriptManager.addScript(std::make_shared<BridgeScript>());
	ScriptManager.addScript(std::make_shared<BushScript>());
	ScriptManager.addScript(std::make_shared<HUDUIScript>());
	ScriptManager.addScript(std::make_shared<GameIconScript>());

	ScriptManager.addScript(std::make_shared<PlayerController>());
	ScriptManager.addScript(std::make_shared<WaterShotScript>());
	ScriptManager.addScript(std::make_shared<Tornado>());
	ScriptManager.addScript(std::make_shared<Firebomb>());
	ScriptManager.addScript(std::make_shared<MeleeSpatula>());
	ScriptManager.addScript(std::make_shared<FireShotScript>());
	ScriptManager.addScript(std::make_shared<ChiliShotScript>());
	ScriptManager.addScript(std::make_shared<EnemyBehaviour>());
	ScriptManager.addScript(std::make_shared<MoveableBounderScript>());
	ScriptManager.addScript(std::make_shared<TeleportScript>());
	ScriptManager.addScript(std::make_shared<MainMenuPlayScript>());
	ScriptManager.addScript(std::make_shared<SpaceToSkipEndSceneScript>());

	//level 1 
	ScriptManager.addScript(std::make_shared<TeleportLevel1Script>());
	ScriptManager.addScript(std::make_shared<TomoSplashScript>());


	//scripts for blender scene 
	ScriptManager.addScript(std::make_shared<MushroomBlenderTextScript>());
	ScriptManager.addScript(std::make_shared<ChilliesBlenderTextScript>());
	ScriptManager.addScript(std::make_shared<BlenderWarningScript>());

	//menu selector
	ScriptManager.addScript(std::make_shared<MenuSelectorLevel1Script>());

	//HUD
	ScriptManager.addScript(std::make_shared<HUDSpeedOverlayScript>());
	ScriptManager.addScript(std::make_shared<HUDShieldOverlayScript>());
	ScriptManager.addScript(std::make_shared<HUDFireballOverlayScript>());
	ScriptManager.addScript(std::make_shared<TomatoCountScript>());
	ScriptManager.addScript(std::make_shared<MushroomCountScript>());
	ScriptManager.addScript(std::make_shared<ChilliesCountScript>());
	ScriptManager.addScript(std::make_shared<CounterUIScript>());

	//main menu
	ScriptManager.addScript(std::make_shared<CreditSceneScript>());


	//scripts for upgrade
	ScriptManager.addScript(std::make_shared<UpgradeButtonFireballScript>());
	ScriptManager.addScript(std::make_shared<UpgradeButtonShieldScript>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogue1Script>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogue2Script>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogue3Script>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogueSpeedUnlockedScript>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogue2UnlockedScript>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogue3UnlockedScript>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogueShield1Script>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogueShield2Script>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogueFlame1Script>());
	ScriptManager.addScript(std::make_shared<UpgradeDialogueFlame2Script>());

	//scripts for pause 
	ScriptManager.addScript(std::make_shared<PauseRestart2Script>());
	ScriptManager.addScript(std::make_shared<PauseScript>());
	ScriptManager.addScript(std::make_shared<PauseResumeScript>());
	ScriptManager.addScript(std::make_shared<PauseMainMenuScript>());
	ScriptManager.addScript(std::make_shared<PauseQuitGameScript>());
	ScriptManager.addScript(std::make_shared<PauseRestartScript>());
	ScriptManager.addScript(std::make_shared<PauseInstrucScript>());
	ScriptManager.addScript(std::make_shared<PauseInstrucFirstScript>());
	ScriptManager.addScript(std::make_shared<PauseInstrucSecondScript>());
	ScriptManager.addScript(std::make_shared<PauseRestartThroneScript>());
	ScriptManager.addScript(std::make_shared<PauseInstructionThirdScript>());

	//script for level 2
	ScriptManager.addScript(std::make_shared<BoulderScript>());
	ScriptManager.addScript(std::make_shared<BlockageScript>());
	ScriptManager.addScript(std::make_shared<CaveTeleportScript>());
	ScriptManager.addScript(std::make_shared<FallingStepScript>());
	ScriptManager.addScript(std::make_shared<MushroomDynamicCameraScript>());
	ScriptManager.addScript(std::make_shared<DynamicHUDUIScript>());
	ScriptManager.addScript(std::make_shared<BoulderCameraScript>());
	ScriptManager.addScript(std::make_shared<SpeedUpgradeDialogueScript>());
	ScriptManager.addScript(std::make_shared<BridgeCutSceneScript>());
	ScriptManager.addScript(std::make_shared<KingCoordSplashScript>());
	ScriptManager.addScript(std::make_shared<LevelTwoTasklistScript>());

	//scripts for enemies
	ScriptManager.addScript(std::make_shared<MushroomMole>());
	ScriptManager.addScript(std::make_shared<MushroomBoss>());
	ScriptManager.addScript(std::make_shared<TomatoTrooper>());
	ScriptManager.addScript(std::make_shared<TomatoTimebomb>());
	ScriptManager.addScript(std::make_shared<TomatoBoss>());
	ScriptManager.addScript(std::make_shared<ChilliCrackler>());
}
