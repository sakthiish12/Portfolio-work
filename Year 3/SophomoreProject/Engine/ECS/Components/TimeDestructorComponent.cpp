// /******************************************************************************/
// /*!
// \file         TimeDestructorComponent.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the time destructor component
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
bool Com_TimeDestructor::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<Com_TimeDestructor>(toSave)) //Prevent saving non-existent data
	{
		const Com_TimeDestructor& c = ecs.getComponent<Com_TimeDestructor>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct TimeDestructor");
		{
			writer->StartObject();
			writer->Key("timeDelay");
			writer->Double(c.timeDelayDestructor); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->EndObject();
		}
	}
	return true;
}

//TODO NO IMPLEMENTATION. DO NOT CALL
bool Com_TimeDestructor::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct TimeDestructor") &&
		data["struct TimeDestructor"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct TimeDestructor"];
		sm.DeserializeFloat("timeDelay", timeDelayDestructor, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

void Com_TimeDestructor::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//Render data into ImGui inspector
void Com_TimeDestructor::inspectorView() {
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_TimeDestructor>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Time Destructor ##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be changed to something else soon");
				ImGui::Text("Time Destructor Tweaks");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_TimeDestructor>(selected);

			//time delay
			ImGui::Text("Time Delay");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Setting the time delay for each self destruct");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiDragFloatSpecial("##Time Delay", &(t.timeDelayDestructor), 0.01f, true, 0.f, 200.f);

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
			if (ecs.removeComponent<Com_TimeDestructor>(selected))
				ecs.loseEntitySelection();
		}
	}
}

//List component to add to entity within 'Add Component' menu 
void Com_TimeDestructor::listView() {
	// If you don't want to make it possible for other entities to add
		//this component to an entity, omit all code within this function

		//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_TimeDestructor>(selected))
	{
		if (ImGui::Selectable("Time Destructor ##componentList"))
		{
			//Add component
			ecs.addComponent<Com_TimeDestructor>(ecs.whichEntityIsSelected(),
				Com_TimeDestructor{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be changed to something else soon");
			ImGui::Text("Time Destructor tweaks");
			ImGui::EndTooltip();
		}
	}
}