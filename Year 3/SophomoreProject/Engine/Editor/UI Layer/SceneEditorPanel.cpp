// /******************************************************************************/
// /*!
// \file         SceneEditorPanel.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the scene editor panel
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"//ui layer 
#include "../../Graphics/Source/Framebuffer.h"//framebuffer 
#include "../../Dependencies/Vendor/glm/vec2.hpp"//glm
#include "../../ECS/ECS.h"//ecs 
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Graphics/Source/PostProcessing.h"//post processing 
#include "../../InputMgr/InputMgr.h"//input manager
#include "../../Graphics/Source/EditorCamera.h"//editor camera 
#include <algorithm>//algorithm
#pragma warning(disable : 4244)
#pragma warning(disable : 4312)


void UiLayer::LoadSceneEditor(bool* p_open) {
    static_cast<void>(p_open);//unreferenced variable 
    ImGui::Begin("Scene Editor");
    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//getting the size of the view port 
    
    //if the view port size is miss match, resize it m_PreProcessingFrame
    if (PostProcessing::GetInstance().m_PostProcessingFrame->GetViewportSize() != *((glm::vec2*)&viewportPanelSize) && !WindowsManager.GetIsQuiting()) {
        PostProcessing::GetInstance().m_PostProcessingFrame->Resize(static_cast<uint32_t>(viewportPanelSize.x), static_cast<uint32_t>(viewportPanelSize.y));//resize it 
    }
    ImGui::Image((ImTextureID)(PostProcessing::GetInstance().m_PostProcessingFrame->GetColorAttachmentRenderID()), ImVec2(PostProcessing::GetInstance().m_PostProcessingFrame->GetViewportSize().x, PostProcessing::GetInstance().m_PostProcessingFrame->GetViewportSize().y), { 0,1 }, { 1,0 });//for the scene editor image 

    ////*****Click*********//
    //if the mouse left button is clicked 
    if (MouseManager.isButtonPressed(MouseButton::LEFT_BUTTON) && KeyManager.isKeyPressed(VK_CONTROL,true)) {
        ImVec2 imgui_image_screen_topleft = ImGui::GetWindowPos() + ImVec2{ 5,25 };//getting the scene editor top left coordinates 
        ImVec2 imgui_image_screen_bottomright = { imgui_image_screen_topleft + viewportPanelSize };//getting the bottom right of the scene editor coordinates

        ImVec2 mousePos = ImGui::GetMousePos();//getting the mouse position in windows 
        Mathf::vec3 mouse_world_posT; //final mouse position in world coordinate 

        //checking if the mouse position is within the imgui scene editor 
        if (mousePos.x > imgui_image_screen_topleft.x && mousePos.x < imgui_image_screen_bottomright.x &&
            mousePos.y > imgui_image_screen_topleft.y && mousePos.y < imgui_image_screen_bottomright.y) {

            //Use imgui_image_screen_topleft as 0,0 refernce point
            ImVec2 Texture_cursor_pos = mousePos - imgui_image_screen_topleft;

            Mathf::Matrix3 topleft_texture_image_to_bottomleft_texture_image{ 1.f, 0.f, 0.f,		0.f, -1.f, (imgui_image_screen_bottomright.y - imgui_image_screen_topleft.y),		0.f, 0.f , 1.f };
            Mathf::Matrix3 bottomleft_texture_image_to_center_texture_image{ 1.f, 0.f, -static_cast<float>(viewportPanelSize.x) / 2.f,		0.f, 1.f, -static_cast<float>(viewportPanelSize.y) / 2.f,		0.f, 0.f, 1.f };
            Mathf::Matrix3 texture_image_to_viewport{ WindowsManager.getWidthF() / viewportPanelSize.x, 0.f, 0.f,		0.f, WindowsManager.getHeightF() / viewportPanelSize.y, 0.f,		0.f, 0.f, 1.f };
            Mathf::Matrix3 viewport_to_scaled_camera{ static_cast<float>(EditorCamera::GetInstance().GetWidth()) / static_cast<float>(WindowsManager.getWidthF()), 0.f, 0.f,		0.f, static_cast<float>(EditorCamera::GetInstance().GetHeight()) / static_cast<float>(WindowsManager.getHeightF()), 0.f,0.f, 0.f, 1.f };
            Mathf::Matrix3 undo_view_transformT{ 1.f, 0.f, EditorCamera::GetInstance().GetPosition().x,		0.f, 1.f, EditorCamera::GetInstance().GetPosition().y,		0.f, 0.f, 1.f };
            mouse_world_posT = undo_view_transformT * viewport_to_scaled_camera * texture_image_to_viewport * bottomleft_texture_image_to_center_texture_image * topleft_texture_image_to_bottomleft_texture_image * Mathf::vec3{ Texture_cursor_pos.x , Texture_cursor_pos.y, 1.f };
            
            //create a map to store the entities and the pos z layering 
            std::unordered_map<float, entityID> clickedEntities;

            //check with all the enities and see which is selected 
            for (auto entity : ecs.getEntityNames()) {

                //check if it's camera 
                if (ecs.entityHas<camera2DCom>(entity.first)) {
                    continue;//skipping 
                }

              

                //aabb check if the mouse position is within the 
                if (mouse_world_posT.x > (ecs.getComponent<transform2D>(entity.first).pos.x - ecs.getComponent<transform2D>(entity.first).scale.x/2.f) &&
                    mouse_world_posT.x < (ecs.getComponent<transform2D>(entity.first).pos.x + ecs.getComponent<transform2D>(entity.first).scale.x/2.f) &&
                    mouse_world_posT.y < (ecs.getComponent<transform2D>(entity.first).pos.y + ecs.getComponent<transform2D>(entity.first).scale.y/2.f) &&
                    mouse_world_posT.y >(ecs.getComponent<transform2D>(entity.first).pos.y - ecs.getComponent<transform2D>(entity.first).scale.y/2.f)) {

                    clickedEntities.emplace(std::make_pair(ecs.getComponent<transform2D>(entity.first).pos.z, entity.first));//emplace into the map 
                }

            }

            //after comparing all the entities to get the highest pos z 
            if (clickedEntities.size() != 0) {
                auto maxElement = std::max_element(std::begin(clickedEntities), std::end(clickedEntities), [](auto a, auto b) { return a < b; });
                ecs.isEntitySelected = true;//setting entity selected to true 
                ecs.selectedEntityID = maxElement->second;//getting the entity id 
            }
        }
    }

    ////*****Click for animation editor *********//
   //if the mouse left button is clicked 
    if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) && KeyManager.isKeyPressed(VK_SHIFT, true) && UiLayer::GetInstance().animationPanelToggle == true) {
        ImVec2 imgui_image_screen_topleft = ImGui::GetWindowPos() + ImVec2{ 5,25 };//getting the scene editor top left coordinates 
        ImVec2 imgui_image_screen_bottomright = { imgui_image_screen_topleft + viewportPanelSize };//getting the bottom right of the scene editor coordinates

        ImVec2 mousePos = ImGui::GetMousePos();//getting the mouse position in windows 
        Mathf::vec3 mouse_world_posT; //final mouse position in world coordinate 

        //checking if the mouse position is within the imgui scene editor 
        if (mousePos.x > imgui_image_screen_topleft.x && mousePos.x < imgui_image_screen_bottomright.x &&
            mousePos.y > imgui_image_screen_topleft.y && mousePos.y < imgui_image_screen_bottomright.y) {

            //Use imgui_image_screen_topleft as 0,0 refernce point
            ImVec2 Texture_cursor_pos = mousePos - imgui_image_screen_topleft;

            Mathf::Matrix3 topleft_texture_image_to_bottomleft_texture_image{ 1.f, 0.f, 0.f,		0.f, -1.f, (imgui_image_screen_bottomright.y - imgui_image_screen_topleft.y),		0.f, 0.f , 1.f };
            Mathf::Matrix3 bottomleft_texture_image_to_center_texture_image{ 1.f, 0.f, -static_cast<float>(viewportPanelSize.x) / 2.f,		0.f, 1.f, -static_cast<float>(viewportPanelSize.y) / 2.f,		0.f, 0.f, 1.f };
            Mathf::Matrix3 texture_image_to_viewport{ WindowsManager.getWidthF() / viewportPanelSize.x, 0.f, 0.f,		0.f, WindowsManager.getHeightF() / viewportPanelSize.y, 0.f,		0.f, 0.f, 1.f };
            Mathf::Matrix3 viewport_to_scaled_camera{ static_cast<float>(EditorCamera::GetInstance().GetWidth()) / static_cast<float>(WindowsManager.getWidthF()), 0.f, 0.f,		0.f, static_cast<float>(EditorCamera::GetInstance().GetHeight()) / static_cast<float>(WindowsManager.getHeightF()), 0.f,0.f, 0.f, 1.f };
            Mathf::Matrix3 undo_view_transformT{ 1.f, 0.f, EditorCamera::GetInstance().GetPosition().x,		0.f, 1.f, EditorCamera::GetInstance().GetPosition().y,		0.f, 0.f, 1.f };
            mouse_world_posT = undo_view_transformT * viewport_to_scaled_camera * texture_image_to_viewport * bottomleft_texture_image_to_center_texture_image * topleft_texture_image_to_bottomleft_texture_image * Mathf::vec3{ Texture_cursor_pos.x , Texture_cursor_pos.y, 1.f };

            //create a box image on the mouse world pos 
            entityID newID = ecs.createEntityFromFile
            ("Resources/PresetEntities/animationFrame.json");
            //shifting the object to the newly created entity
            ecs.getComponent<transform2D>(newID).pos.x = mouse_world_posT.x;
            ecs.getComponent<transform2D>(newID).pos.y = mouse_world_posT.y;
            ecs.getComponent<transform2D>(newID).pos.z = 2.0f;//set it to the max 
            ecs.renameEntity(newID, "Frame " + std::to_string(UiLayer::GetInstance().totalNumFrames));//renaming the entity
            UiLayer::GetInstance().totalNumFrames++;//increment the number of frames created 
            //adjusting the tint 
            ecs.getComponent<material>(newID).color.w = 0.25f;
        }
    }
    

    ////*****DRAG AND DROP*********//
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            const wchar_t* path = (const wchar_t*)payLoad->Data;
            std::wstring wTempString(path);
            std::string pathString(wTempString.begin(), wTempString.end());//getting the string of the level 

            //if it's a json file, load it 
            if (pathString.find("json") != std::string::npos && pathString.find("Level") != std::string::npos) {

                //load in whatever scene 
                ECS::GetInstance().loadLevel(pathString);
            }
            else if (pathString.find("PresetEntities") != std::string::npos && pathString.find("json") != std::string::npos) {
                //it is a preset entity and json file 
                entityID newID = ecs.createEntityFromFile(pathString);
                //Shift focus to new entity
                ecs.isEntitySelected = true;
                ecs.selectedEntityID = newID;
            }
            else if (pathString.find("SavedEntities") != std::string::npos && pathString.find("json") != std::string::npos) {
                //it is a saved entity and json file 
                entityID newID = ecs.createEntityFromFile(pathString);
                //Shift focus to new entity
                ecs.isEntitySelected = true;
                ecs.selectedEntityID = newID;
            }

            //if it's a texture and it is already loaded, create a base entity 
            if(pathString.find(".png") != std::string::npos) {
                pathString = pathString.substr(pathString.find_last_of("\\"));
                pathString = pathString.substr(pathString.find_first_of("qwertyuiopasdfhgjklzxcvnbmQWERTYUIOPASDFGHJKLZXCVBNM1234567890"));

                //find if the texture is already loaded in 
                if (AssetManager::GetInstance().textureSlotMap.find(pathString) != AssetManager::GetInstance().textureSlotMap.end()) {
                    entityID newID = ecs.createEntityFromFile
                    ("Resources/PresetEntities/sampleEntity.json");
                    //Shift focus to new entity
                    ecs.isEntitySelected = true;
                    ecs.selectedEntityID = newID;
                    ecs.getComponent<spriteCom>(newID).textureId = pathString;//updating the texture to the selected one 
                }
            }

         
        }
        ImGui::EndDragDropTarget();
    }
    
    ////*****IMGUIZMO*********//
    UiLayer::GetInstance().ShowGuizmo();



    ImGui::End();
}

