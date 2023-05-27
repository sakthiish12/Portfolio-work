// /******************************************************************************/
// /*!
// \file         AssetManager.h
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         13th April, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declaration for the asset manager 
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef ASSET_MANAGER
#define ASSET_MANAGER


#include "../../Serializer/SerializeManager.h" //for serializing data
#include "../../Debugging/Log.h" //for logging data 
#include "../../Debugging/Assert.h"//for asserts 
#include "../Graphics/Source/texture.h"//for the texture loading 

#include <unordered_map>//for the unordered map 
#include "../pch.h"//pch 
#include <filesystem>//file system 
#include "../../Graphics/Source/texture.h"//for the texture loading 
#include <future>//multithreading future 

using RefMesh = std::unordered_map<std::string, std::array<std::vector<float>, 2>>;

/********************
@ Brief - In charge of managing all assets of game 
*******************/
class AssetManager{
public:
	static AssetManager& GetInstance(); //singleton get instance 
	AssetManager(); //cstor
	~AssetManager() = default; //dstor 
	
	//meshes 
	void LoadMeshes(); //loading in mesh 
	void FlushMeshes(); //unload all mesh 
	RefMesh GetMeshes() const;

	//objects
	void LoadObjects();//load in objects
	void FlushObjects(); //flush the objects 

	//textures/sprites
	void LoadSprites();//loading of the sprites 
	unsigned int GetNumSpritesBinded() const; //getting the number of sprites binded 

	//shader program 
	void LoadShaders();//load in all the shader programs 

	//Editor Textures
	void LoadEditorTextures();//loads the editor textures

	//animations 
	void LoadAnimations();//loads the animations

	//file directory 
	std::filesystem::path GetCurrentFileDirectory() const;//gets the current asset directory, getter
	void SetCurrentFileDirectory(const std::filesystem::path& filename);//set the current asset directory, settter 
	void SetParentFileDirectory(const std::filesystem::path& filename);//set the current directory to the parent 

	//sound/music 
	void LoadSounds() const;

	//loading fonts
	void LoadFonts() const;

	//tracks the enemies collected
	size_t tomatoesCollected = 0;
	size_t mushroomsCollected = 0;
	size_t chilliesCollected = 0;

	bool blenderCollected = false;//tracks if the blender is collected 
public:

	struct GLMesh {
		unsigned int primitiveType; //primitive type of the model 
		unsigned int primitiveCount; //primitve count 
		unsigned int vaoid; //vertex array object id 
		unsigned int drawCount;//draw counts 
	};
	const std::filesystem::path m_StartingFileDirectory = "Resources";

	std::unordered_map<std::string, std::unique_ptr<Shader>> shaderProg; //map of shader programs
	std::unordered_map<std::string, std::unique_ptr<Texture>> textureMap;//map of the textures 
	std::vector<std::string> animationLoaded;//stores all the file path of the animations loaded 
	std::unordered_map<std::string, std::unique_ptr<Texture>> iconTextureMap;//map of icon textures 
	std::unordered_map<std::string, float> textureSlotMap;//map of the texture slots 
	RefMesh meshesloaded;//unordered map with key "name" , array of 2 with vector of string 

	//std::vector<unsigned int>textureSlot;//stores all the texture slots for the game 
	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<size_t>>> animationDataGame;//stores all the animation data 
	std::unordered_map<std::string, std::vector<std::array<Mathf::Vector2, 4>>> frameData;//stores the four corners for all frames in the sprite sheet

private:
	std::filesystem::path m_CurrentDirectory; //stores the path to the current directory 
	unsigned int numOfTexturesBinded{0};//indicates the number of textures binded 
	std::vector<std::future<void>> m_Futures;

	const std::string textureResourcePath = "Resources/textures/CurrentTexture/";//getting the texture path 
	const std::string shaderResourcePath = "Graphics/Resources/shaders/";//getting the shader path 
	const std::string iconResourcePath = "Resources/Icons/";
	const std::string animationResourcePath = "Resources/Animations/";//getting the animations path 
};

#endif // ! ASSET_MANAGER
