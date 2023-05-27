/******************************************************************************/
/*!
\file		Inspector_Window.cpp
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Inspector window for each game object

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "Inspector_Window.h"

#ifdef USE_EDITOR
#include "../Hierarchy_Window/Hierarchy_Window.h"
#include "../Menu_Window/Menu_Window.h"
//#include "../Resources_Window/Resources_Window.h"
//#include "../Graphics_Window/Graphics_Window.h"
#include "../EntityCommand/EntityCommand.h"
#include "../EditorSystem/EditorSystem.h"
#include <Components/ParticleSystem.h>

namespace BE
{
	Inspector_Window* Inspector_Window::p_insp = nullptr;
	std::string TextureName;
	std::string ModelName;
	std::string AudioName;
	std::string FontName{};

	Inspector_Window::Inspector_Window()
	{
	}

	Inspector_Window::~Inspector_Window()
	{
	}

	Inspector_Window* Inspector_Window::init()
	{
		p_insp = new Inspector_Window();
		return p_insp;
	}

	void Inspector_Window::update()
	{
		if (Menu_Window::Get()->showEInspectWindow)
		{
			if (!begin("Entity Inspector", &Menu_Window::Get()->showEInspectWindow))
			{
				end();
			}
			else
			{
				bool isusing = Hierarchy_Window::Get()->isusing;

				if (isusing)
				{
					//Very bad, fix in future
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
					//DisplayProperties(rttr::instance, rttr::type);
					if (ECS->DoesEntityExists(x))
					{
						std::string Ename = ICON_FA_CUBE " " + BE::ECS->GetComponent<BE::EntityInfo>(x).GetName();
						ImGui::Text(Ename.c_str());
						//std::shared_ptr<IScript> scriptPtr;
						//std::string nameOfScript;
						if (ImGui::TreeNode("RTTR Inspector (WIP)"))
						{
							auto map = BE::ECS->GetAllComponentTypes();

							for (const auto& component : map)
							{
								if (BE::ECS->HasComponent(component.second.c_str(), x))
								{
									if (ImGui::TreeNode(component.second.c_str()))
									{
										rttr::instance inst = BE::ECS->GetComponent(component.second.c_str(), x);

										//static bool showLocalTransform = false;

										////If this is transform
										//if (strcmp(component.second.c_str(), "Transform") == 0)
										//{
										//	ImGui::Checkbox("Show Local Transform", &showLocalTransform);
										//}

										const rttr::type& componentType = rttr::type::get_by_name(component.second.c_str());

										DisplayProperties(inst, componentType);

										ImGui::TreePop();
									}

									ImGui::Separator();
								}
							}
							ImGui::TreePop();
						}

						//ImGui::Text("");
						//ImGui::Separator();
						ImGui::Text("");

						//if (ImGui::TreeNode("Normal Inspector"))
						//{
							const std::string& get_name = BE::ECS->GetComponent<BE::EntityInfo>(x).GetName();
							const char* cstr = get_name.data();
							char* name = (char*)(cstr);

							//std::string s_name = ICON_FA_CUBE " " + get_name;
							//ImGui::Text(s_name.c_str());

							if (ImGui::InputText("Entity Name", name, 50))
							{
								BE::ECS->GetComponent<BE::EntityInfo>(x).SetName(name);
							}

							ImGui::Text("");
							//ImGui::Separator();

							if (BE::ECS->HasComponent<BE::EntityInfo>(x))
							{
								if (ImGui::TreeNode("Entity Info Component"))
								{
									BE::EntityInfo& EntityInfoRef = BE::ECS->GetComponent<BE::EntityInfo>(x);

									//auto getTags = BE::ECS->GetComponent<BE::EntityInfo>(x).GetAllTags();
									ImGui::Text("List of Tags");
									const auto& getAllTags = EntityInfoRef.GetAllTags();

									if (ImGui::BeginListBox("Tags"))
									{
										static int selected;
										int i = 0;

										for (const auto& e : getAllTags)
										{
											std::string tagName = BE::ECS->GetTag(e);
											if (ImGui::Selectable(tagName.c_str(), selected == i))
											{
												selected = i;
											}

											if (ImGui::BeginPopupContextItem(tagName.c_str()))
											{
												if (ImGui::Button("Remove Tag"))
												{
													EntityInfoRef.RemoveTag(tagName);
												}

												ImGui::EndPopup();
											}
											++i;
										}

										ImGui::EndListBox();
									}

									ImGui::Text("");
									if (ImGui::TreeNode("Add Tag to the Entity"))
									{
										const auto& AllTags = BE::ECS->GetAllTags();
										static std::string add_tag;

										if (ImGui::BeginListBox("##Tags"))
										{
											static int add_selected;
											int b = 0;

											for (const auto& a : AllTags)
											{
												if (ImGui::Selectable(a.first.c_str(), add_selected == b))
												{
													add_selected = b;
													add_tag = a.first;
												}
												++b;
											}

											ImGui::EndListBox();
										}

										if (ImGui::Button("Add"))
										{
											EntityInfoRef.AddTag(add_tag);
										}

										ImGui::TreePop();
									}

									ImGui::Text("");

									bool flagActive = EntityInfoRef.HasFlag(BE::EntityFlag::active);
									if (ImGui::Checkbox("Entity Active?", &flagActive))
									{
										EntityInfoRef.SetFlag(BE::EntityFlag::active, flagActive);
									}

									bool flagUpdateWhenPaused = EntityInfoRef.HasFlag(BE::EntityFlag::updateWhenPaused);
									if (ImGui::Checkbox("Update When Paused?", &flagUpdateWhenPaused))
									{
										EntityInfoRef.SetFlag(BE::EntityFlag::updateWhenPaused, flagUpdateWhenPaused);
									}

									bool flagUpdateWhenNotVisible = EntityInfoRef.HasFlag(BE::EntityFlag::updateWhenNotVisible);
									if (ImGui::Checkbox("Update When Not Visible?", &flagUpdateWhenNotVisible))
									{
										EntityInfoRef.SetFlag(BE::EntityFlag::updateWhenNotVisible, flagUpdateWhenNotVisible);
									}

									bool dontDestroyOnSceneChange = BE::ECS->GetComponent<BE::EntityInfo>(x).HasFlag(BE::EntityFlag::dontDestroyOnSceneChange);
									if (ImGui::Checkbox("Dont Destroy On Scene Change?", &dontDestroyOnSceneChange))
									{
										EntityInfoRef.SetFlag(BE::EntityFlag::dontDestroyOnSceneChange, dontDestroyOnSceneChange);
									}

									ImGui::TreePop();
								}
							}

							ImGui::Separator();

							if (BE::ECS->HasComponent<BE::Transform>(x))
							{
								if (ImGui::TreeNode("Transform Component"))
								{
									BE::Transform& TransformRef = BE::ECS->GetComponent<BE::Transform>(x);

									static bool showLocalTransform = false;
									ImGui::Checkbox("Show Local Transform", &showLocalTransform);

									if (showLocalTransform)
									{
										float Loc_pos[] = { TransformRef.GetLocalPosition().x,
														TransformRef.GetLocalPosition().y,
														TransformRef.GetLocalPosition().z };
										if (ImGui::DragFloat3("Local Position", Loc_pos))
										{
											TransformRef.SetLocalPosition(BE::Vec3f{ Loc_pos[0], Loc_pos[1], Loc_pos[2] });
										}

										float Loc_scale[] = { TransformRef.GetLocalScale().x,
														  TransformRef.GetLocalScale().y,
														  TransformRef.GetLocalScale().z };
										if (ImGui::DragFloat3("Local Scale", Loc_scale))
										{
											TransformRef.SetLocalScale(BE::Vec3f{ Loc_scale[0], Loc_scale[1], Loc_scale[2] });
										}

										float Loc_rot[] = { TransformRef.GetLocalEulerRotation().x, //pitch
														TransformRef.GetLocalEulerRotation().y, //yaw
														TransformRef.GetLocalEulerRotation().z }; //roll
										if (ImGui::DragFloat3("Local Rotation", Loc_rot))
										{
											TransformRef.SetLocalEulerRotation(BE::Vec3f(Loc_rot[0], Loc_rot[1], Loc_rot[2]));
										}
									}
									else
									{
										float pos[] = { TransformRef.GetPosition().x,
														TransformRef.GetPosition().y,
														TransformRef.GetPosition().z };
										// For UNDO
										Vec3f oldPos = { pos[0], pos[1], pos[2] };

										if (ImGui::InputFloat3("Position", pos, "%0.1f", ImGuiInputTextFlags_EnterReturnsTrue))
										{
											std::shared_ptr<TransformPosCommand> p = std::make_shared<TransformPosCommand>();
											p->setVal("Pos", oldPos, Vec3f{pos[0], pos[1], pos[2]}, x);
											EditorSystem::GetInstance()->command->executeCmd(p);
										}

										/*if (ImGui::DragFloat3("Position", pos))
										{
											TransformRef.SetPosition(BE::Vec3f{ pos[0], pos[1], pos[2] });
										}*/

										float scale[] = { TransformRef.GetScale().x,
														  TransformRef.GetScale().y,
														  TransformRef.GetScale().z };
										Vec3f oldScale = { scale[0], scale[1], scale[2] };

										if (ImGui::InputFloat3("Scale", scale, "%0.1f", ImGuiInputTextFlags_EnterReturnsTrue))
										{
											std::shared_ptr<TransformPosCommand> p = std::make_shared<TransformPosCommand>();
											p->setVal("Scale", oldScale, Vec3f{ scale[0], scale[1], scale[2] }, x);
											EditorSystem::GetInstance()->command->executeCmd(p);			
										}

										/*if (ImGui::DragFloat3("Scale", scale))
										{
											TransformRef.SetScale(BE::Vec3f{ scale[0], scale[1], scale[2] });
										}*/

										float rot[] = { TransformRef.GetEulerRotation().x, //pitch
														TransformRef.GetEulerRotation().y, //yaw
														TransformRef.GetEulerRotation().z }; //roll
										Vec3f oldRot = { rot[0], rot[1], rot[2] };

										if (ImGui::InputFloat3("Rotation", rot, "%0.1f", ImGuiInputTextFlags_EnterReturnsTrue))
										{
											std::shared_ptr<TransformPosCommand> p = std::make_shared<TransformPosCommand>();
											p->setVal("Rot", oldRot, Vec3f{ rot[0], rot[1], rot[2] }, x);
											EditorSystem::GetInstance()->command->executeCmd(p);
										}
										/*if (ImGui::DragFloat3("Rotation", rot))
										{
											TransformRef.SetEulerRotation(BE::Vec3f(rot[0], rot[1], rot[2]));
										}*/
									}

									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::AudioSource>(x))
							{
								if (ImGui::TreeNode("Audio Source Component"))
								{
									BE::AudioSource& AudioSourceRef = BE::ECS->GetComponent<BE::AudioSource>(x);

									ImGui::Text("");

									const auto& Asset = BE::AssetMgr->GetAllAssetIDsOfType<BE::AudioClip>();

									static ImGuiTextFilter filter;
									filter.Draw(ICON_FA_SEARCH);

									static int add_audio_selected;
									std::vector<std::string> AllAudioFiles_selectable;
									std::vector<std::string> FullPath;

									for (auto& it : Asset)
									{
										AllAudioFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
										FullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
									}

									if (ImGui::BeginListBox("Audio"))
									{
										for (int i = 0; i < (int)AllAudioFiles_selectable.size(); ++i)
										{
											if (filter.PassFilter(AllAudioFiles_selectable[i].c_str()))
											{
												if (ImGui::Selectable(AllAudioFiles_selectable[i].c_str(), add_audio_selected == i))
												{
													add_audio_selected = i;

													AudioName = FullPath[i];
												}
											}
										}

										ImGui::EndListBox();
									}

									if (ImGui::Button("Add Audio"))
									{
										AudioSourceRef.AddAudio(AudioName);
									}

									if (AudioSourceRef.HasAudioLinked())
									{
										for (const auto& audio : AudioSourceRef.GetAllAudio())
										{
											auto AudioID = audio.first;
											auto audioName = "Audio: " + BE::AssetMgr->GetAssetTruncatedFileName(AudioID);

											if (ImGui::TreeNode(audioName.c_str()))
											{
												if (ImGui::Button(ICON_FA_PLAY))
												{
													AudioSourceRef.Play(AudioID);
												}
												if (ImGui::IsItemHovered())
													ImGui::SetTooltip("Play Audio");

												ImGui::SameLine();
												if (ImGui::Button(ICON_FA_PAUSE))
												{
													AudioSourceRef.Stop(AudioID);
												}
												if (ImGui::IsItemHovered())
													ImGui::SetTooltip("Pause Audio");

												float volume = AudioSourceRef.GetVolume(AudioID);
												if (ImGui::DragFloat("Volume", &volume, 0.1f, MIN_AUDIO_VOLUME, MAX_AUDIO_VOLUME))
												{
													AudioSourceRef.SetVolume(volume, AudioID);
												}

												float pitch = AudioSourceRef.GetPitch(AudioID);
												if (ImGui::DragFloat("Pitch", &pitch, 0.1f, MIN_AUDIO_PITCH, MAX_AUDIO_PITCH))
												{
													AudioSourceRef.SetPitch(pitch, AudioID);
												}

												ImGui::Text("Highest Priority: %d", MAX_AUDIO_PRIORITY);
												ImGui::Text("Lowest Priority: %d", MIN_AUDIO_PRIORITY);
												int priority = AudioSourceRef.GetPriority(AudioID);
												if (ImGui::DragInt("Priority", &priority, 1, MAX_AUDIO_PRIORITY, MIN_AUDIO_PRIORITY))
												{
													AudioSourceRef.SetPriority(priority, AudioID);
												}

												bool isBGM = AudioSourceRef.GetIsBGM(AudioID);
												if (ImGui::Checkbox("BGM", &isBGM))
												{
													AudioSourceRef.SetIsBGM(isBGM, AudioID);
												}

												bool isMute = AudioSourceRef.GetIsMuted(AudioID);
												if (ImGui::Checkbox("Mute", &isMute))
												{
													AudioSourceRef.SetIsMuted(isMute, AudioID);
												}

												if (BE::AssetMgr->GetAssetData<BE::AudioClip>(AudioID).GetIs3D())
												{
													float minDist = AudioSourceRef.GetMinDist(AudioID);
													if (ImGui::DragFloat("Min Distant", &minDist, 0.1f, 0.0f, 10000.0f))
													{
														AudioSourceRef.SetMinDist(minDist, AudioID);
													}

													float maxDist = AudioSourceRef.GetMaxDist(AudioID);
													if (ImGui::DragFloat("Max Distant", &maxDist, 0.1f, 0.0f, 10000.0f))
													{
														AudioSourceRef.SetMaxDist(maxDist, AudioID);
													}
												}

												ImGui::TreePop();
											}

											if (ImGui::BeginPopupContextItem(audioName.c_str()))
											{
												if (ImGui::Button(ICON_FA_TRASH " Remove Audio"))
												{
													AudioSourceRef.RemoveAudio(AudioID);
													ImGui::EndPopup();
													break;
												}

												ImGui::EndPopup();
											}

											ImGui::OpenPopupOnItemClick(audioName.c_str(), ImGuiPopupFlags_MouseButtonRight);
										}
									}

									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::AudioListener>(x))
							{
								if (ImGui::TreeNode("Audio Listener Component"))
								{
									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::Physics>(x))
							{
								BE::Physics& physicsRef = BE::ECS->GetComponent<BE::Physics>(x);

								if (ImGui::TreeNode("Physics Component"))
								{
									BE::Shape type = physicsRef.GetShapeType();
									int selected;

									if (type == BE::Shape::Sphere)
									{
										selected = 0;
									}
									if (type == BE::Shape::Box)
									{
										selected = 1;
									}
									if (type == BE::Shape::Capsule)
									{
										selected = 2;
									}
									if (type == BE::Shape::Cylinder)
									{
										selected = 3;
									}
									if (type == BE::Shape::StaticCompound)
									{
										selected = 4;
									}
									if (type == BE::Shape::Mesh)
									{
										selected = 5;
									}
									const char* items[] = { "Sphere", "Box", "Capsule", "Cylinder", "StaticCompound", "Mesh"};

									ImGui::Text("Shape Type");

									ImGui::Combo("##combo", &selected, items, IM_ARRAYSIZE(items));

									if (selected == 0)
									{
										physicsRef.SetShape(BE::Shape::Sphere);
									}
									if (selected == 1)
									{
										physicsRef.SetShape(BE::Shape::Box);
									}
									if (selected == 2)
									{
										physicsRef.SetShape(BE::Shape::Capsule);
									}
									if (selected == 3)
									{
										physicsRef.SetShape(BE::Shape::Cylinder);
									}
									if (selected == 4)
									{
										physicsRef.SetShape(BE::Shape::StaticCompound);
									}
									if (selected == 5)
									{
										physicsRef.SetShape(BE::Shape::Mesh);
									}

									if (physicsRef.GetShapeType() == BE::Shape::StaticCompound)
									{
										auto CompoundSubShapes = physicsRef.GetCompoundSubShapes();

										if (CompoundSubShapes.size() > 0)
										{
											for (BE::CompoundSubShape& a : CompoundSubShapes)
											{
												std::string subkey = "Sub Shape: " + std::to_string((int)a.subKey);
												if (ImGui::TreeNode(subkey.c_str()))
												{
													BE::Shape SubKeyType = a.GetShapeType();
													int selectedSubShape;

													if (SubKeyType == BE::Shape::Sphere)
													{
														selectedSubShape = 0;
													}
													if (SubKeyType == BE::Shape::Box)
													{
														selectedSubShape = 1;
													}
													if (SubKeyType == BE::Shape::Capsule)
													{
														selectedSubShape = 2;
													}
													if (SubKeyType == BE::Shape::Cylinder)
													{
														selectedSubShape = 3;
													}
													if (SubKeyType == BE::Shape::StaticCompound)
													{
														selectedSubShape = 4;
													}

													const char* SubKeyItems[] = { "Sphere", "Box", "Capsule", "Cylinder"/*, "StaticCompound"*/ };

													ImGui::Text("Shape Type");

													ImGui::Combo("##comboSubKey", &selectedSubShape, SubKeyItems, IM_ARRAYSIZE(SubKeyItems));

													if (selectedSubShape == 0)
													{
														physicsRef.SetSubShapeShapeType(a.subKey, BE::Shape::Sphere);
														//a.SetShapeType(BE::Shape::Sphere);
													}
													if (selectedSubShape == 1)
													{
														physicsRef.SetSubShapeShapeType(a.subKey, BE::Shape::Box);
														//a.SetShapeType(BE::Shape::Box);
													}
													if (selectedSubShape == 2)
													{
														physicsRef.SetSubShapeShapeType(a.subKey, BE::Shape::Capsule);
														//a.SetShapeType(BE::Shape::Capsule);
													}
													if (selectedSubShape == 3)
													{
														physicsRef.SetSubShapeShapeType(a.subKey, BE::Shape::Cylinder);
														//a.SetShapeType(BE::Shape::Cylinder);
													}
													//if (selectedSubShape == 4 && a.GetShapeType() != BE::Shape::StaticCompound)
													//{
													//	physicsRef.SetSubShapeShapeType(a.subKey, BE::Shape::StaticCompound);
													//	
													//}

													float SubPos[] = { a.GetPosition().x,
																	a.GetPosition().y,
																	a.GetPosition().z };
													if (ImGui::DragFloat3("SubShape Position", SubPos))
													{
														physicsRef.SetSubShapeLocalPosition(a.subKey, BE::Vec3f{ SubPos[0], SubPos[1], SubPos[2] });
														//a.SetPosition(BE::Vec3f{ SubPos[0], SubPos[1], SubPos[2] });
													}

													float SubScale[] = { a.GetScale().x,
																	  a.GetScale().y,
																	  a.GetScale().z };
													if (ImGui::DragFloat3("SubShape Scale", SubScale))
													{
														physicsRef.SetSubShapeScale(a.subKey, BE::Vec3f{ SubScale[0], SubScale[1], SubScale[2] });
													}

													float SubRot[] = { a.GetRotation().GetEulerAngles().x, //pitch
																	a.GetRotation().GetEulerAngles().y, //yaw
																	a.GetRotation().GetEulerAngles().z }; //roll
													if (ImGui::DragFloat3("SubShape Rotation", SubRot))
													{
														physicsRef.SetSubShapeRotation(a.subKey, BE::Vec3f(SubRot[0], SubRot[1], SubRot[2]));
														//a.SetRotation(BE::Quaternion(SubRot[0], SubRot[1], SubRot[2]));
													}

													ImGui::TreePop();
												}
											}
										}

										if (ImGui::Button("Add Sub Shape"))
										{
											//BE::CompoundSubShape temp{};
											physicsRef.AddSubShape(BE::CompoundSubShape{});
										}

										ImGui::Text("");
									}

									float ShapeSize[] = { physicsRef.GetShapeSize().x,
														  physicsRef.GetShapeSize().y,
														  physicsRef.GetShapeSize().z };
									if (ImGui::DragFloat3("Shape Size", ShapeSize))
									{
										physicsRef.SetShapeSize(BE::Vec3f{ ShapeSize[0], ShapeSize[1], ShapeSize[2] });
									}

									JPH::EMotionType Motiontype = physicsRef.GetMotionType();
									int selectedMotion;

									if (Motiontype == JPH::EMotionType::Static)
									{
										selectedMotion = 0;
									}
									if (Motiontype == JPH::EMotionType::Kinematic)
									{
										selectedMotion = 1;
									}
									if (Motiontype == JPH::EMotionType::Dynamic)
									{
										selectedMotion = 2;
									}

									const char* MotionItems[] = { "Static", "Kinematic", "Dynamic" };

									ImGui::Text("Motion Type");

									ImGui::Combo("##combo2", &selectedMotion, MotionItems, IM_ARRAYSIZE(MotionItems));

									if (selectedMotion == 0)
									{
										physicsRef.SetMotionType(JPH::EMotionType::Static);
									}
									if (selectedMotion == 1)
									{
										physicsRef.SetMotionType(JPH::EMotionType::Kinematic);
									}
									if (selectedMotion == 2)
									{
										physicsRef.SetMotionType(JPH::EMotionType::Dynamic);
									}

									JPH::EMotionQuality motionQuality = physicsRef.GetMotionQuality();
									int selectedMotionQuality;

									if (motionQuality == JPH::EMotionQuality::Discrete)
									{
										selectedMotionQuality = 0;
									}
									if (motionQuality == JPH::EMotionQuality::LinearCast)
									{
										selectedMotionQuality = 1;
									}

									const char* MotionQualityItems[] = { "Discrete", "LinearCast" };

									ImGui::Text("Motion Quality");

									ImGui::Combo("##combo3", &selectedMotionQuality, MotionQualityItems, IM_ARRAYSIZE(MotionQualityItems));

									if (selectedMotionQuality == 0)
									{
										physicsRef.SetMotionQuality(JPH::EMotionQuality::Discrete);
									}
									if (selectedMotionQuality == 1)
									{
										physicsRef.SetMotionQuality(JPH::EMotionQuality::LinearCast);
									}

									float Angvel[] = { physicsRef.GetAngularVelocity().x,
													   physicsRef.GetAngularVelocity().y,
													   physicsRef.GetAngularVelocity().z };
									if (ImGui::DragFloat3("Angular Velocity", Angvel))
									{
										physicsRef.SetAngularVelocity(BE::Vec3f{ Angvel[0], Angvel[1], Angvel[2] });
									}

									float maxAngVel = physicsRef.GetMaxAngularVelocity();
									if (ImGui::DragFloat("Max Angular Velocity", &maxAngVel))
									{
										physicsRef.SetMaxAngularVelocity(maxAngVel);
									}

									float AngDamping = physicsRef.GetAngularDamping();
									if (ImGui::DragFloat("Angular Damping", &AngDamping))
									{
										physicsRef.SetAngularDamping(AngDamping);
									}

									float Linvel[] = { physicsRef.GetLinearVelocity().x,
													   physicsRef.GetLinearVelocity().y,
													   physicsRef.GetLinearVelocity().z };
									if (ImGui::DragFloat3("Linear Velocity", Linvel))
									{
										physicsRef.SetLinearVelocity(BE::Vec3f{ Linvel[0], Linvel[1], Linvel[2] });
									}

									float maxLinVel = physicsRef.GetMaxLinearVelocity();
									if (ImGui::DragFloat("Max Linear Velocity", &maxLinVel))
									{
										physicsRef.SetMaxLinearVelocity(maxLinVel);
									}

									float LinDamping = physicsRef.GetLinearDamping();
									if (ImGui::DragFloat("Linear Damping", &LinDamping))
									{
										physicsRef.SetLinearDamping(LinDamping);
									}

									float friction = physicsRef.GetFriction();
									if (ImGui::DragFloat("Friction", &friction))
									{
										physicsRef.SetFriction(friction);
									}

									float restitution = physicsRef.GetRestitution();
									if (ImGui::DragFloat("Restitution", &restitution))
									{
										physicsRef.SetRestitution(restitution);
									}

									float GravityFactor = physicsRef.GetGravityFactor();
									if (ImGui::DragFloat("Gravity", &GravityFactor))
									{
										physicsRef.SetGravityFactor(GravityFactor);

									}
									float offset[] = { physicsRef.GetOffset().x,
													   physicsRef.GetOffset().y,
													   physicsRef.GetOffset().z };
									if (ImGui::DragFloat3("Offset", offset))
									{
										physicsRef.SetOffset(BE::Vec3f{ offset[0], offset[1], offset[2] });
									}

									bool trigger = physicsRef.GetIsTrigger();
									if (ImGui::Checkbox("Is Trigger", &trigger))
									{
										physicsRef.SetIsTrigger(trigger);
									}
									bool scaleBind = physicsRef.GetIsScaleBinded();
									if (ImGui::Checkbox("Scale Binded", &scaleBind))
									{
										physicsRef.SetScaleBinded(scaleBind);
									}
									bool canPush = physicsRef.GetCanPushCharacter();
									if (ImGui::Checkbox("Can Push Character", &canPush))
									{
										physicsRef.SetCanPushCharacter(canPush);
									}
									bool canbePush = physicsRef.GetCanReceiveCharacterImpulse();
									if (ImGui::Checkbox("Can Be Pushed By Character", &canbePush))
									{
										physicsRef.SetCanReceiveCharacterImpulse(canbePush);
									}
									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::Renderer>(x))
							{
								if (ImGui::TreeNode("Renderer Component"))
								{
									BE::Renderer& RendererRef = BE::ECS->GetComponent<BE::Renderer>(x);
									ImGui::Text("Model Name:");

									if (BE::AssetMgr->IsAssetDataLoaded(RendererRef.getModelID()))
									{
										ImGui::Text(RendererRef.getModelname().c_str());
									}

									ImGui::Text("");
									
									ImGui::Checkbox("Wireframe Mode:", &RendererRef.getWireframeModeRef());

									glm::vec4 wireframeColor = RendererRef.getWireframeColor();
									ImGui::DragFloat3("Wireframe Color:", &wireframeColor[0], 0.05f,0.0f, 1.0f);
									RendererRef.setWireframeColor(wireframeColor);

									float opacityToUse = RendererRef.getOpacity();
									ImGui::SliderFloat("Opacity:", &opacityToUse, 0.0f, 1.0f);
									RendererRef.setOpacity(opacityToUse);

									bool shouldFlicker = RendererRef.getShouldFlicker();
									ImGui::Checkbox("flicker (For testing speed)", &shouldFlicker);

									if (shouldFlicker)
									{
										RendererRef.flicker();
										shouldFlicker = false;
									}

									float flickerDuration = RendererRef.getFlickerDuration();
									ImGui::SliderFloat("Flicker Duration:", &flickerDuration, 0.0f, 3.0f);
									RendererRef.setFlickerDuration(flickerDuration);


									float flickerColor[4]{ RendererRef.getFlickerColor().x, RendererRef.getFlickerColor().y, RendererRef.getFlickerColor().z, RendererRef.getFlickerColor().w };
									ImGui::SliderFloat4("Flicker Color:", &flickerColor[0], 0.0f, 1.0f);
									glm::vec4 tempColor{ flickerColor[0],flickerColor[1],flickerColor[2],flickerColor[3] };
									RendererRef.setFlickerColor(tempColor);

									bool ignoreLighting = RendererRef.getIgnoreLighting();
									ImGui::Checkbox("Ignore Lighting", &ignoreLighting);
									RendererRef.setIgnoreLighting(ignoreLighting);


									bool reflectSpecular = RendererRef.getReflectSpecular();
									ImGui::Checkbox("Reflect Specular", &reflectSpecular);
									RendererRef.setReflectSpecular(reflectSpecular);

									const auto& ModelAsset = BE::AssetMgr->GetAllAssetIDsOfType<BE::BeModel>();

									static ImGuiTextFilter ModelFilter;
									ModelFilter.Draw(ICON_FA_SEARCH);

									static int add_model_selected;
									std::vector<std::string> AllModelFiles_selectable;
									std::vector<std::string> ModelFullPath;


									for (auto& it : ModelAsset)
									{
										AllModelFiles_selectable.push_back(BE::AssetMgr->GetAssetTruncatedFileName(it.first));
										ModelFullPath.push_back(BE::AssetMgr->GetAssetFileName(it.first));
									}

									if (ImGui::BeginListBox("Model"))
									{
										for (int i = 0; i < (int)AllModelFiles_selectable.size(); ++i)
										{
											if (ModelFilter.PassFilter(AllModelFiles_selectable[i].c_str()))
											{
												if (ImGui::Selectable(AllModelFiles_selectable[i].c_str(), add_model_selected == i && ModelName.size() != 0))
												{
													add_model_selected = i;

													ModelName = ModelFullPath[i];
												}
											}
										}

										ImGui::EndListBox();
									}
									if (ImGui::Button("setModelID") && ModelName.size() != 0)
									{
										RendererRef.setModelIDwithName(ModelName);
									}

									if (ImGui::Button("set Default Textures for model"))
									{
										RendererRef.resetToDefaultTextures();
									}

									ImGui::Text("");

									ImGui::Text("Mesh Info:");
									auto meshInfoData = RendererRef.getMeshInfo();
									
									for (size_t i_meshInfo = 0; i_meshInfo < meshInfoData.size(); ++i_meshInfo)
									{
										std::string mesh = "Mesh " + std::to_string(i_meshInfo) + ": " + meshInfoData[i_meshInfo].meshName;
										if (ImGui::TreeNode(mesh.c_str()))
										{
											float brightnessToUse = RendererRef.getBrightnessFactor(static_cast<int>(i_meshInfo));
											ImGui::SliderFloat("Brightness Factor:", &brightnessToUse, 0.0f, 10.0f);
											RendererRef.setBrightnessFactor(brightnessToUse, static_cast<int>(i_meshInfo));

											auto getTexture = RendererRef.getTextureSetName((int)i_meshInfo);

											for (size_t TextureSize = 0; TextureSize < getTexture.size(); ++TextureSize)
											{
												if (TextureSize == 0)
												{
													ImGui::Text("\talbedoAlphaMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[0].c_str());
												}
												else if (TextureSize == 1)
												{
													ImGui::Text("\tnormalMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[1].c_str());
												}
												else if (TextureSize == 2)
												{
													ImGui::Text("\tambientOcclusionMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[2].c_str());
												}
												else if (TextureSize == 3)
												{
													ImGui::Text("\troughnessMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[3].c_str());
												}
												else if (TextureSize == 4)
												{
													ImGui::Text("\temissiveMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[4].c_str());
												}
												else if (TextureSize == 5)
												{
													ImGui::Text("\tmetallicMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[5].c_str());
												}
												else if (TextureSize == 6)
												{
													ImGui::Text("\theightMap: ");
													ImGui::SameLine();
													ImGui::Text(getTexture[6].c_str());
												}
											}

											ImGui::Text("");

											auto getTextureStrength = RendererRef.getTextureStrength((int)i_meshInfo);
											for (size_t TextureIndex = 0; TextureIndex < getTextureStrength.size(); ++TextureIndex)
											{
												float TextureStrength = getTextureStrength[TextureIndex];

												std::string strength;

												if (TextureIndex == 0)
												{
													strength = "albedoAlphaMap Strength";
												}
												else if (TextureIndex == 1)
												{
													strength = "normalMap Strength";
												}
												else if (TextureIndex == 2)
												{
													strength = "ambientOcclusionMap Strength";
												}
												else if (TextureIndex == 3)
												{
													strength = "roughnessMap Strength";
												}
												else if (TextureIndex == 4)
												{
													strength = "emissiveMap Strength";
												}
												else if (TextureIndex == 5)
												{
													strength = "metallicMap Strength";
												}

												if (ImGui::DragFloat(strength.c_str(), &TextureStrength, 0.01f, 0.0f, 1.0f))
												{
													RendererRef.setTextureStrength(TextureStrength, (int)i_meshInfo, (int)TextureIndex);
												}
											}

											ImGui::Text("");

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

															TextureName = FullPath[i];
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

											if (ImGui::Button("setAlbedoAlphaMap"))
											{
												RendererRef.setAlbedoAlphaMapwithName(TextureName, (int)i_meshInfo);
											}
											ImGui::SameLine();
											if (ImGui::Button("setAmbientOcclusionMap"))
											{
												RendererRef.setAmbientOcclusionMapwithName(TextureName, (int)i_meshInfo);
											}
											ImGui::SameLine();
											if (ImGui::Button("setRoughnessMap"))
											{
												RendererRef.setRoughnessMapwithName(TextureName, (int)i_meshInfo);
											}
											if (ImGui::Button("setNormalMap"))
											{
												RendererRef.setNormalMapwithName(TextureName, (int)i_meshInfo);
											}
											ImGui::SameLine();
											if (ImGui::Button("setEmissiveMap"))
											{
												RendererRef.setEmissiveMapwithName(TextureName, (int)i_meshInfo);
											}
											ImGui::SameLine();
											if (ImGui::Button("setMetallicMap"))
											{
												RendererRef.setMetallicMapwithName(TextureName, (int)i_meshInfo);
											}
											ImGui::SameLine();
											if (ImGui::Button("setHeightMap"))
											{
												RendererRef.setHeightMapwithName(TextureName, (int)i_meshInfo);
											}

											ImGui::TreePop();
										}
									}

									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::AnimationComponent>(x))
							{
								if (ImGui::TreeNode("Animation Component"))
								{
									BE::AnimationComponent& AnimationRef = BE::ECS->GetComponent<BE::AnimationComponent>(x);

									ImGui::Text("Animation Name:");
									ImGui::SameLine();
									ImGui::Text(AnimationRef.getAnimationName().c_str());

									ImGui::Text("Animation Timer:");
									ImGui::SameLine();
									ImGui::Text(std::to_string(AnimationRef.getCurrentTime()).c_str());

									float factorValue = AnimationRef.getTickFactor();
									if (ImGui::SliderFloat("Animation Speed offset:", &factorValue, 0.0f, 10.0f))
									{
										AnimationRef.setTickFactor(factorValue);
									}

									static int add_animation_selected;
									static unsigned animationBEIDSelected;
									static std::string animationNameSelected;

									if (BE::ECS->HasComponent < BE::Renderer>(x))
									{
										BE::Renderer& Rendererref = BE::ECS->GetComponent<BE::Renderer>(x);

										if (BE::AssetMgr->IsAssetDataLoaded(Rendererref.getModelID()))
										{
											auto& vector = AssetMgr->GetAssetData<BeModel>(Rendererref.getModelID()).GetAnimationDetails();

											static ImGuiTextFilter animationFilter;
											animationFilter.Draw(ICON_FA_SEARCH);

											if (ImGui::BeginListBox("Available Animations from current Model"))
											{
												for (unsigned i = 0; i < vector.size(); ++i)
												{
													if (animationFilter.PassFilter(vector[i].GetCurrentAnimationName().c_str()))
													{
														if (ImGui::Selectable(vector[i].GetCurrentAnimationName().c_str(), animationBEIDSelected == i))
														{
															animationBEIDSelected = i;
															animationNameSelected = vector[i].GetCurrentAnimationName();
														}
													}
												}

												ImGui::EndListBox();
											}

											if (ImGui::Button("Set Animation"))
											{
												AnimationRef.PlayAnimation(animationBEIDSelected, animationNameSelected);
											}
										}
									}

									bool shouldFreeze = AnimationRef.getFreezeAtLastFrame();
									if (ImGui::Checkbox("Freeze at last frame", &shouldFreeze))
									{
										AnimationRef.setFreezeAtLastFrame(shouldFreeze);
									}

									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::GameLogic>(x))
							{
								if (ImGui::TreeNode("GameLogic Component"))
								{
									BE::GameLogic& GameLogicRef = BE::ECS->GetComponent<BE::GameLogic>(x);

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

										static ImGuiTextFilter scriptsFilter;
										scriptsFilter.Draw(ICON_FA_SEARCH);

										if (ImGui::BeginListBox("##Scripts"))
										{
											static int add_selected;
											int b = 0;

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
									//auto& ScriptData = GameLogicRef.get
									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::lightComponent>(x))
							{
								if (ImGui::TreeNode("Light Component"))
								{
									BE::lightComponent& LightRef = BE::ECS->GetComponent<BE::lightComponent>(x);

									float Color[] = { LightRef.getColor().r,
													  LightRef.getColor().g,
													  LightRef.getColor().b };
									if (ImGui::ColorEdit3("Color", Color))
									{
										LightRef.setColor(glm::vec3{ Color[0], Color[1], Color[2] });
									}


									float Intensity = LightRef.getIntensity();
									if (ImGui::DragFloat("Intensity", &Intensity))
									{
										LightRef.setIntensity(Intensity);
									}

									float radius = LightRef.getRadius();
									if (ImGui::DragFloat("Radius", &radius))
									{
										LightRef.setRadius(radius);
									}

									ImGui::TreePop();
								}

								ImGui::Separator();
							}

							if (BE::ECS->HasComponent<BE::ParticleSystem>(x))
							{
								if (ImGui::TreeNode("Particle System Component"))
								{
									BE::ParticleSystem& ParticleSystemRef = BE::ECS->GetComponent<BE::ParticleSystem>(x);

									glm::vec4 pos = ParticleSystemRef.getPos();
									if (ImGui::DragFloat4("Position", &pos[0], 0.1f))
									{
										ParticleSystemRef.setPos(pos);
									}

									glm::vec4 color = ParticleSystemRef.getColor();
									if (ImGui::ColorEdit4("Color", &color[0]))
									{
										ParticleSystemRef.setColor(color);
									}

									float alpha = ParticleSystemRef.getAlpha();
									if (ImGui::DragFloat("Alpha", &alpha, 0.1f))
									{
										ParticleSystemRef.setAlpha(alpha);
									}

									glm::vec4 vel = ParticleSystemRef.getVel();
									if (ImGui::DragFloat4("Velocity", &vel[0], 0.1f))
									{
										ParticleSystemRef.setVel(vel);
									}

									float particleSize = ParticleSystemRef.getParticleSize();
									if (ImGui::DragFloat("Particle Size", &particleSize, 0.1f))
									{
										ParticleSystemRef.setParticleSize(particleSize);
									}

									float lifetime = ParticleSystemRef.getLifeTime();
									if (ImGui::DragFloat("Lifetime", &lifetime, 0.1f))
									{
										ParticleSystemRef.setLifeTime(lifetime);
									}

									//if (ImGui::Button("Add Fire Particles"))
									//{
									//	BE::ParticleSystem CreateFireParticles(ParticleSystemRef);
									//}

									int particleCount = ParticleSystemRef.getParticleCount();
									if (ImGui::DragInt("Particle Count", &particleCount))
									{
										ParticleSystemRef.setParticleCount(particleCount);
									}

									ImGui::TreePop();
								}
							}


							//ImGui::TreePop();
						//}
						
						//ImGui::Separator();

						ImGui::Text("");
						ImGui::Text("Add/ Remove Component");

						const char* items[] = { "AudioSource", "AudioListener", "Animation", "Renderer", "Light", "GameLogic",  "ParticleSystem",  "Physics" };
						static bool selected[9];

						static std::string preview_combo;

						if (ImGui::BeginCombo("Components", preview_combo.c_str()))
						{
							preview_combo = "";

							std::vector<std::string> vec;

							for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
							{
								ImGui::Selectable(items[i], &selected[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

								if (selected[i])
								{
									vec.push_back(items[i]);
								}
							}

							for (int i = 0; i < vec.size(); ++i)
							{
								if (vec.size() == 1)
								{
									preview_combo += vec.at(i);
								}
								else if (!(i == vec.size() - 1))
								{
									preview_combo += vec.at(i) + ",";
								}
								else
								{
									preview_combo += vec.at(i);
								}
							}

							ImGui::EndCombo();
						}

						if (ImGui::Button(ICON_FA_PLUS_SQUARE " Add"))
						{
							if (selected[0])
							{
								if (!BE::ECS->HasComponent<BE::AudioSource>(x))
								{
									//BE::ECS->AddComponent<BE::AudioSource>(x, BE::AudioSource{});

									std::shared_ptr<AddComponentCommand<BE::AudioSource>> p = std::make_shared<AddComponentCommand<BE::AudioSource>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}	
							}
							if (selected[1])
							{
								if (!BE::ECS->HasComponent<BE::AudioListener>(x))
								{
									//BE::ECS->AddComponent<BE::AudioListener>(x, BE::AudioListener{});

									std::shared_ptr<AddComponentCommand<BE::AudioListener>> p = std::make_shared<AddComponentCommand<BE::AudioListener>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}
							if (selected[2])
							{
								if (!BE::ECS->HasComponent<BE::AnimationComponent>(x))
								{
									//BE::ECS->AddComponent<BE::AnimationComponent>(x, BE::AnimationComponent{});

									if (!BE::ECS->HasComponent<BE::Renderer>(x))
									{
										std::shared_ptr<AddComponentCommand<BE::Renderer>> p = std::make_shared<AddComponentCommand<BE::Renderer>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}

									std::shared_ptr<AddComponentCommand<BE::AnimationComponent>> p = std::make_shared<AddComponentCommand<BE::AnimationComponent>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}
							if (selected[3])
							{
								if (!BE::ECS->HasComponent<BE::Renderer>(x))
								{
									//BE::ECS->AddComponent(x, BE::Renderer{});

									std::shared_ptr<AddComponentCommand<BE::Renderer>> p = std::make_shared<AddComponentCommand<BE::Renderer>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}
							if (selected[4])
							{
								if (!BE::ECS->HasComponent<BE::lightComponent>(x))
								{
									std::shared_ptr<AddComponentCommand<BE::lightComponent>> p = std::make_shared<AddComponentCommand<BE::lightComponent>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}
							if (selected[5])
							{
								if (!BE::ECS->HasComponent<BE::GameLogic>(x))
								{
									std::shared_ptr<AddComponentCommand<BE::GameLogic>> p = std::make_shared<AddComponentCommand<BE::GameLogic>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}
							if (selected[6])
							{
								if (!BE::ECS->HasComponent<BE::ParticleSystem>(x) && BE::ECS->HasComponent<BE::Transform>(x))
								{
									//BE::ECS->AddComponent(x, BE::ParticleSystem{});
									std::shared_ptr<AddComponentCommand<BE::ParticleSystem>> p = std::make_shared<AddComponentCommand<BE::ParticleSystem>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}
							if (selected[7])
							{
								if (!BE::ECS->HasComponent<BE::Physics>(x))
								{
									//BE::ECS->AddComponent<BE::Physics>(x, BE::Physics{});

									std::shared_ptr<AddComponentCommand<BE::Physics>> p = std::make_shared<AddComponentCommand<BE::Physics>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);
								}
							}

							preview_combo = "";

							for (auto j = 0; j < IM_ARRAYSIZE(selected); ++j)
							{
								selected[j] = false;
							}
						}

						ImGui::SameLine();

						static std::string remove_comp = "";
						if (ImGui::Button(ICON_FA_TRASH " Remove"))
						{
							if (selected[0])
							{
								remove_comp.append("AudioSource\n");
							}
							if (selected[1])
							{
								remove_comp.append("AudioListener\n");
							}
							if (selected[2])
							{
								remove_comp.append("Animation\n");
							}
							if (selected[3])
							{
								remove_comp.append("MeshRenderer\n");

								if (BE::ECS->HasComponent<BE::AnimationComponent>(x))
								{
									remove_comp.append("\n");
									remove_comp.append("This MeshRenderer is required by: Animation Component\n");
								}
							}
							if (selected[4])
							{
								remove_comp.append("Light\n");
							}
							if (selected[5])
							{
								remove_comp.append("GameLogic\n");
							}
							if (selected[6])
							{
								remove_comp.append("ParticleSystem\n");
							}
							if (selected[7])
							{
								remove_comp.append("Physics\n");
							}

							ImGui::OpenPopup("Remove Component?");
						}

						ImVec2 center = ImGui::GetMainViewport()->GetCenter();
						ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

						if (ImGui::BeginPopupModal("Remove Component?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							//Graphics_Window::SetPicking(false);

							ImGui::Text("Are you sure you want to remove the following Components?\n");
							ImGui::Text(remove_comp.c_str());
							ImGui::Separator();

							if (ImGui::Button("Remove"))
							{
								if (selected[0])
								{
									if (BE::ECS->HasComponent<BE::AudioSource>(x))
									{
										//BE::ECS->RemoveComponent<BE::AudioSource>(x);

										std::shared_ptr<RemoveComponentCommand<BE::AudioSource>> p = std::make_shared<RemoveComponentCommand<BE::AudioSource>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}
								if (selected[1])
								{
									if (BE::ECS->HasComponent<BE::AudioListener>(x))
									{
										//BE::ECS->RemoveComponent<BE::AudioListener>(x);

										std::shared_ptr<RemoveComponentCommand<BE::AudioListener>> p = std::make_shared<RemoveComponentCommand<BE::AudioListener>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}
								if (selected[2])
								{
									if (BE::ECS->HasComponent<BE::AnimationComponent>(x))
									{
										//BE::ECS->RemoveComponent<BE::AnimationComponent>(x);

										std::shared_ptr<RemoveComponentCommand<BE::AnimationComponent>> p = std::make_shared<RemoveComponentCommand<BE::AnimationComponent>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}
								if (selected[3])
								{
									if (BE::ECS->HasComponent<BE::Renderer>(x))
									{
										//BE::ECS->RemoveComponent<BE::Renderer>(x);

										if (BE::ECS->HasComponent<BE::AnimationComponent>(x))
										{
											std::shared_ptr<RemoveComponentCommand<BE::AnimationComponent>> p = std::make_shared<RemoveComponentCommand<BE::AnimationComponent>>();
											p->setID(x);
											EditorSystem::GetInstance()->command->executeCmd(p);
										}

										std::shared_ptr<RemoveComponentCommand<BE::Renderer>> p = std::make_shared<RemoveComponentCommand<BE::Renderer>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}
								if (selected[4])
								{
									if (BE::ECS->HasComponent<BE::lightComponent>(x))
									{
										std::shared_ptr<RemoveComponentCommand<BE::lightComponent>> p = std::make_shared<RemoveComponentCommand<BE::lightComponent>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}
								if (selected[5])
								{
									if (BE::ECS->HasComponent<BE::GameLogic>(x))
									{
										std::shared_ptr<RemoveComponentCommand<BE::GameLogic>> p = std::make_shared<RemoveComponentCommand<BE::GameLogic>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}
								if (selected[6])
								{
									std::shared_ptr<RemoveComponentCommand<BE::ParticleSystem>> p = std::make_shared<RemoveComponentCommand<BE::ParticleSystem>>();
									p->setID(x);
									EditorSystem::GetInstance()->command->executeCmd(p);

									/*if (BE::ECS->HasComponent<BE::ParticleSystem>(x))
									{
										BE::ECS->RemoveComponent<BE::ParticleSystem>(x);
									}*/
								}
								if (selected[7])
								{
									if (BE::ECS->HasComponent<BE::Physics>(x))
									{
										//BE::ECS->RemoveComponent<BE::Physics>(x);

										std::shared_ptr<RemoveComponentCommand<BE::Physics>> p = std::make_shared<RemoveComponentCommand<BE::Physics>>();
										p->setID(x);
										EditorSystem::GetInstance()->command->executeCmd(p);
									}
								}

								preview_combo = "";
								remove_comp.clear();
								for (auto j = 0; j < IM_ARRAYSIZE(selected); ++j)
								{
									selected[j] = false;
								}

								ImGui::CloseCurrentPopup();
							}

							ImGui::SetItemDefaultFocus();
							ImGui::SameLine();
							if (ImGui::Button("Cancel"))
							{
								remove_comp.clear();

								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}

					}

				}
				end();
			}
		}
	}

	void Inspector_Window::Shutdown()
	{
		delete p_insp;
	}

	void Inspector_Window::DisplayDetails123(const rttr::variant& prop, std::string propName)
	{
		const rttr::type& memberType = prop.get_type();

		/*if (prop.get_metadata(BE::Serialisation::MetaDataKey::HiddenFromEditor))
			continue;*/

		if (memberType == rttr::type::get<std::string>() || memberType.get_wrapped_type() == rttr::type::get<std::string>())
		{
			rttr::variant var = prop.get_wrapped_value<std::string>();
			const std::string& get_string = var.get_value<std::string>();

			char arrayChar[BE::MAX_STR_BUFFER];
			strcpy_s(arrayChar, BE::MAX_STR_BUFFER, get_string.c_str());

			//ImGui::Text(get_string.c_str());
			if (ImGui::InputText(propName.c_str(), arrayChar, BE::MAX_STR_BUFFER))
			{
				//prop.(inst, std::string{ arrayChar });
			}
		}

		if (memberType == rttr::type::get<bool>() || memberType.get_wrapped_type() == rttr::type::get<bool>())
		{
			rttr::variant var = prop.get_wrapped_value<bool>();

			bool flag = var.get_value<bool>();
			if (ImGui::Checkbox(propName.c_str(), &flag))
			{
				//prop.set_value(inst, flag);
			}
		}

		
		if (memberType == rttr::type::get<float>() || memberType.get_wrapped_type() == rttr::type::get<float>())
		{
			rttr::variant var = prop.get_wrapped_value<float>();

			float getFloat = var.get_value<float>();
			if (ImGui::DragFloat(propName.c_str(), &getFloat))
			{
				//prop.set_value(inst, getFloat);
			}
		}
		
		if (memberType == rttr::type::get<int>() || memberType.get_wrapped_type() == rttr::type::get<int>())
		{
			rttr::variant var = prop.get_wrapped_value<int>();

			int getInt = var.get_value<int>();
			if (ImGui::DragInt(propName.c_str(), &getInt))
			{
				//prop.set_value(inst, getInt);
			}
		}
		/*
		if (memberType.is_sequential_container())
		{
			rttr::variant variantArray = prop.get_value(inst);

			if (ImGui::TreeNode(propName.c_str()))
			{
				std::cout << "hello1" << std::endl;

				rttr::variant temp = variantArray;

				auto view = temp.create_sequential_view();
				for (const auto& curr : view)
				{
					std::cout << "here" << std::endl;
					std::cout << curr.get_type().get_name() << std::endl;

					DisplayDetails(curr, curr);
				}
				//prop.set_value(inst, temp);

				ImGui::TreePop();
			}
		}

		if (memberType.is_associative_container())
		{
			rttr::variant variantArray = prop.get_value(inst);

			if (ImGui::TreeNode(propName.c_str()))
			{
				std::cout << "hello2" << std::endl;

				rttr::variant temp = variantArray;

				auto view = temp.create_associative_view();
				for (const auto& curr : view)
				{
					std::cout << curr.first.get_type().get_name() << curr.second.get_type().get_name() << std::endl;

					DisplayProperties(curr, curr.second.get_type());
				}

				ImGui::TreePop();
			}
		}

		if (memberType.get_properties().size() != 0)
		{


			if (ImGui::TreeNode(propName.c_str()))
			{
				rttr::variant temp = prop.get_value(inst);
				DisplayProperties(temp, temp.get_type());
				prop.set_value(inst, temp);

				ImGui::TreePop();
			}
		}*/
	}

	void Inspector_Window::DisplayDetails(const rttr::instance& inst, const rttr::property& prop)
	{
		const std::string& propName = prop.get_name().to_string();

		const rttr::type& memberType = prop.get_type();

		/*if (prop.get_metadata(BE::Serialisation::MetaDataKey::HiddenFromEditor))
			continue;*/

		if (memberType == rttr::type::get<std::string>() || memberType.get_wrapped_type() == rttr::type::get<std::string>())
		{
			rttr::variant var = prop.get_value(inst);
			const std::string& get_string = var.get_value<std::string>();

			char arrayChar[BE::MAX_STR_BUFFER];
			strcpy_s(arrayChar, BE::MAX_STR_BUFFER, get_string.c_str());

			if (ImGui::InputText(propName.c_str(), arrayChar, BE::MAX_STR_BUFFER))
			{
				prop.set_value(inst, std::string{ arrayChar });
			}
		}

		if (memberType == rttr::type::get<bool>() || memberType.get_wrapped_type() == rttr::type::get<bool>())
		{
			rttr::variant var = prop.get_value(inst);

			bool flag = var.get_value<bool>();
			if (ImGui::Checkbox(propName.c_str(), &flag))
			{
				prop.set_value(inst, flag);
			}
		}

		if (memberType == rttr::type::get<float>() || memberType.get_wrapped_type() == rttr::type::get<float>())
		{
			rttr::variant var = prop.get_value(inst);

			float getFloat = var.get_value<float>();
			if (ImGui::DragFloat(propName.c_str(), &getFloat))
			{
				prop.set_value(inst, getFloat);
			}
		}

		if (memberType == rttr::type::get<int>() || memberType.get_wrapped_type() == rttr::type::get<int>())
		{
			rttr::variant var = prop.get_value(inst);

			int getInt = var.get_value<int>();
			if (ImGui::DragInt(propName.c_str(), &getInt))
			{
				prop.set_value(inst, getInt);
			}
		}

		if (memberType.is_sequential_container())
		{
			rttr::variant variantArray = prop.get_value(inst);

			if (ImGui::TreeNode(propName.c_str()))
			{
				rttr::variant temp = variantArray;

				auto view = temp.create_sequential_view();

				int  imguiID = 0;
				for (const auto& curr : view)
				{
					ImGui::PushID(imguiID);

					DisplayDetails123(curr, propName);

					ImGui::PopID();

					++imguiID;
				}

				ImGui::TreePop();
			}
		}

		if (memberType.is_associative_container())
		{
			rttr::variant variantArray = prop.get_value(inst);

			if (ImGui::TreeNode(propName.c_str()))
			{
				rttr::variant temp = variantArray;

				auto view = temp.create_associative_view();
				const rttr::type& keyType = view.get_key_type().is_wrapper() ? view.get_key_type().get_wrapped_type() : view.get_key_type();
				const rttr::type& valueType = view.get_value_type().is_wrapper() ? view.get_value_type().get_wrapped_type() : view.get_value_type();

				int  imguiID = 0;

				for (const auto& curr : view)
				{
					ImGui::PushID(imguiID);

					if (valueType == rttr::type::get<IScript*>() && keyType == rttr::type::get<std::string>())
					{
						std::string ScriptName = curr.first.get_wrapped_value<std::string>();

						if (ImGui::TreeNode(ScriptName.c_str()))
						{
							rttr::type componentType = rttr::type::get_by_name(curr.first.get_wrapped_value<std::string>());

							for (const auto& ScriptProp : componentType.get_properties())
							{
								auto f = curr.second.get_wrapped_value<std::shared_ptr<BE::IScript>>();
								rttr::instance objTest = f;

								DisplayDetails(objTest, ScriptProp);
							}

							ImGui::TreePop();
						}
					}

					ImGui::PopID();

					++imguiID;

				}

				ImGui::TreePop();
			}
		}

		if (memberType.get_properties().size() != 0)
		{


			if (ImGui::TreeNode(propName.c_str()))
			{
				rttr::variant temp = prop.get_value(inst);
				DisplayProperties(temp, temp.get_type());
				prop.set_value(inst, temp);

				ImGui::TreePop();
			}
		}
	}

	void Inspector_Window::DisplayProperties(rttr::instance inst, rttr::type componentType)
	{
		for (const auto& prop : componentType.get_properties())
		{
			DisplayDetails(inst, prop);
		}
	}
}
#endif