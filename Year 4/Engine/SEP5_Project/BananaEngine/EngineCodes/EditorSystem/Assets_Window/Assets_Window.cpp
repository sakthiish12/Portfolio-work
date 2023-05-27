/******************************************************************************/
/*!
\file		Assets_Window.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Get all game resources into the engine

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "Assets_Window.h"

#ifdef USE_EDITOR

#include "../Menu_Window/Menu_Window.h"
#include "../Resources_Window/Resources_Window.h"
#include "../EditorSystem/EditorSystem.h"

namespace BE
{
	bool UnloadAllAsset = false;

	Assets_Window* Assets_Window::p_asset = nullptr;

	std::string Assets_Window::prefabs_filepath{};

	Assets_Window::Assets_Window()
	{
		//_mode = SH::AudioMode::audioLoopOff;
	}

	Assets_Window::~Assets_Window()
	{
		
	}

	Assets_Window* Assets_Window::init()
	{
		p_asset = new Assets_Window();
		return p_asset;
	}

	void Assets_Window::update()
	{
		if (Menu_Window::Get()->showAssetsWindow)
		{
			if (!begin("Assets", &Menu_Window::Get()->showAssetsWindow))
			{
				end();
			}
			else
			{
				//auto GetFileTypes = BE::AssetMgr->GetFileTypes();

				//for (auto const& x : GetFileTypes)
				//{
				//	//std::cout << x.first << ':' << x.second << std::endl;
				//}

				static float padding = 5.0f;
				static float thumbnailSize = 70;
				float cellSize = thumbnailSize + padding;
				float panelWidth = ImGui::GetContentRegionAvail().x;
				int columnCount = (int)(panelWidth / cellSize);
				if (columnCount < 1)
				{
					columnCount = 1;
				}

				static char file_path[10000] = { 0 };
				ImGui::InputText("File Path", file_path, IM_ARRAYSIZE(file_path));

				//Drag and Drop
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload * payload{ ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM") })
					{
						strcpy_s(file_path, Resources_Window::getdragdrop_path().c_str());
					}
					ImGui::EndDragDropTarget();
				}

				static bool Horizontal = false;
				ImGui::Checkbox("Horizontal Button?", &Horizontal);

				if (ImGui::Button(ICON_FA_PLUS_SQUARE " Load Asset"))
				{
					std::vector<std::string> vec = Resources_Window::getdragdrop_vector();

					if (vec.size() > 1)
					{
						for (size_t i = 0; i < vec.size(); ++i)
						{
							std::string extensionType = std::string(vec[i]).substr(std::string(vec[i]).find_last_of(".") + 1);
							std::transform(extensionType.begin(), extensionType.end(), extensionType.begin(),
								[](char c) { return static_cast<char>(std::tolower(c)); });

							std::string newfilePath{ std::string(vec[i]) };

							if (std::strcmp(extensionType.c_str(), "fbx") == 0)
							{
								BE::AssetMgr->convertModel(std::string(vec[i]));
								newfilePath.replace(newfilePath.find_last_of("."), newfilePath.length() - newfilePath.find_first_of("."), ".bananaModel");
							}
							else if (std::strcmp(extensionType.c_str(), "png") == 0)
							{
								BE::AssetMgr->convertImage(std::string(vec[i]), ImageChannelsFormat::normal);
								newfilePath.replace(newfilePath.find_last_of("."), newfilePath.length() - newfilePath.find_first_of("."), ".dds");
							}

							BE::AssetMgr->LoadAsset(newfilePath, 0);
						}
					}
					else
					{
						std::string extensionType = std::string(file_path).substr(std::string(file_path).find_last_of(".") + 1);
						std::transform(extensionType.begin(), extensionType.end(), extensionType.begin(),
							[](char c) { return static_cast<char>(std::tolower(c)); });

						std::string newfilePath{ std::string(file_path) };

						if (std::strcmp(extensionType.c_str(), "fbx") == 0)
						{
							BE::AssetMgr->convertModel(std::string(file_path));
							newfilePath.replace(newfilePath.find_last_of("."), newfilePath.length() - newfilePath.find_first_of("."), ".bananaModel");
						}
						else if (std::strcmp(extensionType.c_str(), "png") == 0)
						{
							BE::AssetMgr->convertImage(std::string(file_path), ImageChannelsFormat::normal);
							newfilePath.replace(newfilePath.find_last_of("."), newfilePath.length() - newfilePath.find_first_of("."), ".dds");
						}

						BE::AssetMgr->LoadAsset(newfilePath, 0);
					}

					strcpy_s(file_path, "");
				}

				ImGui::SameLine();

				if (ImGui::Button(ICON_FA_ADJUST " Regen Asset"))
				{
					BE::AssetMgr->regenerateThisAsset(std::string(file_path));
					BE::AssetMgr->setWaitingToRegenerateAssets(true);
					strcpy_s(file_path, "");
				}

				ImGui::Text("");

				if (Horizontal)
				{
					if (ImGui::Button(ICON_FA_TRASH " Unload Unused Assets"))
					{
						UnloadAllAsset = true;
					}
					ImGui::SameLine();

					if (UnloadAllAsset)
					{
						ImGui::OpenPopup("Unload All Assets?");
						UnloadAllAsset = false;
					}
					if (ImGui::Button("Load previous assets"))
					{
						BE::AssetMgr->Deserialise("../Assets/asset.json");
					}
					ImGui::SameLine();
					if (ImGui::Button("Overwrite previous assets"))
					{
						BE::AssetMgr->Serialise("../Assets/asset.json");
					}
					ImGui::SameLine();
					if (ImGui::Button("Regenerate .dds"))
					{
						BE::AssetMgr->scanDirectoryForConversion("../Assets", RegenerationAssetType::textures, false);
						BE::AssetMgr->setWaitingToRegenerateAssets(true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Regenerate .bananaModel"))
					{
						BE::AssetMgr->scanDirectoryForConversion("../Assets", RegenerationAssetType::models, false);
						BE::AssetMgr->setWaitingToRegenerateAssets(true);
					}
				}
				else
				{
					if (ImGui::Button(ICON_FA_TRASH " Unload Unused Assets"))
					{
						UnloadAllAsset = true;
					}
					ImGui::SameLine();

					if (UnloadAllAsset)
					{
						ImGui::OpenPopup("Unload All Assets?");
						UnloadAllAsset = false;
					}
					if (ImGui::Button("Load previous assets"))
					{
						BE::AssetMgr->Deserialise("../Assets/asset.json");
					}

					if (ImGui::Button("Overwrite previous assets"))
					{
						BE::AssetMgr->Serialise("../Assets/asset.json");
					}

					if (ImGui::Button("Regenerate .dds"))
					{
						BE::AssetMgr->scanDirectoryForConversion("..\\Assets", RegenerationAssetType::textures, false);
						BE::AssetMgr->setWaitingToRegenerateAssets(true);
					}

					if (ImGui::Button("Regenerate .bananaModel"))
					{
						BE::AssetMgr->scanDirectoryForConversion("..\\Assets", RegenerationAssetType::models, false);
						BE::AssetMgr->setWaitingToRegenerateAssets(true);
					}
				}

				ImVec2 Unloadcenter = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(Unloadcenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (ImGui::BeginPopupModal("Unload All Assets?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Are you sure you want to unload all unused assets?");
					ImGui::Separator();

					ImGui::Text("");

					if (ImGui::Button("Unload"))
					{
						BE::AssetMgr->UnloadUnusedAssets();

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

				ImGui::Text("");

				auto GetAllLoadedAssets = BE::AssetMgr->GetAllLoadedAssets();

				if (ImGui::BeginTabBar("##tab"))
				{
					if (ImGui::BeginTabItem("Audio"))
					{
						static ImGuiTextFilter filter;
						filter.Draw(ICON_FA_SEARCH);

						ImGui::Columns(columnCount, 0, false);

						for (auto const& x : GetAllLoadedAssets)
						{
							if (filter.PassFilter(x.second.c_str()))
							{
								//std::cout << x.first << ':' << x.second << std::endl;
								auto GetAssetTypeID = BE::AssetMgr->GetAssetTypeIDOf(x.first);

								if (GetAssetTypeID == BE::AssetMgr->GetAssetTypeID<BE::AudioClip>())
								{
									ImGui::Button(ICON_FA_FILE_AUDIO, { thumbnailSize, thumbnailSize });

									if (ImGui::BeginPopupContextItem(x.second.c_str()))
									{
										BE::AudioClip& AudioClipRef = BE::AssetMgr->GetAssetData<BE::AudioClip>(x.first);

										bool is3D = AudioClipRef.GetIs3D();
										if (ImGui::Checkbox("3D Sound", &is3D))
										{
											AudioClipRef.SetIs3D(is3D);
										}

										bool isLooping = AudioClipRef.GetIsLooping();
										if (ImGui::Checkbox("Loop", &isLooping))
										{
											AudioClipRef.SetIsLooping(isLooping);
										}

										if (ImGui::Button(ICON_FA_TRASH " Unload Asset"))
										{
											ImGui::OpenPopup("Remove Asset?");
										}

										ImVec2 center = ImGui::GetMainViewport()->GetCenter();
										ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

										if (ImGui::BeginPopupModal("Remove Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
										{
											RemoveAsset_Popup(x.first);
										}

										ImGui::EndPopup();
									}

									ImGui::OpenPopupOnItemClick(x.second.c_str(), ImGuiPopupFlags_MouseButtonRight);

									std::string TruncateFileName = BE::AssetMgr->GetAssetTruncatedFileName(x.first);
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text(TruncateFileName.c_str());
										ImGui::EndTooltip();
									}

									ImGui::TextWrapped(TruncateFileName.c_str());

									ImGui::NextColumn();
								}
							}
						}

						ImGui::Columns(1);

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Texture"))
					{
						static ImGuiTextFilter filter;
						filter.Draw(ICON_FA_SEARCH);

						ImGui::Columns(columnCount, 0, false);

						for (auto const& x : GetAllLoadedAssets)
						{
							if (filter.PassFilter(x.second.c_str()))
							{
								//std::cout << x.first << ':' << x.second << std::endl;
								auto GetAssetTypeID = BE::AssetMgr->GetAssetTypeIDOf(x.first);

								if (GetAssetTypeID == BE::AssetMgr->GetAssetTypeID<BE::Texture>())
								{
									//ImGui::ImageButton((ImTextureID)((void*)((intptr_t)it.second.GetTexId())), { thumbnailSize, thumbnailSize });
									ImGui::Button(ICON_FA_FILE_IMAGE, { thumbnailSize, thumbnailSize });

									if (ImGui::BeginPopupContextItem(x.second.c_str()))
									{
										if (ImGui::Button(ICON_FA_TRASH " Unload Asset"))
										{
											ImGui::OpenPopup("Remove Asset?");
										}

										ImVec2 center = ImGui::GetMainViewport()->GetCenter();
										ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

										if (ImGui::BeginPopupModal("Remove Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
										{
											RemoveAsset_Popup(x.first);
										}

										ImGui::EndPopup();
									}

									ImGui::OpenPopupOnItemClick(x.second.c_str(), ImGuiPopupFlags_MouseButtonRight);

									std::string TruncateFileName = BE::AssetMgr->GetAssetTruncatedFileName(x.first);
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text(TruncateFileName.c_str());
										ImGui::EndTooltip();
									}

									ImGui::TextWrapped(TruncateFileName.c_str());

									ImGui::NextColumn();
								}
							}
						}

						ImGui::Columns(1);

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Model"))
					{
						static ImGuiTextFilter filter;
						filter.Draw(ICON_FA_SEARCH);

						ImGui::Columns(columnCount, 0, false);

						for (auto const& x : GetAllLoadedAssets)
						{
							if (filter.PassFilter(x.second.c_str()))
							{
								//std::cout << x.first << ':' << x.second << std::endl;
								auto GetAssetTypeID = BE::AssetMgr->GetAssetTypeIDOf(x.first);

								if (GetAssetTypeID == BE::AssetMgr->GetAssetTypeID<BE::BeModel>())
								{
									//ImGui::ImageButton((ImTextureID)((void*)((intptr_t)it.second.GetTexId())), { thumbnailSize, thumbnailSize });
									ImGui::Button(ICON_FA_CUBE, { thumbnailSize, thumbnailSize });

									if (ImGui::BeginPopupContextItem(x.second.c_str()))
									{
										if (ImGui::Button(ICON_FA_TRASH " Unload Asset"))
										{
											ImGui::OpenPopup("Remove Asset?");
										}

										ImVec2 center = ImGui::GetMainViewport()->GetCenter();
										ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

										if (ImGui::BeginPopupModal("Remove Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
										{
											RemoveAsset_Popup(x.first);
										}

										ImGui::EndPopup();
									}

									ImGui::OpenPopupOnItemClick(x.second.c_str(), ImGuiPopupFlags_MouseButtonRight);

									std::string TruncateFileName = BE::AssetMgr->GetAssetTruncatedFileName(x.first);
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text(TruncateFileName.c_str());
										ImGui::EndTooltip();
									}

									ImGui::TextWrapped(TruncateFileName.c_str());

									ImGui::NextColumn();
								}
							}
						}

						ImGui::Columns(1);

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Prefab"))
					{
						static ImGuiTextFilter filter;
						filter.Draw(ICON_FA_SEARCH);

						ImGui::Columns(columnCount, 0, false);

						for (auto const& x : GetAllLoadedAssets)
						{
							if (filter.PassFilter(x.second.c_str()))
							{
								//std::cout << x.first << ':' << x.second << std::endl;
								auto GetAssetTypeID = BE::AssetMgr->GetAssetTypeIDOf(x.first);

								if (GetAssetTypeID == BE::AssetMgr->GetAssetTypeID<BE::PrefabAsset>())
								{
									//ImGui::ImageButton((ImTextureID)((void*)((intptr_t)it.second.GetTexId())), { thumbnailSize, thumbnailSize });
									ImGui::Button(ICON_FA_CUBE, { thumbnailSize, thumbnailSize });

									if (ImGui::BeginPopupContextItem(x.second.c_str()))
									{
										if (ImGui::Button(ICON_FA_TRASH " Unload Asset"))
										{
											ImGui::OpenPopup("Remove Asset?");
										}

										ImVec2 center = ImGui::GetMainViewport()->GetCenter();
										ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

										if (ImGui::BeginPopupModal("Remove Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
										{
											RemoveAsset_Popup(x.first);
										}

										if (ImGui::Button(ICON_FA_PLUS_SQUARE " Create Entity from Prefab"))
										{
											BE::ECS->SpawnPrefab(x.first);
										}

										ImGui::EndPopup();
									}

									ImGui::OpenPopupOnItemClick(x.second.c_str(), ImGuiPopupFlags_MouseButtonRight);

									std::string TruncateFileName = BE::AssetMgr->GetAssetTruncatedFileName(x.first);
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text(TruncateFileName.c_str());
										ImGui::EndTooltip();
									}

									ImGui::TextWrapped(TruncateFileName.c_str());

									ImGui::NextColumn();
								}
							}
						}

						ImGui::Columns(1);

						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Font"))
					{
						static ImGuiTextFilter filter;
						filter.Draw(ICON_FA_SEARCH);

						ImGui::Columns(columnCount, 0, false);

						for (auto const& x : GetAllLoadedAssets)
						{
							if (filter.PassFilter(x.second.c_str()))
							{
								auto GetAssetTypeID = BE::AssetMgr->GetAssetTypeIDOf(x.first);

								if (GetAssetTypeID == BE::AssetMgr->GetAssetTypeID<BE::Font>())
								{
									ImGui::Button(ICON_FA_FONT, { thumbnailSize, thumbnailSize });

									if (ImGui::BeginPopupContextItem(x.second.c_str()))
									{
										if (ImGui::Button(ICON_FA_TRASH " Unload Asset"))
										{
											ImGui::OpenPopup("Remove Asset?");
										}

										ImVec2 center = ImGui::GetMainViewport()->GetCenter();
										ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

										if (ImGui::BeginPopupModal("Remove Asset?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
										{
											RemoveAsset_Popup(x.first);
										}

										ImGui::EndPopup();
									}

									ImGui::OpenPopupOnItemClick(x.second.c_str(), ImGuiPopupFlags_MouseButtonRight);

									std::string TruncateFileName = BE::AssetMgr->GetAssetTruncatedFileName(x.first);
									if (ImGui::IsItemHovered())
									{
										ImGui::BeginTooltip();
										ImGui::Text(TruncateFileName.c_str());
										ImGui::EndTooltip();
									}

									ImGui::TextWrapped(TruncateFileName.c_str());

									ImGui::NextColumn();
								}
							}
						}

						ImGui::Columns(1);

						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}

				ImGui::Separator();
				ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 1, 100);
				ImGui::SliderFloat("Padding", &padding, 0, 32);

				end();
			}
		}
	}

	void Assets_Window::Shutdown()
	{
		delete p_asset;
	}

	void Assets_Window::RemoveAsset_Popup(BE::AssetID _assetID)
	{
		std::string file_name = BE::AssetMgr->GetAssetTruncatedFileName(_assetID);
		ImGui::Text("Are you sure you want to unload: %s?", file_name.c_str());
		ImGui::Separator();

		if (ImGui::Button("Remove"))
		{
			BE::AssetMgr->UnloadAsset(_assetID, true);

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