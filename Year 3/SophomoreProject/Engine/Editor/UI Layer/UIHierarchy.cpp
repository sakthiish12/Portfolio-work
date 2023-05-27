// /******************************************************************************/
// /*!
// \file         UIHierarchy.cpp 
// \project name Engine
// \author(s)    Ryan Wang 
// \date         8th Dec, 2021
// \brief        This file contains the definition to render scene hierarchy in ImGui
// \lines        148 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "../../ECS/ECS.h"
#include "../../ECS/component.h"
#include "UiLayer.h"
#include "../../pch.h"

//LOCAL HELPER FUNCTIONS

//Render node for entity e
//May do recursive calls for children
void UiLayer::displayHierarchyEntry(entityID e, std::map<entityID, std::string> const& names)
{
	//Cannot use [] operator on const maps as it may alter the map, use .find() instead
	//https://stackoverflow.com/questions/9357308/subscript-operators-for-class-with-stdmap-member-variable

	//Hopefully this mitigates the exception that sometimes happens after deleting an entity with children
	if (names.find(e) == names.end())
		return;

	//Ensure unique label for entity
	std::string labelText{ names.find(e)->second };
	labelText += "##entity";
	labelText += std::to_string(e);

	//If this entity is not a parent of other entities

	//If this selectable row is clicked...
	if (ImGui::Selectable(labelText.c_str(),
		e == ecs.selectedEntityID && ecs.isEntitySelected))
	{
		ecs.isEntitySelected = true;
		ecs.selectedEntityID = e;
	}

	//Tooltip with entity's ID
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		std::string idText{ "Entity ID: " };
		idText += std::to_string(e);
		ImGui::Text(idText.c_str());
		ImGui::EndTooltip();
	}

	//Context Label needs to be unique
	std::string contextItemLabel{ names.find(e)->second };
	contextItemLabel += " Context";
	contextItemLabel += std::to_string(e); //Truly unique, thanks Chun Ren
	//Right click context menu
	if (ImGui::BeginPopupContextItem(contextItemLabel.c_str()))
	{
		//Select entity
		ecs.isEntitySelected = true;
		ecs.selectedEntityID = e;

		if (ImGui::Selectable("New Entity"))
		{
			entityID newID = ecs.createEntityFromFile
			("Resources/PresetEntities/sampleEntity.json");
			//Shift focus to new entity
			ecs.isEntitySelected = true;
			ecs.selectedEntityID = newID;
			//Break out because entity list changed
			ImGui::EndPopup();
			ecs.toRefreshEntityHierarchy = true;
			return;
		}
		if (ImGui::Selectable("Load Entity"))
		{
			ecs.isEntitySelected = false;
			UiLayer::GetInstance().loadEntityDialogOpen = true;
			ImGui::SetWindowPos("Load Entity", ImVec2{ WindowsManager.getWidthF() / 4.0f, WindowsManager.getHeightF() / 4.0f });
			ImGui::SetWindowSize("Load Entity", ImVec2{ WindowsManager.getWidthF() / 2.0f, WindowsManager.getHeightF() / 2.0f });
			//Break out because entity list changed
			ImGui::EndPopup();
			ecs.toRefreshEntityHierarchy = true;
			return;
		}
		if (ImGui::Selectable("New Child Entity"))
		{
			entityID newID = ecs.createEntityFromFile
			("Resources/PresetEntities/sampleEntity.json");
			//Shift focus to new entity
			ecs.isEntitySelected = true;
			ecs.selectedEntityID = newID;
			ecs.setEntityParent(newID, e);
			//Break out because entity list changed
			ImGui::EndPopup();
			ecs.toRefreshEntityHierarchy = true;
			return;
		}
		if (ImGui::Selectable("Load Child Entity"))
		{
			UiLayer::GetInstance().loadEntityDialogOpen = true;
			ImGui::SetWindowPos("Load Entity", ImVec2{ WindowsManager.getWidthF() / 4.0f, WindowsManager.getHeightF() / 4.0f });
			ImGui::SetWindowSize("Load Entity", ImVec2{ WindowsManager.getWidthF() / 2.0f, WindowsManager.getHeightF() / 2.0f });
		}
		if (ImGui::Selectable("Duplicate Entity"))
		{
			//Duplicate
			auto v = ecs.duplicateEntity(e, 1);
			//Shift focus to duplicated entity
			ecs.isEntitySelected = true;
			ecs.selectedEntityID = v[0];
			//Break out because entity list changed
			ImGui::EndPopup();
			ecs.toRefreshEntityHierarchy = true;
			return;
		}
		if (ImGui::Selectable("Delete Entity"))
		{
			if (ecs.selectedEntityID == e)
				ecs.isEntitySelected = false; //Deselect before delete
			ecs.deleteEntity(e);
			//Break out because entity list changed
			ImGui::EndPopup();
			ecs.toRefreshEntityHierarchy = true;
			return;
		}
		ImGui::EndPopup();
	}

	//Recursively render children
	std::vector<entityID> childrenIDs{};
	if (ecs.doesEntityHaveChildren(e, childrenIDs))
	{
		ImGui::Indent();
		for (entityID childID : childrenIDs)
		{
			displayHierarchyEntry(childID, names);
		}
		ImGui::Unindent();
	}

	//If this entity is a parent of other entities

	/*if (ImGui::TreeNode(e.second.c_str()))
	{
		ImGui::Text("Another entity should be in here");
		ImGui::TreePop(); //All tree nodes must end with this
	}*/
}

