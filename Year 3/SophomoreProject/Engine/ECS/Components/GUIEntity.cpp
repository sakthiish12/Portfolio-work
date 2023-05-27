// /******************************************************************************/
// /*!
// \file         GUIEntity.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for components describing GUI elements
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

bool Com_GUI::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Com_GUI>(toSave))
	{
		const Com_GUI& c = ecs.getComponent<Com_GUI>(toSave);
		writer->Key("struct Com_GUI");
		{
			writer->StartObject();
			writer->Key("isVisible");
			writer->Bool(c.isVisible);
			writer->Key("preCamPosSet");
			writer->Bool(false);
			writer->EndObject();
		}
	}
	return true;
}

bool Com_GUI::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm;
	static_cast<void>(sm);
	if (data.HasMember("struct Com_GUI") &&
		data["struct Com_GUI"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Com_GUI"];
		sm.DeserializeBool("isVisible", isVisible, detail);//deserialize the visibility
		sm.DeserializeBool("preCamPosSet", preCamPosSet, detail);//deserialize whether the pre camera is set 
		return true;
	}
	return false;
}
void Com_GUI::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_GUI::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_GUI>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("GUI##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Forces the entity to follow the scrolling camera's positioning during gameplay");
				ImGui::Text("Effectively turns the entity into a GUI element");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_GUI>(selected);

			//Render in ImGui what you want to be modifiable
			ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "Please check componentImGui.cpp and add ImGui implementation!");
			ImGui::Text("Visible?: %s", t.isVisible ? "True" : "False");
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Forces the entity to follow the scrolling camera's positioning during gameplay");
				ImGui::Text("Effectively turns the entity into a GUI element");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Com_GUI>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Com_GUI::listView()
{

	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_GUI>(selected))
	{
		if (ImGui::Selectable("GUI##componentList"))
		{
			//Add component
			ecs.addComponent<Com_GUI>(ecs.whichEntityIsSelected(),
				Com_GUI{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Forces the entity to follow the scrolling camera's positioning during gameplay");
			ImGui::Text("Effectively turns the entity into a GUI element");
			ImGui::EndTooltip();
		}
	}
}