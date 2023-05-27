// /******************************************************************************/
// /*!
// \file         PostProcessing.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the post processing 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "PostProcessing.h"//post processing 
#include "Renderer.h"//renderer 
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Editor/Editor.h"//editor 
#include "../../InputMgr/InputMgr.h"//input manager

PostProcessing::PostProcessing() {
	
	//rectangle vertices 
	float rectangleVertices[] =
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	//init for the vao, binding 
	unsigned int rectVBO;
	glGenVertexArrays(1, &m_VAOPost);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(m_VAOPost);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);//getting the buffer data 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	//doing up the shader 
	m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postBase.shader")->second);

	m_PreProcessingFrame = std::make_unique<Framebuffer>();//making the pre processing framebuffer
	m_PostProcessingFrame = std::make_unique<Framebuffer>();//making the post processing framebuffer 
}

void PostProcessing::drawPostProcessing() {
	// Draw the framebuffer rectangle
	m_PostProcessingShader->Bind();

	m_PostProcessingShader->SetUniform1f("alphaColor", m_AlphaValue);//updating the alpha value 

	glBindVertexArray(m_VAOPost);//bind the post processing vao 
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glBindTexture(GL_TEXTURE_2D, m_PreProcessingFrame->GetColorAttachmentRenderID());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST); //re enable the depth testing 
}

void PostProcessing::bindPreProcess() {
	if (toggleEffects == true) {
		//binding the shader program
		switch (PostProcessing::currentEffects) {
			case Effects::effectNormal : {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postBase.shader")->second);
				AssetManager::GetInstance().LoadShaders();
				break;
			}
			case Effects::effectGreyScale: {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postGreyScale.shader")->second);
				AssetManager::GetInstance().LoadShaders();
				break;
			}
			case Effects::effectInverse: {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postInverse.shader")->second);
				AssetManager::GetInstance().LoadShaders();
				break;
			}
			case Effects::effectInversion: {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postInversion.shader")->second);
				AssetManager::GetInstance().LoadShaders();
				break;
			}
			case Effects::effectShake: {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postShake.shader")->second);
				AssetManager::GetInstance().LoadShaders();
				break;
			}
			case Effects::effectBlur: {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postBlur.shader")->second);
				AssetManager::GetInstance().LoadShaders();
				break;
			}
			default: {
				PostProcessing::GetInstance().m_PostProcessingShader = std::move(AssetManager::GetInstance().shaderProg.find("postBase.shader")->second);
				AssetManager::GetInstance().LoadShaders();
			}
		}

		toggleEffects = false;
		//Editor::GetInstance().isPlaying ? toggleEffects = true : toggleEffects = false;//reseting the toggle effects if it is not playing 
	}

	m_PreProcessingFrame->Bind();//binding the pre processing 
}
void PostProcessing::unBindPreProessing() {
	m_PreProcessingFrame->UnBind();//unbind the pre processing 
}

void PostProcessing::bindPostProcessing() {
	m_PostProcessingFrame->Bind();//binding the post processing 
}
void PostProcessing::unBindPostProcessing() {
	m_PostProcessingFrame->UnBind();//unbind the post processing 
}