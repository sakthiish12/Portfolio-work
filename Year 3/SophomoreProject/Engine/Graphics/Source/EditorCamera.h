// /******************************************************************************/
// /*!
// \file         EditorCamera.h
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
#ifndef EDITOR_CAMERA
#define EDITOR_CAMERA

#include "Camera.h"//camera 
#include "../../Math/Vector/Vector3.h"//vec3 


class EditorCamera : Camera
{
public:
	EditorCamera();//constructor 
	~EditorCamera();//destructor 
	void OnUpdate(float dt);//on update of camera 

	//getter functions 
	Mathf::vec3 GetPosition() const;
	float GetHeight() const;
	float GetWidth() const; //getting the width of the camera 

	glm::mat4 GetProjection() const;//getting the projection
	glm::mat4 GetTransform() const;//getting the transform 
	

	inline static EditorCamera& GetInstance() { static EditorCamera instance; return instance; }//static access 
private:
	float m_Height = 600.f; //default height of the editor camera
	float m_AspectRatio; //aspect ratio of the editor camera 
	float m_Minheight{ 50 }, m_Maxheight{ 2000 }; //min and max window height of the camera 
	int m_HeightChangeVal{ 5 };//incremental height change of the camera 
	float m_LinearSpeed{ 2.f };//linear speed of camera 
	float m_TranslationSpeed{ 20.f };//translation speed of camera 
	Mathf::vec3 m_CameraPosition{ 0.f, 0.f, 1.f };//starting position of camera 
};


#endif // !EDITOR_CAMERA
