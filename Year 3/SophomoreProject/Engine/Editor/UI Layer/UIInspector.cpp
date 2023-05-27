// /******************************************************************************/
// /*!
// \file         UIInspector.cpp 
// \project name Engine
// \author(s)    Ryan Wang 
// \date         8th Dec, 2021
// \brief        This file contains the definition to render entity inspector
// \lines        167 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "../../ECS/ECS.h"
#include "UiLayer.h"
#include "../../pch.h"
#include "../../ECS/component.h"
#include "ImGuiHelpers.h"

//Function is a friend of ECS, can access its private members
void UiLayer::ShowInspector(bool* p_open)
{
	ImGui::Begin("Inspector", p_open);

	if (ecs.isAnEntitySelected())
	{
		entityID selected = ecs.whichEntityIsSelected();
		//DISPLAY ENTITY ID////////////////////////////////////////////////
		ImGui::Text("Entity ID: ");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("The unique ID of the entity presently selected");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f },
			"%d", selected);

		//DISPLAY NAME/////////////////////////////////////////////////////////
		auto names = ecs.getEntityNames();

		ImGui::Text("Entity Name");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("The entity's name");
			ImGui::Text("Though not a must, it is recommended for it to be");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f },
				"unique");
			ImGui::Text("Length of anywhere from 1 to 255 characters");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		//Technique inspiration:
		//https://stackoverflow.com/a/7352137
		size_t constexpr max = 256; //Maximum name length
		std::string entityName = names[selected];
		static char entityNameCStr[max];
		std::copy(entityName.begin(),
			entityName.length() >= max ?
			entityName.begin() + max :
			entityName.end(), //Does not take into account null terminator
			entityNameCStr);
		entityNameCStr[entityName.length()] = '\0'; //Null terminator
		//First character
		char firstChar = entityNameCStr[0];
		//If the input text is modified, modify the entity's name
		if (ImGui::InputText("##entityName", entityNameCStr, max))
		{
			//Ensure name does not get shorter than 1 character long
			if (std::strlen(entityNameCStr) < 1)
				entityNameCStr[0] = firstChar;
			//Rename entity in ECS
			ecs.renameEntity(selected, std::string{ entityNameCStr });
		}

		ImGui::Separator();

		//CHANGE PARENT////////////////////////////////////////////////////////

		ImGui::Text("Change Parent of Entity");
		//By Entity ID
		static entityID parentTarget{ 0 };
		if (ImGui::Button("By Parent's Entity ID (faster)##Button"))
			ecs.setEntityParent(selected, parentTarget);
		ImGui::SameLine();
		ImGui::InputScalar("##ParentEntityID",
			ImGuiDataType_U64, &parentTarget);

		//By new parent's name
		static char newParentName[max];
		if (ImGui::Button("By Parent's Name (if found) (slower)##Button"))
		{
			for (auto e : ecs.getEntityNames())
			{
				//Found
				if (strcmp(e.second.c_str(), newParentName) == 0)
				{
					ecs.setEntityParent(selected, e.first);
					break;
				}
			}
		}
		ImGui::SameLine();
		ImGui::InputText("##ParentEntityName", newParentName, max);

		//Unparent button
		ImGuiHelpers::ImGuiPushButtonStyleColors(ImVec4{ 0.5f, 0.25f, 0.25f, 1.0f });
		if (ImGui::Button("Unparent Entity##Button"))
			ecs.unparent(selected);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Make this entity not be a child of any entity");
			ImGui::EndTooltip();
		}
		ImGui::PopStyleColor(3);

		ImGui::Separator();

		//DISPLAY BUTTON TO DUPLICATE ENTITY///////////////////////////////////
		static size_t duplicateCount{ 1 };
		static size_t duplicateStep{ 1 };
		//Button Colour
		ImGuiHelpers::ImGuiPushButtonStyleColors(ImVec4{ 0.25f, 0.5f, 0.25f, 1.0f });
		if (ImGui::Button("Duplicate Entity##Button"))
		{
			ecs.duplicateEntity(selected, duplicateCount);
		}
		ImGui::PopStyleColor(3);
		//Tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Duplicate this entity a specified number of times");
			ImGui::Text("Parentage is copied over");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		ImGui::InputScalar("##duplicateCount",
			ImGuiDataType_U64, &duplicateCount, &duplicateStep);

		//DISPLAY BUTTON TO DELETE ENTITY//////////////////////////////////////
		//Button Colour
		ImGuiHelpers::ImGuiPushButtonStyleColors(ImVec4{ 0.5f, 0.25f, 0.25f, 1.0f });
		if (ImGui::Button("Delete Entity##Button"))
		{
			//Lose focus of entity before deletion
			ecs.isEntitySelected = false;
			ecs.deleteEntity(selected);
			ImGui::PopStyleColor(3); //Undo button colouring
			//Stop this function so that non-existent
			//component data won't be accessed
			ImGui::End(); //Properly end the inspector before returning. Thanks, Sakthiish and Wilfred
			return;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Delete this entity and all its descendants from the ECS");
			ImGui::EndTooltip();
		}

		//Remove button colours
		ImGui::PopStyleColor(3);

		//DISPLAY BUTTON TO SAVE ENTITY////////////////////////////////////////
		if (ImGui::Button("Save Entity##Button"))
		{
			//Name of the selected entity as the file name
			std::string filename = "Resources/SavedEntities/" + ecs.getEntityNames().at(ecs.selectedEntityID) + ".json";
			ecs.writeEntityToFile(ecs.selectedEntityID, filename);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Saves the data of this entity into this file:");
			ImGui::Text("Resources/SavedEntities/[THIS ENTITY'S NAME].json");
			ImGui::Text("Note that parentage is not saved");
			ImGui::EndTooltip();
		}

		ImGui::Separator();

		//DISPLAY COMPONENT DATA///////////////////////////////////////////////
		for (auto& c : ecs.componentImGuis)
		{
			if (ecs.isEntitySelected)
				c->inspectorView();
			else
				//Stop this function so that non-existent
			//component data won't be accessed
				return;
		}

		ImGui::Separator();

		//A BUTTON TO ADD A COMPONENT FOLLOWED BY A LIST OF ALL COMPONENT TYPES
		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("Component List");
		}
		if (ImGui::BeginPopup("Component List"))
		{
			for (auto& c : ecs.componentImGuis)
			{
				if (ecs.isEntitySelected)
					c->listView();
				else
					return; //Stop function if no entity selected
			}
			ImGui::EndPopup();
		}
	}
	else
	{
		ImGui::Text("Select an entity by:");
		ImGui::Bullet();
		ImGui::Text("Clicking it in the Hierarchy");
		ImGui::Bullet();
		ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f },
			"(TODO) Clicking in the Scene View");
	}

	ImGui::End();
}