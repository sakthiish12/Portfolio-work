// /******************************************************************************/
// /*!
// \file         Camera2D.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for a 2D Camera Component
//				 The camera covers a region seen by the player when they play the game
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

bool camera2DCom::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<camera2DCom>(toSave))
	{
		const camera2DCom& c = ecs.getComponent<camera2DCom>(toSave);
		writer->Key("struct camera2DCom");
		{
			writer->StartObject();
			writer->Key("height_change_val");
			writer->Int(c.height_change_val);
			writer->Key("height");
			writer->Double(static_cast<double>(c.height));
			writer->Key("min_height");
			writer->Double(static_cast<double>(c.min_height));
			writer->Key("isFixedToPlayer");
			writer->Bool(c.isFixedToPlayer);
			writer->Key("isPrimary");
			writer->Bool(c.isPrimary);
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
bool camera2DCom::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct camera2DCom") &&
		data["struct camera2DCom"].IsObject())
	{
		const rapidjson::Value& detail = data["struct camera2DCom"];
		sm.DeserializeInt("height_change_val", height_change_val, detail);
		sm.DeserializeFloat("height", height, detail);
		sm.DeserializeFloat("min_height", min_height, detail);
		sm.DeserializeBool("isFixedToPlayer", isFixedToPlayer, detail);
		sm.DeserializeVector("cameraPosition", cam_position, detail);
		sm.DeserializeBool("isPrimary", isPrimary, detail);
		return true;
	}
	return false;
}

void camera2DCom::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void camera2DCom::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<camera2DCom>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Camera##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The gameplay camera");
				ImGui::Text("For now, it is advised that only one entity has the camera");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<camera2DCom>(selected);

			//Camera Height
			ImGui::Text("Height");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The higher it is, the further it is zoomed out");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiDragFloatSpecial("##cameraHeight", &(t.height), 0.01f, true, t.min_height, t.max_height);

			//is the camera following the player??
			ImGui::Text("Following Player?: %s", t.isFixedToPlayer ? "True" : "False");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Is the camera following a player?");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Toggle##Following Player"))
				t.isFixedToPlayer = !t.isFixedToPlayer;

			//is the camera a primary camera? 
			ImGui::Text("primary camera?: %s", t.isPrimary ? "True" : "False");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Is the camera the primary camera?");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Toggle##Primary Camera"))
				t.isPrimary = !t.isPrimary;
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The gameplay camera");
				ImGui::Text("For now, it is advised that only one entity has the camera");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<camera2DCom>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void camera2DCom::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<camera2DCom>(selected))
	{
		if (ImGui::Selectable("Camera##componentList"))
		{
			//Add component
			ecs.addComponent<camera2DCom>(ecs.whichEntityIsSelected(),
				camera2DCom{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("The gameplay camera");
			ImGui::Text("For now, it is advised that only one entity has the camera");
			ImGui::EndTooltip();
		}
	}
}