// /******************************************************************************/
// /*!
// \file         Sprite.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the definitions of the sprite class which loads in the sprite and calculates the relative texture coordinates 
// \lines        74 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Sprite.h"
#include "../../AssetManager/AssetManager.h"//asset manager 

//loads in the sprite 
unsigned int Sprite::LoadSprite(std::string path, int cols, int rows, float pspeed) {
	
	static_cast<void>(pspeed); //unreferenced 

	m_NumOfSpritesRow = rows;//getting the rows
	m_NumOfSpritesColumn = cols;//getting the columns the sprite sheet have 
	m_SpriteWidth = static_cast<float>(AssetManager::GetInstance().textureMap.find(path)->second->GetWidth() / cols); //getting the size of the sprite width 
	m_SpriteHeight = static_cast<float>(AssetManager::GetInstance().textureMap.find(path)->second->GetHeight() / rows); //getting the size of the sprite height 
	m_Animated = true;// make the sprite animate 

	float incrementheight = m_SpriteHeight / AssetManager::GetInstance().textureMap.find(path)->second->GetWidth();
	float incrementwidth = m_SpriteWidth / AssetManager::GetInstance().textureMap.find(path)->second->GetHeight();

	//calculating texture coords 
	int i = 0;
	for (auto& spriteElements : m_TextureCoordsRow1) {
		spriteElements[0].y = 0.0f;  //0 left side 
		spriteElements[1].y = 0.0f; //1 right side

		spriteElements[2].y = incrementheight;//2 top right 
		spriteElements[3].y = incrementheight;//3 top left 

		spriteElements[0].x = 0.0f + (i * incrementwidth); //0 left side 
		spriteElements[1].x = incrementwidth + (i * incrementwidth); //1 right side

		spriteElements[2].x = incrementwidth + (i * incrementwidth); //2 top right 
		spriteElements[3].x = 0.0f + (i * incrementwidth);  //3 top left 

		++i;//increment i 
	}	

	i = 0;//reset i 
	for (auto& spriteElements : m_TextureCoordsRow2) {
		spriteElements[0].y = (incrementheight);  //0 left side 
		spriteElements[1].y = (incrementheight); //1 right side

		spriteElements[2].y = incrementheight * 2 ;//2 top right 
		spriteElements[3].y = incrementheight * 2;//3 top left 

		spriteElements[0].x = 0.0f + (i * incrementwidth); //0 left side 
		spriteElements[1].x = incrementwidth + (i * incrementwidth); //1 right side

		spriteElements[2].x = incrementwidth + (i * incrementwidth); //2 top right 
		spriteElements[3].x = 0.0f + (i * incrementwidth);  //3 top left 

		++i;//increment i 
	}	

	i = 0;//reset i 
	for (auto& spriteElements : m_TextureCoordsRow3) {
		spriteElements[0].y = (incrementheight) * 2;  //0 left side 
		spriteElements[1].y = (incrementheight) * 2; //1 right side

		spriteElements[2].y = incrementheight * 3;//2 top right 
		spriteElements[3].y = incrementheight * 3;//3 top left 

		spriteElements[0].x = 0.0f + (i * incrementwidth); //0 left side 
		spriteElements[1].x = incrementwidth + (i * incrementwidth); //1 right side

		spriteElements[2].x = incrementwidth + (i * incrementwidth); //2 top right 
		spriteElements[3].x = 0.0f + (i * incrementwidth);  //3 top left 

		++i;//increment i 
	}	

	i = 0;//reset i 
	for (auto& spriteElements : m_TextureCoordsRow4) {
		spriteElements[0].y = incrementheight * 3;  //0 left side 
		spriteElements[1].y = incrementheight * 3; //1 right side

		spriteElements[2].y = incrementheight * 4;//2 top right 
		spriteElements[3].y = incrementheight * 4;//3 top left 

		spriteElements[0].x = 0.0f + (i * incrementwidth); //0 left side 
		spriteElements[1].x = incrementwidth + (i * incrementwidth); //1 right side

		spriteElements[2].x = incrementwidth + (i * incrementwidth); //2 top right 
		spriteElements[3].x = 0.0f + (i * incrementwidth);  //3 top left 

		++i;//increment i 
	}



	return AssetManager::GetInstance().textureMap.find(path)->second->GetRendererID();
}
