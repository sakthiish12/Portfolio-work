// /******************************************************************************/
// /*!
// \file         EnemyState.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for AABB Component
//				 that details an axis-aligned box collider
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

bool Com_EnemyStates::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Com_EnemyStates>(toSave))
	{
		const Com_EnemyStates& c = ecs.getComponent<Com_EnemyStates>(toSave);
		(void)c;
		writer->Key("struct Com_EnemyStates");
		{
			writer->StartObject();
			writer->EndObject();
		}
	}
	return true;
}

//TODO empty
bool Com_EnemyStates::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm;
	static_cast<void>(sm);
	if (data.HasMember("struct Com_EnemyStates") &&
		data["struct Com_EnemyStates"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Com_EnemyStates"];
		static_cast<void>(detail);
		return true;
	}
	return false;
}

void Com_EnemyStates::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_EnemyStates::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_EnemyStates>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Enemy State##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_EnemyStates>(selected);

			//Render in ImGui what you want to be modifiable
			ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "Please check componentImGui.cpp and add ImGui implementation!");
			ImGui::Text("State: %d", t._current_state);
			ImGui::Text("Speed: %d", t._speed);
			ImGui::Text("Counter: %d", t._counter);
			ImGui::Text("Readhed: %s", t.reached ? "True" : "False");
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Com_EnemyStates>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Com_EnemyStates::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_EnemyStates>(selected))
	{
		if (ImGui::Selectable("Enemy State##componentList"))
		{
			//Add component
			ecs.addComponent<Com_EnemyStates>(ecs.whichEntityIsSelected(),
				Com_EnemyStates{});
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