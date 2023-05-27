// /******************************************************************************/
// /*!
// \file         UIGuizmo.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the guizmos
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
#include "../../ECS/ECS.h"//ecs 
#include "../../Dependencies/Gizmo/ImGuizmo.h"//imguizmo
#include "../../Graphics/Source/EditorCamera.h"//editor camera 
#include "../../Dependencies/Vendor/glm/gtc/matrix_transform.hpp"
#include "../../Dependencies/Vendor/glm/gtc/type_ptr.hpp"
#include "../Editor.h"//editor 
#include"../../InputMgr/InputMgr.h"//input mgr 


void UiLayer::ShowGuizmo() {
    
    entityID selectedEntity = ecs.whichEntityIsSelected();
    //only draw the guizmo when entity is selected and guizmo type is not negative 1 
    if (selectedEntity && Editor::GetInstance().m_IMGUIZMO_Type != -1 && ecs.isAnEntitySelected() && KeyManager.isKeyPressed(VK_CONTROL, true) == false) {
        ImGuizmo::Enable(true);
        ImGuizmo::SetDrawlist();//setting the draw list 
        ImGuizmo::SetOrthographic(true);//orthographic guizmo 

        float windowWidth = (float)ImGui::GetWindowWidth();//getting window width 
        float windowHeight = (float)ImGui::GetWindowHeight();//getting window height 
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);//setting the viewport of the guizmo 

        //camera 
        const glm::mat4& cameraProjection = EditorCamera::GetInstance().GetProjection() * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));//getting the projection
        const glm::mat4& cameraViewMatrix = glm::inverse(EditorCamera::GetInstance().GetTransform());//getting camera view matrix 

        //entity transform 
        auto& entityTransform = ecs.getComponent<transform2D>(selectedEntity);
        glm::mat4 eTransformMatrix = entityTransform.worldMatrix;

        //snapping 
        bool snap = KeyManager.isKeyPressed(VK_CONTROL, true);
        float snapValue = 50.f;//sna to 0.5 for translation/scale 
        //snap to 45 degrees for rotation
        if (Editor::GetInstance().m_IMGUIZMO_Type == ImGuizmo::OPERATION::ROTATE) {
            snapValue = 45.f;
        }
        float snapValues[3] = { snapValue,snapValue,snapValue };

        //manipulate guizmo 
        ImGuizmo::Manipulate(glm::value_ptr(cameraViewMatrix), glm::value_ptr(cameraProjection), static_cast<ImGuizmo::OPERATION>(Editor::GetInstance().m_IMGUIZMO_Type), ImGuizmo::LOCAL,
            glm::value_ptr(eTransformMatrix),nullptr, snap ? snapValues : nullptr);
        
        //activate only when it's being used 
        if (ImGuizmo::IsUsing()) {
            //resulting matrix 
            glm::vec3 translation, rotation, scale;
            Mathf::DecomposeTransform(eTransformMatrix, translation, rotation, scale);//decompose the reuslting matrix into 3 matrix 

            //to prevent the entity transformation from being set when there is nan value cause by imgui IO.mouse pos .
            if (!isnan(translation.x) && !isnan(translation.y)) {
                 
                float deltaRotation = glm::degrees(rotation.z) - entityTransform.rotation;//getting the delta rotation of the object 

                entityTransform.rotation += deltaRotation;//getting updated rotation
                entityTransform.pos =  Mathf::vec3(translation.x, translation.y, entityTransform.pos.z);//getting the position, not tweaking the layering**
                entityTransform.scale = Mathf::vec2(scale.x, scale.y);//getting the scale 
            }
        }
        
    }

}
