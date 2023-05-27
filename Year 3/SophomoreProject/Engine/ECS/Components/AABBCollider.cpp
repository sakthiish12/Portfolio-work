// /******************************************************************************/
// /*!
// \file         AABBCollider.cpp
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

bool AABB::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<AABB>(toSave))
	{
		const AABB& c = ecs.getComponent<AABB>(toSave);
		writer->Key("struct AABB");
		{
			writer->StartObject();
			writer->Key("scale");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.scale.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.scale.y));
				writer->EndObject();
			}
			writer->Key("offset_pos");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.offset_pos.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.offset_pos.y));
				writer->EndObject();
			}
			writer->EndObject();
		}
	}
	return true;
}

bool AABB::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct AABB") &&
		data["struct AABB"].IsObject())
	{
		const rapidjson::Value& detail = data["struct AABB"];
		//sm.DeserializeVector("min", min, detail);
		//sm.DeserializeVector("max", max, detail);
		sm.DeserializeVector("scale", scale, detail);
		sm.DeserializeVector("offset_pos", offset_pos, detail);
		return true;
	}
	return false;
}

void AABB::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void AABB::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<AABB>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Axis-Aligned Box Collider##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Based off the entity's transform");
				ImGui::Text("It cannot be rotated");
				ImGui::Text("Apart from circle colliders,");
				ImGui::Text("is the easiest and fastest to simulate");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<AABB>(selected);

			//Read-Only Minimum and Maximum Points
			ImGui::Text("Bounds");
			ImGui::Text("Minimum Point: (%.3f, %.3f)", t.min.x, t.min.y);
			ImGui::Text("Maximum Point: (%.3f, %.3f)", t.max.x, t.max.y);

			//OFFSET
			ImGui::Text("Offset");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How removed the AABB is from the entity's position");
				ImGui::Text("Should it be (0, 0), the AABB's position will match that of its entity");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Offsetting");
				ImGui::Text("AABB goes further to the right as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##AABBOffsetX", &(t.offset_pos.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Offsetting");
				ImGui::Text("AABB goes further upwards as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##AABBOffsetY", &(t.offset_pos.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//SCALING
			ImGui::Text("Scale");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How much larger (or smaller) the collider should be than its entity");
				ImGui::Text("Should it be (1, 1), the AABB's size will match that of its entity");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Scaling");
				ImGui::Text("AABB gets wider as its magnitude increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##AABBScaleX", &(t.scale.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Scaling");
				ImGui::Text("AABB gets taller as its magnitude increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##AABBScaleY", &(t.scale.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Based off the entity's transform");
				ImGui::Text("It cannot be rotated");
				ImGui::Text("Apart from circle colliders,");
				ImGui::Text("is the easiest and fastest to simulate");
				ImGui::EndTooltip();
			}
		}
		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<AABB>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void AABB::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<AABB>(selected))
	{
		if (ImGui::Selectable("AABB##componentList"))
		{
			//Add component
			ecs.addComponent<AABB>(ecs.whichEntityIsSelected(),
				AABB{});
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