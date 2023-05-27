// /******************************************************************************/
// /*!
// \file         Health.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for a component detailing
//				 an entity's amount of health
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

bool Com_Health::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Com_Health>(toSave))
	{
		const Com_Health& c = ecs.getComponent<Com_Health>(toSave);
		writer->Key("struct Com_Health");
		{
			writer->StartObject();
			writer->Key("health");
			writer->Int(c.health);
			writer->EndObject();
		}
	}
	return true;
}

bool Com_Health::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm;

	if (data.HasMember("struct Com_Health") &&
		data["struct Com_Health"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Com_Health"];
		sm.DeserializeInt("health", health, detail);//deserialize the health
		return true;
	}
	return false;
}

void Com_Health::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_Health::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_Health>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Health##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::Text("Represents the amount of health a character or destructible environment piece has");
				ImGui::Text("Characters include the player and enemies.");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_Health>(selected);

			const int minHealth = 0;
			const int maxHealth = 200;

			//number of spawns
			ImGui::Text("Health Points");
			ImGui::InputInt("##Health Points", &(t.health));
			//Prevent divide by zero
			if (t.health <= minHealth)
				t.health = 1;
			if (t.health > maxHealth) {
				t.health = maxHealth;
			}
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::Text("Represents the amount of health a character or destructible environment piece has");
				ImGui::Text("Characters include the player and enemies.");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Com_Health>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Com_Health::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_Health>(selected))
	{
		if (ImGui::Selectable("Health##componentList"))
		{
			//Add component
			ecs.addComponent<Com_Health>(ecs.whichEntityIsSelected(),
				Com_Health{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be implemented");
			ImGui::Text("Represents the amount of health a character or destructible environment piece has");
			ImGui::Text("Characters include the player and enemies.");
			ImGui::EndTooltip();
		}
	}
}