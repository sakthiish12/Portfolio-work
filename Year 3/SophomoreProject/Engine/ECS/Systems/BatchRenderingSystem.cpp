// /******************************************************************************/
// /*!
// \file         BatchRenderingSystem.cpp
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
#include "BatchRenderingSystem.h"//batching 
#include "../../Editor/Editor.h"//editor manager 
#include "../../ECS/Systems/FontRenderingSystem.h"//font rendering 
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Graphics/Source/BatchRenderer2D.h"//batch rendering 
#include "../../Graphics/Source/EditorCamera.h"//editor camera 

std::shared_ptr<SerializedRenderer2DSystem> SerializedRenderer2DSys;

void SerializedRenderer2DSystem::update(float dt)
{
	static_cast<void>(dt);//unreferenced variable 

	//clearing the color buffer for the rendering 
	GLCall(glClearColor(0.5f, 0.5, 0.5f, 1.f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (auto const& e : getEntitiesRegistered()) {
		if (ecs.entityHas<Com_Health>(e))
		{
			if (ecs.getComponent<Com_Health>(e).isActive == false)
				continue;
		}
		auto& oTransform = ecs.getComponent<transform2D>(e);//getting transform com 
		auto& oSprite = ecs.getComponent<spriteCom>(e);//getting sprite com for the animation
		auto& oMaterial = ecs.getComponent<material>(e);//getting the material com
		if (oMaterial.mesh == "square") {
			//check if the sprite is visible 
			if (oSprite.isVisible) {
				//this is the player 
				if (oSprite.loop) {
					if (AssetManager::GetInstance().textureMap.find(oSprite.textureId) != AssetManager::GetInstance().textureMap.end()) {
						BatchRenderer2D::GetInstance().AddRotatedQuad(oTransform.worldMatrix, oTransform.pos.z,oSprite.textureId, oMaterial.color, oSprite.currentTexCoords); // animated 
						//BatchRenderer2D::GetInstance().AddRotatedQuad(oTransform.pos.x, oTransform.pos.y, oTransform.pos.z, oSprite.textureId, oTransform.scale.x, oTransform.scale.y, oTransform.rotation, oMaterial.color, oSprite.currentTexCoords); // animated 
					}
				}
				else {
					//Batch Render Geometry of square 
					if (AssetManager::GetInstance().textureMap.find(oSprite.textureId) != AssetManager::GetInstance().textureMap.end()) {
						BatchRenderer2D::GetInstance().AddRotatedQuad(oTransform.worldMatrix, oTransform.pos.z,oSprite.textureId, oMaterial.color); // normal rotated quad  
						//BatchRenderer2D::GetInstance().AddRotatedQuad(oTransform.pos.x , oTransform.pos.y, oTransform.pos.z, oSprite.textureId, oTransform.scale.x, oTransform.scale.y, oTransform.rotation, oMaterial.color); // normal rotated quad  
					}
				}
			}
		}
		//check overflow 
		if (Editor::GetInstance().isPlaying == false) {
			BatchRenderer2D::GetInstance().CheckOverFlow(EditorCamera::GetInstance().GetHeight()/2.f, EditorCamera::GetInstance().GetPosition());//use the editor cam data
		}
		else {
			//sanity check for camera data
			if (activeCameraTransform && activeCameraData && activeCameraTransform) {
				BatchRenderer2D::GetInstance().CheckOverFlow(activeCameraData->height, activeCameraTransform->pos);//use the normal camera component data 
			}
			else {
				BatchRenderer2D::GetInstance().CheckOverFlow(EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition());//use the editor cam data
			}
		}
	}
	if (Editor::GetInstance().isPlaying == false) {
		BatchRenderer2D::GetInstance().Draw(EditorCamera::GetInstance().GetHeight() / 2.f, EditorCamera::GetInstance().GetPosition());//draw the objects using editor cam data
		BatchRenderer2D::GetInstance().DrawLine(EditorCamera::GetInstance().GetHeight() / 2.f, EditorCamera::GetInstance().GetPosition());//draw the lines using editor cam data
		BatchRenderer2D::GetInstance().DrawTriangle(EditorCamera::GetInstance().GetHeight() / 2.f, EditorCamera::GetInstance().GetPosition());//draw the lines using editor cam data
		BatchRenderer2D::GetInstance().DrawQuadMesh(EditorCamera::GetInstance().GetHeight() / 2.f, EditorCamera::GetInstance().GetPosition());//drawing the mesh quad using editor cam data 
	}
	else {
		if (activeCameraTransform && activeCameraData && activeCameraTransform) {
			BatchRenderer2D::GetInstance().Draw(activeCameraData->height, activeCameraTransform->pos);//draw the objects using   normal camera component data
			BatchRenderer2D::GetInstance().DrawLine(activeCameraData->height, activeCameraTransform->pos);//draw the lines using normal camera component data
			BatchRenderer2D::GetInstance().DrawTriangle(activeCameraData->height, activeCameraTransform->pos);//draw the lines using normal camera component data
			BatchRenderer2D::GetInstance().DrawQuadMesh(activeCameraData->height, activeCameraTransform->pos);//draw the quad mesh using   normal camera component data
		}
		else {
			BatchRenderer2D::GetInstance().Draw(EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition());//draw the objects using editor cam data
			BatchRenderer2D::GetInstance().DrawLine(EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition());//draw the lines using editor cam data
			BatchRenderer2D::GetInstance().DrawTriangle(EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition());//draw the lines using editor cam data
			BatchRenderer2D::GetInstance().DrawQuadMesh(EditorCamera::GetInstance().GetHeight(), EditorCamera::GetInstance().GetPosition());//draw the quad mesh using editor cam data
		}

	}
	BatchRenderer2D::GetInstance().Flush();//flush the buffer 
	FontRenderingSysPtr->update(dt);//renders the font 
}
