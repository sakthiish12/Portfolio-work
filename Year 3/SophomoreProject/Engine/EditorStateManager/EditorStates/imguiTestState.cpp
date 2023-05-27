// /******************************************************************************/
// /*!
// \file         imguitestState.cpp
// \project name Engine
// \author(s)    Sakthiish % 25 - 60 lines
// \author(s)    Wilfred Ng Jun Hwee %25 - 60 lines
// \author(s)    Ryan Wang % 25 - 60 lines
// \author(s)    Poh Chun Ren % 25 - 60 lines
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the definition for the main engine state,
//				 that handles both the ImGui editor and gameplay
// \lines        241 
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

//TODO Documentation
//A sample game state.
#include "../../pch.h"
#include <iostream>
#include <random>
#include <chrono>
#include <filesystem>
#include "../../WinMgr/WinMgr.h"
#include "../../InputMgr/InputMgr.h"
#include "gameState.h"
//#include "../../ECS/Systems/physicsSystem.h"
#include "../gameStateManager.h"
#include "../../Serializer/ISerializable.h"
#include "../../Serializer/Settings/WindowSetting.h"
#include "../../ECS/ECS.h"
#include "../../ECS/Systems/SerializedOfficialSystem.h"//official serialized system
#include "../../Editor/Editor.h"//for the imgui
#include "../../Serializer/SerializeManager.h"
#include "../../Graphics/Source/FontRendering.h"//font rendering
#include "../../AssetManager/AssetManager.h"//for the asset manager
#include "../../ECS/Systems/BatchRenderingSystem.h"//batch rendering system 
#include "../../ECS/Systems/CameraSystem.h"//camera system 
#include "../../ECS/Systems/DialogueManagerSystem.h"//dialogue manager
//for the editor
#include "../../Editor/Editor.h"
#include "../../Graphics/Source/Framebuffer.h"//frame buffer
#include "../../Mono/Mono.h"
//for physics and controller system integration
#include "../../ECS/Systems/Physics/physicsSystem.h"
#include "../../ECS/Systems/controllerSystem.h"
//for the camera
#include "../../Graphics/Source/EditorCamera.h"
//for the a star pathfinding
#include "../../AI Pathfinding/AStartPathFinding.h"//pathfinding astar
#include "../../ECS/Systems/AStarPathSystem.h"//pathfinding system 
//for enemy state 
#include "../../ECS/Systems/EnemyStateSystem.h"//enemy state 
//font system
#include "../../ECS/Systems/FontRenderingSystem.h"//font render 
//GUI system
#include "../../ECS/Systems/GUISystem.h"//for the gui system
//Level Loader system
#include "../../ECS/Systems/LevelLoaderButton.h"
//post processing frame buffer
#include "../../Graphics/Source/PostProcessing.h"//post processing 
#include "../../ECS/Systems/AbilitiesSystem.h"//abilities 
//Transform Parent system
#include "../../ECS/Systems/TransformParentSystem.h"
#include "../../ECS/Systems/AnimationSystem.h"
#include "../../ECS/Systems/Physics/CollisionSystem.h"
#include "../../ECS/Systems/Logic.h"
#include "../../ECS/Systems/EnemySpawningSystem.h"//enemy spawning system
#include "../../ECS/Systems/TimeDestructor.h"//time destructor 
#include "../../ECS/Systems/DialogueSystem.h"//dialogue 

#include "../../ECS/Systems/ParticleSystem/ParticleSystem.h"
#include "../../Serializer/Settings/AnimationDataSerializer.h" //TODO might remove

imguiTestState g_imguiTestState;



#include"../../StopWatches/EngineTime.h"
#include "../../ECS/Systems/CSharpScriptSystem.h"
#include "../../Sound/SoundManager.h"
#include "../../Sound/SoundManagerV2.h"

static bool gameLoaded{ false };


