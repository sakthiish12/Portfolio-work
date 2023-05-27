// /******************************************************************************/
// /*!
// \file         PlayButtonPanel.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the play button panel 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"
#include <filesystem>//file system 
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Graphics/Source/texture.h"//texture 
#include "../Editor.h"//editor 
#include "../../Graphics/Source/Framebuffer.h"//framebuffer 
#include "../../WinMgr/WinMgr.h"//window manager 
#include "../../ECS/ECS.h"
#include "../../Dependencies/ficons/fa.h"//f icons 
#include "../../Dependencies/Gizmo/ImGuizmo.h"//imguizmo
#include "../../Graphics/Source/PostProcessing.h"//post processing 

void UiLayer::LoadPlayPanel(bool* p_open) {
    static_cast<void>(p_open);//unreferenced variable 

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));//setting the windows padding 
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));//making it transparent 
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));//makign the button color 
    auto& colors = ImGui::GetStyle().Colors;//getting the colors
    const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];//button hovered color 
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));//if hovered
    const auto& buttonActive = colors[ImGuiCol_ButtonActive];//button active color 
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));//if active 

    ImGui::Begin("##toolbar",nullptr,ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float size = ImGui::GetWindowHeight() - 4.f; //4.f is the padding 

    ImGui::SetCursorPosX(0.f);//getting the position of the button in the tool bar padding it, put in the middle 

    //IMGUIZMO translate 
    if (ImGui::ButtonEx(ICON_FA_ARROWS_ALT, ImVec2(size, size), ImGuiButtonFlags_AlignTextBaseLine)) {
        Editor::GetInstance().m_IMGUIZMO_Type = ImGuizmo::OPERATION::TRANSLATE;
    }
    ImGui::SameLine();//making the imgui in the same line     
    //IMGUIZMO scale  
    if (ImGui::ButtonEx(ICON_FA_EXPAND, ImVec2(size, size), ImGuiButtonFlags_AlignTextBaseLine)) {
        Editor::GetInstance().m_IMGUIZMO_Type = ImGuizmo::OPERATION::SCALE;
    }
    ImGui::SameLine();//making the imgui in the same line     
    //IMGUIZMO rotate  
    if (ImGui::ButtonEx(ICON_FA_SYNC, ImVec2(size, size), ImGuiButtonFlags_AlignTextBaseLine)) {
        Editor::GetInstance().m_IMGUIZMO_Type = ImGuizmo::OPERATION::ROTATE;
    }
    ImGui::SameLine();//making the imgui in the same line 

    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x *0.5f) -  (size * 0.5f));//getting the position of the button in the tool bar padding it, put in the middle 
    //unsigned int icon = m_SceneState == SceneState::Edit ? AssetManager::GetInstance().m_PlayIcon->GetRendererID() : AssetManager::GetInstance().m_StopIcon->GetRendererID();//getting the icon 
    //unsigned int icon = AssetManager::GetInstance().iconTextureMap.find("play.png")->second->GetRendererID(); //TODO : change back 
    //setting the image button 
    if (ImGui::ButtonEx(ICON_FA_PLAY, ImVec2(size, size), ImGuiButtonFlags_AlignTextBaseLine)) {
        if (m_SceneState == SceneState::Edit) {
            //onScenePlay();//play 
            //Save temporary level
            ecs.saveLevel("Resources/TemporaryLevel.json");
            ecs.disableEntityIDReuse = true;
            Editor::GetInstance().isPlaying = true; //set scene bool to play
            m_SceneState = SceneState::Play; //Set to start playing
            //resize the post processing frame 
            PostProcessing::GetInstance().m_PostProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
            //resize the pre processing frame 
            PostProcessing::GetInstance().m_PreProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
        }else if (m_SceneState == SceneState::Play) {
            onSceneStop();//stop 
        }else {
            //simulate only physics 
        }
    }

    ImGui::PopStyleVar(2);//poping the style var 
    ImGui::PopStyleColor(3);//poping the style color 
    ImGui::End();
}
