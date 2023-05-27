// /******************************************************************************/
// /*!
// \file         DialogueManagerComponent.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         2nd March, 2022 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the dialogue component
// \lines        
// 
// All content :copyright: 2022 DigiPen (SINGAPORE) Corporation, all rights
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
bool Com_DialogueManager::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<Com_DialogueManager>(toSave)) //Prevent saving non-existent data
	{
		const Com_DialogueManager& c = ecs.getComponent<Com_DialogueManager>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct DialogueManager");
		{
			writer->StartObject();
			writer->Key("Dialogue Storage");
			writer->String(c.dialogueStorage.c_str()); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->Key("Dialogue Soeed");
			writer->Double(c.textSpeed); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->EndObject();
		}
	}
	return true;
}

//Load component data
bool Com_DialogueManager::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct DialogueManager") &&
		data["struct DialogueManager"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct DialogueManager"];
		sm.DeserializeString("Dialogue Storage", dialogueStorage, detail);
		sm.DeserializeFloat("Dialogue Soeed", textSpeed, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

//Add Entity into ECS
void Com_DialogueManager::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_DialogueManager::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_DialogueManager>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Dialogue Manager ##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be changed to something else soon");
				ImGui::Text("When the player enters the hit box, it shows a preset dialogue");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_DialogueManager>(selected);

			ImGui::Text("Dialogue Text");
			//Technique inspiration:
			//https://stackoverflow.com/a/7352137
			size_t constexpr max = 512; //Maximum text length
			static char textCStr[max];
			std::copy(t.dialogueStorage.begin(),
				t.dialogueStorage.length() >= max ?
				t.dialogueStorage.begin() + max :
				t.dialogueStorage.end(), //Does not take into account null terminator
				textCStr);
			textCStr[t.dialogueStorage.length()] = '\0'; //Null terminator
			//If the input text is modified, modify the entity's name
			if (ImGui::InputText("##Dialogue Texture Target", textCStr, max))
			{
				t.dialogueStorage = std::string{ textCStr };
			}


			//dialogue text speed 
			ImGui::Text("Dialogue Text Speed");
			ImGuiHelpers::ImGuiDragFloatSpecial("##Offset X", &(t.textSpeed));

		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be changed to something else soon");
				ImGui::Text("When the player enters the hit box, it shows a preset dialogue");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			///Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Com_DialogueManager>(selected))
				ecs.loseEntitySelection();
		}
	}
}

//To render within the component inspector
//As part of a list of what components can be added into an entity
//Rename all instances of "component" accordingly
void Com_DialogueManager::listView()
{
	//If you don't want to make it possible for other entities to add
	//this component to an entity, omit all code within this function

	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_DialogueManager>(selected))
	{
		if (ImGui::Selectable("Dialogue Manager##componentList"))
		{
			//Add component
			ecs.addComponent<Com_DialogueManager>(ecs.whichEntityIsSelected(),
				Com_DialogueManager{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be changed to something else soon");
			ImGui::Text("When the player enters the hit box, it shows a preset dialogue");
			ImGui::EndTooltip();
		}
	}
}