void imguiTestState::load()
{
	AssetManager::GetInstance();//init and loads all the required assets

	audio;//Loads the new sound manager
}

void imguiTestState::init()
{

	/***************************
	@ Brief - init ecs
	****************************/
	//registering render system
	SerializedRenderer2DSys = ecs.registerSystem<SerializedRenderer2DSystem>();
	ecs.setSystemComponentRequirement<SerializedRenderer2DSystem, transform2D>(); //System requires more components in entity
	ecs.setSystemComponentRequirement<SerializedRenderer2DSystem, material>();//setting up renderer component
	ecs.setSystemComponentRequirement<SerializedRenderer2DSystem, spriteCom>();

	//Register sprite system
	SerializedSprite2DSys = ecs.registerSystem<SerializedSprite2DSystem>();
	ecs.setSystemComponentRequirement<SerializedSprite2DSystem, spriteCom>();//setting up sprite component

	//register transform system
	SerializedTransform2DSys = ecs.registerSystem<SerializedTransform2DSystem>();
	ecs.setSystemComponentRequirement<SerializedTransform2DSystem, transform2D>();//setting up transform component
	ecs.setSystemComponentRequirement<SerializedTransform2DSystem, spriteCom>();

	//register camera system
	SerializedCamera2DSys = ecs.registerSystem<SerializedCamera2DSystem>();
	ecs.setSystemComponentRequirement<SerializedCamera2DSystem, camera2DCom>();//setting up camera component
	ecs.setSystemComponentRequirement<SerializedCamera2DSystem, transform2D>();//setting up camera component

	//registerring physics system
	physicsSystemsPtr = ecs.registerSystem<physicsSystem>();
	physicsSystemsPtr->init(); //register transform2D and rigidBody2D

	//registerring physics system
	collisionSystemsPtr = ecs.registerSystem<CollisionSystem>();
	collisionSystemsPtr->init(); //register transform2D and AABB

	//registerring controller system
	controllerSystemPtr = ecs.registerSystem<controllerSystem>();
	ecs.setSystemComponentRequirement<controllerSystem, transform2D>();
	ecs.setSystemComponentRequirement<controllerSystem, controller>();
	ecs.setSystemComponentRequirement<controllerSystem, rigidBody2D>();

	//register pathfinding system
	AStarPathFindingSysPtr = ecs.registerSystem<AStarPathFindingSys>();//registering the system 
	ecs.setSystemComponentRequirement<AStarPathFindingSys, transform2D>();//setting up required component
	ecs.setSystemComponentRequirement<AStarPathFindingSys, Com_FindPath>();//setting up required component
	ecs.setSystemComponentRequirement<AStarPathFindingSys, Com_type>();//setting up required component

	//register enemy state system 
	EnemyStateSysPtr = ecs.registerSystem<EnemyStateSys>();//registering the system 
	ecs.setSystemComponentRequirement<EnemyStateSys, transform2D>();//setting up required component
	ecs.setSystemComponentRequirement<EnemyStateSys, Com_Health>();//setting up required component
	ecs.setSystemComponentRequirement<EnemyStateSys, Com_type>();//setting up required component
	ecs.setSystemComponentRequirement<EnemyStateSys, Com_EnemyStates>();//setting up required component

	//for the font 
	FontRenderingSysPtr = ecs.registerSystem<FontRenderingSys>();//registering the system
	ecs.setSystemComponentRequirement<FontRenderingSys, transform2D>();//setting up required component
	ecs.setSystemComponentRequirement<FontRenderingSys, Com_Font>();//setting up required component

	//register GUI system
	GUISysPtr = ecs.registerSystem<GUISys>();//registering the system
	ecs.setSystemComponentRequirement<GUISys, transform2D>();//setting up the required component 
	ecs.setSystemComponentRequirement<GUISys, Com_GUI>();//setting up the required component
	ecs.setSystemComponentRequirement<GUISys, Com_type>();//setting up required component

	//Registering level loader system
	LevelLoaderSysPtr = ecs.registerSystem<LevelLoaderSystem>();
	ecs.setSystemComponentRequirement<LevelLoaderSystem, transform2D>();
	ecs.setSystemComponentRequirement<LevelLoaderSystem, levelLoaderButton>();

	//Registering abilities system 
	AbilitiesSysPtr = ecs.registerSystem<AbilitiesSystem>();
	ecs.setSystemComponentRequirement<AbilitiesSystem,Com_type>();
	ecs.setSystemComponentRequirement<AbilitiesSystem,transform2D>();
	ecs.setSystemComponentRequirement<AbilitiesSystem, Com_Abilities>();
	
	//Registering transform parent system
	TransformParentSysPtr = ecs.registerSystem<TransformParentSystem>();
	ecs.setSystemComponentRequirement<TransformParentSystem, transform2D>();

	//Registering animator system
	AnimationSysPtr = ecs.registerSystem<AnimationSystem>();
	ecs.setSystemComponentRequirement<AnimationSystem, spriteCom>();

	//Registering Logic system
	LogicSystemsPtr = ecs.registerSystem<LogicSystem>();//registering the system
	ecs.setSystemComponentRequirement<LogicSystem, scriptComponent>();
	LogicSystemsPtr->init();

	//Registering enemy spawning system 
	EnemySpawningSysPtr = ecs.registerSystem<EnemySpawningSystem>();
	ecs.setSystemComponentRequirement< EnemySpawningSystem, transform2D>();
	ecs.setSystemComponentRequirement< EnemySpawningSystem, Com_EnemySpawner>();

	//Registering the destructor system 
	TimeDestructorPtr = ecs.registerSystem<TimeDestructor>();
	ecs.setSystemComponentRequirement<TimeDestructor, Com_TimeDestructor>();

	//Registering the dialogue system
	DialogueSysPtr = ecs.registerSystem<DialogueSystem>();
	ecs.setSystemComponentRequirement<DialogueSystem, Com_Dialogue>();
	ecs.setSystemComponentRequirement<DialogueSystem, transform2D>();

	//register component not required in any system	
	//ecs.registerComponentType<AABB>();
	//ecs.registerComponentType<Poly>();

	//script
	//CSharpSystemPtr = ecs.registerSystem<CSharpSystem>();
	////ecs.registerComponentType<>();
	//ecs.setSystemComponentRequirement<CSharpSystem, CSharpScriptComponent>();
	//CSharpSystemPtr->init();
	//ATTEMPT AT OBJECT DESERIALISATION
	
	//Particle System
	particleSystemPtr = ecs.registerSystem<ParticleSystem>();
	particleSystemPtr->init();

	//Registering the dialogue manager 
	DialogueManagerSysPtr = ecs.registerSystem<DialogueManagerSystem>();
	ecs.setSystemComponentRequirement<DialogueManagerSystem, Com_DialogueManager>();
	ecs.setSystemComponentRequirement<DialogueManagerSystem, Com_Font>();
	

	//AStarPathFindingSysPtr->init();//A* path finding init
	//SoundClass::getInstance()->LoadCreateSounds();

	//SoundClass::getInstance()->updatesystem();
}

