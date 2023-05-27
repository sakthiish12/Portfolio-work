/******************************************************************************/
/*!
\file		Menu_Windows.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Main Menu options

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "Menu_Window.h"

#ifdef USE_EDITOR

#include "../EditorSystem/EditorSystem.h"
#include <Serialisation/BananaSerialisation.h>
namespace BE
{
	bool save_allAssets = true;
	bool save = false;
	bool newScene = false;
	bool saveSceneAsNew = false;

	std::string Menu_Window::filePath{};
	static std::filesystem::path s_AssetPath = "..";
	static std::string sss{};

	const float minSensitivity = 10.0f;
	const float maxSensitivity = 600.0f;
	const float minMoveSpeed = 1.0f;
	const float maxMoveSpeed = 40.0f;

	const float minRotationSpeed = 5.0f;
	const float maxRotationSpeed = 100.0f;
	const float minRotationDistance = 1.0f;
	const float maxRotationDistance = 10.0f;

	Menu_Window* Menu_Window::p_menu = nullptr;

	Menu_Window::Menu_Window() : showConsoleWindow{ true }, showFPSWindow{ true }, showResourcesWindow{ true },
		showEHierarWindow{ true }, showUHierarWindow{ true }, showEInspectWindow{ true }, showUInspectWindow{ true },
		showSceneWindow{ true }, showEditorGraphicsWindow{ true }, showPlayerGraphicsWindow{ true }, showAssetsWindow{ true },
		showEntityTagWindow{ true },m_currentDir{ s_AssetPath }
	{
	}

	Menu_Window::~Menu_Window()
	{
		
	}

	Menu_Window* Menu_Window::init()
	{
		p_menu = new Menu_Window();
		return p_menu;
	}

	void Menu_Window::update()
	{
		if ((BE::Input->IsKeyPressed(BE::Key::LEFT_CONTROL) || BE::Input->IsKeyPressed(BE::Key::RIGHT_CONTROL)) && BE::Input->IsKeyPressed(BE::Key::_S))
		{
			ImGui::OpenPopup("Save Current Scene?");
		}

		auto GetAllScene = BE::SceneMgr->GetAllSceneIDs();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					Hierarchy_Window::Get()->id = (BE::EntityID)0;
					Hierarchy_Window::Get_Selected().clear();
					newScene = true;
				}

				if (ImGui::BeginMenu("Open Scene"))
				{
					for (const auto a : GetAllScene)
					{
						auto scene = BE::SceneMgr->GetSceneByID(a.second);

						if (scene.GetSceneID() != NULL_SCENE_ID && scene.GetSceneID() != LOADING_SCENE_ID)
						{
							if (ImGui::MenuItem(a.first.c_str()))
							{
								UI_Hierarchy_Window::ui_id = BE::BEID(0);

								Hierarchy_Window::Get()->id = (BE::EntityID)0;
								Hierarchy_Window::Get_Selected().clear();
								BE::SceneMgr->LoadScene(a.first);
							}
						}			
					}			

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Save Current Scene"))
				{
					save = true;
				}

				if (ImGui::MenuItem("Save Current Scene as new scene"))
				{
					saveSceneAsNew = true;
				}

				if (ImGui::MenuItem("Load Scene List"))
					SceneMgr->DeserialiseAllScenes("../Scene/AllScenes.json");

				if (ImGui::MenuItem("Save Scene List"))
					SceneMgr->SerialiseAllScenes("../Scene/AllScenes.json");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Style"))
			{
				if (ImGui::MenuItem("Classic"))
				{
					ImGui::StyleColorsClassic();
				}
				if (ImGui::MenuItem("Dark"))
				{
					ImGui::StyleColorsDark();
				}
				if (ImGui::MenuItem("Light"))
				{
					ImGui::StyleColorsLight();
				}
				
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Open Console"))
				{
					showConsoleWindow = true;
				}
				if (ImGui::MenuItem("Open FPS"))
				{
					showFPSWindow = true;
				}
				if (ImGui::MenuItem("Open Resources"))
				{
					showResourcesWindow = true;
				}
				if (ImGui::MenuItem("Open Entity Hierarchy"))
				{
					showEHierarWindow = true;
				}
				if (ImGui::MenuItem("Open UI Hierarchy"))
				{
					showUHierarWindow = true;
				}
				if (ImGui::MenuItem("Open Entity Inspector"))
				{
					showEInspectWindow = true;
				}
				if (ImGui::MenuItem("Open UI Inspector"))
				{
					showUInspectWindow = true;
				}
				if (ImGui::MenuItem("Open Scene List"))
				{
					showSceneWindow = true;
				}
				if (ImGui::MenuItem("Open Editor View"))
				{
					showEditorGraphicsWindow = true;
				}
				if (ImGui::MenuItem("Open Player View"))
				{
					showPlayerGraphicsWindow = true;
				}
				if (ImGui::MenuItem("Open Assets"))
				{
					showAssetsWindow = true;
				}
				if (ImGui::MenuItem("Open Entity Tag"))
				{
					showEntityTagWindow = true;
				}
				
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::SliderFloat("Zoom Sensitivity", &EditorCameraController::scrollSpeed, minSensitivity, maxSensitivity);
				ImGui::SliderFloat("Move Sensitivity", &EditorCameraController::moveSpeed, minMoveSpeed, maxMoveSpeed);
				ImGui::SliderFloat("Rotation Speed", &EditorCameraController::rotationSpeed, minRotationSpeed, maxRotationSpeed);
				ImGui::SliderFloat("Rotation Distance", &EditorCameraController::rotateDistance, minRotationDistance, maxRotationDistance);
				ImGui::SliderFloat3("Global Illumination Color", &RendererSystem::GetInstance()->getUBO().ambientLightColor[0],0.0f,1.0f);
				ImGui::SliderFloat("Global Illumination Intensity", &RendererSystem::GetInstance()->getUBO().ambientLightColor[3], 0.0f, 1.0f);
				ImGui::Checkbox("Global Wireframe Mode", &RendererSystem::GetInstance()->getGlobalWireframeMode());
				ImGui::Checkbox("Show Grid", &RendererSystem::GetInstance()->getShowGrid());
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (save)
		{
			ImGui::OpenPopup("Save Current Scene?");
			save = false;
		}

		if (saveSceneAsNew)
		{
			ImGui::OpenPopup("Save Current Scene as new Scene?");
			saveSceneAsNew = false;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Save Current Scene?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to save current scene?");
			ImGui::Separator();

			//ImGui::Checkbox("Save All Imported Assets?", &save_allAssets);
			ImGui::Text("");

			if (ImGui::Button("Save"))
			{
				//SH::sceneID id = SH::ScM->GetCurrScene();
				//std::string name = SH::ScM->GetSceneNameFromList(id);

				//SH::ScM->SaveScene(name, save_allAssets);

				//Get the current scene id & file path
				//Asset manager stuff -> skipping this step for now
				//Save the tags used in the entity manager
				//Save the entities in ecs
				const std::string& outputFile = SceneMgr->GetCurrentScene().GetSceneFile();
				if(outputFile.size() != 0)
					BE::Serialisation::SaveEntities(outputFile);
				else
					BE::Serialisation::SaveEntities("temp.json");
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

		if (ImGui::BeginPopupModal("Save Current Scene as new Scene?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to save current scene as a new scene?");
			ImGui::Separator();

			static char file_path[100] = { 0 };
			ImGui::InputText("File Name", file_path, IM_ARRAYSIZE(file_path));
			ImGui::SameLine();
			ImGui::Text(".json");
			
			if (ImGui::Button("Save"))
			{
				std::string outputFile = "../Scene/" + std::string(file_path) + ".json";
				if (outputFile.size() != 0)
					BE::Serialisation::SaveEntities(outputFile);
				else
					BE::Serialisation::SaveEntities("temp.json");
				ImGui::CloseCurrentPopup();
				
				//Switch over to the new scene
				Scene& newlyCreatedScnee = SceneMgr->CreateScene(std::string(file_path), std::move(outputFile));
				SceneMgr->LoadScene(newlyCreatedScnee.GetSceneID());
				file_path[0] = '\0';
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


		if (newScene)
		{
			ImVec2 Scenecenter = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(Scenecenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (begin("Create New Scene"))
			{
				static ImGuiTextFilter filter;
				filter.Draw(ICON_FA_SEARCH);

				if (m_currentDir != std::filesystem::path(s_AssetPath))
				{
					if (ImGui::Button(ICON_FA_LONG_ARROW_ALT_LEFT))
					{
						m_currentDir = m_currentDir.parent_path();
					}
				}

				static float padding = 5.0f;
				static float thumbnailSize = 65;
				float cellSize = thumbnailSize + padding;
				float panelWidth = ImGui::GetContentRegionAvail().x;
				int columnCount = (int)(panelWidth / cellSize);
				if (columnCount < 1)
				{
					columnCount = 1;
				}

				ImGui::Columns(columnCount, 0, false);

				for (auto& dirEntry : std::filesystem::directory_iterator(m_currentDir))
				{
					const auto& path = dirEntry.path();
					auto relativePath = std::filesystem::relative(path, s_AssetPath);
					std::string fileNameString = relativePath.filename().string();

					if (filter.PassFilter(fileNameString.c_str()))
					{
						ImGui::PushID(fileNameString.c_str());

						std::string new_string = fileNameString.substr(fileNameString.find(".") + 1);

						const char* file = ICON_FA_FOLDER;

						if (new_string != "")
						{
							if (new_string == "lua")
							{
								file = ICON_FA_FILE_CODE;
							}
							if (new_string == "wav")
							{
								file = ICON_FA_FILE_AUDIO;
							}
							if (new_string == "png")
							{
								file = ICON_FA_FILE_IMAGE;
							}
						}

						ImGui::Button(file, { thumbnailSize, thumbnailSize });

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (dirEntry.is_directory())
							{
								m_currentDir /= path.filename();
							}
						}

						ImGui::TextWrapped(fileNameString.c_str());

						ImGui::PopID();

						ImGui::NextColumn();
					}
				}

				ImGui::Columns(1);

				filePath = m_currentDir.string() + "\\" + sss;

				ImGui::InputText("Folder Name", const_cast<char*>(filePath.c_str()), filePath.size());

				static char file_name[100] = { 0 };
				ImGui::InputText("File Name", file_name, IM_ARRAYSIZE(file_name));

				if (ImGui::Button("Create"))
				{
					BE::SceneMgr->CreateScene(file_name, (filePath + file_name + ".json").c_str());

					EditorSystem::GetInstance()->command->clearUndoStack();

					BE::SceneMgr->LoadScene(file_name);

					newScene = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Close"))
				{
					newScene = false;
				}
				end();
			}
		}
	}

	void Menu_Window::Shutdown()
	{
		delete p_menu;
	}
}
#endif