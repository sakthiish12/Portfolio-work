// /******************************************************************************/
// /*!
// \file         CircleCollider.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for a circular collider component
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

bool Circle::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Circle>(toSave))
	{
		const Circle& c = ecs.getComponent<Circle>(toSave);
		writer->Key("struct Circle");
		{
			writer->StartObject();
			writer->Key("m_center");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.m_center.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.m_center.y));
				writer->EndObject();
			}
			writer->Key("m_radius");
			writer->Double(static_cast<double>(c.m_radius));
			writer->Key("m_mass");
			writer->Double(static_cast<double>(c.m_mass));
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
bool Circle::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct Circle") &&
		data["struct Circle"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Circle"];
		sm.DeserializeVector("m_center", m_center, detail);
		sm.DeserializeFloat("m_radius", m_radius, detail);
		sm.DeserializeFloat("m_mass", m_mass, detail);
		return true;
	}
	return false;
}

void Circle::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Circle::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Circle>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Circular Collider##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Based off the entity's transform");
				ImGui::Text("Is the simplest and fastest to simulate among all collider types");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Circle>(selected);

			//Offsetting
			ImGui::Text("Offset");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How removed the collider is from the entity's position");
				ImGui::Text("Should it be (0, 0), the collider's position will match that of its entity");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Offsetting");
				ImGui::Text("Collider goes further to the right as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##CircleOffsetX", &(t.m_center.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Offsetting");
				ImGui::Text("Collider goes further upwards as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##CircleOffsetY", &(t.m_center.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//Radius
			ImGui::Text("Radius");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The radius of the circular collider in arbitrary units");
				ImGui::Text("It is a good idea to decide your units");
				ImGui::Text("early on (i.e. metres, feet, etc...)");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiDragFloatSpecial("##CircleRadius", &(t.m_radius));
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Based off the entity's transform");
				ImGui::Text("Is the simplest and fastest to simulate among all collider types");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Circle>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Circle::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Circle>(selected))
	{
		if (ImGui::Selectable("Circular Collider##componentList"))
		{
			//Add component
			ecs.addComponent<Circle>(ecs.whichEntityIsSelected(),
				Circle{});
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