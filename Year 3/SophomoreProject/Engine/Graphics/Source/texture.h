// /******************************************************************************/
// /*!
// \file         Texture.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declaration of the class texture which loads in the texture and binds the texture accordingly 
// \lines        21 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "Renderer.h"

class Texture {
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;


public:
	Texture(const std::string& path);
	//Texture(const std::string& path,unsigned int& textureid);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }//getting the width of the texture 
	inline int GetHeight() const { return m_Height; }//getting the height of the texture 

	static GLuint LoadTexture(const std::string& path);//loads in the texture 

	std::string GetFilePath() const;//getting the file path of the texture 
	unsigned int GetRendererID();//returns the texture id 
};
#endif // !TEXTURE_H