//Function is a friend of ECS, can access its private members
//(isEntitySelected and selectedEntityID)
void UiLayer::ShowHierarchy(bool* p_open)
{
	ImGui::Begin("Hierarchy", p_open);

	//Entity searcher
	static ImGuiTextFilter filter;

	ImGui::Text("Search for entities");

	filter.Draw("##EntitySearch");
	//Tooltip
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Filter usage:\n"
			"  \"\"         display all lines\n"
			"  \"xxx\"      display lines containing \"xxx\"\n"
			"  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
			"  \"-xxx\"     hide lines containing \"xxx\"");
		ImGui::EndTooltip();
	}

	ImGui::Separator();

	//Deselection (Left click or right click anywhere beyond entity)
	if ((ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1)) && ImGui::IsWindowHovered())
	{
		ecs.isEntitySelected = false;
	}

	//Right click in window to show context to add new entity
	if (ImGui::BeginPopupContextWindow("Hierarchy Context"))
	{
		if (ImGui::Selectable("New Entity"))
		{
			entityID newID = ecs.createEntityFromFile
			("Resources/PresetEntities/sampleEntity.json");
			//Shift focus to new entity
			ecs.isEntitySelected = true;
			ecs.selectedEntityID = newID;
		}
		if (ImGui::Selectable("Load Entity"))
		{
			UiLayer::GetInstance().loadEntityDialogOpen = true;
			ImGui::SetWindowPos("Load Entity", ImVec2{ WindowsManager.getWidthF() / 4.0f, WindowsManager.getHeightF() / 4.0f });
			ImGui::SetWindowSize("Load Entity", ImVec2{ WindowsManager.getWidthF() / 2.0f, WindowsManager.getHeightF() / 2.0f });
		}
		ImGui::EndPopup();
	}

	//Get the names of all entities in the hierarchy
	//Render them accordingly in the hierarchy
	//(27 Nov)
	//(A) Withold from drawing node directly if entity is a child of anything
	//(B) If entity is not a parent of anything, render as non-tree selectable
	//(C) If entity is a parent, render as tree with children entities,
	//    if child is a parent itself, recurse (C)

	//Reset hierarchy refresh flag
	ecs.toRefreshEntityHierarchy = false;

	for (std::pair<entityID, std::string> const& e : ecs.getEntityNames())
	{
		//If name matches filter
		//AND if entity is not a child of any other entity
		if (filter.PassFilter(e.second.c_str()) && !(ecs.doesEntityHaveParent(e.first)))
			displayHierarchyEntry(e.first, ecs.getEntityNames());

		//Break if hierarchy needs to be refreshed
		if (ecs.toRefreshEntityHierarchy)
			break;
	}
	ImGui::End();

	//ImGui::ShowDemoWindow();
}