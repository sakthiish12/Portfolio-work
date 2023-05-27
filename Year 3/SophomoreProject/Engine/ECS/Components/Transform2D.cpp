// /******************************************************************************/
// /*!
// \file         Transform2D.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         8th Dec, 2021
// \brief        Definition of Serialization and ImGui Code for Transform Component
//				 Transform Component details position, rotation and scale of entity
// \lines        238 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "../component.h"
#include "../../ECS/ECS.h"
#include "../../pch.h"
#include "../../Editor/UI Layer/ImGuiHelpers.h"

//SERIALIZATION////////////////////////////////////////////////////////////////

bool transform2D::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<transform2D>(toSave))
	{
		const transform2D& c = ecs.getComponent<transform2D>(toSave);
		writer->Key("struct transform2D");
		{
			writer->StartObject();
			writer->Key("pos");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(isnan(c.pos.x) ? 0.0f : c.pos.x)); //Save as default value instead if nan, thanks Delvin
				writer->Key("y");
				writer->Double(static_cast<double>(isnan(c.pos.y) ? 0.0f : c.pos.y));
				writer->Key("z");
				writer->Double(static_cast<double>(isnan(c.pos.z) ? 0.0f : c.pos.z));
				writer->EndObject();
			}
			writer->Key("rotation");
			writer->Double(static_cast<double>(isnan(c.rotation) ? 0.0f : c.rotation));
			writer->Key("scale");
			{
				writer->StartObject();
				writer->Key("x");
				writer->Double(static_cast<double>(isnan(c.scale.x) ? 1.0f : c.scale.x));
				writer->Key("y");
				writer->Double(static_cast<double>(isnan(c.scale.y) ? 1.0f : c.scale.y));
				writer->EndObject();
			}
			writer->EndObject();
		}
	}
	return true;
}

bool transform2D::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct transform2D") &&
		data["struct transform2D"].IsObject())
	{
		const rapidjson::Value& detail = data["struct transform2D"];
		sm.DeserializeVector("pos", pos, detail);
		sm.DeserializeFloat("rotation", rotation, detail);
		sm.DeserializeVector("scale", scale, detail);
		return true;
	}
	return false;
}

void transform2D::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}


//IMGUI////////////////////////////////////////////////////////////////////////

