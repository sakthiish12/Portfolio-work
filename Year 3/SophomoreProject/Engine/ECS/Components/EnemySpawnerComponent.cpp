// /******************************************************************************/
// /*!
// \file         EnemySpawnerComponent.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definition for the enemyspawner component
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
#include "../../Editor/UI Layer/ImGuiHelpers.h"

//TODO NO IMPLEMENTATION. DO NOT CALL
bool Com_EnemySpawner::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<Com_EnemySpawner>(toSave)) //Prevent saving non-existent data
	{
		const Com_EnemySpawner& c = ecs.getComponent<Com_EnemySpawner>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct EnemySpawner");
		{
			writer->StartObject();
			writer->Key("numOfSpawns");
			writer->Int(c.numOfSpawns); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->Key("timeDelay");
			writer->Double(c.timeDelay); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->Key("typeOfEnemy");
			writer->Int(c.typeOfEnemy); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->EndObject();
		}
	}
	return true;
}

//TODO NO IMPLEMENTATION. DO NOT CALL
bool Com_EnemySpawner::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct EnemySpawner") &&
		data["struct EnemySpawner"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct EnemySpawner"];
		sm.DeserializeInt("numOfSpawns", numOfSpawns, detail);
		sm.DeserializeFloat("timeDelay", timeDelay, detail);
		sm.DeserializeInt("typeOfEnemy", typeOfEnemy, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

void Com_EnemySpawner::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//Render data into ImGui inspector
void Com_EnemySpawner::inspectorView() {
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_EnemySpawner>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Enemy Spawner ##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be changed to something else soon");
				ImGui::Text("Enemy Spawner Tweaks");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_EnemySpawner>(selected);
			
			//time delay
			ImGui::Text("Time Delay");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Setting the time delay for each spawn");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiDragFloatSpecial("##Time Delay", &(t.timeDelay), 0.01f, true,0.f,200.f);


			//number of spawns
			ImGui::Text("Number of Spawns");
			ImGui::InputInt("##Number of Spawns", &(t.numOfSpawns));
			//Prevent divide by zero
			if (t.numOfSpawns <= 0)
				t.numOfSpawns = 1;

			//type of enemy
			ImGui::Text("Type of Enemy");
			ImGui::InputInt("##Type of Enemy", &(t.typeOfEnemy));
			//Prevent divide by zero
			if (t.typeOfEnemy < 0)
				t.typeOfEnemy = 0;


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
			if (ecs.removeComponent<Com_EnemySpawner>(selected))
				ecs.loseEntitySelection();
		}
	}
}

//List component to add to entity within 'Add Component' menu 
void Com_EnemySpawner::listView() {
	// If you don't want to make it possible for other entities to add
		//this component to an entity, omit all code within this function

		//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_EnemySpawner>(selected))
	{
		if (ImGui::Selectable("Enemy Spawner ##componentList"))
		{
			//Add component
			ecs.addComponent<Com_EnemySpawner>(ecs.whichEntityIsSelected(),
				Com_EnemySpawner{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be changed to something else soon");
			ImGui::Text("Enemy Spawner tweaks");
			ImGui::EndTooltip();
		}
	}
}