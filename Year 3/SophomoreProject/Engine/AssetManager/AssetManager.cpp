// /******************************************************************************/
// /*!
// \file         AssetManager.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         13th April, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the AssetManager
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "AssetManager.h"//asset manager h
#include "../Sound/SoundManager.h"//sound manager h 
#include <filesystem> //for file system 
#include "../Graphics/Source/FontRendering.h"
#include <future>//multi threading 
#include "../../Serializer/Settings/AnimationDataSerializer.h"//animation data 

void AssetManager::LoadMeshes(){
	//loading in vertices 
	SerializeManager serializeManager;
	Mesh mesh;
	Log logger;
	serializeManager.DeserializeFromFile("Config/Meshes.json", mesh); //loading in the mesh 
	logger.Info(true, "Loading in Mesh..\n");//log loading in of mesh 

	//store the meshes into the static container
	std::vector<float> tmpindices;
	std::vector<float> tmpvertices;
	size_t iter = 0;

	//loading in of the circle mesh 
	tmpindices.resize(mesh.Circle_m_indices.size());//set to the size first 
	tmpvertices.resize(mesh.Circle_m_vertices.size());//set to the size 
	for (std::string& str : mesh.Circle_m_indices) {
		tmpindices[iter] = (std::stof(str));//make it into int  
		++iter;
	}
	iter = 0;//reset  
	for (std::string& str : mesh.Circle_m_vertices) {
		tmpvertices[iter] = (std::stof(str));//make it into float 
		++iter;
	}
	//load load mesh into unorderedmap 
	std::array<std::vector<float>, 2> tmparry;//tmp array 
	tmparry[0] = tmpvertices;//pass in vertices 
	tmparry[1] = tmpindices; //pass in indices 
	meshesloaded.emplace(std::make_pair("circle", tmparry));//emplace the circle 

	iter = 0;//reset 
	tmpindices.clear();
	tmpvertices.clear();
	tmparry[0].clear();
	tmparry[1].clear();

	//loading in of the square mesh 
	tmpindices.resize(mesh.Square_m_indices.size());//set to the size first 
	tmpvertices.resize(mesh.Square_m_vertices.size());//set to the size 
	for (std::string& str : mesh.Square_m_indices) {
		tmpindices[iter] = (std::stof(str));//make it into int  
		++iter;
	}
	iter = 0;//reset  
	for (std::string& str : mesh.Square_m_vertices) {
		tmpvertices[iter] = (std::stof(str));//make it into float 
		++iter;
	}
	//load load mesh into unorderedmap 
	tmparry[0] = tmpvertices;//pass in vertices 
	tmparry[1] = tmpindices; //pass in indices 
	meshesloaded.emplace(std::make_pair("square", tmparry));//emplace the circle 

	iter = 0;//reset 
	tmpindices.clear();
	tmpvertices.clear();
	tmparry[0].clear();
	tmparry[1].clear();

	//loading in the triangle mesh 
	tmpindices.resize(mesh.Triangle_m_indices.size());//set to the size first 
	tmpvertices.resize(mesh.Triangle_m_vertices.size());//set to the size 
	for (std::string& str : mesh.Triangle_m_indices) {
		tmpindices[iter] = (std::stof(str));//make it into int  
		++iter;
	}
	iter = 0;//reset  
	for (std::string& str : mesh.Triangle_m_vertices) {
		tmpvertices[iter] = (std::stof(str));//make it into float 
		++iter;
	}
	//load load mesh into unorderedmap 
	tmparry[0] = tmpvertices;//pass in vertices 
	tmparry[1] = tmpindices; //pass in indices 
	meshesloaded.emplace(std::make_pair("triangle", tmparry));//emplace the circle 

}
void AssetManager::FlushMeshes(){
	meshesloaded.clear();//flush the mesh 
}

RefMesh AssetManager::GetMeshes() const {
	return meshesloaded;//returns the loaded meshes 
}

void AssetManager::LoadObjects(){

}
void AssetManager::FlushObjects(){

}

AssetManager& AssetManager::GetInstance() {
	//getting the instance of the class
	static AssetManager instance;

	return instance;
}


