// /******************************************************************************/
// /*!
// \file         Sprite.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations of the class sprite 
// \lines        36 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef SPRITE_H
#define SPRITE_H

#include <array>
#include "texture.h"//texture 
#include "../../Math/Vector/Vector4.h"//vector 4 
#include "../../Math/Vector/Vector2.h"//vector 2 
#include "../../Dependencies/Vendor/stb_image.h"//stb image 


class Sprite{

public:
	Sprite() = default; //cstor
	~Sprite() = default; //dstor 

	unsigned int LoadSprite(std::string path,int cols, int rows, float speed );//loads the sprite in



	//get
	inline std::array<std::array< Mathf::Vector2, 4 >, 4> GetRow1TexCoords() const { return m_TextureCoordsRow1; }//getting the texture coords of row 1 
	inline std::array<std::array< Mathf::Vector2, 4 >, 4> GetRow2TexCoords() const { return m_TextureCoordsRow2; }//getting the texture coords of row 1 
	inline std::array<std::array< Mathf::Vector2, 4 >, 4> GetRow3TexCoords() const { return m_TextureCoordsRow3; }//getting the texture coords of row 1 
	inline std::array<std::array< Mathf::Vector2, 4 >, 4> GetRow4TexCoords() const { return m_TextureCoordsRow4; }//getting the texture coords of row 1 
	inline Mathf::Vector4 GetTexCoords() const { return spriteTexCoords; } //getting the texture coords of the sprite 


private:
	Mathf::Vector4 spriteTexCoords;//stores the 4 texture coords of the sprite

	int m_NumOfSpritesRow = 4;//indicates the number of sprites for the row 
	int m_NumOfSpritesColumn = 4;//indicate the number of sprites for the column

	float m_SpriteWidth;//stores the sprite width 
	float m_SpriteHeight;//stores the sprite height 

	//float offSetStartingPos; //offset of the starting position of the frame 
	bool visible;// true if visible 
	int speed = 100;//delay between frames in ms 
	bool m_Animated = false;//loop sprite sheet (false by default)


	//from the texture 
	unsigned int m_SpriteId;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_TextureWidth, m_TextureHeight, m_BPP;//width, height and pixels data 


	//exclusive for texture coords (we seperate the different types by rows) 
	std::array<std::array< Mathf::Vector2, 4 >, 4> m_TextureCoordsRow1; 
	std::array<std::array< Mathf::Vector2, 4 >, 4> m_TextureCoordsRow2;
	std::array<std::array< Mathf::Vector2, 4 >, 4> m_TextureCoordsRow3;
	std::array<std::array< Mathf::Vector2, 4 >, 4> m_TextureCoordsRow4;
	
};

#endif // !SPRITE_H

