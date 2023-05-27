// /******************************************************************************/
// /*!
// \file         UiLayer.cpp 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         13th April, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition of the UI layer 
// \lines        188 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"
#include "../../Dependencies/ficons/fa.h"//f icons 
#include "../../Dependencies/Vendor/imgui/imconfig.h"//config 

#include "../../Math/Vector/Vector3.h"
#include <imgui/imgui_memory_editor.h>

#include <imgui/imgui_widget_flamegraph.h>

#include "imgui/imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui/imgui_internal.h"
#include "../../AssetManager/AssetManager.h"//asset manager 

#include "../../ECS/ECS.h"
#include "../Editor.h"//editor 
#include "../../Graphics/Source/PostProcessing.h"//post processing 


   static MemoryEditor mem_edit_1;
   static char data[0x10000];
   size_t data_size = 0x10000;
  // mem_edit_1.DrawWindow("Memory Editor", data, data_size);

UiLayer& UiLayer::GetInstance() {
	static UiLayer instance;
	return instance; 
}


void UiLayer::ShowObjects(bool* p_open) {
    static_cast<void>(p_open);//unreferenced variable 
	//doing up the imgui for the show objects 
}


void UiLayer::LoadDockSpace(bool* p_open) {
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
   // PartyAssert(false, "loggedMessage");
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    //for the full screen viewport & border size 
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", p_open, window_flags);//imgui begin 
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        //file menu bar 
        //For actual keybinding, refer to implementation in Editor/Editor.cpp
        //The shortcuts are not binded here. Here is to only display the shortcuts
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem(ICON_FA_FILE " New Scene", "Ctrl+N"))
            {
                NewScene();
            }
            if (ImGui::MenuItem(ICON_FA_FILE_ALT " Load Scene", "Ctrl+O")){ 
                OpenScene();//opens the scene 
            }
            if (ImGui::MenuItem(ICON_FA_SAVE " Save Scene", "Ctrl+S")) {
                SaveSceneAs();//save the scene 
            }
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_FILE " Open File Explorer", "")) {
                WindowsManager.openWindowExplorer(".\\Config");//opens the file in the window explorer 
            }
            if (ImGui::MenuItem(ICON_FA_SYNC " Reload Textures")) {
                AssetManager::GetInstance().LoadSprites();//loading of the textures
            }
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT " Exit","ECS", false, p_open != NULL)) {
                //Warn before quitting
                int decision = MessageBox(WindowsManager.getHWnd(), L"Progress is not automatically saved upon quitting. Unsaved progress will be lost. Quit?", L"Confirm Quit", MB_OKCANCEL | MB_ICONWARNING);
                if (decision == IDOK)
                    gameStateManager::quitGame();
                else
                    EndDialog(WindowsManager.getHWnd(), decision);//if exit, quit game 
            }
            ImGui::EndMenu();
        }
        //edit menu bar 
        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem(ICON_FA_UNDO " Undo", "Ctrl+Z",false,UiLayer::GetInstance().undoRedoIndex != -1)) {
                UiLayer::GetInstance().editorUndo();//undo the latest 
                UiLayer::GetInstance().decrementIndex();//decrement the index 
            }
            if (ImGui::MenuItem(ICON_FA_REDO " Redo", "Ctrl+Y",false, UiLayer::GetInstance().undoRedoVec.size() != 0 && (UiLayer::GetInstance().undoRedoIndex+1 != UiLayer::GetInstance().undoRedoVec.size()))){
                //if it's not empty and if the index is not at the max 
                UiLayer::GetInstance().incrementIndex();//increment index
                UiLayer::GetInstance().editorRedo();//redo the latest 
            }
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_CUT " Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem(ICON_FA_CLONE " Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem(ICON_FA_PASTE " Paste", "Ctrl+V")) {}
            if (ImGui::MenuItem(ICON_FA_TRASH_ALT " Delete", "Del")) {}
            if (ImGui::MenuItem(ICON_FA_MOUSE_POINTER " Select All", "Ctrl+A")) {}
            ImGui::EndMenu();
        }
        //scene menu bar 
        if (ImGui::BeginMenu("Scene")) {

            if (ImGui::MenuItem(ICON_FA_CAMERA_RETRO " Focus Selected", "CTRL+F", &(Editor::GetInstance().isFocus))) { 
                //Clicking on the menu item already toggles isFocus
                //Bottom code will render clicking the toggle redundant
                //Editor::GetInstance().isFocus = !Editor::GetInstance().isFocus;
            }

            //post processing effects for the game 
            if(ImGui::BeginMenu("Effects")) {
                if (ImGui::MenuItem(ICON_FA_EYE " No Effects", "")) { PostProcessing::GetInstance().toggleEffects = true; PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectNormal; }//changing back to normal effects
                if (ImGui::MenuItem(ICON_FA_EYE " Grey Scale Effect", "")) { PostProcessing::GetInstance().toggleEffects = true; PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectGreyScale;}//changing to the grey scale 
                if (ImGui::MenuItem(ICON_FA_EYE " Blur Effect", "")) {PostProcessing::GetInstance().toggleEffects = true; PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectBlur;}//changing to the blur scale 
                if (ImGui::MenuItem(ICON_FA_EYE " Inverse Effect", "")) {PostProcessing::GetInstance().toggleEffects = true; PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectInverse;}//changing to the inverse scale 
                if (ImGui::MenuItem(ICON_FA_EYE " Inversion Effect", "")) {PostProcessing::GetInstance().toggleEffects = true; PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectInversion;}//changing to the inversion scale 
                if (ImGui::MenuItem(ICON_FA_EYE " Shake Effect", "")) {PostProcessing::GetInstance().toggleEffects = true; PostProcessing::GetInstance().currentEffects = PostProcessing::Effects::effectShake;}//changing to the shake scale 
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        //settings menu bar 
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::MenuItem(ICON_FA_PALETTE " Theme")) {}
            if (ImGui::MenuItem(ICON_FA_EDIT " Animation Editor", NULL, (UiLayer::GetInstance().animationPanelToggle))) {
                //if the animation panel is not toggled yet 
                if (UiLayer::GetInstance().animationPanelToggle == false) {
                    int decision;
                    decision = MessageBox(WindowsManager.getHWnd(), L"Progress is not automatically saved switching to animation editor. Unsaved progress will be lost. Go to Animation Editor?", L"Confirm Go to Animation Editor", MB_OKCANCEL | MB_ICONWARNING);
                    if (decision == IDOK) {
                        ecs.clearAllEntities();
                        UiLayer::GetInstance().animationPanelToggle = true;
                    }
                    else {
                        EndDialog(WindowsManager.getHWnd(), decision);
                    }
                }
                else {
                    int decision;
                    decision = MessageBox(WindowsManager.getHWnd(), L"Progress is not automatically saved switching back to engine. Unsaved progress will be lost. Go to Engine Editor?", L"Confirm Go to ENGINE EDITOR", MB_OKCANCEL | MB_ICONWARNING);
                    if (decision == IDOK) {
                        //animation is already toggled 
                        UiLayer::GetInstance().animationPanelToggle = false;
                        UiLayer::GetInstance().totalNumFrames = 0;//reset the number of frames drawn 
                        UiLayer::GetInstance().animationPanelFirstToggle = true;//reset 
                        ecs.clearAllEntities();
                    }
                    else {
                        EndDialog(WindowsManager.getHWnd(), decision);
                    }
                }
            }
            ImGui::EndMenu();
        }
        //help menu bar 
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem(ICON_FA_INFO_CIRCLE " Help", "Ctrl+H")) {}
            if (ImGui::MenuItem(ICON_FA_QUESTION_CIRCLE " About")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar(); //end of the menu bar 
    }

    ImGui::End(); //imgui end 
}


