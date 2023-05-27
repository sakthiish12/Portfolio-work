/******************************************************************************/
/*!
\file		UI_Inspector_Window.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Inspector window for each UI object

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "UI_Inspector_Window.h"

#ifdef USE_EDITOR
#include "../UI_Hierarchy_Window/UI_Hierarchy_Window.h"
#include "../Menu_Window/Menu_Window.h"
//#include "../../Systems/UIManager/UIManager.h"
#include "../Resources_Window/Resources_Window.h"

namespace BE
{
	UI_Inspector_Window* UI_Inspector_Window::p_ui_insp = nullptr;
	std::string SliderBarTextureName;
	std::string CheckmarkTextureName;
	std::string ProgressBarTextureName;
	std::string ImageTextureName;
	std::string InteractableTextureName;
	std::string InteractableAudioName;

	UI_Inspector_Window::UI_Inspector_Window()
	{
		//_volume = 0.0f;
		//_pitch = 0.5f;
		//_type = BE::UIAudioQueType::pressed;
	}

	UI_Inspector_Window::~UI_Inspector_Window()
	{
	}

	UI_Inspector_Window* UI_Inspector_Window::init()
	{
		p_ui_insp = new UI_Inspector_Window();
		return p_ui_insp;
	}

	void UI_Inspector_Window::update()
	{
		if (Menu_Window::Get()->showUInspectWindow)
		{
			if (!begin("UI Inspector", &Menu_Window::Get()->showUInspectWindow))
			{
				end();
			}
			else
			{
				bool isusing = UI_Hierarchy_Window::Get()->isusing;

				if (isusing)
				{
					BEID getcanvasID = UI_Hierarchy_Window::Get()->canvasID;

					if (BE::UI->GetUICanvasCount() != 0)
					{
						auto& canvas = UI->GetCanvas((CanvasID)getcanvasID);

						BE::BEID getID = UI_Hierarchy_Window::Get()->ui_id;

						if (canvas->GetUIElementCount() != 0 && getID != 0)
						{
							std::shared_ptr<UIElement>& getUIE = canvas->GetUI(getID);

							const std::string& get_name = getUIE->GetName();

							std::string s_name = ICON_FA_CUBE " " + get_name;
							ImGui::Text(s_name.c_str());

							static char new_name[50] = { 0 };
							ImGui::InputText("##UI Name", new_name, IM_ARRAYSIZE(new_name));
							ImGui::SameLine();
							if (ImGui::Button("Set UI Name"))
							{
								if (new_name[0] != 0)
								{
									getUIE->SetName(new_name);

									for (int name_i = 0; name_i < 50; ++name_i)
									{
										new_name[name_i] = 0;
									}
								}
							}

							int Gettype = (int)getUIE->GetUIType();
							std::string type = "UI Element Type:";

							if (Gettype == 1)
							{
								type.append(" Button");
							}
							if (Gettype == 2)
							{
								type.append(" FontText");
							}
							if (Gettype == 3)
							{
								type.append(" Image");
							}
							if (Gettype == 4)
							{
								type.append(" Slider");
							}
							if (Gettype == 5)
							{
								type.append(" Toggle");
							}
							if (Gettype == 6)
							{
								type.append(" ProgressBar");
							}

							ImGui::Text(type.c_str());
							ImGui::Text("");

							int UIElementSortOrder = getUIE->GetSortLayer();
							if (ImGui::DragInt("Sort UI Element Layer", &UIElementSortOrder))
							{
								canvas->SetUIElement_SortLayer(getID, UIElementSortOrder);
							}	

							bool isActive = getUIE->GetUIElementFlag(BE::UIElementFlags::isActive);
							if (ImGui::Checkbox("isActive?", &isActive))
							{
								getUIE->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
							}		

							bool isHovering = getUIE->GetUIElementFlag(BE::UIElementFlags::isHovering);
							std::string isHovering_str = "isHovering? ";
							isHovering_str.append(isHovering ? "True" : "False");
							ImGui::Text(isHovering_str.c_str());

							bool isPressed = getUIE->GetUIElementFlag(BE::UIElementFlags::isPressed);
							std::string isPressed_str = "isPressed? ";
							isPressed_str.append(isPressed ? "True" : "False");
							ImGui::Text(isPressed_str.c_str());
							ImGui::Text("");

							float brightnessFactor = getUIE->getBrightnessFactor();
							if (ImGui::DragFloat("Brightness Factor", &brightnessFactor, 0.1f))
							{
								getUIE->setBrightnessFactor(brightnessFactor);
							}

							float opacityFactor = getUIE->getOpacity();
							if (ImGui::DragFloat("Opacity Factor", &opacityFactor, 0.1f,0.0f,1.0f))
							{
								getUIE->setOpacity(opacityFactor);
							}

							glm::vec4 color = getUIE->getFlickerColor();
							if (ImGui::DragFloat4("Color (Flicker)", &color[0], 0.05f,0.0f,2.0f))
							{
								getUIE->setFlickerColor(color);
							}

							glm::vec4 factor = getUIE->getColorIntensityFactor();
							if (ImGui::DragFloat3("RGB Intensity Factor", &factor[0], 0.05f, 0.0f, 5.0f))
							{
								getUIE->setColorIntensityFactor(factor);
							}

							int GetHorizontal = (int)getUIE->GetUIHorizontalAlignment();

							static const char* items_hor[] = { "Left", "Middle", "Right" };
							int selected_hor = GetHorizontal;

							ImGui::Text("Horizontal Alignment");

							ImGui::Combo("##combo_hor", &selected_hor, items_hor, IM_ARRAYSIZE(items_hor));

							if (selected_hor == 0)
							{
								getUIE->SetUIHorizontalAlignment(BE::UIHorizontalAlignment::left);
							}
							if (selected_hor == 1)
							{
								getUIE->SetUIHorizontalAlignment(BE::UIHorizontalAlignment::middle);
							}
							if (selected_hor == 2)
							{
								getUIE->SetUIHorizontalAlignment(BE::UIHorizontalAlignment::right);
							}
							ImGui::Text("");

							int GetVertical = (int)getUIE->GetUIVerticalAlignment();

							static const char* items_vert[] = { "Top", "Middle", "Bottom" };
							int selected_vert = GetVertical;

							ImGui::Text("Vertical Alignment");

							ImGui::Combo("##combo_vert", &selected_vert, items_vert, IM_ARRAYSIZE(items_vert));

							if (selected_vert == 0)
							{
								getUIE->SetUIVerticalAlignment(BE::UIVerticalAlignment::top);
							}
							if (selected_vert == 1)
							{
								getUIE->SetUIVerticalAlignment(BE::UIVerticalAlignment::middle);
							}
							if (selected_vert == 2)
							{
								getUIE->SetUIVerticalAlignment(BE::UIVerticalAlignment::bottom);
							}
							ImGui::Text("");

							float pos[] = { getUIE->GetUIPos().x,
											getUIE->GetUIPos().y};
							if (ImGui::DragFloat2("Position", pos, 0.01f))
							{
								getUIE->SetUIPos(BE::Vec3f{ pos[0], pos[1], 0.0f });
							}
							
							float scale[] = { getUIE->GetUIScale().x,
											  getUIE->GetUIScale().y};
							if (ImGui::DragFloat2("Scale", scale, 0.01f))
							{
								getUIE->SetUIScale(BE::Vec3f{ scale[0], scale[1], 0.0f });
							}
							
							ImGui::Text("");

							BE::UIScript& GameLogicRef = getUIE->GetUIScript();

							if (ImGui::Button("Reload All Scripts"))
							{
								GameLogicRef.ReloadAllScripts();
							}

							ImGui::Text("List of Scripts:");
							const auto& GetNameOfAllScripts = GameLogicRef.GetNameOfAllScripts();

							if (ImGui::BeginListBox("Scripts"))
							{
								static int selected;
								int i = 0;

								for (size_t a = 0; a < GetNameOfAllScripts.size(); ++a)
								{
									std::string scriptName = GetNameOfAllScripts[a];
									if (ImGui::Selectable(scriptName.c_str(), selected == i))
									{
										selected = i;
									}

									if (ImGui::BeginPopupContextItem(scriptName.c_str()))
									{
										if (ImGui::Button("Remove Script"))
										{
											GameLogicRef.RemoveScript(scriptName);
										}

										if (ImGui::Button("Reload Script"))
										{
											GameLogicRef.ReloadScript(scriptName);
										}

										ImGui::EndPopup();
									}
									++i;
								}

								ImGui::EndListBox();
							}

							ImGui::Text("");

							if (ImGui::TreeNode("Add Script to the Entity"))
							{
								const auto& GetAllScriptNames = BE::ScriptAPI->GetAllScriptNames();
								static std::string add_script;

								if (ImGui::BeginListBox("##Scripts"))
								{
									static int add_selected;
									int b = 0;

									static ImGuiTextFilter scriptsFilter;
									scriptsFilter.Draw(ICON_FA_SEARCH);

									for (size_t a = 0; a < GetAllScriptNames.size(); ++a)
									{
										if (scriptsFilter.PassFilter(GetAllScriptNames[a].c_str()))
										{
											if (ImGui::Selectable(GetAllScriptNames[a].c_str(), add_selected == b))
											{
												add_selected = b;
												add_script = GetAllScriptNames[a];
											}
										}
										++b;
									}

									ImGui::EndListBox();
								}

								if (ImGui::Button("Add"))
								{
									GameLogicRef.AddScript(add_script);
								}

								ImGui::TreePop();
							}

							ImGui::Text("");

							//Button
							if (Gettype == 1)
							{
								bool isDisabled = getUIE->GetUIElementFlag(BE::UIElementFlags::isDisabled);
								if (ImGui::Checkbox("isDisabled?", &isDisabled))
								{
									getUIE->SetUIElementFlag(BE::UIElementFlags::isDisabled, isDisabled);
								}

								ImGui::Text("");

								std::shared_ptr<BE::UIButton> sPtr = std::dynamic_pointer_cast<BE::UIButton>(getUIE);

								UI_Inspector_Window::UI_Interactable(sPtr);
							}

							//FontText
							else if (Gettype == 2)
							{
								std::shared_ptr<BE::UIFontText> sPtr = std::dynamic_pointer_cast<BE::UIFontText>(getUIE);

								const std::string& GetText = sPtr->GetText();
								const char* button_text = GetText.data();
								char* text = (char*)(button_text);

								/*static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

								ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", (unsigned int*)&flags, ImGuiInputTextFlags_ReadOnly);
								ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", (unsigned int*)&flags, ImGuiInputTextFlags_AllowTabInput);
								ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", (unsigned int*)&flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
								if (ImGui::InputTextMultiline("##source", text, 100, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags))
								{
									sPtr->SetText2(text);
								}*/

								if (ImGui::InputText("Text", text, 100))
								{
									sPtr->SetText2(text);
								}

								ImGui::Text("");

								static std::string font_path;

								static ImGuiTextFilter filter;
								filter.Draw(ICON_FA_SEARCH);

								static int set_font_selected;
								std::vector<std::string> AllFontFiles_selectable;

								//BE::FontFile
								for (auto& it : BE::AssetMgr->GetAllAssetIDsOfType<Font>())
								{
									AllFontFiles_selectable.push_back(BE::AssetMgr->GetAssetFileName(it.first));
								}

								if (ImGui::BeginListBox("Set Font To Use"))
								{
									for (int i = 0; i < (int)AllFontFiles_selectable.size(); ++i)
									{
										if (filter.PassFilter(AllFontFiles_selectable[i].c_str()))
										{
											if (ImGui::Selectable(AllFontFiles_selectable[i].c_str(), set_font_selected == i))
											{
												set_font_selected = i;
												font_path = AllFontFiles_selectable[i];
												sPtr->SetFont(AssetMgr->GetAssetID(font_path));
											}
										}
									}

									ImGui::EndListBox();
								}

								ImGui::Text("");

								int GetAlignment = (int)sPtr->GetAlignment();

								static const char* items_Alignment[] = { "Left", "Center", "Right" };
								static int selected_Alignment = GetAlignment;

								ImGui::Text("Font Alignment");

								ImGui::Combo("##combo_Alignment", &selected_Alignment, items_Alignment, IM_ARRAYSIZE(items_Alignment));

								if (selected_Alignment == 0)
								{
									sPtr->SetAlignment(BE::UIFontTextAlignment::left);
								}
								if (selected_Alignment == 1)
								{
									sPtr->SetAlignment(BE::UIFontTextAlignment::center);
								}
								if (selected_Alignment == 2)
								{
									sPtr->SetAlignment(BE::UIFontTextAlignment::right);
								}

								float GetColour[] = { sPtr->GetColour().r,
													sPtr->GetColour().g,
													sPtr->GetColour().b};
								if (ImGui::DragFloat3("Text Colour", GetColour,0.01f,0.0f,1.0f))
								{
									sPtr->SetColour(glm::vec3{ GetColour[0], GetColour[1], GetColour[2]});
								}				
							}

							//Image
							else if (Gettype == 3)
							{
								std::shared_ptr<BE::UIImage> sPtr = std::dynamic_pointer_cast<BE::UIImage>(getUIE);

								float rotation = BEMath::RadToDeg(sPtr->GetUIRot());
								if (ImGui::DragFloat("Rotation", &rotation, 1.0f))
								{
									sPtr->SetUIRotDeg(rotation);
								}

								ImGui::Text("Image Texture:");

								if (sPtr->HasTexture())
								{
									auto image = sPtr->GetTexture();
									ImGui::SameLine();
									ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(image).c_str());
								}				

								const auto& TextureAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::Texture>();

								static ImGuiTextFilter TextureFilter;
								TextureFilter.Draw(ICON_FA_SEARCH);

								static int add_texture_selected;
								std::vector<std::string> AllTextureFiles_selectable;
								std::vector<std::string> FullPath;

								for (auto& it : TextureAsset)
								{
									AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
									//AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetFileName(it.first));
									FullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
								}

								if (ImGui::BeginListBox("Texture"))
								{
									for (int i = 0; i < (int)AllTextureFiles_selectable.size(); ++i)
									{
										if (TextureFilter.PassFilter(FullPath[i].c_str()))
										{
											ImGui::PushID(i);
											if (ImGui::Selectable(AllTextureFiles_selectable[i].c_str(), add_texture_selected == i))
											{
												add_texture_selected = i;

												ImageTextureName = FullPath[i];
											}
											if (ImGui::IsItemHovered())
											{
												ImGui::BeginTooltip();
												ImGui::Text(FullPath[i].c_str());
												ImGui::EndTooltip();
											}
											ImGui::PopID();
										}
									}

									ImGui::EndListBox();
								}

								if (ImGui::Button("Set Image Texture"))
								{
									sPtr->SetTexture(BE::AssetMgr->GetAssetID(ImageTextureName));
								}
							}

							//Slider
							else if (Gettype == 4)
							{
								bool isDisabled = getUIE->GetUIElementFlag(BE::UIElementFlags::isDisabled);
								if (ImGui::Checkbox("isDisabled?", &isDisabled))
								{
									getUIE->SetUIElementFlag(BE::UIElementFlags::isDisabled, isDisabled);
								}
								std::shared_ptr<BE::UISlider> sPtr = std::dynamic_pointer_cast<BE::UISlider>(getUIE);				

								float SliderPercentage = sPtr->GetSliderPercentage();
								if (ImGui::SliderFloat("Slider Percentage", &SliderPercentage, 0.0f, 1.0f))
								{
									sPtr->SetSliderPercentage(SliderPercentage);
								}				

								float SliderKnobScale[] = { sPtr->GetSliderKnobScale().x,
											sPtr->GetSliderKnobScale().y,
											sPtr->GetSliderKnobScale().z };
								if (ImGui::DragFloat3("Slider Knob Scale", SliderKnobScale, 0.01f))
								{
									sPtr->SetSliderKnobScale(BE::Vec3f{ SliderKnobScale[0], SliderKnobScale[1], SliderKnobScale[2] });
								}

								ImGui::Text("");

								UI_Inspector_Window::UI_Interactable(sPtr);

								//static bool SliderBarTexture = false;
								//ImGui::Checkbox("Enable Slider Bar Texture?", &SliderBarTexture);
								//
								//if (SliderBarTexture)
								//{
								auto GetSliderBarBackgroundTexture = sPtr->GetSliderBarBackgroundTexture();

								ImGui::Text("Slider Bar Background Texture:");
								ImGui::SameLine();
								ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(GetSliderBarBackgroundTexture).c_str());

								auto GetSliderBarForegroundTexture = sPtr->GetSliderBarForegroundTexture();

								ImGui::Text("Slider Bar Foreground Texture:");
								ImGui::SameLine();
								ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(GetSliderBarForegroundTexture).c_str());

								const auto& TextureAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::Texture>();

								static ImGuiTextFilter TextureFilter;
								TextureFilter.Draw(ICON_FA_SEARCH);

								static int add_texture_selected;
								std::vector<std::string> AllTextureFiles_selectable;
								std::vector<std::string> FullPath;


								for (auto& it : TextureAsset)
								{
									AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
									//AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetFileName(it.first));
									FullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
								}

								if (ImGui::BeginListBox("Texture ##slider"))
								{
									for (int i = 0; i < (int)AllTextureFiles_selectable.size(); ++i)
									{
										if (TextureFilter.PassFilter(FullPath[i].c_str()))
										{
											ImGui::PushID(i);
											if (ImGui::Selectable(AllTextureFiles_selectable[i].c_str(), add_texture_selected == i))
											{
												add_texture_selected = i;

												SliderBarTextureName = FullPath[i];
											}
											if (ImGui::IsItemHovered())
											{
												ImGui::BeginTooltip();
												ImGui::Text(FullPath[i].c_str());
												ImGui::EndTooltip();
											}
											ImGui::PopID();
										}
									}

									ImGui::EndListBox();
								}

								if (ImGui::Button("Set Slider Bar Background Texture"))
								{
									sPtr->SetSliderBarBackgroundTexture(BE::AssetMgr->GetAssetID(SliderBarTextureName));
								}
								ImGui::SameLine();
								if (ImGui::Button("Set Slider Bar Foreground Texture"))
								{
									sPtr->SetSliderBarForegroundTexture(BE::AssetMgr->GetAssetID(SliderBarTextureName));
								}
								//}
								//else
								//{
								//	//sPtr->SetSliderBarTexture((BE::AssetID)0);
								//}
							}

							//Toggle
							else if (Gettype == 5)
							{
								bool isDisabled = getUIE->GetUIElementFlag(BE::UIElementFlags::isDisabled);
								if (ImGui::Checkbox("isDisabled?", &isDisabled))
								{
									getUIE->SetUIElementFlag(BE::UIElementFlags::isDisabled, isDisabled);
								}
								std::shared_ptr<BE::UIToggle> sPtr = std::dynamic_pointer_cast<BE::UIToggle>(getUIE);

								UI_Inspector_Window::UI_Interactable(sPtr);

								bool enabletoggle = sPtr->GetToggle();
								if (ImGui::Checkbox("Enable Toggle?", &enabletoggle))
								{
									sPtr->SetToggle(enabletoggle);
								}							

								//static bool CheckmarkTexture = false;
								//ImGui::Checkbox("Enable Checkmark Texture?", &CheckmarkTexture);
								//
								//if (CheckmarkTexture)
								//{
								auto GetCheckmarkTexture = sPtr->GetCheckmarkTexture();

								ImGui::Text("Checkmark Texture:");
								ImGui::SameLine();
								ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(GetCheckmarkTexture).c_str());

								const auto& TextureAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::Texture>();

								static ImGuiTextFilter TextureFilter;
								TextureFilter.Draw(ICON_FA_SEARCH);

								static int add_texture_selected;
								std::vector<std::string> AllTextureFiles_selectable;
								std::vector<std::string> FullPath;


								for (auto& it : TextureAsset)
								{
									AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
									//AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetFileName(it.first));
									FullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
								}

								if (ImGui::BeginListBox("Texture ##toggle"))
								{
									for (int i = 0; i < (int)AllTextureFiles_selectable.size(); ++i)
									{
										if (TextureFilter.PassFilter(FullPath[i].c_str()))
										{
											ImGui::PushID(i);
											if (ImGui::Selectable(AllTextureFiles_selectable[i].c_str(), add_texture_selected == i))
											{
												add_texture_selected = i;

												CheckmarkTextureName = FullPath[i];
											}
											if (ImGui::IsItemHovered())
											{
												ImGui::BeginTooltip();
												ImGui::Text(FullPath[i].c_str());
												ImGui::EndTooltip();
											}
											ImGui::PopID();
										}
									}

									ImGui::EndListBox();
								}

								if (ImGui::Button("Set Checkmark Texture"))
								{
									sPtr->SetCheckmarkTexture(BE::AssetMgr->GetAssetID(CheckmarkTextureName));
								}
								//}
								//else
								//{
								//	//sPtr->SetSliderBarTexture((BE::AssetID)0);
								//}
							}

							//ProgressBar
							else if (Gettype == 6)
							{
								std::shared_ptr<BE::UIProgressBar> sPtr = std::dynamic_pointer_cast<BE::UIProgressBar>(getUIE);

								float GetPercentage = sPtr->GetPercentage();
								if (ImGui::SliderFloat("Percentage", &GetPercentage, 0, 1))
								{
									sPtr->SetPercentage(GetPercentage);
								}

								ImGui::Text("");

								int GetProgBarType = (int)sPtr->GetProgBarType();

								static const char* items_ProgBarType[] = { "Horizontal", "Vertical", "Circular" };

								static int selected_ProgBarType = GetProgBarType;

								ImGui::Text("Progress Bar Type");

								ImGui::Combo("##combo_ProgBarType", &selected_ProgBarType, items_ProgBarType, IM_ARRAYSIZE(items_ProgBarType));

								if (selected_ProgBarType == 0)
								{
									sPtr->SetProgBarType(BE::UIProgressBarType::horizontal);
								}
								if (selected_ProgBarType == 1)
								{
									sPtr->SetProgBarType(BE::UIProgressBarType::vertical);
								}
								if (selected_ProgBarType == 2)
								{
									sPtr->SetProgBarType(BE::UIProgressBarType::circular);
								}

								bool isflicker = sPtr->GetIsFlickering();
								if (ImGui::Checkbox("Enable Flickering?", &isflicker))
								{
									sPtr->SetIsFlickering(isflicker);
								}

								if (isflicker)
								{
									float GetFlickerTimePerSecond = sPtr->GetFlickerRate();
									if (ImGui::DragFloat("Flicker Rate", &GetFlickerTimePerSecond))
									{
										sPtr->SetFlickerRate(GetFlickerTimePerSecond);
									}

									float GetFlickerThreshold = sPtr->GetFlickerThreshold();
									if (ImGui::SliderFloat("Flicker Threshold", &GetFlickerThreshold, 0.0, 1.0))
									{
										sPtr->SetFlickerThreshold(GetFlickerThreshold);
									}

									bool reverseFlicker = sPtr->GetIsReverseThreshold();
									if (ImGui::Checkbox("Reverse Flicker", &reverseFlicker))
									{
										sPtr->SetIsReverseThreshold(reverseFlicker);
									}
								}

								ImGui::Text("");

								ImGui::Text("Background Texture:");
								ImGui::SameLine();
								BE::AssetID background = sPtr->GetTexture(UIProgressBarParts::background);
								ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(background).c_str());

								ImGui::Text("Border Texture:");
								ImGui::SameLine();
								BE::AssetID border = sPtr->GetTexture(UIProgressBarParts::border);
								ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(border).c_str());

								ImGui::Text("Foreground Texture:");
								ImGui::SameLine();
								BE::AssetID foreground = sPtr->GetTexture(UIProgressBarParts::foreground);
								ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(foreground).c_str());

								const auto& TextureAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::Texture>();

								static ImGuiTextFilter TextureFilter;
								TextureFilter.Draw(ICON_FA_SEARCH);

								static int add_texture_selected;
								std::vector<std::string> AllTextureFiles_selectable;
								std::vector<std::string> FullPath;


								for (auto& it : TextureAsset)
								{
									AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
									//AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetFileName(it.first));
									FullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
								}

								if (ImGui::BeginListBox("Texture"))
								{
									for (int i = 0; i < (int)AllTextureFiles_selectable.size(); ++i)
									{
										if (TextureFilter.PassFilter(FullPath[i].c_str()))
										{
											ImGui::PushID(i);
											if (ImGui::Selectable(AllTextureFiles_selectable[i].c_str(), add_texture_selected == i))
											{
												add_texture_selected = i;

												ProgressBarTextureName = FullPath[i];
											}
											if (ImGui::IsItemHovered())
											{
												ImGui::BeginTooltip();
												ImGui::Text(FullPath[i].c_str());
												ImGui::EndTooltip();
											}
											ImGui::PopID();
										}
									}

									ImGui::EndListBox();
								}

								if (ImGui::Button("Set Background Texture"))
								{
									sPtr->SetTexture(UIProgressBarParts::background, BE::AssetMgr->GetAssetID(ProgressBarTextureName));
								}
								ImGui::SameLine();
								if (ImGui::Button("Set Border Texture"))
								{
									sPtr->SetTexture(UIProgressBarParts::border, BE::AssetMgr->GetAssetID(ProgressBarTextureName));
								}
								ImGui::SameLine();
								if (ImGui::Button("Set Foreground Texture"))
								{
									sPtr->SetTexture(UIProgressBarParts::foreground, BE::AssetMgr->GetAssetID(ProgressBarTextureName));
								}
							}
						}
					}
				}

				end();
			}
		}
	}
		//		bool isusing = UI_Hierarchy_Window::Get()->isusing;

		//		if (isusing)
		//		{
		//			std::string getcanvas = UI_Hierarchy_Window::Get()->canvasName;

		//			if (BE::UI->DoesCanvasExist(getcanvas) && BE::UI->GetUIElementActiveCount(getcanvas) != 0)
		//			{
		//				BE::BEID getID = UI_Hierarchy_Window::Get()->ui_id;

		//				const std::string& get_name = BE::UI->GetUIElementName(getcanvas, getID);

		//				std::string s_name = ICON_FA_CUBE " " + get_name;
		//				ImGui::Text(s_name.c_str());

		//				static char new_name[50] = { 0 };
		//				ImGui::InputText("##UI Name", new_name, IM_ARRAYSIZE(new_name));
		//				ImGui::SameLine();
		//				if (ImGui::Button("Set UI Name"))
		//				{
		//					if (new_name[0] != 0)
		//					{
		//						BE::UI->SetUIElementName(getcanvas, getID, new_name);
		//					}
		//				}

		//				int Gettype = (int)BE::UI->GetUIElementType(getcanvas, getID);
		//				std::string type = "UI Element Type:";

		//				auto& getElement = BE::UI->GetUIElement(getcanvas, getID);

		//				if (Gettype == 0)
		//				{
		//					type.append(" Button");
		//				}
		//				if (Gettype == 1)
		//				{
		//					type.append(" FontText");
		//				}
		//				if (Gettype == 2)
		//				{
		//					type.append(" Image");
		//				}
		//				if (Gettype == 3)
		//				{
		//					type.append(" Slider");
		//				}
		//				if (Gettype == 4)
		//				{
		//					type.append(" Toggle");
		//				}
		//				if (Gettype == 5)
		//				{
		//					type.append(" ProgressBar");
		//				}

		//				ImGui::Text(type.c_str());
		//				ImGui::Text("");

		//				int UIElementSortOrder = BE::UI->GetUIElementSortOrder(getcanvas, getID);
		//				if (ImGui::DragInt("UIElementSortOrder", &UIElementSortOrder))
		//				{
		//					BE::UI->SetUIElementSortOrder(getcanvas, getID, UIElementSortOrder);
		//				}				

		//				bool isDisabled = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isDisabled);
		//				if (ImGui::Checkbox("isDisabled?", &isDisabled))
		//				{
		//					getElement.get()->SetUIElementFlag(BE::UIElementFlags::isDisabled, isDisabled);
		//				}				

		//				bool isHovering = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isHovering);
		//				std::string isHovering_str = "isHovering? ";
		//				isHovering_str.append(isHovering ? "True" : "False");
		//				ImGui::Text(isHovering_str.c_str());

		//				bool isPressed = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isPressed);
		//				std::string isPressed_str = "isPressed? ";
		//				isPressed_str.append(isPressed ? "True" : "False");
		//				ImGui::Text(isPressed_str.c_str());
		//				ImGui::Text("");

		//				int GetHorizontal = (int)getElement.get()->GetHorizontalAlignment();

		//				static const char* items_hor[] = { "Left", "Middle", "Right" };
		//				static int selected_hor = GetHorizontal;

		//				ImGui::Text("Horizontal Alignment");

		//				ImGui::Combo("##combo_hor", &selected_hor, items_hor, IM_ARRAYSIZE(items_hor));

		//				if (selected_hor == 0)
		//				{
		//					getElement.get()->SetHorizontalAlignment(BE::UIHorizontalAlignment::left);
		//				}
		//				if (selected_hor == 1)
		//				{
		//					getElement.get()->SetHorizontalAlignment(BE::UIHorizontalAlignment::middle);
		//				}
		//				if (selected_hor == 2)
		//				{
		//					getElement.get()->SetHorizontalAlignment(BE::UIHorizontalAlignment::right);
		//				}
		//				ImGui::Text("");

		//				int GetVertical = (int)getElement.get()->GetVerticalAlignment();

		//				static const char* items_vert[] = { "Top", "Middle", "Bottom" };
		//				static int selected_vert = GetVertical;

		//				ImGui::Text("Vertical Alignment");

		//				ImGui::Combo("##combo_vert", &selected_vert, items_vert, IM_ARRAYSIZE(items_vert));

		//				if (selected_vert == 0)
		//				{
		//					getElement.get()->SetVerticalAlignment(BE::UIVerticalAlignment::top);
		//				}
		//				if (selected_vert == 1)
		//				{
		//					getElement.get()->SetVerticalAlignment(BE::UIVerticalAlignment::middle);
		//				}
		//				if (selected_vert == 2)
		//				{
		//					getElement.get()->SetVerticalAlignment(BE::UIVerticalAlignment::bottom);
		//				}
		//				ImGui::Text("");

		//				float pos[] = { getElement.get()->GetUIPos().x,
		//								getElement.get()->GetUIPos().y,
		//								getElement.get()->GetUIPos().z };
		//				if (ImGui::DragFloat3("Position", pos))
		//				{
		//					getElement.get()->SetUIPos(BE::Vector2D{ pos[0], pos[1], pos[2] });
		//				}
		//				
		//				float scale[] = { getElement.get()->GetUIScale().x,
		//								getElement.get()->GetUIScale().y,
		//								getElement.get()->GetUIScale().z };
		//				if (ImGui::DragFloat3("Scale", scale))
		//				{
		//					getElement.get()->SetUIScale(BE::Vector2D{ scale[0], scale[1], scale[2] });
		//				}
		//				
		//				ImGui::Text("");

		//				std::vector<std::string> AllTexName;

		//				std::vector<BE::TextureFile>& AllTextureFiles = getElement.get()->GetAllTextureFiles();

		//				for (size_t i = 0; i < AllTextureFiles.size(); ++i)
		//				{
		//					const std::string& GetKeyName = AllTextureFiles[i].GetKeyName();
		//					AllTexName.push_back(GetKeyName);
		//				}

		//				if (AllTextureFiles.size() > 0)
		//				{
		//					if (ImGui::BeginListBox("Set Texture To Use"))
		//					{
		//						static int selected;
		//						for (int i = 0; i < (int)AllTexName.size(); ++i)
		//						{
		//							if (ImGui::Selectable(AllTexName[i].c_str(), selected == i))
		//							{
		//								getElement.get()->SetTextureToUse(AllTexName[i].c_str(), 0);
		//								selected = i;
		//							}
		//						}

		//						ImGui::EndListBox();
		//					}
		//				}

		//				for (size_t i = 0; i < AllTextureFiles.size(); ++i)
		//				{
		//					std::string texture = "Texture " + std::to_string(i + 1) + ": " + AllTextureFiles[i].GetKeyName();
		//					if (ImGui::TreeNode(texture.c_str()))
		//					{
		//						const std::string& get_file_name = AllTextureFiles[i].GetFileName();
		//						const char* cstr1 = get_file_name.data();
		//						char* file_name = (char*)(cstr1);

		//						ImGui::Text("File Name:");
		//						ImGui::SameLine();
		//						ImGui::Text(file_name);

		//						const std::string& get_key_name = AllTextureFiles[i].GetKeyName();
		//						const char* cstr2 = get_key_name.data();
		//						char* key_name = (char*)(cstr2);

		//						ImGui::Text("Key Name:");
		//						ImGui::SameLine();
		//						ImGui::Text(key_name);

		//						int sprite_u = (int)AllTextureFiles[i].GetSpriteU();
		//						int sprite_v = (int)AllTextureFiles[i].GetSpriteV();

		//						ImGui::Text("Sprite Dimension");
		//						ImGui::DragInt("sprite_u", &sprite_u);
		//						ImGui::DragInt("sprite_v", &sprite_v);

		//						if (sprite_u != 0 && sprite_v != 0)
		//						{
		//							AllTextureFiles[i].SetSpriteU(sprite_u);
		//							AllTextureFiles[i].SetSpriteV(sprite_v);
		//						}

		//						if (sprite_u != 1)
		//						{
		//							auto frame_vector = AllTextureFiles[i].GetNumberOfFramesPerState();

		//							size_t vec_size = frame_vector.size();

		//							std::vector<size_t> frame;
		//							for (size_t j = 0; j < sprite_v; ++j)
		//							{
		//								int state;
		//								if (j < vec_size)
		//								{
		//									state = (int)frame_vector[j];
		//								}
		//								else
		//								{
		//									state = 0;
		//								}

		//								std::string str = "State " + std::to_string(j);
		//								ImGui::DragInt(str.c_str(), &state);
		//								frame.push_back((size_t)state);
		//							}

		//							AllTextureFiles[i].SetNumberofFrames(frame);
		//							frame.clear();

		//							float framepersec = AllTextureFiles[i].GetFramesPerSecond();
		//							ImGui::DragFloat("Frame Per Sec", &framepersec);
		//							AllTextureFiles[i].SetFramesPerSecond(framepersec);

		//							int current_state = (int)AllTextureFiles[i].GetCurrentState();
		//							ImGui::SliderInt("Current State", &current_state, 0, (int)vec_size - 1);
		//							AllTextureFiles[i].SetCurrentState(current_state);
		//						}

		//						ImGui::TreePop();
		//					}
		//				}

		//				if (ImGui::TreeNode("Add Texture"))
		//				{
		//					static std::string file_path;
		//					static std::string key_name;

		//					static ImGuiTextFilter filter;
		//					filter.Draw(ICON_FA_SEARCH);

		//					static int add_tex_selected;
		//					std::vector<std::string> AllTextureFiles_selectable;

		//					for (auto& it : BE::AssetMgr->GetAllTextureFiles())
		//					{
		//						AllTextureFiles_selectable.push_back((it.first).substr(it.first.find_last_of("/\\") + 1));
		//					}

		//					if (ImGui::BeginListBox("Textures"))
		//					{
		//						for (int i = 0; i < (int)AllTextureFiles_selectable.size(); ++i)
		//						{
		//							if (filter.PassFilter(AllTextureFiles_selectable[i].c_str()))
		//							{
		//								if (ImGui::Selectable(AllTextureFiles_selectable[i].c_str(), add_tex_selected == i))
		//								{
		//									add_tex_selected = i;

		//									for (auto& it : BE::AssetMgr->GetAllTextureFiles())
		//									{
		//										std::size_t found = it.first.find(AllTextureFiles_selectable[i]);
		//										if (found != std::string::npos)
		//										{
		//											file_path = it.first;
		//											key_name = AllTextureFiles_selectable[i].substr(0, AllTextureFiles_selectable[i].find_last_of("."));
		//										}
		//									}
		//								}
		//							}
		//						}

		//						ImGui::EndListBox();
		//					}

		//					static float color[4] = { 0 };
		//					ImGui::DragFloat4("Color", color);

		//					static int sprite_u = 1;
		//					ImGui::DragInt("sprite_u", &sprite_u);

		//					static int sprite_v = 1;
		//					ImGui::DragInt("sprite_v", &sprite_v);

		//					static float framesPerSecond = 0.f;
		//					ImGui::DragFloat("Frame Per Sec", &framesPerSecond);

		//					static int currState = 0;
		//					ImGui::SliderInt("Current State", &currState, 0, (sprite_v - 1));

		//					static bool usingTexture = true;
		//					ImGui::Checkbox("Use Texture?", &usingTexture);

		//					if (ImGui::Button("Add"))
		//					{
		//						static std::vector<size_t> temp;

		//						if (sprite_v != 0)
		//						{
		//							for (int j = 0; j < sprite_v; ++j)
		//							{
		//								temp.push_back(sprite_u);
		//							}
		//						}

		//						if (sprite_u <= 0)
		//							sprite_u = 1;

		//						if (sprite_v <= 0)
		//							sprite_v = 1;

		//						getElement.get()->AddTextureFile(std::move(file_path),
		//							std::move(key_name),
		//							sprite_u,
		//							sprite_v,
		//							framesPerSecond,
		//							currState,
		//							temp);
		//					}

		//					ImGui::TreePop();
		//				}

		//				if (ImGui::TreeNode("Remove Texture"))
		//				{
		//					static std::string file_path;
		//					static int remove_tex_selected;

		//					if (ImGui::BeginListBox("Textures"))
		//					{
		//						for (int i = 0; i < (int)AllTextureFiles.size(); ++i)
		//						{
		//							if (ImGui::Selectable(AllTextureFiles[i].GetKeyName().c_str(), remove_tex_selected == i))
		//							{
		//								remove_tex_selected = i;
		//								file_path = AllTextureFiles[i].GetFileName();
		//							}
		//						}

		//						ImGui::EndListBox();
		//					}

		//					if (ImGui::Button("Remove"))
		//					{
		//						getElement.get()->RemoveTextureFile(file_path);
		//					}

		//					ImGui::TreePop();
		//				}

		//				ImGui::Text("");

		//				const std::vector<BE::LuaScript> GetAllScript = getElement.get()->GetAllLuaScripts();

		//				for (size_t i = 0; i < GetAllScript.size(); ++i)
		//				{
		//					ImGui::Text("Script %d:", (int)(i + 1));

		//					const std::string& getFilePath = GetAllScript[i].GetFileName();
		//					ImGui::Text("Script File: %s", getFilePath.c_str());
		//					const std::string& getKeyPath = GetAllScript[i].GetKeyName();
		//					ImGui::Text("Script Name: %s", getKeyPath.c_str());

		//					ImGui::Text("");
		//				}

		//				if (ImGui::TreeNode("Add Script"))
		//				{
		//					static std::string script_path;
		//					static std::string script_name;

		//					static ImGuiTextFilter filter;
		//					filter.Draw(ICON_FA_SEARCH);

		//					static int set_script_selected;
		//					std::vector<std::string> AllScriptFiles_selectable;

		//					for (auto& it : BE::AssetMgr->GetAllScriptFiles())
		//					{
		//						AllScriptFiles_selectable.push_back(it.substr(it.find_last_of("/\\") + 1));
		//					}

		//					if (ImGui::BeginListBox("Scripts"))
		//					{
		//						for (int i = 0; i < (int)AllScriptFiles_selectable.size(); ++i)
		//						{
		//							if (filter.PassFilter(AllScriptFiles_selectable[i].c_str()))
		//							{
		//								if (ImGui::Selectable(AllScriptFiles_selectable[i].c_str(), set_script_selected == i))
		//								{
		//									set_script_selected = i;

		//									for (auto& it : BE::AssetMgr->GetAllScriptFiles())
		//									{
		//										std::size_t found = it.find(AllScriptFiles_selectable[i]);
		//										if (found != std::string::npos)
		//										{
		//											script_path = it;
		//											script_name = AllScriptFiles_selectable[i].substr(0, AllScriptFiles_selectable[i].find_last_of("."));
		//										}
		//									}
		//								}
		//							}
		//						}

		//						ImGui::EndListBox();
		//					}

		//					if (ImGui::Button("Add"))
		//					{
		//						getElement.get()->AddLuaScripts(std::move(script_path), std::move(script_name));
		//					}

		//					ImGui::TreePop();
		//				}

		//				if (ImGui::TreeNode("Remove Script"))
		//				{
		//					static std::string file_path;
		//					static int remove_script_selected;

		//					if (ImGui::BeginListBox("Scripts"))
		//					{
		//						for (int i = 0; i < (int)GetAllScript.size(); ++i)
		//						{
		//							if (ImGui::Selectable(GetAllScript[i].GetKeyName().c_str(), remove_script_selected == i))
		//							{
		//								remove_script_selected = i;
		//								file_path = GetAllScript[i].GetFileName();
		//							}
		//						}

		//						ImGui::EndListBox();
		//					}

		//					if (ImGui::Button("Remove"))
		//					{
		//						getElement.get()->RemoveLuaScripts(std::move(file_path));
		//					}

		//					ImGui::TreePop();
		//				}

		//				ImGui::Text("");

		//				//Button
		//				if (Gettype == 0)
		//				{
		//					bool isActive = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isActive);
		//					if (ImGui::Checkbox("isActive?", &isActive))
		//					{
		//						getElement.get()->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
		//					}					

		//					std::shared_ptr<BE::UIButton> sPtr = std::dynamic_pointer_cast<BE::UIButton>(BE::UI->GetUIElement(getcanvas, getID));

		//					float NormTintClr[] = { sPtr->GetNormalTintColour().r,
		//											sPtr->GetNormalTintColour().g,
		//											sPtr->GetNormalTintColour().b,
		//											sPtr->GetNormalTintColour().a };
		//					if (ImGui::DragFloat4("Normal Tint Colour", NormTintClr))
		//					{
		//						sPtr->SetNormalTintColour(BE::Colour4F{ NormTintClr[0], NormTintClr[1], NormTintClr[2], NormTintClr[3] });
		//					}						

		//					float PressTintClr[] = { sPtr->GetPressedTintColour().r,
		//											sPtr->GetPressedTintColour().g,
		//											sPtr->GetPressedTintColour().b,
		//											sPtr->GetPressedTintColour().a };
		//					if (ImGui::DragFloat4("Pressed Tint Colour", PressTintClr))
		//					{
		//						sPtr->SetPressedTintColour(BE::Colour4F{ PressTintClr[0], PressTintClr[1], PressTintClr[2], PressTintClr[3] });
		//					}
		//					
		//					float DisableTintClr[] = { sPtr->GetDisabledTintColour().r,
		//											sPtr->GetDisabledTintColour().g,
		//											sPtr->GetDisabledTintColour().b,
		//											sPtr->GetDisabledTintColour().a };
		//					if (ImGui::DragFloat4("Disabled Tint Colour", DisableTintClr))
		//					{
		//						sPtr->SetDisabledTintColour(BE::Colour4F{ DisableTintClr[0], DisableTintClr[1], DisableTintClr[2], DisableTintClr[3] });
		//					}
		//					
		//					float HoverTintClr[] = { sPtr->GetHoverTintColour().r,
		//											sPtr->GetHoverTintColour().g,
		//											sPtr->GetHoverTintColour().b,
		//											sPtr->GetHoverTintColour().a };
		//					if (ImGui::DragFloat4("Hovered Tint Colour", HoverTintClr))
		//					{
		//						sPtr->SetHoveredTintColour(BE::Colour4F{ HoverTintClr[0], HoverTintClr[1], HoverTintClr[2], HoverTintClr[3] });
		//					}
		//					
		//					ImGui::Text("");
		//					float TintClrToUse[] = { sPtr->GetTintColourToUse().r,
		//											sPtr->GetTintColourToUse().g,
		//											sPtr->GetTintColourToUse().b,
		//											sPtr->GetTintColourToUse().a };
		//					ImGui::Text("Tint Colour To Use");
		//					ImGui::InputFloat4("##TintToUse", TintClrToUse);

		//					ImGui::Text("");

		//					ImGui::Text("Pressed Sound Path:");
		//					ImGui::SameLine();
		//					ImGui::Text(sPtr->GetPressedSound().GetFileName().c_str());

		//					ImGui::Text("Hovered Sound Path:");
		//					ImGui::SameLine();
		//					ImGui::Text(sPtr->GetHoveredSound().GetFileName().c_str());

		//					UI_Inspector_Window::Add_Remove_Audio_Que(sPtr, Audio_UI::Button);
		//				}

		//				//FontText
		//				else if (Gettype == 1)
		//				{
		//					std::shared_ptr<BE::UIFontText> sPtr = std::dynamic_pointer_cast<BE::UIFontText>(BE::UI->GetUIElement(getcanvas, getID));

		//					const std::string& GetText = sPtr->GetText();
		//					const char* button_text = GetText.data();
		//					char* text = (char*)(button_text);

		//					if (ImGui::InputText("Text", text, 100))
		//					{
		//						sPtr->SetText(text);
		//					}						

		//					ImGui::Text("");

		//					static std::string font_path;

		//					static ImGuiTextFilter filter;
		//					filter.Draw(ICON_FA_SEARCH);

		//					static int set_font_selected;
		//					std::vector<std::string> AllFontFiles_selectable;
							
		//					BE::FontFile
		//					for (auto& it : BE::AssetMgr->GetallFontFiles())
		//					{
		//						AllFontFiles_selectable.push_back(it.first.substr(it.first.find_last_of("/\\") + 1));
		//					}

		//					if (ImGui::BeginListBox("Set Font To Use"))
		//					{
		//						for (int i = 0; i < (int)AllFontFiles_selectable.size(); ++i)
		//						{
		//							if (filter.PassFilter(AllFontFiles_selectable[i].c_str()))
		//							{
		//								if (ImGui::Selectable(AllFontFiles_selectable[i].c_str(), set_font_selected == i))
		//								{
		//									set_font_selected = i;

		//									for (auto& it : BE::AssetMgr->GetallFontFiles())
		//									{
		//										std::size_t found = it.first.find(AllFontFiles_selectable[i]);
		//										if (found != std::string::npos)
		//										{
		//											font_path = it.first;
		//										}
		//									}

		//									sPtr->SetFont(std::move(font_path));
		//								}
		//							}
		//						}

		//						ImGui::EndListBox();
		//					}

		//					if (sPtr->GetFont().size() != 0)
		//					{
		//						bool isActive = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isActive);
		//						if (ImGui::Checkbox("isActive?", &isActive))
		//						{
		//							getElement.get()->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
		//						}					
		//					}

		//					ImGui::Text("");

		//					bool MiddleAlign = sPtr->GetMiddleAlign();
		//					if (ImGui::Checkbox("MiddleAlign?", &MiddleAlign))
		//					{
		//						sPtr->SetMiddleAlign(MiddleAlign);
		//					}						

		//					float GetColour[] = { sPtr->GetColour().r,
		//										sPtr->GetColour().g,
		//										sPtr->GetColour().b,
		//										sPtr->GetColour().a };
		//					if (ImGui::DragFloat4("Text Colour", GetColour))
		//					{
		//						sPtr->SetColour(BE::Colour4F{ GetColour[0], GetColour[1], GetColour[2], GetColour[3] });
		//					}				
		//				}

		//				//Image
		//				else if (Gettype == 2)
		//				{
		//					bool isActive = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isActive);
		//					if (ImGui::Checkbox("isActive?", &isActive))
		//					{
		//						getElement.get()->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
		//					}					
		//				}

		//				//Slider
		//				else if (Gettype == 3)
		//				{
		//					bool isActive = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isActive);
		//					if (ImGui::Checkbox("isActive?", &isActive))
		//					{
		//						getElement.get()->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
		//					}
		//					
		//					std::shared_ptr<BE::UISlider> sPtr = std::dynamic_pointer_cast<BE::UISlider>(BE::UI->GetUIElement(getcanvas, getID));

		//					float NormTintClr[] = { sPtr->GetNormalTintColour().r,
		//											sPtr->GetNormalTintColour().g,
		//											sPtr->GetNormalTintColour().b,
		//											sPtr->GetNormalTintColour().a };
		//					if (ImGui::DragFloat4("Normal Tint Colour", NormTintClr))
		//					{
		//						sPtr->SetNormalTintColour(BE::Colour4F{ NormTintClr[0], NormTintClr[1], NormTintClr[2], NormTintClr[3] });
		//					}						

		//					float PressTintClr[] = { sPtr->GetPressedTintColour().r,
		//											sPtr->GetPressedTintColour().g,
		//											sPtr->GetPressedTintColour().b,
		//											sPtr->GetPressedTintColour().a };
		//					if (ImGui::DragFloat4("Pressed Tint Colour", PressTintClr))
		//					{
		//						sPtr->SetPressedTintColour(BE::Colour4F{ PressTintClr[0], PressTintClr[1], PressTintClr[2], PressTintClr[3] });
		//					}
		//					
		//					float DisableTintClr[] = { sPtr->GetDisabledTintColour().r,
		//											sPtr->GetDisabledTintColour().g,
		//											sPtr->GetDisabledTintColour().b,
		//											sPtr->GetDisabledTintColour().a };
		//					if (ImGui::DragFloat4("Disabled Tint Colour", DisableTintClr))
		//					{
		//						sPtr->SetDisabledTintColour(BE::Colour4F{ DisableTintClr[0], DisableTintClr[1], DisableTintClr[2], DisableTintClr[3] });
		//					}						

		//					float HoverTintClr[] = { sPtr->GetHoverTintColour().r,
		//											sPtr->GetHoverTintColour().g,
		//											sPtr->GetHoverTintColour().b,
		//											sPtr->GetHoverTintColour().a };
		//					if (ImGui::DragFloat4("Hovered Tint Colour", HoverTintClr))
		//					{
		//						sPtr->SetHoveredTintColour(BE::Colour4F{ HoverTintClr[0], HoverTintClr[1], HoverTintClr[2], HoverTintClr[3] });
		//					}
		//					
		//					float TintClrToUse[] = { sPtr->GetTintColourToUse().r,
		//											sPtr->GetTintColourToUse().g,
		//											sPtr->GetTintColourToUse().b,
		//											sPtr->GetTintColourToUse().a };
		//					ImGui::Text("Tint Colour To Use");
		//					ImGui::InputFloat4("##TintToUse", TintClrToUse);

		//					ImGui::Text("");					

		//					float SliderPercentage = sPtr->GetSliderPercentage();
		//					if (ImGui::SliderFloat("Slider Percentage", &SliderPercentage, 0.0f, 1.0f))
		//					{
		//						sPtr->SetSliderPercentage(SliderPercentage);
		//					}				

		//					float SliderKnobScale[] = { sPtr->GetSliderKnobScale().x,
		//								sPtr->GetSliderKnobScale().y,
		//								sPtr->GetSliderKnobScale().z };
		//					if (ImGui::DragFloat3("Slider Knob Scale", SliderKnobScale))
		//					{
		//						sPtr->SetSliderKnobScale(BE::Vector2D{ SliderKnobScale[0], SliderKnobScale[1], SliderKnobScale[2] });
		//					}						
		//					
		//					ImGui::Text("");

		//					ImGui::Text("Pressed Sound Path:");
		//					ImGui::SameLine();
		//					ImGui::Text(sPtr->GetPressedSound().GetFileName().c_str());

		//					ImGui::Text("Hovered Sound Path:");
		//					ImGui::SameLine();
		//					ImGui::Text(sPtr->GetHoveredSound().GetFileName().c_str());

		//					UI_Inspector_Window::Add_Remove_Audio_Que(sPtr, Audio_UI::Slider);
		//				}

		//				//Toggle
		//				else if (Gettype == 4)
		//				{
		//					bool isActive = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isActive);
		//					if (ImGui::Checkbox("isActive?", &isActive))
		//					{
		//						getElement.get()->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
		//					}
		//					
		//					std::shared_ptr<BE::UIToggle> sPtr = std::dynamic_pointer_cast<BE::UIToggle>(BE::UI->GetUIElement(getcanvas, getID));

		//					bool enabletoggle = sPtr->GetToggle();
		//					if (ImGui::Checkbox("Enable Toggle?", &enabletoggle))
		//					{
		//						sPtr->SetToggle(enabletoggle);
		//					}							

		//					float NormTintClr[] = { sPtr->GetNormalTintColour().r,
		//											sPtr->GetNormalTintColour().g,
		//											sPtr->GetNormalTintColour().b,
		//											sPtr->GetNormalTintColour().a };
		//					if (ImGui::DragFloat4("Normal Tint Colour", NormTintClr))
		//					{
		//						sPtr->SetNormalTintColour(BE::Colour4F{ NormTintClr[0], NormTintClr[1], NormTintClr[2], NormTintClr[3] });
		//					}						

		//					float PressTintClr[] = { sPtr->GetPressedTintColour().r,
		//											sPtr->GetPressedTintColour().g,
		//											sPtr->GetPressedTintColour().b,
		//											sPtr->GetPressedTintColour().a };
		//					if (ImGui::DragFloat4("Pressed Tint Colour", PressTintClr))
		//					{
		//						sPtr->SetPressedTintColour(BE::Colour4F{ PressTintClr[0], PressTintClr[1], PressTintClr[2], PressTintClr[3] });
		//					}						

		//					float DisableTintClr[] = { sPtr->GetDisabledTintColour().r,
		//											sPtr->GetDisabledTintColour().g,
		//											sPtr->GetDisabledTintColour().b,
		//											sPtr->GetDisabledTintColour().a };
		//					if (ImGui::DragFloat4("Disabled Tint Colour", DisableTintClr))
		//					{
		//						sPtr->SetDisabledTintColour(BE::Colour4F{ DisableTintClr[0], DisableTintClr[1], DisableTintClr[2], DisableTintClr[3] });
		//					}							

		//					float HoverTintClr[] = { sPtr->GetHoverTintColour().r,
		//											sPtr->GetHoverTintColour().g,
		//											sPtr->GetHoverTintColour().b,
		//											sPtr->GetHoverTintColour().a };
		//					if (ImGui::DragFloat4("Hovered Tint Colour", HoverTintClr))
		//					{
		//						sPtr->SetHoveredTintColour(BE::Colour4F{ HoverTintClr[0], HoverTintClr[1], HoverTintClr[2], HoverTintClr[3] });
		//					}					

		//					float TintClrToUse[] = { sPtr->GetTintColourToUse().r,
		//											sPtr->GetTintColourToUse().g,
		//											sPtr->GetTintColourToUse().b,
		//											sPtr->GetTintColourToUse().a };
		//					ImGui::Text("Tint Colour To Use");
		//					ImGui::InputFloat4("##TintToUse", TintClrToUse);

		//					ImGui::Text("");

		//					ImGui::Text("Pressed Sound Path:");
		//					ImGui::SameLine();
		//					ImGui::Text(sPtr->GetPressedSound().GetFileName().c_str());

		//					ImGui::Text("Hovered Sound Path:");
		//					ImGui::SameLine();
		//					ImGui::Text(sPtr->GetHoveredSound().GetFileName().c_str());

		//					UI_Inspector_Window::Add_Remove_Audio_Que(sPtr, Audio_UI::Toggle);
		//				}

		//				//ProgressBar
		//				else if (Gettype == 5)
		//				{
		//					bool isActive = getElement.get()->GetUIElementFlag(BE::UIElementFlags::isActive);
		//					if (ImGui::Checkbox("isActive?", &isActive))
		//					{
		//						getElement.get()->SetUIElementFlag(BE::UIElementFlags::isActive, isActive);
		//					}						

		//					std::shared_ptr<BE::UIProgressBar> sPtr = std::dynamic_pointer_cast<BE::UIProgressBar>(BE::UI->GetUIElement(getcanvas, getID));

		//					float GetPercentage = sPtr->GetPercentage();
		//					if (ImGui::SliderFloat("Percentage", &GetPercentage, 0, 1))
		//					{
		//						sPtr->SetPercentage(GetPercentage);
		//					}
		//					
		//					ImGui::Text("");

		//					bool isflicker = sPtr->GetIsFlickering();
		//					ImGui::Checkbox("Enable Flickering?", &isflicker);

		//					if (isflicker)
		//					{
		//						float GetFlickerTimePerSecond = sPtr->GetFlickerTimePerSecond();
		//						ImGui::DragFloat("Flicker Rate Per Sec", &GetFlickerTimePerSecond);

		//						float GetFlickerThreshold = sPtr->GetFlickerThreshold();
		//						ImGui::SliderFloat("Flicker Threshold", &GetFlickerThreshold, 0.0, 1.0);

		//						sPtr->SetFlickering(GetFlickerTimePerSecond, GetFlickerThreshold);
		//					}
		//					else
		//					{
		//						sPtr->DisableFlickering();
		//					}
		//				}

		//				ImGui::Text("");
		//			}
		//		}

		//		end();
		//	}
		//}
	//}

	void UI_Inspector_Window::Shutdown()
	{
		delete p_ui_insp;
	}

	template<class T>
	void UI_Inspector_Window::UI_Interactable(std::shared_ptr<T> sPtr)
	{
		//Texture
		ImGui::Text("Normal Texture:");
		auto buttonNormal = sPtr->GetTexture(UIInteractStatus::normal);
		ImGui::SameLine();
		ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(buttonNormal).c_str());

		ImGui::Text("Pressed Texture:");
		auto buttonPress = sPtr->GetTexture(UIInteractStatus::pressed);
		ImGui::SameLine();
		ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(buttonPress).c_str());

		ImGui::Text("Hovered Texture:");
		auto buttonHover = sPtr->GetTexture(UIInteractStatus::hovered);
		ImGui::SameLine();
		ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(buttonHover).c_str());

		ImGui::Text("Disabled Texture:");
		auto buttonDisable = sPtr->GetTexture(UIInteractStatus::disabled);
		ImGui::SameLine();
		ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(buttonDisable).c_str());

		ImGui::Text("");

		const auto& TextureAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::Texture>();

		static ImGuiTextFilter TextureFilter;
		TextureFilter.Draw(ICON_FA_SEARCH "##Texture");

		static int add_texture_selected;
		std::vector<std::string> AllTextureFiles_selectable;
		std::vector<std::string> FullPath;

		for (auto& it : TextureAsset)
		{
			AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
			//AllTextureFiles_selectable.push_back(BE::AssetMgr->GetAssetFileName(it.first));
			FullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
		}

		if (ImGui::BeginListBox("Texture"))
		{
			for (int i = 0; i < (int)AllTextureFiles_selectable.size(); ++i)
			{
				if (TextureFilter.PassFilter(FullPath[i].c_str()))
				{
					ImGui::PushID(i);
					if (ImGui::Selectable(AllTextureFiles_selectable[i].c_str(), add_texture_selected == i))
					{
						add_texture_selected = i;

						InteractableTextureName = FullPath[i];
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(FullPath[i].c_str());
						ImGui::EndTooltip();
					}
					ImGui::PopID();
				}
			}

			ImGui::EndListBox();
		}

		if (ImGui::Button("Set Normal Texture"))
		{
			sPtr->SetTexture(UIInteractStatus::normal, BE::AssetMgr->GetAssetID(InteractableTextureName));
		}
		ImGui::SameLine();
		if (ImGui::Button("Set Pressed Texture"))
		{
			sPtr->SetTexture(UIInteractStatus::pressed, BE::AssetMgr->GetAssetID(InteractableTextureName));
		}
		ImGui::SameLine();
		if (ImGui::Button("Set Hovered Texture"))
		{
			sPtr->SetTexture(UIInteractStatus::hovered, BE::AssetMgr->GetAssetID(InteractableTextureName));
		}
		ImGui::SameLine();
		if (ImGui::Button("Set Disabled Texture"))
		{
			sPtr->SetTexture(UIInteractStatus::disabled, BE::AssetMgr->GetAssetID(InteractableTextureName));
		}

		ImGui::Text("");

		//AudioProperties
		ImGui::Text("Pressed Audio:");
		auto AudiobuttonPress = sPtr->GetAudioQue(UIAudioQueType::pressed);
		ImGui::SameLine();
		ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(AudiobuttonPress).c_str());

		ImGui::Text("Hovered Audio:");
		auto AudiobuttonHover = sPtr->GetAudioQue(UIAudioQueType::hovered);
		ImGui::SameLine();
		ImGui::Text(BE::AssetMgr->GetAssetTruncatedFileName(AudiobuttonHover).c_str());

		ImGui::Text("");

		const auto& AudioAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::AudioClip>();

		static ImGuiTextFilter Audiofilter;
		Audiofilter.Draw(ICON_FA_SEARCH "##Audio");

		static int add_audio_selected;
		std::vector<std::string> AllAudioFiles_selectable;
		std::vector<std::string> AudioFullPath;

		for (auto& it : AudioAsset)
		{
			AllAudioFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
			AudioFullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
		}

		if (ImGui::BeginListBox("Audio"))
		{
			for (int i = 0; i < (int)AllAudioFiles_selectable.size(); ++i)
			{
				if (Audiofilter.PassFilter(AllAudioFiles_selectable[i].c_str()))
				{
					if (ImGui::Selectable(AllAudioFiles_selectable[i].c_str(), add_audio_selected == i))
					{
						add_audio_selected = i;

						InteractableAudioName = AudioFullPath[i];
					}
				}
			}

			ImGui::EndListBox();
		}
		if (ImGui::Button("Set Pressed Audio"))
		{
			sPtr->SetAudioQue(UIAudioQueType::pressed, BE::AssetMgr->GetAssetID(InteractableAudioName));
		}
		ImGui::SameLine();
		if (ImGui::Button("Set Hovered Audio"))
		{
			sPtr->SetAudioQue(UIAudioQueType::hovered, BE::AssetMgr->GetAssetID(InteractableAudioName));
		}

		ImGui::Text("");

		float volume = sPtr->GetVolume();
		if (ImGui::DragFloat("Volume", &volume, 0.1f, MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME))
		{
			sPtr->SetVolume(volume);
		}

		float pitch = sPtr->GetPitch();
		if (ImGui::DragFloat("Pitch", &pitch, 0.1f, MIN_AUDIO_PITCH, MAX_AUDIO_PITCH))
		{
			sPtr->SetPitch(pitch);
		}

		ImGui::Text("Highest Priority: %d", MAX_AUDIO_PRIORITY);
		ImGui::Text("Lowest Priority: %d", MIN_AUDIO_PRIORITY);
		int priority = sPtr->GetPriority();
		if (ImGui::DragInt("Priority", &priority, 1, MAX_AUDIO_PRIORITY, MIN_AUDIO_PRIORITY))
		{
			sPtr->SetPriority(priority);
		}

		ImGui::Text("");
	}

	/*void UI_Inspector_Window::Add_Remove_Audio_Que(std::shared_ptr<BE::UIElement> sPtr, Audio_UI UIType)
	{
		std::shared_ptr<BE::UIButton> button;
		std::shared_ptr<BE::UISlider> slider;
		std::shared_ptr<BE::UIToggle> toggle;

		if (UIType == Audio_UI::Button)
		{
			button = std::dynamic_pointer_cast<BE::UIButton>(sPtr);
		}
		if (UIType == Audio_UI::Slider)
		{
			slider = std::dynamic_pointer_cast<BE::UISlider>(sPtr);
		}
		if (UIType == Audio_UI::Toggle)
		{
			toggle = std::dynamic_pointer_cast<BE::UIToggle>(sPtr);
		}
		
		if (ImGui::TreeNode("Add AudioProperties Que"))
		{
			static const char* items[] = { "pressed", "hovered" };
			static int selected = 0;

			ImGui::Text("UI AudioProperties Que Type");

			ImGui::Combo("##combo", &selected, items, IM_ARRAYSIZE(items));

			static std::string file_path;

			static ImGuiTextFilter filter;
			filter.Draw(ICON_FA_SEARCH);

			static int add_audio_selected;
			std::vector<std::string> AllAudioFiles_selectable;

			for (auto& it : BE::AssetMgr->GetAllAudioFiles())
			{
				AllAudioFiles_selectable.push_back((it.first).substr(it.first.find_last_of("/\\") + 1));
			}

			if (ImGui::BeginListBox("AudioProperties"))
			{
				for (int i = 0; i < (int)AllAudioFiles_selectable.size(); ++i)
				{
					if (filter.PassFilter(AllAudioFiles_selectable[i].c_str()))
					{
						if (ImGui::Selectable(AllAudioFiles_selectable[i].c_str(), add_audio_selected == i))
						{
							add_audio_selected = i;

							for (auto& it : BE::AssetMgr->GetAllAudioFiles())
							{
								std::size_t found = it.first.find(AllAudioFiles_selectable[i]);
								if (found != std::string::npos)
								{
									file_path = it.first;
								}
							}
						}
					}
				}

				ImGui::EndListBox();
			}

			float volume = _volume;
			ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);
			_volume = volume;

			float pitch = _pitch;
			ImGui::SliderFloat("Pitch", &pitch, 0.5f, 2.0f);
			_pitch = pitch;

			if (ImGui::Button("Add"))
			{
				if (selected == 0)
				{
					_type = BE::UIAudioQueType::pressed;
				}
				if (selected == 1)
				{
					_type = BE::UIAudioQueType::hovered;
				}

				if (UIType == Audio_UI::Button)
				{
					button->AddAudioQue(_type, std::move(file_path), _volume, _pitch);
				}
				if (UIType == Audio_UI::Slider)
				{
					slider->AddAudioQue(_type, std::move(file_path), _volume, _pitch);
				}
				if (UIType == Audio_UI::Toggle)
				{
					toggle->AddAudioQue(_type, std::move(file_path), _volume, _pitch);
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Remove AudioProperties Que"))
		{
			static const char* remove_items[] = { "pressed", "hovered" };
			static int remove_selected = 0;

			ImGui::Text("UI AudioProperties Que Type");

			ImGui::Combo("##combo", &remove_selected, remove_items, IM_ARRAYSIZE(remove_items));

			if (ImGui::Button("Remove"))
			{
				if (remove_selected == 0)
				{
					_type = BE::UIAudioQueType::pressed;
				}
				if (remove_selected == 1)
				{
					_type = BE::UIAudioQueType::hovered;
				}

				if (UIType == Audio_UI::Button)
				{
					button->RemoveAudioQue(_type);
				}
				if (UIType == Audio_UI::Slider)
				{
					slider->RemoveAudioQue(_type);
				}
				if (UIType == Audio_UI::Toggle)
				{
					toggle->RemoveAudioQue(_type);
				}
			}

			ImGui::TreePop();
		}
	}*/
}

#endif
