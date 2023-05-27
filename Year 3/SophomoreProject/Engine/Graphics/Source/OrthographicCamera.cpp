// /******************************************************************************/
// /*!
// \file         OrthographicCamera.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the Orthographic camera
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "OrthographicCamera.h"//ortho 


OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
{
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;//calculating the view projection matrix of the camera 
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
{

	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);//calculating the projection matrix of the camera 
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;//calculating view projection matrix of camera 
}

void OrthographicCamera::RecalculateViewMatrix()
{
	//recalculate the transform matrix 
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

	m_ViewMatrix = glm::inverse(transform);//recalculate view matrix 
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;//recalculate view projection matrix 
}

