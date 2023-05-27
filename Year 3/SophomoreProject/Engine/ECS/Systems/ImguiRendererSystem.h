// /******************************************************************************/
// /*!
// \file         imguiRendererSystem.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the systems of the imgui test state 
// \lines        44 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef IMGUIRENDERERSYS_H
#define IMGUIRENDERERSYS_H

//#include <iostream>
//#include <random>
//#include <chrono>
#include "../../WinMgr/WinMgr.h"
//#include "InputMgr/InputMgr.h"
//#include "../../EditorStateManager/EditorStates/gameState.h"
#include "../../Dependencies/Vendor/imgui/imgui_impl_win32.h"
#include "../../Dependencies/Vendor/imgui/imgui_impl_opengl3.h"
#include "../../EditorStateManager/gameStateManager.h"
#include "../ECS.h"

//#include "../../Graphics/Source/Renderer.h"
//renderer 

//#include "../../Graphics/Source/GraphicsApplication.h"//for the graphics 
#include "test/TestClearColor.h"

//externs 
extern test::Test* current_test; //stores ptr to the current test 
extern test::TestMenu* test_menu;//stores ptr to the test menu 

/***************************************
* @Brief : this is the renderer system for the ecs
****************************************/
class imguiRenderSystem : public systemBase
{
public:
    imguiRenderSystem(bool requireComponentMatch = false) :
        systemBase(requireComponentMatch)
    {
    }

    void update(float dt)
    {
        static_cast<void>(dt);//unreferenced variable 

            //renderer abstraction for clearing the buffer 
        Renderer::GetInstance().Clear();

        //clearing the frame 

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        if (current_test) 
        {
            current_test->OnUpdate(0.0f);
            current_test->OnRender();
            ImGui::NewFrame();
            ImGui::Begin("Test");
            if (current_test != test_menu && ImGui::Button("-<")) {
                //if the button is clicked
                delete current_test;
                current_test = test_menu; //go back the the test menu
            }
            current_test->OnImGuiRender();
            ImGui::End();
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGui::EndFrame();
        //ImGui::UpdatePlatformWindows();

        GLCall(SwapBuffers(WindowsManager.getWindowDC()));
    }
};
std::shared_ptr<imguiRenderSystem> imguiRenderSys;

#endif