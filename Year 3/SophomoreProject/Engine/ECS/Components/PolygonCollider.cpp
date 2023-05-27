// /******************************************************************************/
// /*!
// \file         PolygonCollider.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for polygonal colliders
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

bool Poly::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Poly>(toSave))
	{
		const Poly& c = ecs.getComponent<Poly>(toSave);//unreferenced 
		writer->Key("struct Poly");
		{
			writer->StartObject();
			writer->Key("offset_pos");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.offset_pos.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.offset_pos.y));
				writer->EndObject();
			}
			writer->Key("angle_offset");
			writer->Double(static_cast<double>(c.angle_offset));
			writer->Key("scale_ratio");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.scale_ratio.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.scale_ratio.y));
				writer->EndObject();
			}
			writer->Key("model_size");
			writer->Uint(static_cast<unsigned int>(c.models.size()));
			for (unsigned int i = 0; i < c.models.size(); ++i)
			{
				std::string name{};
				name += "modelPoint";
				name += std::to_string(i);
				writer->Key(name.c_str());
				{
					writer->StartObject();
					writer->Key("x");
					writer->Double(static_cast<double>(c.models[i].x));
					writer->Key("y");
					writer->Double(static_cast<double>(c.models[i].y));
					writer->EndObject();
				}
			}
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
//TODO Deserialization is non-trivial as of now. No true implementation yet.
//Perhaps add a counter for number of points in the Poly component
bool Poly::OnDeserialize(const rapidjson::Value& data)
{
	//There should be an integer (or two) stating the
	//size of vectors points and models,
	//then do a for loop to deserialise every point and model into vectors

	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct Poly") &&
		data["struct Poly"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Poly"];
		sm.DeserializeVector("offset_pos", offset_pos, detail);
		sm.DeserializeFloat("angle_offset", angle_offset, detail);
		sm.DeserializeVector("scale_ratio", scale_ratio, detail);
		unsigned int size;
		sm.DeserializeUInt("model_size", size, detail);
		models.clear();
		for (unsigned int i = 0; i < size; ++i)
		{
			vec2 newModel{};
			std::string name{};
			name += "modelPoint";
			name += std::to_string(i);
			sm.DeserializeVector(name.c_str(), newModel, detail);
			models.push_back(newModel);
		}
		points.resize(size);
		return true;
	}
	return false; //Cannot retrieve component from file
}

void Poly::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Poly::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Poly>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Polygonal Collider##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Poly>(selected);

			//MODELS (Points is global space after being affected by transform)
			ImGui::Text("Model Points");
			ImGui::Indent();
			size_t pointNumber{ 1 };
			//std::cout << t.models.size() << std::endl;
			for (auto& p : t.models)
			{
				ImGui::Text("%d (%.3f, %.3f)", pointNumber, p.x, p.y);
				++pointNumber;

				//Ensure unique label for entity
				std::string labelText{};
				labelText += "##ModelX";
				labelText += std::to_string(pointNumber);

				std::string labelTextY{};
				labelTextY += "##ModelY";
				labelTextY += std::to_string(pointNumber);


				ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Horizontal Positioning");
					ImGui::Text("Goes to the right as it increases");
					ImGui::EndTooltip();
				}
				ImGui::SameLine();
				//Colouration of the slider
				ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
				//Why "##PositionX"?
				//ImGui cannot function properly if the labels are not unique
				//The two #s means the label is hidden
				//While still maintaining that the label is unique
				//https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-why-is-my-widget-not-reacting-when-i-click-on-it
				//TODO variable drag sensitivity
				ImGuiHelpers::ImGuiDragFloatSpecial(labelText.c_str(), &(p.x));
				ImGui::PopStyleColor(3); //Only colour this slider red

				ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Vertical Positioning");
					ImGui::Text("Goes upwards as it increases");

					ImGui::EndTooltip();
				}
				ImGui::SameLine();

				ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
				ImGuiHelpers::ImGuiDragFloatSpecial(labelTextY.c_str(), &(p.y));
				ImGui::PopStyleColor(3);
			}
			if (ImGui::Button("Add Point##Polygon Collider"))
			{
				t.models.emplace_back(0.f, 0.f);
				t.points.emplace_back(0.f, 0.f);
			}
			if (ImGui::Button("Delete Point##Polygon Collider"))
			{
				t.models.pop_back();
				t.points.pop_back();
			}
			ImGui::Unindent();


			//Offset Positioning
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
			ImGuiHelpers::ImGuiDragFloatSpecial("##PolyOffsetX", &(t.offset_pos.x));
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
			ImGuiHelpers::ImGuiDragFloatSpecial("##PolyOffsetY", &(t.offset_pos.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//angle_offset
			ImGui::Text("angle_offset");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Collider angle offset against its transform2D rotation");
				ImGui::Text("Should it be (0), the collider's rotation will match that of its entity");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "angle_offset");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("angle_offset");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##angle_offset", &(t.angle_offset));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//scale_ratio
			ImGui::Text("scale_ratio");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("");
				ImGui::Text("");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal scale_ratio");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##scale_ratioX", &(t.scale_ratio.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical scale_ratio");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##scale_ratioY", &(t.scale_ratio.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();


			//TODO Rotationial Offsetting?
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
			if (ecs.removeComponent<Poly>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Poly::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Poly>(selected))
	{
		if (ImGui::Selectable("Polygonal Collider##componentList"))
		{
			//Add component
			ecs.addComponent<Poly>(ecs.whichEntityIsSelected(),
				Poly{});
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