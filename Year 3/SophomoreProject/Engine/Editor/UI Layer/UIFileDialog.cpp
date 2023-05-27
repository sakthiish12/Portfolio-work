// /******************************************************************************/
// /*!
// \file         UIFileDialog.cpp 
// \project name Engine
// \author(s)    Ryan Wang 
// \date         12th Nov, 2021
// \brief        This file contains the definition to render a file dialog
// \lines        260 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "UiLayer.h"
#include "../../WinMgr/WinMgr.h"
#include "../../ECS/ECS.h"
#include <filesystem>
#include <iostream>

//LOCAL HELPER FUNCTIONS

//If file name ends with (No C++20)
bool stringEndsWith(std::string const& toCheck, std::string const& endsWith)
{
	size_t checkedLength = toCheck.length();
	size_t endLength = endsWith.length();

	return !(toCheck.substr(checkedLength - endLength, endLength).compare(endsWith));
}

//Assess the status of the current directory
void assessCurrentDirectory(std::filesystem::path const& path)
{
	size_t totalSize{ 0 };
	size_t fileCounter{ 0 };
	for (auto const& dir_entry : std::filesystem::directory_iterator{ path }) {
		++fileCounter;
		static_cast<void>(dir_entry);
	}
		
	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ path }) {
		totalSize += std::filesystem::file_size(dir_entry);
		static_cast<void>(dir_entry);
	}
		
	ImGui::Separator();
	ImGui::Text("%d total item(s), occupying %d bytes", fileCounter, totalSize);
	ImGui::Text("%d bytes available", std::filesystem::space(path).available);
	
	
}

//Get time of file
void fileTime(std::filesystem::path const& path)
{
	//https://stackoverflow.com/a/61067330 (RIP NO C++20 IN THIS VS)
	auto fileTime = std::filesystem::last_write_time(path); //Get the file time
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()); //Turn it into correct type of time point
	std::time_t fileTime_t = std::chrono::system_clock::to_time_t(sctp); //Convert into time_t
	char timeString[26];
	struct tm lt; //Readable time_t
	localtime_s(&lt, &fileTime_t); //Convert fileTime_t into readable format at put results in lt
	asctime_s(timeString, 26, &lt); //Prints out contents of struct tm lt into a C string format
	ImGui::Text("%s", timeString); //Finally put the text in ImGui
}