void imguiTestState::input(float dt)
{
	static_cast<void>(dt);//unreferenced variable

	//updating windows & input
	WindowsManager.Update(dt);
	m_CategoryStopWatch.LapTimer("WindowsManager");
	InputManager.update(dt);
	m_CategoryStopWatch.LapTimer("InputManager");
	ControllerSystem->inputUpdate(dt);
	m_CategoryStopWatch.LapTimer("ControllerSystem");
	//Debug key
	if (KeyManager.isKeyPressed('Y', true) && KeyManager.isKeyPressed(VK_CONTROL,true))
	{
		PhysicsSystem->setDebugPhysics(false);
	}
	else if (KeyManager.isKeyPressed('U', true) && KeyManager.isKeyPressed(VK_CONTROL, true))
	{
		PhysicsSystem->setDebugPhysics(true);
	}

	//if the scene has been paused 
	if (Editor::GetInstance().isPlaying == true) {
		if (KeyManager.isKeyTriggered(VK_ESCAPE, true) && UiLayer::GetInstance().isGamePlayLevel == true) {
			UiLayer::GetInstance().m_SceneState = UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause ? UiLayer::SceneState::Play : UiLayer::SceneState::Pause;//setting the scene state to pause 
		}
	}
}

float timeCounter = 0.0f;
bool switched = false;