void ImGuiWidgetFlameGraph::PlotFlame(const char* label, void (*values_getter)(float* start, float* end, ImU8* level, const char** caption, const void* data, int idx), const void* data2, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    static_cast<void>(data2);//unreferenced variable 
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    // Find the maximum depth
    ImU8 maxDepth = 0;
    for (int i = values_offset; i < values_count; ++i)
    {
        ImU8 depth;
        values_getter(nullptr, nullptr, &depth, nullptr, data, i);
        maxDepth = ImMax(maxDepth, depth);
    }

    const auto blockHeight = ImGui::GetTextLineHeight() + (style.FramePadding.y * 2);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    if (graph_size.x == 0.0f)
        graph_size.x = ImGui::CalcItemWidth();
    if (graph_size.y == 0.0f)
        graph_size.y = label_size.y + (style.FramePadding.y * 3) + blockHeight * (maxDepth + 1);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + graph_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0, &frame_bb))
        return;

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = values_offset; i < values_count; i++)
        {
            float v_start, v_end;
            values_getter(&v_start, &v_end, nullptr, nullptr, data, i);
            if (v_start == v_start) // Check non-NaN values
                v_min = ImMin(v_min, v_start);
            if (v_end == v_end) // Check non-NaN values
                v_max = ImMax(v_max, v_end);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    bool any_hovered = false;
    if (values_count - values_offset >= 1)
    {
        const ImU32 col_base = ImGui::GetColorU32(ImGuiCol_PlotHistogram) & 0x77FFFFFF;
        const ImU32 col_hovered = ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered) & 0x77FFFFFF;
        const ImU32 col_outline_base = ImGui::GetColorU32(ImGuiCol_PlotHistogram) & 0x7FFFFFFF;
        const ImU32 col_outline_hovered = ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered) & 0x7FFFFFFF;

        for (int i = values_offset; i < values_count; ++i)
        {
            float stageStart, stageEnd;
            ImU8 depth;
            const char* caption;

            values_getter(&stageStart, &stageEnd, &depth, &caption, data, i);

            auto duration = scale_max - scale_min;
            if (duration == 0)
            {
                return;
            }

            auto start = stageStart - scale_min;
            auto end = stageEnd - scale_min;

            float startX = static_cast<float>(start / (double)duration);
            float endX = static_cast<float>(end / (double)duration);

            float width = inner_bb.Max.x - inner_bb.Min.x;
            float height = blockHeight * (maxDepth - depth + 1) - style.FramePadding.y;

            auto pos0 = inner_bb.Min + ImVec2(startX * width, height);
            auto pos1 = inner_bb.Min + ImVec2(endX * width, height + blockHeight);

            bool v_hovered = false;
            if (ImGui::IsMouseHoveringRect(pos0, pos1))
            {
                ImGui::SetTooltip("%s: %8.4g", caption, static_cast<double>(stageEnd) - static_cast<double>(stageStart));
                v_hovered = true;
                any_hovered = v_hovered;
            }

            window->DrawList->AddRectFilled(pos0, pos1, v_hovered ? col_hovered : col_base);
            window->DrawList->AddRect(pos0, pos1, v_hovered ? col_outline_hovered : col_outline_base);
            auto textSize = ImGui::CalcTextSize(caption);
            auto boxSize = (pos1 - pos0);
            auto textOffset = ImVec2(0.0f, 0.0f);
            if (textSize.x < boxSize.x)
            {
                textOffset = ImVec2(0.5f, 0.5f) * (boxSize - textSize);
                ImGui::RenderText(pos0 + textOffset, caption);
            }
        }

        // Text overlay
        if (overlay_text)
            ImGui::RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

        if (label_size.x > 0.0f)
            ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
    }

    if (!any_hovered && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Total: %8.4g", static_cast<double>(scale_max) - static_cast<double>(scale_min));
    }
}


