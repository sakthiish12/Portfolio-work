/******************************************************************************/
/*!
\file		Graphics_Window.cpp
\par        Project: Candy Crusade
\author		Pham Thao Mien, Liao RuiQi Ricky
\date   	October 7, 2021
\brief		Render graphics into ImGui

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include <Utilities/Utility.h>

#ifdef USE_EDITOR
#include "Graphics_Window.h"
#include "../Assets_Window/Assets_Window.h"
#include "../Hierarchy_Window/Hierarchy_Window.h"
#include "../Tool_Bar/Tool_Bar.h"
#include "../Menu_Window/Menu_Window.h"
#include "../EditorSystem/EditorSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "../UI_Hierarchy_Window/UI_Hierarchy_Window.h"
#pragma warning (push)
#pragma warning (disable:26812)
namespace BE
{
	Graphics_Window* Graphics_Window::p_graph = nullptr;

	BE::OffScreen* getOffscreen;

	bool Graphics_Window::picking = false;
	GizmoType Graphics_Window::gizmo_option = GizmoType::ENTITY_GIZMO;
	int Graphics_Window::selected_type = 0;
	int Graphics_Window::gizmoType = -1;

	Graphics_Window::Graphics_Window() : isFocused{ false }, isUsingGizmo{ false }
	{
		
	}

	Graphics_Window::~Graphics_Window()
	{
	}

	Graphics_Window* Graphics_Window::init()
	{
		p_graph = new Graphics_Window();
		return p_graph;
	}

	void Graphics_Window::run(BE::OffScreen* MOffScreen)
	{
		//auto mf = MOffScreen;

		getOffscreen = MOffScreen;
	}

	void Graphics_Window::update()
	{
		if (Menu_Window::Get()->showEditorGraphicsWindow)
		{
			if (!begin("Editor View", &Menu_Window::Get()->showEditorGraphicsWindow))
			{
				end();
			}
			else
			{
				windowPos = glm::vec2{ ImGui::GetWindowPos().x,ImGui::GetWindowPos().y };
				windowSize = glm::vec2{ ImGui::GetWindowSize().x,ImGui::GetWindowSize().y };

				if (Tool_Bar::Get()->GetSceneState() == SceneState::Play)
				{
					gizmoType = -1;
				}

				if (ImGui::IsWindowFocused())
				{
					isFocused = true;

					if (Input->IsMouseReleased(Mouse::MOUSE_LEFT) && !isUsingGizmo)
					{
						if ((EntityID)Renderersystem->getSelectedID() == 0)
						{
							gizmoType = -1;
						}

						Hierarchy_Window::Get()->id = (EntityID)Renderersystem->getSelectedID();
	
						auto& vec = Hierarchy_Window::Get()->Get_Selected();
						vec.clear();

						EntityID pickID = Hierarchy_Window::Get()->id;

						if (pickID != 0)
						{
							Hierarchy_Window::Get()->isusing = true;

							vec.push_back(pickID);
						}
					}
				}
				else
				{
					isFocused = false;
				}

				if (isFocused || Editorsystem->getHierarchyWindow()->isHierarchyFocused())
				{
					if (Tool_Bar::Get()->GetSceneState() == SceneState::Edit || Tool_Bar::Get()->GetSceneState() == SceneState::Pause)
					{
						if (BE::Input->IsKeyPressed(BE::Key::_Q))
						{
							gizmoType = -1;
						}

						if (BE::Input->IsKeyPressed(BE::Key::_E))
						{
							gizmoType = ImGuizmo::OPERATION::ROTATE;
						}

						if (BE::Input->IsKeyPressed(BE::Key::_W))
						{
							gizmoType = ImGuizmo::OPERATION::TRANSLATE;
						}

						if (BE::Input->IsKeyPressed(BE::Key::_R))
						{
							gizmoType = ImGuizmo::OPERATION::SCALE;
						}
					}
				}

				//BE::Debug::GetInstance()->SetEditorScreen(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
				BE::Debug::GetInstance()->SetEditorProperties(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y,
															  ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y,
															  ImGui::GetMousePos().x, ImGui::GetMousePos().y);

				image = ImGui_ImplVulkan_AddTexture(getOffscreen->getSampler(), getOffscreen->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				ImGui::Image(image, ImGui::GetContentRegionAvail());

				//Guizmo

				if (gizmo_option == GizmoType::ENTITY_GIZMO)
				{
					bool isusing = Hierarchy_Window::Get()->isusing;

					if (isusing)
					{
						BE::EntityID x = Hierarchy_Window::Get()->id;
						//if (BE::ECS->GetAllEntities().size() == 0)
						//{
						//	end();
						//	return;
						//}
						//if (BE::ECS->GetAllEntities().end() == std::find(BE::ECS->GetAllEntities().begin(), BE::ECS->GetAllEntities().end(), Hierarchy_Window::Get()->id))
						//{
						//	Hierarchy_Window::Get()->id = *BE::ECS->GetAllEntities().begin();
						//	x = Hierarchy_Window::Get()->id;
						//}
						//BE::EntityID x = Hierarchy_Window::Get()->id;

						if (x != 0 && gizmoType != -1)
						{
							ImGuizmo::SetOrthographic(false);
							ImGuizmo::SetDrawlist();

							float window_width = ImGui::GetWindowWidth();
							float window_height = ImGui::GetWindowHeight();
							ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);

							//Camera
							BE::cameraComponent& EditorCam = BE::Renderersystem->GameCamera.cc;

							const glm::mat4& camproj = EditorCam.getProjection();
							glm::mat4 camview = EditorCam.getView();

							auto& tc = BE::ECS->GetComponent<BE::Transform>(x);
							glm::mat4 transform = tc.getModelMatrix();

							glm::vec3 old_pos = tc.GetPosition().toGLM();
							glm::vec3 old_scale = tc.GetScale().toGLM();
							glm::quat old_rot = tc.GetRotation().toGLM();
							//glm::vec3 old_roteuler = tc.GetEulerRotation().toGLM();

							ImGuizmo::Manipulate(glm::value_ptr(camview), glm::value_ptr(camproj), ImGuizmo::OPERATION(gizmoType), ImGuizmo::LOCAL, glm::value_ptr(transform));
						
							if (ImGuizmo::IsUsing())
							{
								isUsingGizmo = true;

								glm::vec3 translation, scale;
								glm::quat rotation;

								//Not used
								glm::vec3 skew;
								glm::vec4 pers;

								glm::decompose(transform, scale, rotation, translation, skew, pers);
							
								//Translate
								if (gizmoType == ImGuizmo::OPERATION::TRANSLATE)
									tc.SetPosition(Vec3f(translation.x, translation.y, translation.z));

								//Rotate
								if (gizmoType == ImGuizmo::OPERATION::ROTATE)
									tc.SetRotation(Quatf(rotation.w, rotation.x, rotation.y, rotation.z));

								//Scale
								if (gizmoType == ImGuizmo::OPERATION::SCALE)
									tc.SetScale(Vec3f(scale.x, scale.y, scale.z));

								//check for multiple entities
								std::vector<BE::EntityID>& vec = Hierarchy_Window::Get_Selected();
								if (vec.size() > 1)
								{
									BE::Vec3f pos_offset = { old_pos.x - translation.x, old_pos.y - translation.y, old_pos.z - translation.z };
									BE::Vec3f scale_offset = { old_scale.x - scale.x, old_scale.y - scale.y, old_scale.z - scale.z };
									BE::Quatf rot_offset = { old_rot.w - rotation.w, old_rot.x - rotation.x, old_rot.y - rotation.y, old_rot.z - rotation.z };

									for (auto a = 0; a < vec.size(); ++a)
									{
										if (vec[a] != x)
										{
											auto& transEntity = BE::ECS->GetComponent<BE::Transform>(vec[a]);

											if (gizmoType == ImGuizmo::OPERATION::SCALE)
												transEntity.SetScale(transEntity.GetScale() - scale_offset);

											if (gizmoType == ImGuizmo::OPERATION::TRANSLATE)
												transEntity.SetPosition(transEntity.GetPosition() - pos_offset);

											if (gizmoType == ImGuizmo::OPERATION::ROTATE)
												transEntity.SetRotation(Quatf{ transEntity.GetRotation().toGLM().w + rot_offset.toGLM().w,
																			   transEntity.GetRotation().toGLM().x + rot_offset.toGLM().x,
																			   transEntity.GetRotation().toGLM().y + rot_offset.toGLM().y,
																			   transEntity.GetRotation().toGLM().z + rot_offset.toGLM().z, });
										}
									}
								}
							}
							else
							{
								isUsingGizmo = false;
							}
						}
					}
				}

				///////GLuint texture = BE::GFX->Editor_GetSceneTextureObject();//////////
				//BE::Input->SetEditorSceneWindowTopLeftPosition(ImGui::GetCursorScreenPos().x,
					//ImGui::GetCursorScreenPos().y);

				///////ImVec2 windowSize = ImGui::GetContentRegionAvail();
				///////ImVec2 pos = ImGui::GetCursorScreenPos();
				///////
				///////ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				///////ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
				///////
				///////ImGui::Image((void*)(uint64_t)texture,
				///////	ImVec2((float)windowSize.x, (float)windowSize.y),
				///////	ImVec2(0, 1),
				///////	ImVec2(1, 0), tint_col);

				//BE::GFX->SetEditorSceneWindowSize(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);

				//BE::Input->SetEditorMousePosition(ImGui::GetMousePos().x,
					//ImGui::GetMousePos().y, ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

				//Guizmo

				/*if (gizmo_option == GizmoType::ENTITY_GIZMO)
				{
					BE::EntityID x = Hierarchy_Window::Get()->id;

					if (BE::ECS->HasComponent<BE::Transform>(x) && !BE::ECS->GetFlag(x, BE::EntityFlags::flagDestroy) && BE::ECS->GetLivingEntityArray().size() != 0 && gizmoType != -1)
					{
						ImGuizmo::SetOrthographic(true);
						ImGuizmo::SetDrawlist();
						float window_width = ImGui::GetWindowWidth();
						float window_height = ImGui::GetWindowHeight();
						ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);
						//std::cout << "GetWindowPos" << ImGui::GetWindowPos().x << "H: " << ImGui::GetWindowPos().y;

						//Camera
						BE::Camera defaultCam;
						BE::Camera* editorCam = &defaultCam;

						if (SHE::Tool_Bar::Get()->GetSceneState() == SceneState::Pause)
						{
							editorCam = &BE::GFX->GetEditorCamera();
						}
						else
						{
							for (const BE::EntityID& id : BE::ECS->GetLivingEntityArray())
							{
								if (BE::ECS->HasComponent<BE::Camera>(id))
								{
									BE::Camera& graphicCam = BE::ECS->GetComponent<BE::Camera>(id);
									if (graphicCam.GetMainInGameCamera())
									{
										editorCam = &graphicCam;
										break;
									}
								}
							}
						}

						glm::mat4 camproj = editorCam->GetCamProjectionMatrix();
						glm::mat4 camview = editorCam->GetCamViewMatrix();

						glm::mat4 trans{};

						BE::Vector2D old_pos = BE::ECS->GetComponent<BE::Transform>(x).GetPos();
						BE::Vector2D old_scale = BE::ECS->GetComponent<BE::Transform>(x).GetScale();
						float old_rot = BE::ECS->GetComponent<BE::Transform>(x).GetRot();

						glm::vec3 Recompose_position = (BE::ECS->GetComponent<BE::Transform>(x).GetPos() * WORLD_UNIT).toGlmVec3();
						glm::vec3 Recompose_scale = BE::ECS->GetComponent<BE::Transform>(x).GetScale().toGlmVec3();
						glm::vec3 Recompose_rot = { 1, 1, BE::ECS->GetComponent<BE::Transform>(x).GetRot() };

						ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(Recompose_position),
							glm::value_ptr(Recompose_rot),
							glm::value_ptr(Recompose_scale),
							glm::value_ptr(trans));

						ImGuizmo::Manipulate(glm::value_ptr(camview), glm::value_ptr(camproj), ImGuizmo::OPERATION(gizmoType), ImGuizmo::LOCAL, glm::value_ptr(trans));

						if (ImGuizmo::IsUsing())
						{
							BE::GFX->SetUsingGizmo(true);

							glm::vec3 translation, rotation, scale;
							ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(trans), glm::value_ptr(translation),
								glm::value_ptr(rotation), glm::value_ptr(scale));

							//Translate
							BE::ECS->GetComponent<BE::Transform>(x).SetPos({ translation.x / WORLD_UNIT, translation.y / WORLD_UNIT });

							//Rotate
							if (rotation.z < 0)
							{
								rotation.z += 360;
							}
							BE::ECS->GetComponent<BE::Transform>(x).SetRot(rotation.z);
							//std::cout << "Rot: " << rotation.z << std::endl;

							//Scale
							BE::ECS->GetComponent<BE::Transform>(x).SetScale({ scale.x, scale.y });
							//std::cout << "ScaleX: " << scale.x << " ScaleY: " << scale.y << std::endl;

							//check for multiple entities
							std::vector<BE::EntityID>& vec = Hierarchy_Window::Get_Selected();
							if (vec.size() > 1)
							{
								BE::Vector2D pos_offset = { old_pos.x - (translation.x / WORLD_UNIT), old_pos.y - (translation.y / WORLD_UNIT) };
								BE::Vector2D scale_offset = { old_scale.x - scale.x, old_scale.y - scale.y };
								float rot_offset = old_rot - rotation.z;

								for (auto a = 0; a < vec.size(); ++a)
								{
									if (vec[a] != x)
									{
										if (gizmoType == ImGuizmo::OPERATION::SCALE)
											BE::ECS->GetComponent<BE::Transform>(vec[a]).SetScale(BE::ECS->GetComponent<BE::Transform>(vec[a]).GetScale() - scale_offset);

										if (gizmoType == ImGuizmo::OPERATION::TRANSLATE)
											BE::ECS->GetComponent<BE::Transform>(vec[a]).SetPos(BE::ECS->GetComponent<BE::Transform>(vec[a]).GetPos() - pos_offset);

										if(gizmoType == ImGuizmo::OPERATION::ROTATE)
											BE::ECS->GetComponent<BE::Transform>(vec[a]).SetRot(BE::ECS->GetComponent<BE::Transform>(vec[a]).GetRot() - rot_offset);
									}
								}
							}
						}
						else
						{
							BE::GFX->SetUsingGizmo(false);
						}
					}
				}*/

				//Drag and Drop
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload * payload{ ImGui::AcceptDragDropPayload("ADD PREFABS") })
					{
						//Factory::LoadFile(Assets_Window::GetPrefabsPath());
						//
						//Tool_Bar::Get()->SetSceneState(SceneState::Pause);
						//
						//BE::Debug::SetDebugFlag(BE::DebugFlags::imguiPause, true);
					}
					ImGui::EndDragDropTarget();
				}

				end();
			}
		}

		/*if (Menu_Window::Get()->showPlayerGraphicsWindow)
		{
			if (!begin("Player View", &Menu_Window::Get()->showPlayerGraphicsWindow))
			{
				end();
			}
			else
			{
				GLuint player_texture = BE::GFX->Editor_GetPlayerViewTextureObject();

				ImVec2 player_windowSize = ImGui::GetContentRegionAvail();

				ImVec4 player_tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint

				ImGui::Image((void*)(uint64_t)player_texture,
					ImVec2((float)player_windowSize.x, (float)player_windowSize.y),
					ImVec2(0, 1),
					ImVec2(1, 0), player_tint_col);

				BE::GFX->SetPlayerSceneWindowSize(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);
				end();
			}
		}*/
	}

	void Graphics_Window::Shutdown()
	{
		delete p_graph;
	}

	void Graphics_Window::destroyImage(VkDescriptorSet& set)
	{
		if (set != VK_NULL_HANDLE)
		{
			vkFreeDescriptorSets(EditorSystem::GetInstance()->getDevice()->device(), EditorSystem::GetInstance()->getDescriptorPool(), 1, &set);
			set = VK_NULL_HANDLE;
		}
	}

	VkDescriptorSet& Graphics_Window::getImage()
	{
		return image;
	}
	glm::vec2& Graphics_Window::getWindowPos()
	{
		return windowPos;
	}
	glm::vec2& Graphics_Window::getWindowSize()
	{
		return windowSize;
	}
}
#pragma warning (pop)
#endif