// /******************************************************************************/
// /*!
// \file         scriptComponent.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Dec, 2021
// \brief        script component
// \lines        141 Lines
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
#include "../../Editor/UI Layer/ImGuiHelpers.h"
#include "../../AssetManager/AssetManager.h"
#include "../Systems/Logic.h"

bool scriptComponent::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<scriptComponent>(toSave))
	{
		const scriptComponent& c = ecs.getComponent<scriptComponent>(toSave);
		writer->Key("struct scriptComponent");
		{
			writer->StartObject();
			writer->Key("script_size");
			writer->Uint(static_cast<unsigned int>(c.scriptContainer.size()));
			unsigned int i = 0;
			for (auto x : c.scriptContainer)
			{
				std::string name{};
				name += "ScriptName";
				name += std::to_string(i);
				writer->Key(name.c_str());
				{
					//writer->StartObject();
					//writer->Key("ScriptNameID");
					writer->String(x.first.c_str());
					//writer->EndObject();
				}
				++i;
			}
			writer->EndObject();
		}
	}
	return true;
}

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
bool scriptComponent::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct scriptComponent") &&
		data["struct scriptComponent"].IsObject())
	{
		const rapidjson::Value& detail = data["struct scriptComponent"];

		unsigned int size;
		sm.DeserializeUInt("script_size", size, detail);
		scriptContainer.clear();
		for (unsigned int i = 0; i < size; ++i)
		{
			std::string sName{};
			std::string name{};
			name += "ScriptName";
			name += std::to_string(i); //ScriptName0
			sm.DeserializeString(name.c_str(), sName, detail);
			scriptContainer.insert(std::make_pair(sName, ScriptManager.createFromScriptDatabase(sName.c_str())));
		}
		return true;
	}
	return false;
}

void scriptComponent::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void scriptComponent::inspectorView()
{

	//Need to add ability to remove script


	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<scriptComponent>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Script##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Add scripts to entity");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<scriptComponent>(selected);

			//Adding a script
			if (ImGui::Button("Add Script##ScriptAdder")) {
				ImGui::OpenPopup("Script Selector");
			}
			if (ImGui::BeginPopup("Script Selector"))
			{
				for (auto& script : ScriptManager.getScriptDatabase())
					if (ImGui::Selectable(script.first.c_str(), false))
						t.scriptContainer[script.first.c_str()] = ScriptManager.createFromScriptDatabase(script.first.c_str());
				ImGui::EndPopup();
			}

			//removing a script
			if (ImGui::Button("Remove Script##ScriptRemover")) {
				ImGui::OpenPopup("Script Remover");
			}
			if (ImGui::BeginPopup("Script Remover"))
			{
				auto it = t.scriptContainer.begin();
				for (; it != t.scriptContainer.end();)
				{
					if (ImGui::Selectable(it->first.c_str(), false))
						it = t.scriptContainer.erase(it);
					else
						++it;
				}


				/*for (auto& script : scriptContainer)
					if (ImGui::Selectable(script.first.c_str(), false))
						scriptContainer.erase(script.first);*/
					
				ImGui::EndPopup();
			}

			//List scripts
			ImGui::Text("Current Scripts:");
			ImGui::Separator();
			for (auto& s : t.scriptContainer)
			{
				ImGui::Text(s.first.c_str());
			}
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				//ImGui::Text("The texture applied onto the entity");
				//ImGui::Text("May or may not be animated");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<scriptComponent>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void scriptComponent::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<scriptComponent>(selected))
	{
		if (ImGui::Selectable("Script##componentList"))
		{
			//Add component
			ecs.addComponent<scriptComponent>(ecs.whichEntityIsSelected(),
				scriptComponent{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Contain scripts for an entity to handle its logic");
			ImGui::EndTooltip();
		}
	}
}