void imguiTestState::update(float dt)
{
	//Required in order for the audio architecture to not be overwhelmed.
	//Not having this would mean all non-virtual channels will eventually be
	//taken up and audio will not play.
	audio.fmodSystem->update();

	//Set volume as needed
	if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play)
	{
		audio.chBGM->setPaused(false);
		audio.chBGM->setVolume(audio.volumeBGM * (float)audio.unmuteBGM);
	}

	particleSystemPtr->update(dt); // Testing for now
	
	if (WindowsManager.getIsWindowActive() && WindowsManager.getIsMinimised() == false)
	{
		//Pause BGM as needed
		if (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Pause && !UiLayer::GetInstance().dialoguePause)
		{
			//pause if active and 'pause'
			audio.chBGM->setPaused(true);
		}
		else
		{
			//play normally when active
			audio.chBGM->setPaused(false);
		}
	}
	else
	{
		//Paused due to inactive
		audio.chBGM->setPaused(true);
	}

	//Handle Fullscreen via F11
	if (KeyManager.isKeyReleased(VK_F11)) {
		if (WindowsManager.getFullscreenState()) {
			WindowsManager.setFullscreenState(false);
		}
		else {
			WindowsManager.setFullscreenState(true);
		}
	}
	static_cast<void>(dt);//unreferenced variable

	//Pause while playing
	if((KeyManager.isKeyTriggered('P')) && (UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play))
	{
		//disables the sound
		//SoundClass::getInstance()->Stop(background);
		//SoundClass::getInstance()->Pause(WALKING);
		//SoundClass::getInstance()->Pause(BACKGROUND);
		//SoundClass::getInstance()->Pause(SUCTION);
		//SoundClass::getInstance()->Pause(ENDGAME);

		UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//setting the scene state to edit 
	}

	//Alt-Tab to pause while playing
	if((KeyManager.isKeyPressed(LLKHF_ALTDOWN) && KeyManager.isKeyPressed(VK_TAB))&&(UiLayer::GetInstance().m_SceneState== UiLayer::SceneState::Play))
	{
		//disables the sound
		//SoundClass::getInstance()->Pause(background);
		//SoundClass::getInstance()->Pause(WALKING);
		//SoundClass::getInstance()->Pause(BACKGROUND);
		//SoundClass::getInstance()->Pause(SUCTION);
		//SoundClass::getInstance()->Pause(ENDGAME);

		UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//setting the scene state to pause
	}

	if ((KeyManager.isKeyPressed(LLKHF_ALTDOWN) &&
		KeyManager.isKeyPressed(VK_ESCAPE) &&
		KeyManager.isKeyPressed(VK_DELETE)) &&
		(UiLayer::GetInstance().m_SceneState == UiLayer::SceneState::Play))
	{
		//disables the sound
		//SoundClass::getInstance()->Pause(background);
		//SoundClass::getInstance()->Pause(WALKING);
		//SoundClass::getInstance()->Pause(BACKGROUND);
		//SoundClass::getInstance()->Pause(SUCTION);
		//SoundClass::getInstance()->Pause(ENDGAME);

		UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Pause;//setting the scene state to edit 
	}

	/**********
	Cheat Codes 
	***********/
	if (KeyManager.isKeyTriggered('U'))
	{
		//cap at 80 for each 
		if (AssetManager::GetInstance().tomatoesCollected <= 80 && AssetManager::GetInstance().mushroomsCollected <= 80 && AssetManager::GetInstance().chilliesCollected <= 80) {
			AssetManager::GetInstance().tomatoesCollected += 5;
			AssetManager::GetInstance().mushroomsCollected += 5;
			AssetManager::GetInstance().chilliesCollected += 5;
		}
	}


	//Finish Testing condition
	if (KeyManager.isKeyReleased(VK_ESCAPE) && KeyManager.isKeyPressed(VK_SHIFT, true)) {
		//SoundClass::getInstance()->Pause(background);
		//disables the sound
		//SoundClass::getInstance()->Pause(WALKING);
		//SoundClass::getInstance()->Pause(SUCTION);
		//SoundClass::getInstance()->Pause(ENDGAME);
		if (Editor::GetInstance().isPlaying == false) {
			//gameStateManager::quitGame();//quit game
			return;
		}
		else {
			UiLayer::GetInstance().m_SceneState = UiLayer::SceneState::Edit;//setting the scene state to edit 
			Editor::GetInstance().isPlaying = false;//is playing false 
			ecs.disableEntityIDReuse = false;
			//Reload temporary level
			ecs.loadLevel("Resources/TemporaryLevel.json");
			//Delete temporary level
			std::filesystem::remove("Resources/TemporaryLevel.json");

			//Stop ongoing sounds
			audio.chBGM->stop();
			audio.chSFX->stop();
		}
	}
	//exit condition
	if (Editor::GetInstance().isPlaying == true && UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Pause) {
		physicsSystemsPtr->update(dt);
		m_CategoryStopWatch.LapTimer("physicsSystemsPtr");
		AbilitiesSysPtr->update(dt);//updating the abilities
		EnemySpawningSysPtr->update(dt);//updating the enemy spawner 
	
	}
	SerializedCamera2DSys->update(dt);//camera update
	m_CategoryStopWatch.LapTimer("SerializedCamera2DSys");

	SerializedTransform2DSys->update(dt);//transform update
	m_CategoryStopWatch.LapTimer("SerializedTransform2DSys");

	SerializedSprite2DSys->update(dt);//sprite update
	m_CategoryStopWatch.LapTimer("SerializedSprite2DSys");


	if (Editor::GetInstance().isPlaying == false) {
		EditorCamera::GetInstance().OnUpdate(dt);//update editor camera
		m_CategoryStopWatch.LapTimer("editor");
	}
		
	if (Editor::GetInstance().isPlaying == true) {
		
		LogicSystemsPtr->update(dt);

		//particleSystemPtr->update(dt); // need to recheck if this is the correct place to add
	}
	else
	{
		if (physicsSystemsPtr->getDebugPhysics())
			physicsSystemsPtr->drawPhysicDebuggingEditorMode();
	}

	TransformParentSysPtr->update();
	m_CategoryStopWatch.LapTimer("Parent Transform");


	if (Editor::GetInstance().isPlaying == true) {
		DialogueSysPtr->update(dt);//dialogue update 
		DialogueManagerSysPtr->update(dt);//dialogue manager update
	}
	if (Editor::GetInstance().isPlaying == true && UiLayer::GetInstance().m_SceneState != UiLayer::SceneState::Pause) {
		GUISysPtr->update(dt);//GUI system update 

		//AStarPathFindingSysPtr->updateNodes();//update path finding nodes
		//AStarPathFindingSysPtr->update(dt);//path finding update 
		m_CategoryStopWatch.LapTimer("path finding");

		EnemyStateSysPtr->update(dt);//enemy state update 
		m_CategoryStopWatch.LapTimer("enemy");

		LevelLoaderSysPtr->update(dt); //Level Loader update
		m_CategoryStopWatch.LapTimer("Level Loading Button");

		TimeDestructorPtr->update(dt);//update the timing destructor 

		AnimationSysPtr->update(dt); //Update animator
		m_CategoryStopWatch.LapTimer("Animations");
	}


	//for the drawing of the grid map for A* path finding 
	if (KeyManager.isKeyTriggered('M', true) && KeyManager.isKeyPressed(VK_CONTROL,true)) {
		PathFindingGrid::GetInstance().debugState = PathFindingGrid::GetInstance().debugState ? false : true;//set the debug state 
	}
	if (PathFindingGrid::GetInstance().debugState == true) {
		PathFindingGrid::GetInstance().drawGridMap();//drawing of the gridmap
	}

	//PathFindingGrid::GetInstance().drawGridMap();//drawing of the gridmap
	PathFindingGrid::GetInstance().resetGridMap();//reseting the gridmap

	//CSharpSystemPtr->update(dt);

	//SoundClass::getInstance()->updatesystem();
	
	
	//(1 Jan) ECS Cleanup. Must be called at the end of every loop
	ecs.cleanupECS();

	//EXE VERSION
	/*if (!gameLoaded)
	{
		//Disable entity reuse so that certain bugs won't appear
		ecs.disableEntityIDReuse = true;

		//Automatically load the main menu level
		ecs.loadLevel("Resources/SavedLevels/digipenlogoLevel.json");

		//FORCE GAMEPLAY
		Editor::GetInstance().isPlaying = true; //set scene bool to play      
		//resize the post processing frame 
		PostProcessing::GetInstance().m_PostProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
		PostProcessing::GetInstance().m_PostProcessingFrame->Bind();//bind 
		PostProcessing::GetInstance().m_PostProcessingFrame->UnBind();//bind 
		//resize the pre processing frame 
		PostProcessing::GetInstance().m_PreProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
		PostProcessing::GetInstance().m_PreProcessingFrame->Bind();//bind 
		PostProcessing::GetInstance().m_PreProcessingFrame->UnBind();//bind 
		gameLoaded = true;
	}
	else
	{
		//Force to quit the game upon ending
		if (Editor::GetInstance().isPlaying == false)
			gameStateManager::quitGame();
	}*/
}

