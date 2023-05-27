// /******************************************************************************/
// /*!
// \file         AStarComponent.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for A* Pathfinding component
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

bool Com_FindPath::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Com_FindPath>(toSave))
	{
		const Com_FindPath& c = ecs.getComponent<Com_FindPath>(toSave);
		(void)c;
		writer->Key("struct Com_FindPath");
		{
			writer->StartObject();
			writer->EndObject();
		}
	}
	return true;
}

bool Com_FindPath::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm;
	static_cast<void>(sm);
	if (data.HasMember("struct Com_FindPath") &&
		data["struct Com_FindPath"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Com_FindPath"];
		static_cast<void>(detail);
		return true;
	}
	return false;
}
void Com_FindPath::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_FindPath::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_FindPath>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("A* Pathfinding##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_FindPath>(selected);

			//Render in ImGui what you want to be modifiable
			ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "Please check componentImGui.cpp and add ImGui implementation!");
			ImGui::Text("Finding Path?: %s", t._find ? "True" : "False");
			ImGui::Text("Path Found?: %s", t._found ? "True" : "False");
			ImGui::Text("Reached?: %s", t._reached ? "True" : "False");
			ImGui::Text("Target Position:");
			ImGui::Indent();
			if (t.targetPosInitialized)
			{
				ImGui::Text("X: %f", t.targetPos.x);
				ImGui::Text("Y: %f", t.targetPos.y);
			}
			ImGui::Unindent();
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
			if (ecs.removeComponent<Com_FindPath>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Com_FindPath::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_FindPath>(selected))
	{
		if (ImGui::Selectable("A* Pathfinding##componentList"))
		{
			//Add component
			ecs.addComponent<Com_FindPath>(ecs.whichEntityIsSelected(),
				Com_FindPath{});
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