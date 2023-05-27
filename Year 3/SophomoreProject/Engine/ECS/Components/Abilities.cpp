// /******************************************************************************/
// /*!
// \file         Abilities.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function declarations for the abilities component 
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


//TODO NO IMPLEMENTATION. DO NOT CALL
bool Com_Abilities::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<Com_Abilities>(toSave)) //Prevent saving non-existent data
	{
		const Com_Abilities& c = ecs.getComponent<Com_Abilities>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct abilities");
		{
			writer->StartObject();
			writer->Key("active abilities");
			writer->Int(c.typeAbilities); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->EndObject();
		}
	}
	return true;
}

//TODO NO IMPLEMENTATION. DO NOT CALL
bool Com_Abilities::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct abilities") &&
		data["struct abilities"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct abilities"];
		sm.DeserializeInt("active abilities", typeAbilities, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

void Com_Abilities::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//Render data into ImGui inspector
void Com_Abilities::inspectorView() {
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_Abilities>(selected))
	{
		bool notToDelete = false; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Abilities ##Component", &notToDelete))
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
			if (ImGui::InputText("##Abilities Target", textCStr, max))
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

//List component to add to entity within 'Add Component' menu 
void Com_Abilities::listView() {
	// If you don't want to make it possible for other entities to add
		//this component to an entity, omit all code within this function

		//Get the ID of the presently selected entity
		entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_Abilities>(selected))
	{
		if (ImGui::Selectable("Abilties ##componentList"))
		{
			//Add component
			ecs.addComponent<Com_Abilities>(ecs.whichEntityIsSelected(),
				Com_Abilities{});
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