// /******************************************************************************/
// /*!
// \file         Controller.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for controller component
//				 which holds inputs by the player to influence gameplay
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

bool controller::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<controller>(toSave))
	{
		//const controller& c = ecs.getComponent<controller>(toSave);
		writer->Key("struct controller");
		{
			writer->StartObject();
			writer->EndObject();
		}
	}
	return true;
}

//TODO Empty implementation
bool controller::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;
	static_cast<void>(sm);
	if (data.HasMember("struct controller") &&
		data["struct controller"].IsObject())
	{
		return true;
	}
	return false;
}

void controller::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void controller::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<controller>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Controller##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<controller>(selected);

			//Render in ImGui what you want to be modifiable
			ImGui::Text("Up   : %s", (t.up) ? "True" : "False");
			ImGui::Text("Down : %s", (t.down) ? "True" : "False");
			ImGui::Text("Right: %s", (t.right) ? "True" : "False");
			ImGui::Text("Left : %s", (t.left) ? "True" : "False");
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
			if (ecs.removeComponent<controller>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void controller::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<controller>(selected))
	{
		if (ImGui::Selectable("Controller##componentList"))
		{
			//Add component
			ecs.addComponent<controller>(ecs.whichEntityIsSelected(),
				controller{});
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