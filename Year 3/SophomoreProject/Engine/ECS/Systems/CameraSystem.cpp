// /******************************************************************************/
// /*!
// \file         CameraSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the Camera System
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "CameraSystem.h"//camera system 
#include "../../Editor/Editor.h"//editor manager 
#include "../../Graphics/Source/BatchRenderer2D.h"//batch rendering 
#include "../../ECS/Systems/controllerSystem.h"//controller 
#include "../../Sound/SoundManager.h"


std::shared_ptr<SerializedCamera2DSystem> SerializedCamera2DSys;
transform2D* activeCameraTransform = nullptr;
camera2DCom* activeCameraData = nullptr;

//unsigned int activeCameraID = 0;
//transform2D* activeCameraTrans = nullptr;
void SerializedCamera2DSystem::update(float dt)
{
	static_cast<void>(dt);//unreferenced variable 
	
	//registering the entities 
	for (auto const& e : getEntitiesRegistered())
	{
		auto& oCamera = ecs.getComponent<camera2DCom>(e);//getting transform com 
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting transform com 
		oCamera.aspectRatio = (WindowsManager.getWidthF() / WindowsManager.getHeightF());//getting the aspect ratio of the camera 
		//update the primary camera 
		if (oCamera.isPrimary == true) {
			activeCameraTransform = &oTransform;//getting the position of the transform 
			activeCameraData = &oCamera;//getting the height and aspect ratio of camera 
		}

		//if the camera is set to fixed to the player 
		if (oCamera.isFixedToPlayer == true) {
			for (auto& eInner : controllerSystemPtr->getEntitiesRegistered())
			{
				activeCameraTransform = &ecs.getComponent<transform2D>(e);
				oTransform.pos = ecs.getComponent<transform2D>(eInner).pos;
			}
		}

		//if it's in the editor mode , draw the camera boundary 
		if (Editor::GetInstance().isPlaying == false) {
			
			float camWidth = oCamera.aspectRatio * oCamera.height;//getting the width of the camera 
			Mathf::Vec2 bottomLeft = { oTransform.pos.x - (camWidth), oTransform.pos.y - (oCamera.height) };
			Mathf::Vec2 bottomRight = { oTransform.pos.x + (camWidth), oTransform.pos.y - (oCamera.height) };
			Mathf::Vec2 topLeft = { oTransform.pos.x - (camWidth), oTransform.pos.y + (oCamera.height) };
			Mathf::Vec2 topRight = { oTransform.pos.x + (camWidth), oTransform.pos.y + (oCamera.height) };
			BatchRenderer2D::GetInstance().AddRotateLine(bottomLeft.x, bottomLeft.y, bottomRight.x, bottomRight.y, 0.f, 0.f, 0.f);//bottom 
			BatchRenderer2D::GetInstance().AddRotateLine(topLeft.x, topLeft.y, topRight.x, topRight.y, 0.f, 0.f, 0.f);//top
			BatchRenderer2D::GetInstance().AddRotateLine(bottomLeft.x, bottomLeft.y, topLeft.x, topLeft.y, 0.f, 0.f, 0.f);//left
			BatchRenderer2D::GetInstance().AddRotateLine(bottomRight.x, bottomRight.y, topRight.x, topRight.y, 0.f, 0.f, 0.f);//right
		}

	}


}