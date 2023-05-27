// /******************************************************************************/
// /*!
// \file         UiLayer.h 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         13th April, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declaration for the UILayer.h
// \lines        19 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef UILAYER
#define UILAYER

#include "../../Dependencies/Vendor/imgui/imgui.h"
#include "../../WinMgr/WinMgr.h"
#include "../../Dependencies/Vendor/imgui/imgui_impl_win32.h"
#include "../../Dependencies/Vendor/imgui/imgui_impl_opengl3.h"
#include "../../../Graphics/Source/GraphicsApplication.h" //for all the graphics 
#include "../../EditorStateManager/gameStateManager.h"// game manager
#include <filesystem>//file system 

class UiLayer {

	
public:
	~UiLayer() = default;
	UiLayer() = default;
	UiLayer(const UiLayer&) = delete;
	UiLayer& operator=(const UiLayer&) = delete;

	static UiLayer& GetInstance();//singleton getting instance

	//For displaying hierarchy entries recursively
	using entityID = uint32_t; //4 294 967 296 entities. More than enough.
	void displayHierarchyEntry(entityID e, std::map<entityID, std::string> const& names);

	//layers to DO: 
	void LoadDockSpace(bool* p_open); //load the dock space 
	void LoadMemory(bool* p_open); //load the memory manager
	void LoadSceneEditor(bool* p_open);//load the scene editor 
	void ShowObjects(bool* p_open);//UI manager 
	void LoadConsole(bool* p_open);//load console for asset, scripting & animation
	void ShowHierarchy(bool* p_open); //Hierarchy: displays list of entities
	void ShowInspector(bool* p_open); //Inspector: displays details of selected entity
	void LoadPlayPanel(bool* p_open);//loading of the play button panel
	void LoadPerFormancePanel(bool* p_open);
	void ShowAnimationEditorPanel(bool* p_open);//opens the animation editor 
	void ShowSaveAnimationDialog(bool* p_open);//opens the animation editor 
	void DrawPanel(bool* p_open);
	//void LoadPerFormancePanel(bool* p_open);
	enum class SceneState {
		Edit = 0, Play = 1, Pause = 2
	};

	bool isGamePlayLevel = false;//indicates if the game is in gameplay level 

	//tracks the state of the instructions in main menu
	enum class InstructionsState {
		Instruction1 = 0, Instruction2 = 1, InstructionsOff = 2 , Instruction3 = 3
	};

	//tracks the state of the level 
	enum class LevelState {
		level1 = 0, level2 = 1
	};

	bool dialoguePause = false;//tracks the dialogue pause 
	bool pauseQuitConfirmation = false;//tracks the pause quit confirmation page 
	bool cameraTransition = false;//tracks if there is a camera transition 

	//for guizmo
	void ShowGuizmo();//guizmo 

	//for the scene 
	void onScenePlay();
	void onSceneStop();

	//for file tools
	void NewScene();

	bool openLevelDialogOpen{ false };
	void ShowOpenLevelDialog(bool* p_open);
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);

	bool saveLevelDialogOpen{ false };
	void ShowSaveLevelDialog(bool *p_open);
	void SaveSceneAs();

	bool loadEntityDialogOpen{ false };
	void ShowLoadEntityDialog(bool* p_open);
	//sound
	void DrawSoundPanel(bool* p_open);

	LevelState m_currentLevelState = LevelState::level1;//starts from level 1 
	SceneState m_SceneState = SceneState::Edit;//set starting as edit state 
	InstructionsState m_InstructionsState = InstructionsState::InstructionsOff;//set starting state of the instructions toggle 

	bool saveNewAnimationOpen = false;
	bool animationPanelToggle = false;//tracks the animation panel toggle
	bool animationPanelFirstToggle = true;//tracks the first time the animation panel is toggled
	size_t totalNumFrames = 0;//tracks the total number of frames present in the animation editor 

	//redo undo 
	struct sUndoRedo {
		entityID id;
		std::string component;
		std::string variable;
		float oldVal;
		float newVal;
		sUndoRedo() = default;
		sUndoRedo(entityID& pId,const std::string& pComponent,const std::string& pVariable, float& poldVal, float& pnewVal) : id(pId) , component(pComponent) , variable(pVariable) , oldVal(poldVal) , newVal(pnewVal){}
	};
	void editorUndo();
	void editorRedo();
	void incrementIndex();
	void decrementIndex();
	void recordAction(entityID& pId, const std::string& pComponent, const std::string& pVariable, float& poldVal, float& pnewVal);
	int undoRedoIndex = -1; //tracking the current undo redo -1 means no index 
	std::vector<sUndoRedo> undoRedoVec;//vector that stores all the undo and redos 

private:
	CategoryStopWatch systemExecutionTimings;
	SceneMetrics m_Metrics{};
	std::vector<float> m_DeltaTimeList = { 0 };
	
	

};

#endif // !UILAYERf

