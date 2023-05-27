// /******************************************************************************/
// /*!
// \file         Camera.h
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
#include "../../Dependencies/Vendor/glm/glm.hpp"//glm 
#include <glm/gtc/matrix_transform.hpp>//matrix transforms 

#ifndef _CAMERA_H
#define _CAMERA_H

class Camera
{
public:
	Camera() = default;
	Camera(const glm::mat4& projection)
		: m_Projection(projection) {}

	virtual ~Camera() = default;

	const glm::mat4& GetProjection() const { return m_Projection; }
protected:
	glm::mat4 m_Projection = glm::mat4(1.0f);
};

#endif 