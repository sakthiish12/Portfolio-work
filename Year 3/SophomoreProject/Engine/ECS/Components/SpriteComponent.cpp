// /******************************************************************************/
// /*!
// \file         SpriteComponent.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for entity's image texture
//				 which may be animated or not
// \lines        ??? Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "../component.h"
#include "../../ECS/ECS.h"
#include "../../pch.h"
#include "../../Editor/UI Layer/ImGuiHelpers.h"
#include "../../AssetManager/AssetManager.h"
#pragma warning(disable : 4244)

//SERIALIZATION////////////////////////////////////////////////////////////////

bool spriteCom::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<spriteCom>(toSave))
	{
		const spriteCom& c = ecs.getComponent<spriteCom>(toSave);
		writer->Key("struct spriteCom");
		{
			writer->StartObject();
			writer->Key("textureId");
			writer->String(c.textureId.c_str());
			writer->Key("textureSlotsId");
			writer->Double(static_cast<double>(c.textureSlotsId));
			writer->Key("loop");
			writer->Bool(c.loop);
			writer->Key("toLoaded");
			writer->Bool(true);
			writer->Key("rows");
			writer->Int(c.rows);
			writer->Key("columns");
			writer->Int(c.columns);
			writer->Key("speed");
			writer->Double(static_cast<double>(c.speed));			
			writer->Key("isVisible");
			writer->Bool(c.isVisible);
			writer->Key("animationFilePath");
			writer->String(c.animationFilePath.c_str());
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
bool spriteCom::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct spriteCom") &&
		data["struct spriteCom"].IsObject())
	{
		const rapidjson::Value& detail = data["struct spriteCom"];
		sm.DeserializeString("textureId", textureId, detail);
		sm.DeserializeFloat("textureSlotsId", textureSlotsId, detail);
		sm.DeserializeBool("loop", loop, detail);
		sm.DeserializeBool("toLoaded", toLoaded, detail);
		sm.DeserializeInt("rows", rows, detail);
		sm.DeserializeInt("columns", columns, detail);
		sm.DeserializeFloat("speed", speed, detail);
		sm.DeserializeBool("isVisible", isVisible, detail);
		sm.DeserializeString("animationFilePath", animationFilePath, detail);
		return true;
	}
	return false;
}

