// /******************************************************************************/
// /*!
// \file         AAATEMPLATE.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Template Component Serialization and ImGui Code
//				 Provides no functionality whatsoever,
//				 only provides details on how to make component serialization
//				 and ImGui Code
// \lines        0 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#if 0

#include "../component.h"
#include "../../ECS/ECS.h"
#include "../../pch.h"
#include "../../Editor/UI Layer/ImGuiHelpers.h"

//SERIALIZATION////////////////////////////////////////////////////////////////

//IT IS HIGHLY SUGGESTED THAT THE LOADED ELEMENTS MATCH THE SAVED ELEMENTS
//EXAMPLE CASE
//The Component will be structured as such in the JSON file:
{
"struct component": {
	"Example": 1234
}
}

//Save component data
bool component::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<component>(toSave)) //Prevent saving non-existent data
	{
		const component& c = ecs.getComponent<component>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct component");
		{
			writer->StartObject();
			writer->Key("Example");
			writer->Int(c.intValue); //It is important to prefix with "c." to get the data from the correct entity that is to be saved
			writer->EndObject();
		}
	}
	return true;
}

//Load component data
bool component::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct component") &&
		data["struct component"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		const rapidjson::Value& detail = data["struct component"];
		sm.DeserializeInt("Example", intValue, detail);
		return true;
	}
	return false; //Cannot retrieve component from file
}

//Add Entity into ECS
void component::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//MINIMAL CASE
//If an entity has a component that has its details not to be saved (i.e. only note that the entity has such a component)
//Write your functions as such:
//(Be careful that if data isn't to be loaded, that the components' data is to have a default defined in the component struct's definition)
//(Be careful to not turn it into hard-coding)
//The Component will be structured as such in the JSON file:
{
	"struct emptyComponent": {
	}
}

//Save component data
bool component::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved(); //Get the entity to save (its value is modified in the ECS's own function to save an entity)
	if (ecs.entityHas<component>(toSave)) //Prevent saving non-existent data
	{
		const component& c = ecs.getComponent<component>(toSave); //Get the component provided it exists
		//Do your serialization here
		writer->Key("struct component");
		{
			writer->StartObject();
			writer->EndObject();
		}
	}
	return true;
}

//Load component data
bool component::OnDeserialize(const rapidjson::Value& data)
{
	//SerializeManager sm; //Get the serializer manager
	if (data.HasMember("struct component") &&
		data["struct component"].IsObject()) //Perform deserialization provided the component type exists in the file
	{
		return true;
	}
	return false; //Cannot retrieve component from file
}

//Put loaded data into ECS
//Suggested that no changes to be made beyond the struct specifier (component::)
void component::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

//To render within a component inspector
//Giving the opportunity for editor user to modify component data
//Rename all instances of "component" accordingly
void component::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<component>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		//Default the collapsing header to be open
		//https://github.com/ocornut/imgui/issues/579
		//ImGui::GetStateStorage()->SetInt
		//(ImGui::GetID("Component##Component"), 1); //SOMEWHAT CLUTTERED WITH ALL HEADERS OPENED
		if (ImGui::CollapsingHeader("Component##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<component>(selected);

			//Render in ImGui what you want to be modifiable
			ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "Please check componentImGui.cpp and add ImGui implementation!");
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
			///Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<component>(selected))
				ecs.loseEntitySelection();

			//If you want to render it impossible to delete the component,
			//do this instead:
			//notToDelete = true; //Reset the flag
		}
	}
}

//To render within the component inspector
//As part of a list of what components can be added into an entity
//Rename all instances of "component" accordingly
void component::listView()
{
	//If you don't want to make it possible for other entities to add
	//this component to an entity, omit all code within this function

	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<component>(selected))
	{
		if (ImGui::Selectable("component##componentList"))
		{
			//Add component
			ecs.addComponent<component>(ecs.whichEntityIsSelected(),
				component{});
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

#endif