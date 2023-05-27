/******************************************************************************/
/*!
\file		UI_Hierachy_Window.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Show all UI objects in the scene

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once
#include "pch.h"
#include "UI_Hierarchy_Window.h"

#ifdef USE_EDITOR

#include "../../Systems/UIManager/UIManager.h"
#include "../Menu_Window/Menu_Window.h"
#include "../Resources_Window/Resources_Window.h"

namespace BE
{
	UI_Hierarchy_Window* UI_Hierarchy_Window::p_ui_hier = nullptr;

	BE::BEID UI_Hierarchy_Window::ui_id = BE::BEID(0);
	BE::BEID UI_Hierarchy_Window::canvasID = BE::BEID(0);

	bool UI_Hierarchy_Window::DrawUI = true;

	UI_Hierarchy_Window::UI_Hierarchy_Window() : /*canvasID{ 0 },*/ /*ui_id{ 0 },*/ isusing{false}
	{
	}

	UI_Hierarchy_Window::~UI_Hierarchy_Window()
	{
		
	}

	UI_Hierarchy_Window* UI_Hierarchy_Window::init()
	{
		p_ui_hier = new UI_Hierarchy_Window();
		return p_ui_hier;
	}

	void UI_Hierarchy_Window::update()
	{
		if (Menu_Window::Get()->showUHierarWindow)
		{
			if (!begin("UI Hierarchy", &Menu_Window::Get()->showUHierarWindow))
			{
				end();
			}
			else
			{
				ImGui::Checkbox("Draw UI?", &DrawUI);
				ImGui::Text("");

				//static char canvas_name[100] = { 0 };
				//ImGui::InputText("Canvas Name", canvas_name, IM_ARRAYSIZE(canvas_name));

				if (ImGui::Button(ICON_FA_PLUS_SQUARE " Create Canvas"))
				{
					BE::UI->CreateCanvas();
				}

				ImGui::Text("");

				const auto& allCanvases = BE::UI->GetAllCanvases();
				for (size_t a = 0; a < allCanvases.size(); ++a)
				{
					// canvas id = allCanvases[a]

					auto& canvas = BE::UI->GetCanvas(allCanvases[a]);
					
					ImGui::PushID((int)allCanvases[a]);

					if (ImGui::TreeNode(canvas->GetCanvasName().c_str()))
					{
						const auto& allUIElements = canvas->GetAllUIElements();

						std::vector<UIElementID> deleted_element;
						std::vector<std::string> AllUIName;

						//Get names of all the UI Element sfor the given canvas
						for (UIElementID uid : allUIElements)
						{
							auto& getUI = canvas->GetUI(uid);
							const std::string& name = getUI->GetName();
							AllUIName.push_back(name);
						}

						//Display the UI Elements on the hierachy
						static UIElementID selectedUI{ 0 };
						for (size_t j = 0; j < allUIElements.size(); ++j)
						{
							ImGui::PushID((int)allUIElements[j]);

							if (ImGui::Selectable(AllUIName[j].c_str(), selectedUI == allUIElements[j]))
							{
								canvasID = allCanvases[a];
								ui_id = allUIElements[j];
								isusing = true;
								selectedUI = allUIElements[j];
							}

							if (ImGui::BeginPopupContextItem(AllUIName[j].c_str()))
							{
								if (ImGui::Button(ICON_FA_TRASH " Destroy UI Element"))
								{
									deleted_element.push_back(allUIElements[j]);
								}

								ImGui::EndPopup();
							}

							ImGui::OpenPopupOnItemClick(AllUIName[j].c_str(), ImGuiPopupFlags_MouseButtonRight);

							ImGui::PopID();
						}
						for (size_t del = 0; del < deleted_element.size(); ++del)
						{
							isusing = false;
							canvas->DestroyUI(UIElementID(deleted_element[del]));
							//SH::UI->DestroyUIElement(SH::UI->GetCanvasName(ptr), SH::UIElementID(deleted_element[a]));
						}
						deleted_element.clear();

						ImGui::Text("");
						if (ImGui::TreeNode(ICON_FA_PLUS_SQUARE " Create UI Element"))
						{
							static const char* items[] = { "Button", "FontText", "Image", "Slider", "Toggle", "ProgressBar" };
							static int selected = 0;

							ImGui::Text("UI Element Type");

							ImGui::Combo("##combo", &selected, items, IM_ARRAYSIZE(items));

							if (ImGui::Button("Create"))
							{
								if (selected == 0)
								{
									canvas->CreateUI(UIElementType::button);
								}
								if (selected == 1)
								{
									canvas->CreateUI(UIElementType::fontText);
								}
								if (selected == 2)
								{
									canvas->CreateUI(UIElementType::image);
								}
								if (selected == 3)
								{
									canvas->CreateUI(UIElementType::slider);
								}
								if (selected == 4)
								{
									canvas->CreateUI(UIElementType::toggle);
								}
								if (selected == 5)
								{
									canvas->CreateUI(UIElementType::progressBar);
								}
							}

							ImGui::TreePop();
						}

						ImGui::TreePop();
					}

					if (ImGui::BeginPopupContextItem(canvas->GetCanvasName().c_str()))
					{
						//Show if the canvas is active
						bool canvasflag = canvas->GetIsActive();
						if (ImGui::Checkbox("Is Active?", &canvasflag))
						{
							canvas->SetIsActive(canvasflag);
						}

						//Show offset of the UI
						const Vec3f& offset = canvas->GetUIOffset();
						float v[2] = { offset.x, offset.y };
						if (ImGui::DragFloat2("UI Position Offset", v, 0.01f))
						{
							canvas->SetUIOffset(Vec3f(v[0], v[1], 0.0f));
						}

						int CanvasSortOrder = canvas->GetSortLayer();
						if (ImGui::DragInt("Sort Canvas Layer", &CanvasSortOrder))
						{
							BE::UI->SetUICanvas_SortLayer(allCanvases[a], CanvasSortOrder);
						}

						if (ImGui::Button(ICON_FA_TRASH " Destroy Canvas"))
						{
							ImGui::OpenPopup("Destroy Canvas?");
						}

						if (ImGui::Button(ICON_FA_EDIT " Rename Canvas"))
						{
							ImGui::OpenPopup("Rename Canvas?");
						}

						ImVec2 center = ImGui::GetMainViewport()->GetCenter();
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

						if (ImGui::BeginPopupModal("Destroy Canvas?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("Are you sure you want to destroy?");
							ImGui::Text(canvas->GetCanvasName().c_str());
							ImGui::Text("");

							ImGui::Separator();

							if (ImGui::Button("Destroy"))
							{
								isusing = false;

								BE::UI->DestroyCanvas(allCanvases[a]);
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

						if (ImGui::BeginPopupModal("Rename Canvas?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							static char Cname[50] = "";
							ImGui::InputText("New Canvas Name", Cname, IM_ARRAYSIZE(Cname));

							ImGui::Separator();

							if (ImGui::Button("Rename"))
							{
								canvas->SetCanvasName(Cname);

								for (int name_i = 0; name_i < 50; ++name_i)
								{
									Cname[name_i] = 0;
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

						ImGui::EndPopup();
					}

					ImGui::OpenPopupOnItemClick(canvas->GetCanvasName().c_str(), ImGuiPopupFlags_MouseButtonRight);

					ImGui::PopID();
				}

				end();
			}
		}
	}

	void UI_Hierarchy_Window::Shutdown()
	{
		delete p_ui_hier;
	}
}

#endif