void UiLayer::LoadMemory(bool* p_open) {
    static_cast<void>(p_open);//unreferenced variable 
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Info");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    // ImGui::Checkbox("Another Window", &show_another_window);

    static MemoryEditor mem_edit;
    mem_edit.DrawWindow("Memory Editor", data, data_size);
    mem_edit_1.DrawWindow("Memory Editor", data, data_size);
    //mem_edit_2.DrawContents(this, sizeof(*this), (size_t)this);
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    
}


void UiLayer::onScenePlay() {
    m_SceneState = SceneState::Play;//set to play
}

void UiLayer::onSceneStop() {
    m_SceneState = SceneState::Edit;//set to edit 
}

void UiLayer::NewScene()
{
    int decision;
    decision = MessageBox(WindowsManager.getHWnd(), L"Progress is not automatically saved upon loading a new scene. Unsaved progress will be lost. Make new scene?", L"Confirm New Scene", MB_OKCANCEL | MB_ICONWARNING);
    if (decision == IDOK)
        ecs.clearAllEntities();
    else
        EndDialog(WindowsManager.getHWnd(), decision);
}

void UiLayer::OpenScene()
{
    //std::string filepath = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0");
    //if (!filepath.empty())
    //    OpenScene(filepath);

    ImGui::SetWindowPos("Open Level", ImVec2{ WindowsManager.getWidthF() / 4.0f, WindowsManager.getHeightF() / 4.0f });
    ImGui::SetWindowSize("Open Level", ImVec2{ WindowsManager.getWidthF() / 2.0f, WindowsManager.getHeightF() / 2.0f });
    openLevelDialogOpen = true;
}

void UiLayer::OpenScene(const std::filesystem::path& path)
{
    UNREFERENCED_PARAMETER(path);
    //if (path.extension().string() != ".hazel")
    //{
    //    HZ_WARN("Could not load {0} - not a scene file", path.filename().string());
    //    return;
    //}

    //Ref<Scene> newScene = CreateRef<Scene>();
    //SceneSerializer serializer(newScene);
    //if (serializer.Deserialize(path.string()))
    //{
    //    m_ActiveScene = newScene;
    //    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    //    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    //}
}

void UiLayer::SaveSceneAs()
{
    //std::string filepath = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0");
    //if (!filepath.empty())
    //{
    //    SceneSerializer serializer(m_ActiveScene);
    //    serializer.Serialize(filepath);
    //}

    ImGui::SetWindowPos("Save Level", ImVec2{ WindowsManager.getWidthF() / 4.0f, WindowsManager.getHeightF() / 4.0f });
    ImGui::SetWindowSize("Save Level", ImVec2{ WindowsManager.getWidthF() / 2.0f, WindowsManager.getHeightF() / 2.0f });
    saveLevelDialogOpen = true;
}

