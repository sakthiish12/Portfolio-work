// /******************************************************************************/
// /*!
// \file         LevelLoaderButtonComponent.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for a button that
//				 loads a level, uses transform to determine clicking bounds
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

//SERIALIZATION////////////////////////////////////////////////////////////////

//Save component data
bool levelLoaderButton::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<levelLoaderButton>(toSave)) //Prevent saving non-existent data
	{
		const levelLoaderButton& c = ecs.getComponent<levelLoaderButton>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct levelLoaderButton");
		{
			writer->StartObject();
			writer->Key("Target level");
			writer->String(c.targetLevelFile.c_str()); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->EndObject();
		}
	}
	return true;
}

//Load component data
bool levelLoaderButton::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct levelLoaderButton") &&
		data["struct levelLoaderButton"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct levelLoaderButton"];
		sm.DeserializeString("Target level", targetLevelFile, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

//Add Entity into ECS
void levelLoaderButton::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void levelLoaderButton::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<levelLoaderButton>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Level Loader##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be changed to something else soon");
				ImGui::Text("When the player click this button while playing, loads a level as stated in the string");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<levelLoaderButton>(selected);

			ImGui::Text("Target Level");
			//Technique inspiration:
			//https://stackoverflow.com/a/7352137
			size_t constexpr max = 256; //Maximum text length
			static char textCStr[max];
			std::copy(t.targetLevelFile.begin(),
				t.targetLevelFile.length() >= max ?
				t.targetLevelFile.begin() + max :
				t.targetLevelFile.end(), //Does not take into account null terminator
				textCStr);
			textCStr[t.targetLevelFile.length()] = '\0'; //Null terminator
			//If the input text is modified, modify the entity's name
			if (ImGui::InputText("##LevelLoaderTarget", textCStr, max))
			{
				t.targetLevelFile = std::string{ textCStr };
			}
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be changed to something else soon");
				ImGui::Text("When the player click this button while playing, loads a level as stated in the string");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			///Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<levelLoaderButton>(selected))
				ecs.loseEntitySelection();
		}
	}
}

//To render within the component inspector
//As part of a list of what components can be added into an entity
//Rename all instances of "component" accordingly
void levelLoaderButton::listView()
{
	//If you don't want to make it possible for other entities to add
	//this component to an entity, omit all code within this function

	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<levelLoaderButton>(selected))
	{
		if (ImGui::Selectable("Level Loader##componentList"))
		{
			//Add component
			ecs.addComponent<levelLoaderButton>(ecs.whichEntityIsSelected(),
				levelLoaderButton{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be changed to something else soon");
			ImGui::Text("When the player click this button while playing, loads a level as stated in the string");
			ImGui::EndTooltip();
		}
	}
}