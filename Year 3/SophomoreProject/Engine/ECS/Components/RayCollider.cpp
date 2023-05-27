// /******************************************************************************/
// /*!
// \file         RayCollider.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for ray-based colliders
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

bool Ray::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Ray>(toSave))
	{
		const Ray& c = ecs.getComponent<Ray>(toSave);
		writer->Key("struct Ray");
		{
			writer->StartObject();
			writer->Key("m_pt0");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.m_pt0.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.m_pt0.y));
				writer->EndObject();
			}
			writer->Key("m_dir");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.m_dir.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.m_dir.y));
				writer->EndObject();
			}
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
bool Ray::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct Ray") &&
		data["struct Ray"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Ray"];
		sm.DeserializeVector("m_pt0", m_pt0, detail);
		sm.DeserializeVector("m_dir", m_dir, detail);
		return true;
	}
	return false;
}

void Ray::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Ray::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Ray>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Ray Collider##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Ray>(selected);

			//ORIGIN
			ImGui::Text("Origin");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Where the ray collider begins from");
				ImGui::Text("Should it be (0, 0), the origin's position will match that of its entity");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Offsetting");
				ImGui::Text("Origin goes further to the right as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##RayOffsetX", &(t.m_pt0.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Offsetting");
				ImGui::Text("Origin goes further upwards as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##RayOffsetY", &(t.m_pt0.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//DIRECTION
			ImGui::Text("Direction");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The direction of where the ray should be projected to");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##RayDirectionX", &(t.m_dir.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##RayDirectionY", &(t.m_dir.y));
			ImGui::PopStyleColor(3);

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
			if (ecs.removeComponent<Ray>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Ray::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Ray>(selected))
	{
		if (ImGui::Selectable("Ray Collider##componentList"))
		{
			//Add component
			ecs.addComponent<Ray>(ecs.whichEntityIsSelected(),
				Ray{});
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