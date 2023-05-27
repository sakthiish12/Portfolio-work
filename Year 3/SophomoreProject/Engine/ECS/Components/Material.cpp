// /******************************************************************************/
// /*!
// \file         Material.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for an entity's material
//				 describing its shader and mesh used
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
#include "../../AssetManager/AssetManager.h"

//SERIALIZATION////////////////////////////////////////////////////////////////

bool material::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<material>(toSave))
	{
		const material& c = ecs.getComponent<material>(toSave);
		writer->Key("struct material");
		{
			writer->StartObject();
			writer->Key("shader");
			writer->String(c.shader.c_str());
			writer->Key("mesh");
			writer->String(c.mesh.c_str());
			writer->Key("color");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.color.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.color.y));
				writer->Key("z");
				writer->Double(static_cast<double>(c.color.z));
				writer->Key("w");
				writer->Double(static_cast<double>(c.color.w));
				writer->EndObject();
			}
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
//TODO eventually there should be a string detailing the mesh
bool material::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct material") &&
		data["struct material"].IsObject())
	{
		const rapidjson::Value& detail = data["struct material"];
		sm.DeserializeString("shader", shader, detail);
		sm.DeserializeString("mesh", mesh, detail);
		sm.DeserializeVector("color", color, detail);//deserialize color vector 
		return true;
	}
	return false;
}

void material::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void material::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<material>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Material##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The shader and mesh (geometry) to apply onto the entity for rendering");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<material>(selected);

			//Shader
			ImGui::Text("Shader: %s", t.shader.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The file location of the shader");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Change Shader##Material"))
				ImGui::OpenPopup("Shader Selector");
			if (ImGui::BeginPopup("Shader Selector"))
			{
				for (auto& sha : AssetManager::GetInstance().shaderProg)
				{
					if (ImGui::Selectable(sha.first.c_str(), false))
					{
						t.shader = sha.first;
					}
				}
				ImGui::EndPopup();
			}

			//Mesh
			ImGui::Text("Mesh: %s", t.mesh.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The type of mesh to use");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Change Mesh##Material"))
				ImGui::OpenPopup("Mesh Selector");
			if (ImGui::BeginPopup("Mesh Selector"))
			{
				for (auto& meshLoad : AssetManager::GetInstance().meshesloaded)
				{
					if (ImGui::Selectable(meshLoad.first.c_str(), false))
					{
						t.mesh = meshLoad.first;
					}
				}
				ImGui::EndPopup();
			}

			//Color
			std::vector<float*> arr{ &t.color.x, &t.color.y, &t.color.z, &t.color.w };
			ImGui::ColorEdit4("Color", *arr.data());//edits to color 


			//ImGui::Text("Color");
			//if (ImGui::IsItemHovered())
			//{
			//	ImGui::BeginTooltip();
			//	ImGui::Text("The colouration of the mesh if no texture is provided for the entity");
			//	ImGui::Text("Is not the multiply blender, so will not re-colour the texture (if any)");
			//	ImGui::EndTooltip();
			//}
			//ImGui::Indent();
			//ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "R");
			//if (ImGui::IsItemHovered())
			//{
			//	ImGui::BeginTooltip();
			//	ImGui::Text("Red");
			//	ImGui::EndTooltip();
			//}
			//ImGui::SameLine();
			//ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			//ImGuiHelpers::ImGuiDragFloatSpecial("##materialColorR", &(t.color.x), 0.01f, true, 0.0f, 1.0f);
			//ImGui::PopStyleColor(3);

			//ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "G");
			//if (ImGui::IsItemHovered())
			//{
			//	ImGui::BeginTooltip();
			//	ImGui::Text("Green");
			//	ImGui::EndTooltip();
			//}
			//ImGui::SameLine();
			//ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			//ImGuiHelpers::ImGuiDragFloatSpecial("##materialColorG", &(t.color.y), 0.01f, true, 0.0f, 1.0f);
			//ImGui::PopStyleColor(3);

			//ImGui::TextColored(ImVec4{ 0.5f, 0.5f, 1.0f, 1.0f }, "B");
			//if (ImGui::IsItemHovered())
			//{
			//	ImGui::BeginTooltip();
			//	ImGui::Text("Blue");
			//	ImGui::EndTooltip();
			//}
			//ImGui::SameLine();
			//ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.2f, 0.4f, 1.0f }); //Blue
			//ImGuiHelpers::ImGuiDragFloatSpecial("##materialColorB", &(t.color.z), 0.01f, true, 0.0f, 1.0f);
			//ImGui::PopStyleColor(3);

			//ImGui::TextColored(ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }, "A");
			//if (ImGui::IsItemHovered())
			//{
			//	ImGui::BeginTooltip();
			//	ImGui::Text("Alpha / Transparency");
			//	ImGui::EndTooltip();
			//}
			//ImGui::SameLine();
			//ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f }); //Alpha
			//ImGuiHelpers::ImGuiDragFloatSpecial("##materialColorA", &(t.color.w), 0.01f, true, 0.0f, 1.0f);
			//ImGui::PopStyleColor(3);

			//ImGui::Unindent();
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The shader and mesh (geometry) to apply onto the entity for rendering");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<material>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void material::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<material>(selected))
	{
		if (ImGui::Selectable("Material##componentList"))
		{
			//Add component
			ecs.addComponent<material>(ecs.whichEntityIsSelected(),
				material{});
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