void transform2D::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<transform2D>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Transform##Component", &notToDelete))
		{
			//Component tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The location of the entity");
				ImGui::Text("Including its position, rotation and scaling");
				ImGui::Text("The most important component of an entity");
				ImGui::Text("Most systems require an entity to have a transform component");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<transform2D>(selected);

			//POSITION
			ImGui::Text("Position");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Where the entity is in arbitrary units");
				ImGui::Text("It is a good idea to decide your units");
				ImGui::Text("early on (i.e. metres, feet, etc...)");
				ImGui::Text("If a child of another entity, will then instead");
				ImGui::Text("describe positioning relative to parent");
				ImGui::Text("with (0, 0) meaning identical positioning");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Positioning");
				ImGui::Text("Goes to the right as it increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			//Colouration of the slider
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			//Why "##PositionX"?
			//ImGui cannot function properly if the labels are not unique
			//The two #s means the label is hidden
			//While still maintaining that the label is unique
			//https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-why-is-my-widget-not-reacting-when-i-click-on-it
			//TODO variable drag sensitivity

			ImGuiHelpers::ImGuiDragFloatSpecial("##PositonX", &(t.pos.x));
			/******************
			* Redo undo feature 
			******************/
			static float oldVal;
			if (ImGui::IsItemActivated()) // THIS WORKS!
			{
				// here we record the old value for later undo record.
				oldVal = t.pos.x;
			}

			if (ImGui::IsItemDeactivatedAfterEdit() && oldVal != t.pos.x) // THIS WORKS!
			{
				// here we record a new undo
				UiLayer::GetInstance().recordAction(selected ,"transform","posx",oldVal,t.pos.x);//RecordUndo(oldVal, newVal);
				UiLayer::GetInstance().incrementIndex();
			}

			ImGui::PopStyleColor(3); //Only colour this slider red

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Positioning");
				ImGui::Text("Goes upwards as it increases");

				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##PositionY", &(t.pos.y));
			/******************
			* Redo undo feature
			******************/
			static float oldValY;
			if (ImGui::IsItemActivated()) // THIS WORKS!
			{
				// here we record the old value for later undo record.
				oldValY = t.pos.y;
			}

			if (ImGui::IsItemDeactivatedAfterEdit() && oldValY != t.pos.y) // THIS WORKS!
			{
				// here we record a new undo
				UiLayer::GetInstance().recordAction(selected, "transform", "posy", oldValY, t.pos.y);//RecordUndo(oldVal, newVal);
				UiLayer::GetInstance().incrementIndex();
			}
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 0.5f, 1.0f, 1.0f }, "Z Layer");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Layering (not positioning)");
				ImGui::Text("A more positive Z value means it gets higher priority in rendering");
				ImGui::Text("(i.e. an entity with higher Z coordinate will be rendered over an entity with a lower Z coordinate");
				ImGui::Text("Should be between 0.0 and 2.0 (ends inclusive)");
				ImGui::Text("Note that it does not actually modify Z coordinates of the entity as there is no Z axis");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.2f, 0.4f, 1.0f }); //Blue
			ImGuiHelpers::ImGuiDragFloatSpecial("##PositionZ", &(t.pos.z));
			/******************
			* Redo undo feature
			******************/
			static float oldValposZ;
			if (ImGui::IsItemActivated()) // THIS WORKS!
			{
				// here we record the old value for later undo record.
				oldValposZ = t.pos.z;
			}

			if (ImGui::IsItemDeactivatedAfterEdit() && oldValposZ != t.pos.z) // THIS WORKS!
			{
				// here we record a new undo
				UiLayer::GetInstance().recordAction(selected, "transform", "posz", oldValposZ, t.pos.z);//RecordUndo(oldVal, newVal);
				UiLayer::GetInstance().incrementIndex();
			}
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//ROTATION
			ImGui::Text("Rotation");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Rotation in degrees");
				ImGui::Text("Goes counter-clockwise while increasing");
				ImGui::Text("Best to be between 0 and 360");
				ImGui::Text("If a child of another entity, will then instead");
				ImGui::Text("have rotation based off parent");
				ImGui::EndTooltip();
			}
			ImGui::Indent();
			ImGui::Text("=");
			ImGui::SameLine();
			ImGuiHelpers::ImGuiDragFloatSpecial("##Rotation",
				&(t.rotation), 0.01f, true, -0.01f, 360.01f);
			/******************
			* Redo undo feature
			******************/
			static float oldValRotation;
			if (ImGui::IsItemActivated()) // THIS WORKS!
			{
				// here we record the old value for later undo record.
				oldValRotation = t.rotation;
			}

			if (ImGui::IsItemDeactivatedAfterEdit() && oldValRotation != t.rotation) // THIS WORKS!
			{
				// here we record a new undo
				UiLayer::GetInstance().recordAction(selected, "transform", "rotation", oldValRotation, t.rotation);//RecordUndo(oldVal, newVal);
				UiLayer::GetInstance().incrementIndex();
			}

			ImGui::Unindent();

			//SCALING
			ImGui::Text("Scale");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How large the entity is in arbitrary units");
				ImGui::Text("It is a good idea to decide your units");
				ImGui::Text("early on (i.e. metres, feet, etc...)");
				ImGui::Text("If a child of another entity, will then instead");
				ImGui::Text("describe how large entity is relative to parent");
				ImGui::Text("with (1, 1) meaning identical sizing");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Scaling");
				ImGui::Text("Gets wider as its magnitude increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##ScaleX", &(t.scale.x));
			/******************
			* Redo undo feature
			******************/
			static float oldValScalex;
			if (ImGui::IsItemActivated()) // THIS WORKS!
			{
				// here we record the old value for later undo record.
				oldValScalex = t.scale.x;
			}

			if (ImGui::IsItemDeactivatedAfterEdit() && oldValScalex != t.scale.x) // THIS WORKS!
			{
				// here we record a new undo
				UiLayer::GetInstance().recordAction(selected, "transform", "scalex", oldValScalex, t.scale.x);//RecordUndo(oldVal, newVal);
				UiLayer::GetInstance().incrementIndex();
			}
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Scaling");
				ImGui::Text("Gets taller as its magnitude increases");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##ScaleY", &(t.scale.y));
			/******************
			* Redo undo feature
			******************/
			static float oldValScaleY;
			if (ImGui::IsItemActivated()) // THIS WORKS!
			{
				// here we record the old value for later undo record.
				oldValScaleY = t.scale.y;
			}

			if (ImGui::IsItemDeactivatedAfterEdit() && oldValScaleY != t.scale.y) // THIS WORKS!
			{
				// here we record a new undo
				UiLayer::GetInstance().recordAction(selected, "transform", "scaley", oldValScaleY, t.scale.y);//RecordUndo(oldVal, newVal);
				UiLayer::GetInstance().incrementIndex();
			}
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//READ-ONLY WORLD MATRIX
			ImGui::Text("World Matrix");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Represents the world matrix of the transform after applying");
				ImGui::Text("transform values of itself and its ancestors (if any)");
				ImGui::EndTooltip();
			}
			//MATRIX ELEMENTS:
			//See section 9 of this
			//http://www.c-jump.com/bcc/common/Talk3/Math/GLM/GLM.html
			//And this
			//https://stackoverflow.com/a/17718408
			//PRINTF
			//https://en.cppreference.com/w/c/io/fprintf
			ImGui::Text("% 08.3f    % 08.3f    % 08.3f    % 08.3f", t.worldMatrix[0][0], t.worldMatrix[1][0], t.worldMatrix[2][0], t.worldMatrix[3][0]);
			ImGui::Text("% 08.3f    % 08.3f    % 08.3f    % 08.3f", t.worldMatrix[0][1], t.worldMatrix[1][1], t.worldMatrix[2][1], t.worldMatrix[3][1]);
			ImGui::Text("% 08.3f    % 08.3f    % 08.3f    % 08.3f", t.worldMatrix[0][2], t.worldMatrix[1][2], t.worldMatrix[2][2], t.worldMatrix[3][2]);
			ImGui::Text("% 08.3f    % 08.3f    % 08.3f    % 08.3f", t.worldMatrix[0][3], t.worldMatrix[1][3], t.worldMatrix[2][3], t.worldMatrix[3][3]);
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The location of the entity");
				ImGui::Text("Including its position, rotation and scaling");
				ImGui::Text("The most important component of an entity");
				ImGui::Text("Most systems require an entity to have a transform component");
				ImGui::EndTooltip();
			}
		}
		
		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<transform2D>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void transform2D::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<transform2D>(selected))
	{
		if (ImGui::Selectable("Transform 2D##componentList"))
		{
			//Add component
			ecs.addComponent<transform2D>(ecs.whichEntityIsSelected(),
				transform2D{});
		}
		//Component Tool Tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("The location of the entity");
			ImGui::Text("Including its position, rotation and scaling");
			ImGui::Text("The most important component of an entity");
			ImGui::Text("Most systems require an entity to have a transform component");
			ImGui::EndTooltip();
		}
	}
}