void imguiTestState::render(float dt)
{
	PostProcessing::GetInstance().bindPreProcess();//binding the post processing 
	SerializedRenderer2DSys->update(dt);//renderer update
	PostProcessing::GetInstance().unBindPreProessing();//binding the post processing
	if (Editor::GetInstance().isPlaying == false) {
		PostProcessing::GetInstance().bindPostProcessing();//binding the post processing 
		PostProcessing::GetInstance().drawPostProcessing();//draw the scene out
		PostProcessing::GetInstance().unBindPostProcessing();//unbind the post processing 
	}
	if (Editor::GetInstance().isPlaying == true) {
		PostProcessing::GetInstance().drawPostProcessing();//draw the scene out
	}	
	// Swap buffers
	SwapBuffers(WindowsManager.getWindowDC());

	//****IMGUI RENDER*****//
	if (Editor::GetInstance().isPlaying == false) {
		Editor::GetInstance().Begin();//begin imgui render
		Editor::GetInstance().OnImguiRender(dt); //rendering for the imgui
		Editor::GetInstance().End();//end imgui render
	}
	m_CategoryStopWatch.LapTimer("Imgui render");
	m_PreviousCategoryStopWatch = m_CategoryStopWatch;
}

void imguiTestState::free()
{
	ecs.resetECS(); //Clear ECS
//	ass.cleanchannels();
	//SoundClass::getInstance()->Release();

}

void imguiTestState::unload()
{

}

SceneMetrics imguiTestState::GetMetrics()
{
	SceneMetrics metrics;
	metrics.systemExecutionTimings = m_PreviousCategoryStopWatch; // Always using the last frame's StopWatch
	return metrics;
}