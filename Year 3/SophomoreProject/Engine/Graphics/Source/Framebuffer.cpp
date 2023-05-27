// /******************************************************************************/
// /*!
// \file         Framebuffer.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the framebuffer
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Framebuffer.h"//frame buffer 
#include "Renderer.h"//renderer 
#include "../../Debugging/Assert.h"//asserts 
#include "../../WinMgr/WinMgr.h"//window manager 
#include "../../AssetManager/AssetManager.h"//asset manager 


Framebuffer::Framebuffer() {

	//set the initial framebuffer width & height 
	m_Specification.height = Framebuffer::framebufferHeight;
	m_Specification.width = Framebuffer::framebufferWidth; 

	glCreateFramebuffers(1, &m_RendererID);//create framebuffer 
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//bind frame buffer 

	//color attachment 
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);//create texture 
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);//bind texture 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.width, m_Specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

	//deoth attachment 
	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);//create texture with depth 
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);//bind 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height, 0,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

	PartyAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);//bind frame buffer

}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &m_RendererID);//delete the framebuffer 
	glDeleteTextures(1, &m_ColorAttachment);//delete color attachment 
	glDeleteTextures(1, &m_DepthAttachment);//delete depth attachments
}


void Framebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//bind frame buffer 
	glViewport(0, 0, m_Specification.width, m_Specification.height);//changes the view port
}


void Framebuffer::UnBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//unbind frame buffer 
}


uint32_t Framebuffer::GetColorAttachmentRenderID() const {
	return m_ColorAttachment;//return the color attachment
}


void Framebuffer::Resize(const uint32_t& width ,const uint32_t& height) {
	if (WindowsManager.isMinimised()) {
		//set default width & height 
		m_Specification.height = Framebuffer::framebufferHeight;
		m_Specification.width = Framebuffer::framebufferWidth;
	}
	else {
		//update the specification widhth & height 
		m_Specification.width = width;
		m_Specification.height = height;
	}

	//if there is an existing framebuffer, delete it 
	if (m_RendererID) {
		glDeleteFramebuffers(1, &m_RendererID);//delete the framebuffer 
		glDeleteTextures(1, &m_ColorAttachment);//delete color attachment 
		glDeleteTextures(1, &m_DepthAttachment);//delete depth attachments
	}
	//create a new one 
	glCreateFramebuffers(1, &m_RendererID);//create framebuffer 
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);//bind frame buffer 

	//color attachment 
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);//create texture 
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);//bind texture 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.width, m_Specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

	//deoth attachment 
	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);//create texture with depth 
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);//bind 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height, 0,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

	PartyAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);//bind frame buffer

}


glm::vec2 Framebuffer::GetViewportSize() const {
	return glm::vec2{ m_Specification.width,m_Specification.height };
}


void Framebuffer::DrawFramebuffer() const {
	m_FramebufferShader->Bind();//binding the framebuffer shader 
	glBindVertexArray(this->VAO);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

