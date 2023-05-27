// /******************************************************************************/
// /*!
// \file         Font.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         19th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for Font Component
//				 that displays text of a select colour at a position
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

bool Com_Font::OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const {
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<Com_Font>(toSave))
	{
		const Com_Font& c = ecs.getComponent<Com_Font>(toSave);
		writer->Key("struct Com_Font");
		{
			writer->StartObject();
			writer->Key("text");
			writer->String(c.text.c_str());			
			writer->Key("isVisible");
			writer->Bool(c.isVisible);
			writer->Key("color");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(c.color.x));
				writer->Key("y");
				writer->Double(static_cast<double>(c.color.y));
				writer->Key("z");
				writer->Double(static_cast<double>(c.color.z));
				writer->EndObject();
			}
			writer->EndObject();
		}
	}
	return true;
}

bool Com_Font::OnDeserialize(const rapidjson::Value& data) {
	SerializeManager sm;
	static_cast<void>(sm);
	if (data.HasMember("struct Com_Font") &&
		data["struct Com_Font"].IsObject())
	{
		const rapidjson::Value& detail = data["struct Com_Font"];
		sm.DeserializeString("text", text, detail);//deserialize the text 
		sm.DeserializeBool("isVisible", isVisible, detail);//deserialize the visibility 
		sm.DeserializeVector("color", color, detail);//deserialize the color
		static_cast<void>(detail);
		return true;
	}
	return false;
}
void Com_Font::addComponent(uint32_t toEntity) {
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void Com_Font::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<Com_Font>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Font##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Displays font at a position based on the transform");
				ImGui::Text("X scale of the transform dictates font sizing");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<Com_Font>(selected);

			//Text
			ImGui::Text("Text");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The text to display (up to 255 characters)");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			//Technique inspiration:
			//https://stackoverflow.com/a/7352137
			size_t constexpr max = 256; //Maximum text length
			static char textCStr[max];
			std::copy(t.text.begin(),
				t.text.length() >= max ?
				t.text.begin() + max :
				t.text.end(), //Does not take into account null terminator
				textCStr);
			textCStr[t.text.length()] = '\0'; //Null terminator
			//If the input text is modified, modify the entity's name
			if (ImGui::InputText("##TextDisplayed", textCStr, max))
			{
				t.text = std::string{ textCStr };
			}
			
			//isVisible?
			ImGui::Text("Visible?: %s", t.isVisible ? "True" : "False");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Is the font visible?");
				ImGui::EndTooltip();
			}
			if (ImGui::Button("Toggle##Visibility of Font"))
				t.isVisible = !t.isVisible;

			//Color
			ImGui::Text("Color");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The coloration of the text");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			float* colorPointer[3] = { &t.color.x, &t.color.y, &t.color.z };
			ImGui::ColorEdit3("##textColorEditor", *colorPointer);
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Displays font at a position based on the transform");
				ImGui::Text("X scale of the transform dictates font sizing");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<Com_Font>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void Com_Font::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<Com_Font>(selected))
	{
		if (ImGui::Selectable("Font##componentList"))
		{
			//Add component
			ecs.addComponent<Com_Font>(ecs.whichEntityIsSelected(),
				Com_Font{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Displays font at a position based on the transform");
			ImGui::EndTooltip();
		}
	}
}