//TO SAVE A LEVEL
void UiLayer::ShowSaveLevelDialog(bool* p_open)
{
	static std::filesystem::path p{ "Resources/SavedLevels" };
	ImGui::Begin("Save Level", p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
	ImGui::Text("Current Path: %s", p.string().c_str());
	if (p.has_parent_path())
		if (ImGui::Button("Up One Level"))
			p = p.parent_path(); //Go to parent file
	ImGui::Separator();
	ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Save as NEW in here (May overwrite)");
	static char levelName[256];
	ImGui::InputText("Level.json##FileSaveInput", levelName, 256);
	if (ImGui::Button("Save as new"))
	{
		std::string targetFileName{ p.string() };
		targetFileName += "/";
		targetFileName += levelName;
		targetFileName += "Level.json";
		//SAVE DONE, REVERT IMGUI STATE AND CLOSE
		ecs.saveLevel(targetFileName);
		ImGui::End();
		*p_open = false;
		return;
	}

	ImGui::Separator();
	ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "OR OVERWRITE an existing \"*Level.json\" file");

	ImGui::Columns(3, "Save Level Columns");
	ImGui::Separator();
	ImGui::Text("Filename"); ImGui::NextColumn();
	ImGui::Text("Size (Bytes)"); ImGui::NextColumn();
	ImGui::Text("Last Modified"); ImGui::NextColumn();
	ImGui::Separator();
	for (auto const& dir_entry : std::filesystem::directory_iterator{ p })
	{
		//Directories
		if (dir_entry.is_directory())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//NAME
			std::string directoryText = "<DIR> " + dir_entry.path().filename().string();
			if (ImGui::Selectable(directoryText.c_str(), false))
				p = dir_entry.path(); //Go to child directory
			ImGui::NextColumn();
			//SIZE
			size_t totalSize{ 0 };
			for (auto const& subEntry : std::filesystem::recursive_directory_iterator{ dir_entry.path() })
				totalSize += std::filesystem::file_size(subEntry);
			ImGui::Text("%d", totalSize);
			ImGui::NextColumn();
		}
		//Relevant files
		else if (stringEndsWith(dir_entry.path().string(), "Level.json"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//FILE NAME (*Level.json)
			std::string overwritePopupString = "ConfirmOverwrite" + dir_entry.path().string();
			std::string overwritePopupSelectableString = "OK##" + overwritePopupString;
			if (ImGui::Selectable(dir_entry.path().filename().string().c_str(), false))
				ImGui::OpenPopup(overwritePopupString.c_str());
			if (ImGui::BeginPopup(overwritePopupString.c_str()))
			{
				ImGui::Text("Confirm Overwrite?");
				ImGui::Separator();
				if (ImGui::Selectable(overwritePopupSelectableString.c_str()))
				{
					//OVERWRITE DONE, REVERT IMGUI STATE AND CLOSE
					ecs.saveLevel(dir_entry.path().string());
					ImGui::EndPopup();
					ImGui::PopStyleColor();
					ImGui::Columns(1);
					ImGui::End();
					*p_open = false;
					return;
				}
				ImGui::EndPopup();
			}
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		else //Irrelevant file
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
			//FILE NAME (*Level.json)
			ImGui::Text("%s", dir_entry.path().filename().string().c_str());
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		//TIME MODIFIED
		fileTime(dir_entry.path());
		ImGui::NextColumn();
		ImGui::PopStyleColor();
	}
	
	ImGui::Columns(1); //Reset columning
	ImGui::Separator();
	assessCurrentDirectory(p);
	ImGui::End();
}

//TO LOAD A LEVEL
void UiLayer::ShowOpenLevelDialog(bool* p_open)
{
	static std::filesystem::path p{ "Resources/SavedLevels" };
	ImGui::Begin("Open Level", p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
	ImGui::Text("Current Path: %s", p.string().c_str());
	if (p.has_parent_path())
		if (ImGui::Button("Up One Level"))
			p = p.parent_path(); //Go to parent file

	ImGui::Separator();
	ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Load an existing \"*Level.json\" file");

	ImGui::Columns(3, "Load Level Columns");
	ImGui::Separator();
	ImGui::Text("Filename"); ImGui::NextColumn();
	ImGui::Text("Size (Bytes)"); ImGui::NextColumn();
	ImGui::Text("Last Modified"); ImGui::NextColumn();
	ImGui::Separator();
	for (auto const& dir_entry : std::filesystem::directory_iterator{ p })
	{
		//Directories
		if (dir_entry.is_directory())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//NAME
			std::string directoryText = "<DIR> " + dir_entry.path().filename().string();
			if (ImGui::Selectable(directoryText.c_str(), false))
				p = dir_entry.path(); //Go to child directory
			ImGui::NextColumn();
			//SIZE
			size_t totalSize{ 0 };
			for (auto const& subEntry : std::filesystem::recursive_directory_iterator{ dir_entry.path() })
				totalSize += std::filesystem::file_size(subEntry);
			ImGui::Text("%d", totalSize);
			ImGui::NextColumn();
		}
		//Relevant files
		else if (stringEndsWith(dir_entry.path().string(), "Level.json"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//FILE NAME (*Level.json)
			if (ImGui::Selectable(dir_entry.path().filename().string().c_str(), false))
			{
				//Prompt user if they want to load a level
				int decision;
				decision = MessageBox(WindowsManager.getHWnd(), L"Progress is not automatically saved upon loading levels. Unsaved progress will be lost. Load Level?", L"Confirm Load", MB_OKCANCEL | MB_ICONWARNING);
				if (decision == IDOK)
				{
					//LOAD DONE, REVERT IMGUI STATE AND CLOSE
					ecs.loadLevel(dir_entry.path().string());
					ImGui::PopStyleColor();
					ImGui::Columns(1);
					ImGui::End();
					*p_open = false;
					return;
				}
				else
					EndDialog(WindowsManager.getHWnd(), decision);
			}
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		else //Irrelevant file
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
			//FILE NAME (*Level.json)
			ImGui::Text("%s", dir_entry.path().filename().string().c_str());
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		//TIME MODIFIED
		fileTime(dir_entry.path());
		ImGui::NextColumn();
		ImGui::PopStyleColor();
	}
	ImGui::Columns(1); //Reset columning
	ImGui::Separator();
	assessCurrentDirectory(p);
	ImGui::End();
}

//TO LOAD AN ENTITY (Friend of ECS)
void UiLayer::ShowLoadEntityDialog(bool* p_open)
{
	static std::filesystem::path p{ "Resources/SavedEntities" };
	ImGui::Begin("Load Entity", p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
	ImGui::Text("Current Path: %s", p.string().c_str());
	if (p.has_parent_path())
		if (ImGui::Button("Up One Level"))
			p = p.parent_path(); //Go to parent file

	ImGui::Separator();
	ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Load an existing \".json\" file");

	ImGui::Columns(3, "Load Level Columns");
	ImGui::Separator();
	ImGui::Text("Filename"); ImGui::NextColumn();
	ImGui::Text("Size (Bytes)"); ImGui::NextColumn();
	ImGui::Text("Last Modified"); ImGui::NextColumn();
	ImGui::Separator();
	for (auto const& dir_entry : std::filesystem::directory_iterator{ p })
	{
		//Directories
		if (dir_entry.is_directory())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//NAME
			std::string directoryText = "<DIR> " + dir_entry.path().filename().string();
			if (ImGui::Selectable(directoryText.c_str(), false))
				p = dir_entry.path(); //Go to child directory
			ImGui::NextColumn();
			//SIZE
			size_t totalSize{ 0 };
			for (auto const& subEntry : std::filesystem::recursive_directory_iterator{ dir_entry.path() })
				totalSize += std::filesystem::file_size(subEntry);
			ImGui::Text("%d", totalSize);
			ImGui::NextColumn();
		}
		//Relevant files (.json files that don't end with "Level.json" (level files don't match format with individual entity files))
		else if (stringEndsWith(dir_entry.path().string(), ".json") && !stringEndsWith(dir_entry.path().string(), "Level.json"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//FILE NAME (*.json)
			if (ImGui::Selectable(dir_entry.path().filename().string().c_str(), false))
			{
				//LOAD DONE, REVERT IMGUI STATE AND CLOSE
				entityID newID = ecs.createEntityFromFile
				(dir_entry.path().string());
				ecs.renameEntity(newID, dir_entry.path().stem().string());
				//Should an entity have been selected beforehand, the new entity becomes a child of that
				if (ecs.isAnEntitySelected())
					ecs.setEntityParent(newID, ecs.whichEntityIsSelected());
				//Shift focus to new entity
				ecs.isEntitySelected = true;
				ecs.selectedEntityID = newID;
				ImGui::PopStyleColor();
				ImGui::Columns(1);
				ImGui::End();
				*p_open = false;
				return;
			}
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		else //Irrelevant file
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
			//FILE NAME (*Level.json)
			ImGui::Text("%s", dir_entry.path().filename().string().c_str());
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		//TIME MODIFIED
		fileTime(dir_entry.path());
		ImGui::NextColumn();
		ImGui::PopStyleColor();
	}
	ImGui::Columns(1); //Reset columning
	ImGui::Separator();
	assessCurrentDirectory(p);
	ImGui::End();
}