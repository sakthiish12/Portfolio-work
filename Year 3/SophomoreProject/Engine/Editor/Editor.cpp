// /******************************************************************************/
// /*!
// \file         Editor.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "Editor.h"
#include "UI Layer/UiLayer.h"//UI layer
#include "../../Dependencies/ficons/fa.h"//f icons
#include "../Mono/Mono.h"
#include"../InputMgr/InputMgr.h"
#include "../Dependencies/Gizmo/ImGuizmo.h"//imguizmo
#include "../ECS/ECS.h"//ecs



/******************************
* @Brief - this function intialize the dear imgui
*******************************/
void Editor::initailize_editor() {
    //TextEditor editor;
    //Mono::GetInstanced();
    ImGui::CreateContext();// Setup ImGui binding

    //imgui input/output
    ImGuiIO& IO = ImGui::GetIO();
	IO.WantCaptureMouse |= true;
	IO.WantCaptureKeyboard |= true;
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
	IO.ConfigFlags |= ImGuiDockNodeFlags_NoResize;

    //set back end flags
    IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; //setting mouse cursors
    IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; //setting the mouse position


    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;
    static bool unsaved_document = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

	//IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls
	IO.DisplaySize.x = static_cast<float>(WindowsManager.getWidth());
	IO.DisplaySize.y = static_cast<float>(WindowsManager.getHeight());

    IO.FontDefault = IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Regular.ttf", 15.f);  //setting the default font

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    IO.Fonts->AddFontFromFileTTF("Resources/Fonts/FA-Solid-900.otf", 16.0f, &icons_config, icons_ranges);


    // use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid
    //for the fonts of the imgui
    //IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf", 14.0f);
    //IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf", 11.5f);

    ImGui_ImplWin32_Init(WindowsManager.getHWnd());
    ImGui_ImplOpenGL3_Init("#version 430");

    ImGui::StyleColorsDark();//setting the imgui to dark mode

}

/********************************
* @Brief - this function delete the context of the dear imgui & clears
*******************************/
void Editor::clear_editor() {

    ImGui_ImplWin32_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();

    ImGui::DestroyContext();//destroy imgui context
}