AssetManager::AssetManager() : m_CurrentDirectory(m_StartingFileDirectory){
	LoadEditorTextures();//load the editor texture 
	LoadShaders();//load shader
	LoadFonts();//loads the fonts
	LoadSprites();//load the sprites 
	LoadAnimations();//load the animations
}


std::filesystem::path AssetManager::GetCurrentFileDirectory() const{
	return m_CurrentDirectory;//return the current asset directory
}

void AssetManager::SetCurrentFileDirectory(const std::filesystem::path& filename) {
	if (filename.has_relative_path()) {
		m_CurrentDirectory /= filename;//go into relative 
	}
}

void AssetManager::SetParentFileDirectory(const std::filesystem::path& filename) {
	m_CurrentDirectory = filename;//set the current directory name which is the parent file directory 
}

void AssetManager::LoadEditorTextures() {
	//loading in the icon 
	Log logger;
	logger.Info(true, "Loading Editor Icon Textures");

	for (const auto& directoryEntry : std::filesystem::directory_iterator(iconResourcePath)) {
		std::string iconPath = directoryEntry.path().string();//getting the shader path 

		std::string name = iconPath.substr(iconPath.find_last_of('/') + 1);//getting the name of the shader 
		iconTextureMap.emplace(name, std::make_unique<Texture>(iconPath));//emplace back into the map of shaders
	}

}

void AssetManager::LoadAnimations() {
	animationLoaded.clear();//clearing all the animation path 

	//loading in the animations
	Log logger;
	logger.Info(true, "Loading Animations file path");

	for (const auto& directoryEntry : std::filesystem::directory_iterator(animationResourcePath)) {
		SerializeManager sm;
		AnimationSerializer as;

		std::string animationPath = directoryEntry.path().string();//getting the animation path 

		std::string name = animationPath.substr(animationPath.find_last_of('/') + 1);//getting the name of the animation path 
		animationLoaded.emplace_back(name);//pushback into the vector of animation path 

		//load in animation data 
		sm.DeserializeFromFile(as.animationResourcePath + name, as);
		animationDataGame.emplace(std::make_pair(name, as.aniData));

		//load in frame data 
		frameData.emplace(std::make_pair(name, as.uvFramePoints));
	}
}


void AssetManager::LoadSounds() const {
	//SoundManager::GetInstance();//load all the sound 
}


void AssetManager::LoadShaders()  {

	//loading in the shaders 
	Log logger;
	logger.Info(true, "Loading Shaders");

	//clean the entire map
	shaderProg.clear();

	for (const auto& directoryEntry : std::filesystem::directory_iterator(shaderResourcePath)) {
		std::string shaderPath = directoryEntry.path().string();//getting the shader path 

		std::string name = shaderPath.substr(shaderPath.find_last_of('/')+1);//getting the name of the shader 
		shaderProg.emplace(name, std::make_unique<Shader>(shaderPath));//emplace back into the map of shaders
	}


}


void AssetManager::LoadFonts() const {

	//loading in fonts 
	Log logger;
	logger.Info(true, "Loading Fonts");

	TextRendering::TextRenderer::GetInstance().InitFont("Resources/fonts/ComicHelvetic_Heavy.otf");//load text renderer 
}

void AssetManager::LoadSprites() {
	textureMap.clear();
	textureSlotMap.clear();
	numOfTexturesBinded = 0;

	//loading in the textures to the texture slots
	Log logger;
	logger.Info(true, "Loading Textures");



	float i = 0.f;//start from 1
	for (const auto& directoryEntry : std::filesystem::directory_iterator(textureResourcePath)) {
		std::string texturePath = directoryEntry.path().string();//getting the shader path 

		std::string name = texturePath.substr(texturePath.find_last_of('/') + 1);//getting the name of the shader 
		textureMap.emplace(name, std::make_unique<Texture>(texturePath));//emplace back into the map of shaders
		textureSlotMap.emplace(name, i);
		i += 1.f;//increment 
		numOfTexturesBinded++;//increment the number of textures binded 
	}

}

unsigned int AssetManager::GetNumSpritesBinded() const {
	return numOfTexturesBinded;//returns the number of textures binded
}