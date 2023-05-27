/******************************************************************************/
/*!
\file       Tool_Bar.cpp
\par        Project:
\author     Pham Thao Mien
\date       12 November 2021
\brief      This header file contains the handling of changing of scene state
			when the tool bar functions are being clicked.

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "EntityTag_Window.h"

#ifdef USE_EDITOR

#include "../EditorSystem/EditorSystem.h"

namespace BE
{
	EntityTag_Window* EntityTag_Window::p_entityTag = nullptr;

	EntityTag_Window::EntityTag_Window() : tagID{ 1 }
	{
	}
	EntityTag_Window::~EntityTag_Window()
	{
	}

	EntityTag_Window* EntityTag_Window::init()
	{
		p_entityTag = new EntityTag_Window();
		return p_entityTag;
	}

	void EntityTag_Window::update()
	{
		if (Menu_Window::Get()->showEntityTagWindow)
		{
			if (!begin("Entity Tag", &Menu_Window::Get()->showEntityTagWindow))
			{
				end();
			}
			else
			{
				ImGui::Text("List of Tags");
				auto getAllTags = BE::ECS->GetAllTags();

				if (ImGui::BeginListBox("Tags"))
				{
					static int selected;
					int i = 0;

					for (auto it : getAllTags)
					{
						if (ImGui::Selectable(it.first.c_str(), selected == i))
						{
							selected = i;
						}

						if (ImGui::BeginPopupContextItem(it.first.c_str()))
						{
							if (ImGui::Button("Remove Tag"))
							{
								BE::ECS->DestroyTag(it.first);
							}

							if (ImGui::Button("Rename Tag"))
							{
								ImGui::OpenPopup("Rename Tag");
							}

							ImVec2 center = ImGui::GetMainViewport()->GetCenter();
							ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

							if (ImGui::BeginPopupModal("Rename Tag", NULL, ImGuiWindowFlags_AlwaysAutoResize))
							{
								RenameTag_Popup(it.first);
							}

							ImGui::EndPopup();
						}
						++i;
					}

					ImGui::EndListBox();
				}

				ImGui::Separator();

				ImGui::Text("Create Tag");
				static char tag_name[100] = { 0 };
				ImGui::InputText("Tag Name", tag_name, IM_ARRAYSIZE(tag_name));
				ImGui::InputInt("Tag ID", &tagID);

				if (ImGui::Button("Create Tag"))
				{
					BE::ECS->CreateTag(tag_name, BE::BEID(tagID));
					strcpy_s(tag_name, "");
					tagID = 1;
				}

				ImGui::End();
			}
		}
	}

	void EntityTag_Window::Shutdown()
	{
		delete p_entityTag;
	}

	void EntityTag_Window::RenameTag_Popup(std::string old_name)
	{
		ImGui::Text("New Tag Name");

		static char rename_tag[100] = { 0 };
		ImGui::InputText("##New Tag Name", rename_tag, IM_ARRAYSIZE(rename_tag));

		ImGui::Separator();

		if (ImGui::Button("Rename"))
		{
			if (rename_tag[0] != 0)
			{
				BE::ECS->RenameTag(old_name, rename_tag);
				strcpy_s(rename_tag, "");
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
#endif