void spriteCom::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void spriteCom::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<spriteCom>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Sprite##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The texture applied onto the entity");
				ImGui::Text("May or may not be animated");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<spriteCom>(selected);
				
			//Texture Path
			ImGui::Text("Texture File: %s", t.textureId.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The file location of the texture");
				ImGui::EndTooltip();
			}

			static bool textureErrorJpeg = false;//for the jpeg error
			static bool textureErrorUnloaded = false;//for the texture unloaded error 
			static std::string errorFile;//error file string 

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payLoad->Data;
					std::wstring wTempString(path);
					std::string pathString(wTempString.begin(), wTempString.end());//getting the string of the texture 
					//seperate out 
					pathString = pathString.substr(pathString.find_last_of("\\"));
					pathString = pathString.substr(pathString.find_first_of("qwertyuiopasdfhgjklzxcvnbmQWERTYUIOPASDFGHJKLZXCVBNM"));

					//if it's a texture file, load it, else ignore 
					if (pathString.find(".png") != std::string::npos && AssetManager::GetInstance().textureMap.find(pathString) != AssetManager::GetInstance().textureMap.end()) {
						//update the texture 
						t.textureId = pathString;
					}
					//if it's a texture file jpeg, reject and give warning! 
					if (pathString.find(".jpeg") != std::string::npos || pathString.find(".jpg") != std::string::npos) {
						//error message! 
						errorFile = pathString;
						textureErrorJpeg = true;
					}
					//if it's a texture file is png but it's not loaded 
					if (pathString.find(".png") != std::string::npos && AssetManager::GetInstance().textureMap.find(pathString) == AssetManager::GetInstance().textureMap.end()) {
						//error message! -> not in the right directory or not reloaded during run time 
						errorFile = pathString;
						textureErrorUnloaded = true;
					}
				}
				ImGui::EndDragDropTarget();
			}

			//pop up error message for jpeg file but not loaded 
			if (textureErrorJpeg) {
				ImGui::OpenPopup("Error Loading JPEG");
				ImGui::BeginPopup("Error Loading JPEG");
				ImGui::Text("Texture File: % s | JPEG is not supported!", errorFile.c_str());
				//button for validation 
				if (ImGui::Button("Ok")) {
					textureErrorJpeg = false;
				}
				ImGui::Separator();
				ImGui::EndPopup();
			}


			//pop up error message for png file but not loaded 
			if (textureErrorUnloaded) {
				ImGui::OpenPopup("Error Loading Texture");
				ImGui::BeginPopup("Error Loading Texture");
				ImGui::Text("Texture File: % s HAS NOT BEEN RELOADED/NOT IN THE CORRECT TEXTURE DIRECTORY", errorFile.c_str());
				//button for validation 
				if (ImGui::Button("Ok")) {
					textureErrorUnloaded = false;
				}
				ImGui::Separator();
				ImGui::EndPopup();
			}

			if (ImGui::Button("Change Texture##texture")) {
				ImGui::OpenPopup("Texture Selector");
			}
			if (ImGui::BeginPopup("Texture Selector"))
			{
				for (auto& tex : AssetManager::GetInstance().textureMap)
					if (ImGui::Selectable(tex.first.c_str(), false))
						t.textureId = tex.first;
				ImGui::EndPopup();
			}

			//isVisible?
			ImGui::Text("Visible?: %s", t.isVisible ? "True" : "False");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Is the sprite visible?");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Toggle##Visibility of Sprite"))
				t.isVisible = !t.isVisible;

			//Animated?
			ImGui::Text("Animated?: %s", t.loop ? "True" : "False");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("If it's an animated sprite or not?");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Toggle##Looping Sprite"))
				t.loop = !t.loop;

			//animation Path
			ImGui::Text("Animation File: %s", t.animationFilePath.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The file location of the animation data");
				ImGui::EndTooltip();
			}

			//animation drag and drop 
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payLoad->Data;
					std::wstring wTempString(path);
					std::string pathString(wTempString.begin(), wTempString.end());//getting the string of the texture 
					//seperate out 
					pathString = pathString.substr(pathString.find_last_of("\\"));
					pathString = pathString.substr(pathString.find_first_of("qwertyuiopasdfhgjklzxcvnbmQWERTYUIOPASDFGHJKLZXCVBNM"));


					//if it's a texture file, load it, else ignore 
					if (pathString.find(".json") != std::string::npos && 
						(std::find(AssetManager::GetInstance().animationLoaded.begin(), AssetManager::GetInstance().animationLoaded.end(),pathString) != AssetManager::GetInstance().animationLoaded.end())) {
						//update the texture 
						t.animationFilePath = pathString;
					}
				}
				ImGui::EndDragDropTarget();
			}

			//animation path selector 
			if (ImGui::Button("Change Texture##Animation")) {
				ImGui::OpenPopup("Animation Selector");
			}
			if (ImGui::BeginPopup("Animation Selector"))
			{
				for (auto& animation : AssetManager::GetInstance().animationLoaded)
					if (ImGui::Selectable(animation.c_str(), false))
						t.animationFilePath = animation.c_str();
				ImGui::EndPopup();
			}

		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The texture applied onto the entity");
				ImGui::Text("May or may not be animated");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<spriteCom>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void spriteCom::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<spriteCom>(selected))
	{
		if (ImGui::Selectable("Sprite##componentList"))
		{
			//Add component
			ecs.addComponent<spriteCom>(ecs.whichEntityIsSelected(),
				spriteCom{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be implemented");
			ImGui::EndTooltip();
		}
	}
}

void spriteCom::setAnimation(std::string const& newAnimation)
{
	animationToLoad = newAnimation;

	//Managed to find animation
	if (animationData.find(newAnimation) != animationData.end())
	{
		currentAnimation = animationData.find(newAnimation)->second;
		currentFrame = 0;
		currentTexCoords = frameData[currentAnimation[currentFrame]];
		//isVisible = true;
		//toLoaded = false;
		animationLoaded = true;
	}
	else //Did not manage to find animation, stop animation and display whole texture
	{
		//std::cout << "(SpriteComponent.cpp) ANIMATION " << newAnimation << " FAILED TO FIND!\n";
		//loop = false;
		currentTexCoords[0] = Mathf::Vector2(0.0f, 0.0f);
		currentTexCoords[1] = Mathf::Vector2(1.0f, 0.0f);
		currentTexCoords[2] = Mathf::Vector2(1.0f, 1.0f);
		currentTexCoords[3] = Mathf::Vector2(0.0f, 1.0f);
		//isVisible = false;
		//toLoaded = true;
		animationLoaded = false;
	}
}