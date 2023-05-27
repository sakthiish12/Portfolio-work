// /******************************************************************************/
// /*!
// \file         Framebuffer.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations for the framebuffer
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <iostream>
#include "../../Dependencies/Vendor/glm/vec2.hpp"//vec2 glm 
#include "Shader.h"//shader 

struct FramebufferSpecification {
	uint32_t width, height;
	//framebuffer format 
	uint32_t samples = 1; 

	bool swapChainTarget = false;//framebuffer unbind 0, if it's false you render to the screen without framebuffer
};


class Framebuffer {

public:
	Framebuffer();//ctor 
	~Framebuffer();//dtor 

	void Bind(); //bind framebuffer
	void UnBind(); //unbind framebuffer 

	void Resize(const uint32_t& width, const uint32_t& height);//resize the framebuffer 

	glm::vec2 GetViewportSize() const; //getting current viewport size 

	uint32_t GetColorAttachmentRenderID() const;

	//inline static Framebuffer& GetInstance(){ static Framebuffer instance; return instance; }//for testing 

	void DrawFramebuffer() const;


	std::unique_ptr<Shader> m_FramebufferShader;//frame buffer post processing shader 

	unsigned int VAO;//vertex array object 

private:
	const uint32_t framebufferHeight = 1280;
	const uint32_t framebufferWidth = 780;

	FramebufferSpecification m_Specification;//specification of the frame buffer 
	uint32_t m_DepthAttachment = 0;//the depth attachment of the framebuffer 
	uint32_t m_ColorAttachment = 0;//the color attachement of framebuffer 
	uint32_t m_RendererID = 0;//id 
};



#endif // !FRAMEBUFFER_H
