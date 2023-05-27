// /******************************************************************************/
// /*!
// \file         ScriptScript.cpp
// \project name Engine
// \author(s)    Sakthiish, Chun Ren
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        
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
#include "../Systems/CSharpScriptSystem.h"


//IMPORTANT, need to see if null pointer will crash or not.


bool CSharpScriptComponent::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const { 
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<CSharpScriptComponent>(toSave))
	{
		const CSharpScriptComponent& c = ecs.getComponent<CSharpScriptComponent>(toSave);
		writer->Key("struct CSharpScriptComponent");
		{
			writer->StartObject();
			writer->Key("script_size");
			writer->Uint(static_cast<unsigned int>(c._scriptMap.size()));
			unsigned int i = 0;
			for (auto x : c._scriptMap)
			{
				std::string ClassNamespace{};
				ClassNamespace += "ClassNamespace";
				ClassNamespace += std::to_string(i);
				writer->Key(ClassNamespace.c_str());
				{
					writer->String(x.second.classNamespace.c_str());
				}

				std::string className{};
				className += "className";
				className += std::to_string(i);
				writer->Key(className.c_str());
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

bool CSharpScriptComponent::OnDeserialize(const rapidjson::Value& data) { 
	SerializeManager sm;

	if (data.HasMember("struct CSharpScriptComponent") &&
		data["struct CSharpScriptComponent"].IsObject())
	{
		const rapidjson::Value& detail = data["struct CSharpScriptComponent"];

		unsigned int size;
		sm.DeserializeUInt("script_size", size, detail);
		_scriptMap.clear();
		for (unsigned int i = 0; i < size; ++i)
		{
			std::string classNamespace{};
			std::string classNamespaceResult{};
			std::string className{};
			std::string classNameResult{};

			classNamespace += "classNamespace";
			classNamespace += std::to_string(i); //ScriptName0

			className += "className";
			className += std::to_string(i); //ScriptName0


			sm.DeserializeString(classNamespace.c_str(), classNamespaceResult, detail);
			sm.DeserializeString(className.c_str(), classNameResult, detail);


			_scriptMap.insert(std::make_pair(classNameResult, MonoScriptManager.createFromScriptDatabase(classNameResult.c_str())));
		}
		return true;
	}
	return false; 
}

void CSharpScriptComponent::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

void CSharpScriptComponent::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<CSharpScriptComponent>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("CSharpScript##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Add scripts to entity");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<CSharpScriptComponent>(selected);

			//Adding a script
			if (ImGui::Button("Add CSharpScript##ScriptAdder")) {
				ImGui::OpenPopup("CSharpScript Selector");
			}
			if (ImGui::BeginPopup("CSharpScript Selector"))
			{
				for (auto& script : MonoScriptManager.getCSharpClassDatabase())
					if (ImGui::Selectable(script.first.c_str(), false))
						t._scriptMap[script.first.c_str()] = MonoScriptManager.createFromScriptDatabase(script.first.c_str());
				ImGui::EndPopup();
			}

			//removing a script
			if (ImGui::Button("Remove Script##ScriptRemover")) {
				ImGui::OpenPopup("Script Remover");
			}
			if (ImGui::BeginPopup("Script Remover"))
			{
				auto it = t._scriptMap.begin();
				for (; it != t._scriptMap.end();)
				{
					if (ImGui::Selectable(it->first.c_str(), false))
						it = t._scriptMap.erase(it);
					else
						++it;
				}


				/*for (auto& script : scriptContainer)
					if (ImGui::Selectable(script.first.c_str(), false))
						scriptContainer.erase(script.first);*/

				ImGui::EndPopup();
			}

			//List scripts
			ImGui::Text("Current CSharpScripts:");
			ImGui::Separator();
			for (auto& s : t._scriptMap)
			{
				ImGui::Text(s.first.c_str());
			}

			//Adding of script to database in runtime? dk if works or not
			//Might not work yet
			ImGui::Separator();
			ImGui::Text("Add new CSharpClass to Database");
			ImGui::Text("Might not be working fully yet");

			char newSpriteName[256] = { 0 };
			ImGui::InputText("", newSpriteName, 256);
			if (ImGui::Button("Save in database"))
			{
				MonoScriptManager.addCSharpClass(newSpriteName);
				ImGui::End();
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
			if (ecs.removeComponent<CSharpScriptComponent>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void CSharpScriptComponent::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<CSharpScriptComponent>(selected))
	{
		if (ImGui::Selectable("CSharpScript##componentList"))
		{
			//Add component
			ecs.addComponent<CSharpScriptComponent>(ecs.whichEntityIsSelected(),
				CSharpScriptComponent{});
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