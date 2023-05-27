// /******************************************************************************/
// /*!
// \file         texture.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions of the texture class 
// \lines        46 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Texture::Texture(const std::string& path) : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr) , m_Width(0), m_Height(0) , m_BPP(0) {


	/*
	You probably noticed that the texture is flipped upside-down! This happens because OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image, 
	but images usually have 0.0 at the top of the y-axis. Luckily for us, stb_image.h can flip the y-axis during image loading by adding the following statement before loading any image:*/
	//flips the texture vertically, because opengl expects it to starts vertically 
	stbi_set_flip_vertically_on_load(1);


	//4 for the RGBA 
	m_LocalBuffer = stbi_load(path.c_str(),&m_Width, &m_Height, &m_BPP, 4); //loading of the image using library stbi 

	/*Generating textures
	*/

	//bind textures and generate textures 
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	
	/*Texture Filtering 
	*/

	//settings for the texture, you need to specify this else you might get a black texture 
	GLCall((glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)));
	GLCall((glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)));

	/*
	Texture Wrapping 
	*/
	GLCall((glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))); //for x
	GLCall((glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))); //for y 

	//send to open gl
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	//if local buffer contains data, free it 
	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer); //free local buffer
	}
}

Texture::~Texture() {
	GLCall(glDeleteTextures(1,&m_RendererID));
}

//this is to bind the texture 
void Texture::Bind(unsigned int slot) const {
	//binding the textures 
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

//this is to unload the texture 
void Texture::Unbind() const {
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}


unsigned int Texture::GetRendererID() {
	return m_RendererID;//returns the render id
}


GLuint Texture::LoadTexture(const std::string& path) {
	//width, height and bits of the texture 
	int w, h, bits;

	stbi_set_flip_vertically_on_load(1);//stbi to flip the image 
	auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, 4); //loading in the pixels 
	GLuint textureID; //texture id 
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);//creating texture 
	glBindTexture(GL_TEXTURE_2D, textureID);//binding texture to the texture id 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);//free stb buffer 

	return textureID;
}


std::string Texture::GetFilePath() const {
	return m_FilePath;//returns the file path 
}