void UiLayer::editorUndo() {
    //undo transform 
    if(undoRedoVec[undoRedoIndex].component == "transform"){
        auto& eTransform = ecs.getComponent<transform2D>(undoRedoVec[undoRedoIndex].id);
        //if variable changed is pos x 
        if (undoRedoVec[undoRedoIndex].variable == "posx") {
            eTransform.pos.x = undoRedoVec[undoRedoIndex].oldVal; //undo this 
            return;
        }

        //if variable changed is pos y
        if (undoRedoVec[undoRedoIndex].variable == "posy") {
            eTransform.pos.y = undoRedoVec[undoRedoIndex].oldVal; //undo this 
            return;
        }

        //if variable changed is pos z
        if (undoRedoVec[undoRedoIndex].variable == "posz") {
            eTransform.pos.z = undoRedoVec[undoRedoIndex].oldVal; //undo this 
            return;
        }

        //if variable changed is rotation 
        if (undoRedoVec[undoRedoIndex].variable == "rotation") {
            eTransform.rotation = undoRedoVec[undoRedoIndex].oldVal; //undo this 
            return;
        }

        //if variable changed is scalex  
        if (undoRedoVec[undoRedoIndex].variable == "scalex") {
            eTransform.scale.x = undoRedoVec[undoRedoIndex].oldVal; //undo this 
            return;
        }

        //if variable changed is scale
        if (undoRedoVec[undoRedoIndex].variable == "scaley") {
            eTransform.scale.y = undoRedoVec[undoRedoIndex].oldVal; //undo this 
            return;
        }
    }
}

void UiLayer::incrementIndex() {
    //if nothing loaded 
    if (UiLayer::GetInstance().undoRedoIndex == -1) {
        UiLayer::GetInstance().undoRedoIndex = 0;//set index to 0
    }
    else {
        UiLayer::GetInstance().undoRedoIndex++;//increment 
    }
}

void UiLayer::decrementIndex() {
    //if it's the last thing,
    if (UiLayer::GetInstance().undoRedoIndex == 0) {
        UiLayer::GetInstance().undoRedoIndex = -1;//set index to 0
    }
    else {
        UiLayer::GetInstance().undoRedoIndex--;//decrement 
    }
}

void UiLayer::recordAction(entityID& pId, const std::string& pComponent,const std::string& pVariable, float& poldVal, float& pnewVal) {

    //if it's empty or if it's at the latest index 
    if (UiLayer::GetInstance().undoRedoVec.size() == 0 || UiLayer::GetInstance().undoRedoIndex == (UiLayer::GetInstance().undoRedoVec.size()-1)) {
        UiLayer::GetInstance().undoRedoVec.emplace_back(sUndoRedo(pId,pComponent,pVariable,poldVal,pnewVal));
    }
    else {
        //start replacing the stuff infront of the currentindex 
        UiLayer::GetInstance().undoRedoVec.resize(UiLayer::GetInstance().undoRedoIndex + 1);
        UiLayer::GetInstance().undoRedoVec.emplace_back(sUndoRedo(pId, pComponent, pVariable, poldVal, pnewVal));//then emplace it back 
    }
}

void UiLayer::editorRedo() {
    //undo transform 
    if (undoRedoVec[undoRedoIndex].component == "transform") {
        auto& eTransform = ecs.getComponent<transform2D>(undoRedoVec[undoRedoIndex].id);
        //if variable changed is pos x 
        if (undoRedoVec[undoRedoIndex].variable == "posx") {
            eTransform.pos.x = undoRedoVec[undoRedoIndex].newVal; //undo this 
            return;
        }

        //if variable changed is pos y
        if (undoRedoVec[undoRedoIndex].variable == "posy") {
            eTransform.pos.y = undoRedoVec[undoRedoIndex].newVal; //undo this 
            return;
        }

        //if variable changed is pos z
        if (undoRedoVec[undoRedoIndex].variable == "posz") {
            eTransform.pos.z = undoRedoVec[undoRedoIndex].newVal; //undo this 
            return;
        }

        //if variable changed is rotation 
        if (undoRedoVec[undoRedoIndex].variable == "rotation") {
            eTransform.rotation = undoRedoVec[undoRedoIndex].newVal; //undo this 
            return;
        }

        //if variable changed is scalex  
        if (undoRedoVec[undoRedoIndex].variable == "scalex") {
            eTransform.scale.x = undoRedoVec[undoRedoIndex].newVal; //undo this 
            return;
        }

        //if variable changed is scale
        if (undoRedoVec[undoRedoIndex].variable == "scaley") {
            eTransform.scale.y = undoRedoVec[undoRedoIndex].newVal; //undo this 
            return;
        }
    }
}