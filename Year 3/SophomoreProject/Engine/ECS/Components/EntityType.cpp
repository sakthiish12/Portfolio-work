// /******************************************************************************/
// /*!
// \file         EntityType.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for special entity typing
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

bool Com_type::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Com_type>(toSave))
	{
		const Com_type& c = ecs.getComponent<Com_type>(toSave);
		writer->Key("struct Com_type");
		{
			writer->StartObject();
			writer->Key("Type");
			writer->Int(c.type);
			writer->EndObject();
		}
	}
	return true;
}

//TODO empty
bool Com_type::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm;

	if (data.HasMember("struct Com_type") &&
		data["struct Com_type"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Com_type"];
		sm.DeserializeInt("Type", type, detail);//deserialize the type 
		return true;
	}
	return false;
}

void Com_type::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_type::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_type>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Special Entity Type##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Represents the special type of entity they are");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_type>(selected);

			//Type
			std::string entityType;
			switch (t.type)
			{
			case(type::camera): entityType = "Camera"; break;
			case(type::enemy): entityType = "Enemy"; break;
			case(type::enemyrange): entityType = "Ranged Enemy"; break;
			case(type::gui): entityType = "GUI"; break;
			case(type::player): entityType = "Player"; break;
			case(type::obstacles): entityType = "Obstacles"; break;
			case(type::bullet): entityType = "Bullet"; break;
			case(type::spawner): entityType = "Spawner"; break;
			case(type::enemyBullet): entityType = "Enemy Bullet"; break;
			case(type::bush): entityType = "Bush"; break;
			case(type::blockage): entityType = "Blockage"; break;
			case(type::boulder): entityType = "Boulder"; break;
			}
			ImGui::Text("Type: %s", entityType.c_str());
			if (ImGui::Button("Change Type"))
				ImGui::OpenPopup("Type List");
			if (ImGui::BeginPopup("Type List"))
			{
				if (ImGui::Selectable("Camera")) t.type = type::camera;
				if (ImGui::Selectable("Enemy")) t.type = type::enemy;
				if (ImGui::Selectable("Enemy Range")) t.type = type::enemyrange;
				if (ImGui::Selectable("GUI")) t.type = type::gui;
				if (ImGui::Selectable("Player")) t.type = type::player;
				if (ImGui::Selectable("Obstacles")) t.type = type::obstacles;
				if (ImGui::Selectable("Bullet")) t.type = type::bullet;
				if (ImGui::Selectable("Spawner")) t.type = type::spawner;
				if (ImGui::Selectable("Enemy Bullet")) t.type = type::enemyBullet;
				if (ImGui::Selectable("Bush")) t.type = type::bush;
				if (ImGui::Selectable("Blockage")) t.type = type::blockage;
				if (ImGui::Selectable("Blouder")) t.type = type::boulder;
				ImGui::EndPopup();
			}
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Represents the special type of entity they are");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Com_type>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Com_type::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_type>(selected))
	{
		if (ImGui::Selectable("Special Entity Type##componentList"))
		{
			//Add component
			ecs.addComponent<Com_type>(ecs.whichEntityIsSelected(),
				Com_type{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Represents the special type of entity they are");
			ImGui::EndTooltip();
		}
	}
}