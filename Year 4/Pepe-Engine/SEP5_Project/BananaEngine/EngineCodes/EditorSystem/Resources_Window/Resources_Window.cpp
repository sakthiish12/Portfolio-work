/******************************************************************************/
/*!
\file		Resources_Window.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "Resources_Window.h"

#ifdef USE_EDITOR

#include "../Menu_Window/Menu_Window.h"

namespace BE
{
	Resources_Window* Resources_Window::p_resource = nullptr;
	std::string Resources_Window::filename{};
	std::string Resources_Window::dragdrop_path{};
	std::vector<std::string> Resources_Window::pressed_Resources;
	static std::filesystem::path s_AssetPath = "..";

	static std::string sss{};	

	Resources_Window::Resources_Window() : m_currentDir{ s_AssetPath }
	{
	}

	Resources_Window::~Resources_Window()
	{
		
	}

	Resources_Window* Resources_Window::init()
	{
		p_resource = new Resources_Window();
		return p_resource;
	}

	void Resources_Window::update()
	{
		if (Menu_Window::Get()->showResourcesWindow)
		{
			if (!begin("Resources", &Menu_Window::Get()->showResourcesWindow))
			{
				end();
			}
			else
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

				ImGui::BeginChild("child_2", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);

				static float padding = 5.0f;
				static float thumbnailSize = 70;
				float cellSize = thumbnailSize + padding;
				float panelWidth = ImGui::GetContentRegionAvail().x;
				int columnCount = (int)(panelWidth / cellSize);
				if (columnCount < 1)
				{
					columnCount = 1;
				}

				ImGui::Columns(columnCount, 0, false);

				for (auto& dirEntry : std::filesystem::directory_iterator(m_currentDir)) //this is slightly expensive
				{
					const auto& path = dirEntry.path();
					//auto relativePath = std::filesystem::relative(path, s_AssetPath); //this is bloody expensive!!! - Donavern
					std::string fileNameString = path.string().substr(path.string().find_last_of("\\")+1); //Better

					if (filter.PassFilter(fileNameString.c_str()))
					{
						ImGui::PushID(fileNameString.c_str());

						std::string new_string = fileNameString.substr(fileNameString.find_last_of(".") + 1);

						const char* file = ICON_FA_FOLDER;

						if (new_string != "")
						{
							if (new_string == "fbx")
							{
								file = ICON_FA_CUBE;
							}
							if (new_string == "wav")
							{
								file = ICON_FA_FILE_AUDIO;
							}
							if (new_string == "dds" || new_string == "png")
							{
								file = ICON_FA_FILE_IMAGE;
							}
							if (new_string == "ttf")
							{
								file = ICON_FA_FONT;
							}
							if (new_string == "prefab")
							{
								file = ICON_FA_CUBE;
							}
						}

						//static bool pressed = false;

						static std::map<std::string, bool> PressedBool;

						if (PressedBool[path.string()] == true)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(7.0f, 0.0f, 0.0f));
						}

						if (ImGui::Button(file, { thumbnailSize, thumbnailSize }) && (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL)))
						{
							//pressed = true;

							PressedBool[path.string()] = true;

							if (std::find(pressed_Resources.begin(), pressed_Resources.end(), path.string()) != pressed_Resources.end())
							{
							}
							else
							{
								pressed_Resources.push_back(path.string());
							}
						}
						else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							//pressed = false;

							PressedBool[path.string()] = false;

							pressed_Resources.clear();
						}

						ImGui::PopStyleColor(1);

						//test
						/*std::cout << pressed_Resources.size() << std::endl;

						for (size_t i = 0; i < pressed_Resources.size(); ++i)
						{
							std::cout << pressed_Resources[i] << std::endl;
						}*/

						if (ImGui::BeginDragDropSource())
						{
							// Update clicked file's name
							filename = fileNameString;

							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", NULL, 0);

							ImGui::Text(fileNameString.c_str());

							sss = fileNameString;

							ImGui::EndDragDropSource();
						}

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (dirEntry.is_directory())
							{
								m_currentDir /= path.filename();
							}
						}


						std::size_t pos = fileNameString.find(".");

						if (pos != std::string::npos)
						{
							std::string test = fileNameString.substr(pos);

							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && test == ".json")
							{
								//HI SHAUN
								using namespace BE;
								if (SceneMgr->GetSceneByName(fileNameString.substr(0, pos)) == NULL_SCENE)
								{
									SceneMgr->CreateScene(fileNameString.substr(0, pos), path.string());
								}

								UI_Hierarchy_Window::Get()->ui_id = BE::BEID(0);

								Hierarchy_Window::Get()->id = (BE::EntityID)0;
								Hierarchy_Window::Get_Selected().clear();

								SceneMgr->LoadScene(fileNameString.substr(0, pos));
							}
						}
						

						ImGui::TextWrapped(fileNameString.c_str());

						ImGui::PopID();

						ImGui::NextColumn();
					}
				}

				ImGui::Columns(1);

				if (pressed_Resources.size() <= 1)
				{
					dragdrop_path = m_currentDir.string() + "\\" + sss;
				}
				else
				{
					dragdrop_path.clear();

					for (size_t i = 0; i < pressed_Resources.size(); ++i)
					{
						dragdrop_path += pressed_Resources[i] + ";";
					}
					
				}
				

				ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 1, 100);
				ImGui::SliderFloat("Padding", &padding, 0, 32);

				ImGui::EndChild();

				end();
			}
		}
	}

	void Resources_Window::Shutdown()
	{
		delete p_resource;
	}
}
#endif