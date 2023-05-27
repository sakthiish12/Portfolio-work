// /******************************************************************************/
// /*!
// \file         OrthographiscCamera.h
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
#ifndef ORTHO_CAMERA
#define ORTHO_CAMERA

#include "../../Dependencies/Vendor/glm/glm.hpp"//glm 
#include <glm/gtc/matrix_transform.hpp>//matrix transforms 

class OrthographicCamera {
public:
	OrthographicCamera(float left, float right, float bottom, float top);//ortho camera init 

	void SetProjection(float left, float right, float bottom, float top);//setting the projection of the camera 

	const glm::vec3& GetPosition() const { return m_Position; }//getting the position of the camera 
	void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }//setting the position of the camera 

	float GetRotation() const { return m_Rotation; }//getting the rotation of the camera 
	void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }//setting the rotation of the camera 

	inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }//getting the projection of the camera 
	inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }//getting the view matrix of the camera 
	inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }//getting the view projection of the camera 
private:
	void RecalculateViewMatrix();//recalculate the view matrix 
private:
	glm::mat4 m_ProjectionMatrix;//projection matrix 
	glm::mat4 m_ViewMatrix;//view matrix 
	glm::mat4 m_ViewProjectionMatrix;//view projection matrix

	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };//position of the camera 
	float m_Rotation = 0.0f;//rotation of the camera 
};




#endif // !ORTHO_CAMERA
