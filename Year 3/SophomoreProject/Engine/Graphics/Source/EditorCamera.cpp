// /******************************************************************************/
// /*!
// \file         EditorCamera.cpp
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
#include "EditorCamera.h"//editor camera 
#include "../../InputMgr/InputMgr.h"//input 
#include "../../Dependencies/Vendor/glm/glm.hpp"//glm
#include "../../WinMgr/WinMgr.h"//windows manager
#include "../../ECS/ECS.h" //ECS
#include "../../Editor/Editor.h"//editor 

//constructor 
EditorCamera::EditorCamera() : m_AspectRatio(1.0f){
		
}

//destructor 
EditorCamera::~EditorCamera(){

}

//on update function 
void EditorCamera::OnUpdate(float dt) {
	static_cast<void>(dt);//unreferenced variable 

	//for increasing zoom
	if ((MouseManager.getMouseScroll() > 0) && KeyManager.isKeyPressed(VK_CONTROL,true)) {
		if (this->m_Height <= this->m_Minheight) {
			//do nothing 
		}
		else {
			this->m_Height -= this->m_HeightChangeVal;//else decrease the height of camera
		}
	}

	//for decreasing zoom 
	if ((MouseManager.getMouseScroll() < 0) && KeyManager.isKeyPressed(VK_CONTROL, true)) {
		this->m_Height += this->m_HeightChangeVal;//increase the height of the camera 
	}

	//translation of the camera 
	if (KeyManager.isKeyPressed(VK_UP, true)) {
		this->m_CameraPosition.y += this->m_TranslationSpeed * this->m_Height / 1000.0f;
	}
	if (KeyManager.isKeyPressed(VK_DOWN, true)) {
		this->m_CameraPosition.y -= this->m_TranslationSpeed * this->m_Height / 1000.0f;
	}
	if (KeyManager.isKeyPressed(VK_LEFT, true)) {
		this->m_CameraPosition.x -= this->m_TranslationSpeed * this->m_Height / 1000.0f;
	}
	if (KeyManager.isKeyPressed(VK_RIGHT, true)) {
		this->m_CameraPosition.x += this->m_TranslationSpeed * this->m_Height / 1000.0f;
	}

	//Press F to focus camera on presently selected entity
	if (KeyManager.isKeyPressed('F', true) || Editor::GetInstance().isFocus)
	{
		if (ecs.isAnEntitySelected() &&
			ecs.entityHas<transform2D>(ecs.whichEntityIsSelected()))
		{
			//(23 Feb) Take into account parenting
			transform2D const& t = ecs.getComponent<transform2D>(ecs.whichEntityIsSelected());
			this->m_CameraPosition.x = t.worldMatrix[3][0];
			this->m_CameraPosition.y = t.worldMatrix[3][1];
			//this->m_Height = max(t.scale.x, t.scale.y);
		}
	}
}

Mathf::vec3 EditorCamera::GetPosition() const {
	return m_CameraPosition;//return positon
}

float EditorCamera::GetHeight() const {
	return m_Height;//return height
}

float EditorCamera::GetWidth() const {
	float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
	return aspectRatio * m_Height;//return width
}

glm::mat4 EditorCamera::GetProjection() const {
	float aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());
	float cameraWidth = aspectRatio * m_Height;

	//getting the projection matrix 
	glm::mat4 projectionMatrix = glm::ortho(-cameraWidth/2.f, cameraWidth/2.f, -m_Height/2.f, m_Height/2.f, -1.0f, 1.0f);
	//glm::mat4 projectionMatrix = glm::ortho(0.f, cameraWidth, 0.f, m_Height, 0.f, 0.f);
	
	return projectionMatrix;
}

glm::mat4 EditorCamera::GetTransform() const{
	//getting the transform of the matrix 
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(m_CameraPosition.x, m_CameraPosition.y, 1.f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));

	//glm::mat4 m_ViewMatrix = glm::inverse(transform);

	return transform;
}