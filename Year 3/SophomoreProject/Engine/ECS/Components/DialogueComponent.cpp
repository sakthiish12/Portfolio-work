// /******************************************************************************/
// /*!
// \file         DialogueComponent.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the dialogue component
// \lines        
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
bool Com_Dialogue::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<Com_Dialogue>(toSave)) //Prevent saving non-existent data
	{
		const Com_Dialogue& c = ecs.getComponent<Com_Dialogue>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct Dialogue");
		{
			writer->StartObject();
			writer->Key("Dialogue Texture");
			writer->String(c.dialogueTexture.c_str()); //It is important to prefix with "c." to get the data from the correct entity that is to be saved			
			writer->Key("Dialogue Text");
			writer->String(c.dialogueText.c_str()); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->Key("X OffSet");
			writer->Double(c.offSetXPos);
			writer->Key("Y OffSet");
			writer->Double(c.offSetYPos);
			writer->EndObject();
		}
	}
	return true;
}

//Load component data
bool Com_Dialogue::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct Dialogue") &&
		data["struct Dialogue"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct Dialogue"];
		sm.DeserializeString("Dialogue Texture", dialogueTexture, detail);
		sm.DeserializeString("Dialogue Text", dialogueText, detail);
		sm.DeserializeFloat("X OffSet", offSetXPos, detail);
		sm.DeserializeFloat("Y OffSet", offSetYPos, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

//Add Entity into ECS
void Com_Dialogue::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_Dialogue::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_Dialogue>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Dialogue ##Component", &notToDelete))
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
			auto& t = ecs.getComponent<Com_Dialogue>(selected);

			ImGui::Text("Dialogue Texture");
			//Technique inspiration:
			//https://stackoverflow.com/a/7352137
			size_t constexpr max = 256; //Maximum text length
			static char textCStr[max];
			std::copy(t.dialogueTexture.begin(),
				t.dialogueTexture.length() >= max ?
				t.dialogueTexture.begin() + max :
				t.dialogueTexture.end(), //Does not take into account null terminator
				textCStr);
			textCStr[t.dialogueTexture.length()] = '\0'; //Null terminator
			//If the input text is modified, modify the entity's name
			if (ImGui::InputText("##Dialogue Texture Target", textCStr, max))
			{
				t.dialogueTexture = std::string{ textCStr };
			}

			ImGui::Text("Dialogue Text");
			//Technique inspiration:
			//https://stackoverflow.com/a/7352137
			size_t constexpr max2 = 256; //Maximum text length
			static char textCStr2[max2];
			std::copy(t.dialogueText.begin(),
				t.dialogueText.length() >= max2 ?
				t.dialogueText.begin() + max2 :
				t.dialogueText.end(), //Does not take into account null terminator
				textCStr2);
			textCStr2[t.dialogueText.length()] = '\0'; //Null terminator
			//If the input text is modified, modify the entity's name
			if (ImGui::InputText("##Dialogue Text Target", textCStr2, max2))
			{
				t.dialogueText = std::string{ textCStr2 };
			}



			//x off set 
			ImGui::Text("Dialogue OffSet X");
			ImGuiHelpers::ImGuiDragFloatSpecial("##Offset X", &(t.offSetXPos));			
			
			//Y off set 
			ImGui::Text("Dialogue OffSet Y");
			ImGuiHelpers::ImGuiDragFloatSpecial("##Offset Y", &(t.offSetYPos));
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
			if (ecs.removeComponent<Com_Dialogue>(selected))
				ecs.loseEntitySelection();
		}
	}
}

//To render within the component inspector
//As part of a list of what components can be added into an entity
//Rename all instances of "component" accordingly
void Com_Dialogue::listView()
{
	//If you don't want to make it possible for other entities to add
	//this component to an entity, omit all code within this function

	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_Dialogue>(selected))
	{
		if (ImGui::Selectable("Dialogue Texture##componentList"))
		{
			//Add component
			ecs.addComponent<Com_Dialogue>(ecs.whichEntityIsSelected(),
				Com_Dialogue{});
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