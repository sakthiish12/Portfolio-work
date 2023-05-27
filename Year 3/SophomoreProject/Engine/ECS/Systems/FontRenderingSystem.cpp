// /******************************************************************************/
// /*!
// \file         FontRenderingSystem.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the font rendering system
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "FontRenderingSystem.h"//font rendering 
#include "../../Editor/Editor.h"//editor 
#include "../../Graphics/Source/EditorCamera.h"//editor camera 

std::shared_ptr<FontRenderingSys> FontRenderingSysPtr;//declare 

/**********************PATH FINDING SYSTEM*********************/
FontRenderingSys::FontRenderingSys(bool requireComponentMatch) {
	UNREFERENCED_PARAMETER(requireComponentMatch);
}



void FontRenderingSys::update(float dt) {
	static_cast<void>(dt);//unreferenced variable 
	for (auto const& e : getEntitiesRegistered()) {
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting the transform of the entity 
		auto& oFont = ecs.getComponent<Com_Font>(e);//getting the font of the entity 

		//if the font is visible
		if (oFont.isVisible == true) {
			//if in the editor mode 
			if (Editor::GetInstance().isPlaying == false) {
				TextRendering::TextRenderer::GetInstance().RenderTextMVPMatrix(oFont.text,oTransform.worldMatrix,oTransform.pos.z, EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition(), { oFont.color.x,oFont.color.y,oFont.color.z });//drawing of the font 
			}
			else {
				//if active camera 
				if (activeCameraTransform) {
					TextRendering::TextRenderer::GetInstance().RenderTextMVPMatrix(oFont.text, oTransform.worldMatrix, oTransform.pos.z, activeCameraData->height, activeCameraTransform->pos, { oFont.color.x,oFont.color.y,oFont.color.z });//drawing of the font 
				}
				else {
					TextRendering::TextRenderer::GetInstance().RenderTextMVPMatrix(oFont.text, oTransform.worldMatrix, oTransform.pos.z, EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition(), { oFont.color.x,oFont.color.y,oFont.color.z });//drawing of the font 
				}
			}
		}
	}
}