/************************************
* @Brief - this function renders the imgui
*************************************/
void Editor::OnImguiRender(float dt) {
    static_cast<void>(dt);// unreferenced varaible

    //toggle
    static bool dockToggle = true;
    static bool memoryToggle = true;
    static bool sceneEditorToggle = true;
    static bool consoleToggle = true;
    static bool hierarchyToggle = true;
    static bool inspectorToggle = true;
    static bool playToggle = true;
    static bool logToggle = true;
    static bool performanceToggle = true;
    static bool SoundToggle = false;
    static bool animationToggle = true;

    if (!Editor::GetInstance().isPlaying)
    {
        //ctrl + n for new scene
        if (KeyManager.isKeyTriggered('n', true) && KeyManager.isKeyPressed(VK_CONTROL, true))
        {
            UiLayer::GetInstance().NewScene();
        }

        //ctrl + o for loading scene
        if (KeyManager.isKeyTriggered('o', true) && KeyManager.isKeyPressed(VK_CONTROL, true))
        {
            UiLayer::GetInstance().OpenScene();
        }

        //ctrl + s for save scene 
        if (KeyManager.isKeyTriggered('s', true) && KeyManager.isKeyPressed(VK_CONTROL, true)) {
            UiLayer::GetInstance().SaveSceneAs();//open up save scene 
        }

        //undo 
        if (KeyManager.isKeyTriggered('Z') && KeyManager.isKeyPressed(VK_CONTROL, true) && UiLayer::GetInstance().undoRedoIndex != -1) {
            UiLayer::GetInstance().editorUndo();//undo the latest 
            UiLayer::GetInstance().decrementIndex();//decrement the index 
        }

        //redo 
        if (KeyManager.isKeyTriggered('Y') && KeyManager.isKeyPressed(VK_CONTROL, true) &&  UiLayer::GetInstance().undoRedoVec.size() != 0 && (UiLayer::GetInstance().undoRedoIndex + 1 != UiLayer::GetInstance().undoRedoVec.size())) {
            //if it's not empty and if the index is not at the max 
            UiLayer::GetInstance().incrementIndex();//increment index
            UiLayer::GetInstance().editorRedo();//redo the latest 
        }

        //ctrl + f for foucs
        if (KeyManager.isKeyTriggered('f', true) && KeyManager.isKeyPressed(VK_CONTROL, true)) {
            isFocus = !isFocus;
        }

        //for IMGUIZMO operations toggle 
        if (KeyManager.isKeyTriggered('q', true)) {
            m_IMGUIZMO_Type = ImGuizmo::OPERATION::TRANSLATE;
        }
        else if (KeyManager.isKeyTriggered('w', true)) {
            m_IMGUIZMO_Type = ImGuizmo::OPERATION::SCALE;
        }
        else if (KeyManager.isKeyTriggered('e', true)) {
            m_IMGUIZMO_Type = ImGuizmo::OPERATION::ROTATE;
        }
    }

#if 1
    //static bool t = true;
    //ImGui::ShowDemoWindow(&t);
#endif

    //if it's not playing, show the editor 
    if (Editor::GetInstance().isPlaying == false) {
        //******DOCK SPACE*********//
        UiLayer::GetInstance().LoadDockSpace(&dockToggle);
        //*****ASSET MANAGER, SCRIPTING & ANIMATION*********//
        UiLayer::GetInstance().LoadConsole(&consoleToggle);
        //******SCENE EDITOR*********//
        UiLayer::GetInstance().LoadSceneEditor(&sceneEditorToggle);
        //******MEMORY MANAGER & INFO*********//
        UiLayer::GetInstance().LoadMemory(&memoryToggle);
        //*****PLAY AND STOP BUTTON PANEL*********//
        UiLayer::GetInstance().LoadPlayPanel(&playToggle);        
        //*****ANIMATION EDITOR PANEL*********//
        if (UiLayer::GetInstance().animationPanelToggle == true) {
            UiLayer::GetInstance().ShowAnimationEditorPanel(&animationToggle);
        }

        //HIERARCHY
        UiLayer::GetInstance().ShowHierarchy(&hierarchyToggle);

        //INSPECTOR
        UiLayer::GetInstance().ShowInspector(&inspectorToggle);

        //LOGGER
        UiLayer::GetInstance().DrawPanel(&logToggle);

        //PERFORMANCE
        UiLayer::GetInstance().LoadPerFormancePanel(&performanceToggle);

        //UiLayer::GetInstance().DrawSoundPanel(&SoundToggle);

        //SAVE LEVEL DIALOG
        if (UiLayer::GetInstance().saveLevelDialogOpen)
        {
            UiLayer::GetInstance().ShowSaveLevelDialog(&(UiLayer::GetInstance().saveLevelDialogOpen));
        }

        //LOAD LEVEL DIALOG
        if (UiLayer::GetInstance().openLevelDialogOpen)
        {
            UiLayer::GetInstance().ShowOpenLevelDialog(&(UiLayer::GetInstance().openLevelDialogOpen));
        }

        //LOAD ENTITY DIALOG
        if (UiLayer::GetInstance().loadEntityDialogOpen)
        {
            UiLayer::GetInstance().ShowLoadEntityDialog(&(UiLayer::GetInstance().loadEntityDialogOpen));
        }
    }

    /*if (KeyManager.isKeyPressed('a',true))
    {
        Mono::GetInstanced();
    }*/
    //*this must be after all the stuff is done*//
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

/****************************************
* @Brief - imgui begin frame
****************************************/
void Editor::Begin() {

    //renderer abstraction for clearing the buffer
    //Renderer::GetInstance().Clear();

    //getting imgui io
    ImGuiIO& IO = ImGui::GetIO();
    IO.DisplaySize = ImVec2(static_cast<float>(WindowsManager.getWidth()), static_cast<float>(WindowsManager.getHeight()));

    //new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    //guizmo start frame
    ImGuizmo::BeginFrame();

}

/*****************************************
@Brief - imgui end frame
*******************************************/
void Editor::End() {
    ImGui::EndFrame();
    //ImGui::UpdatePlatformWindows();

    //GLCall(SwapBuffers(WindowsManager.getWindowDC()));
}
