// /******************************************************************************/
// /*!
// \file         GUISystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the GUI system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "GUISystem.h"//GUI system 

std::shared_ptr<GUISys> GUISysPtr;//declare 

/**********************GUI SYSTEM*********************/
GUISys::GUISys(bool requireComponentMatch) {
	UNREFERENCED_PARAMETER(requireComponentMatch);
}

void GUISys::update(float dt) {
	static_cast<void>(dt);//unreferenced variable 
	for (auto const& e : getEntitiesRegistered()) {
		(void)e;
		auto& oGUI = ecs.getComponent<Com_GUI>(e);//getting the gui type 
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting the transform of the entity 
		auto& oEntityType = ecs.getComponent<Com_type>(e);//getting the entity type 
		
		//if entity is camera 
		if (oEntityType.type == Com_type::camera) {
			if (oGUI.preCamPosSet == false) {
				for (auto const& f : getEntitiesRegistered()) {
					(void)f;
					auto& oGUIInner = ecs.getComponent<Com_GUI>(f);//getting the gui type 
					auto& oTransformInner = ecs.getComponent<transform2D>(f);//getting the transform of the entity 
					auto& oEntityTypeInner = ecs.getComponent<Com_type>(f);//getting the entity type 
					(void)oTransformInner;
					if (oEntityTypeInner.type == Com_type::gui) {
						oGUIInner.prevCameraPos = oTransform.pos;
					}
				}
				//oGUI.prevCameraPos = oGUI.CameraPos;//set the previous camera position 
				//oGUI.CameraPos = oTransform.pos;//set the current camera position
				oGUI.preCamPosSet = true;//set to true 
				return;
			}
		}
		if (oEntityType.type == Com_type::camera && oGUI.preCamPosSet == true) {
			for (auto const& g : getEntitiesRegistered()) {
				(void)g;
				auto& oGUIInner = ecs.getComponent<Com_GUI>(g);//getting the gui type 
				auto& oTransformInner = ecs.getComponent<transform2D>(g);//getting the transform of the entity 
				auto& oEntityTypeInner = ecs.getComponent<Com_type>(g);//getting the entity type 
				//if it's a gui 
				if (oEntityTypeInner.type == Com_type::gui) {
					//if the camera has been attached, 
					if (oGUIInner.preCamPosSet == true) {
						//if the camera moves, it follows the camera movement 
						//if camera current position is more than the previous camera position
						if (oTransform.pos.x > oGUIInner.prevCameraPos.x) {
							oTransformInner.pos.x += abs(oTransform.pos.x - oGUIInner.prevCameraPos.x);//shift to the right 
						}

						//if camera current position is more than the previous camera position
						if (oTransform.pos.x < oGUIInner.prevCameraPos.x) {
							oTransformInner.pos.x -= abs(oTransform.pos.x - oGUIInner.prevCameraPos.x);//shift to the right 
						}

						//if camera current position is more than the previous camera position
						if (oTransform.pos.y > oGUIInner.prevCameraPos.y) {
							oTransformInner.pos.y += abs(oTransform.pos.y - oGUIInner.prevCameraPos.y);//shift to the right 
						}

						//if camera current position is lesser than the previous camera position
						if (oTransform.pos.y < oGUIInner.prevCameraPos.y) {
							oTransformInner.pos.y -= abs(oTransform.pos.y - oGUIInner.prevCameraPos.y);//shift to the right 
						}
					}
				}
			}
			oGUI.prevCameraPos = oTransform.pos;//updates the camera position 
		}
	}
}
