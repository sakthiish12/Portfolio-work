// /******************************************************************************/
// /*!
// \file         Editor.h
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
#pragma once 

#ifndef EDITOR_H
#define EDITOR_H

class Editor {
public:
	void initailize_editor(); //this function initializes the dear imgui
	void clear_editor();//this function deletes the context of the dear imgui 

	void Begin();//begin new frame 
	void OnImguiRender(float dt);//render for the imgui 
	void End(); //end new frame 

	inline static Editor& GetInstance() { static Editor instance; return instance; } //singleton get instance of this class 

	bool isPlaying{ false };//indicates if it is in editor mode 
	bool isFocus{ false };//focused on selected entity 

	int m_IMGUIZMO_Type = -1;//indicate the type of guizmo

private:

};
#endif