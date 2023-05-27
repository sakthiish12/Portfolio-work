// /******************************************************************************/
// /*!
// \file         RigidBody2D.cpp
// \project name Engine
// \author(s)    Ryan Wang
// \date         17th Nov, 2021
// \brief        Definition of Serialization and ImGui Code for entity's rigidbody
// \lines        ??? Lines
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

bool rigidBody2D::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<rigidBody2D>(toSave))
	{
		const rigidBody2D& c = ecs.getComponent<rigidBody2D>(toSave);
		writer->Key("struct rigidBody2D");
		{
			writer->StartObject();
			writer->Key("Mass");
			writer->Double(static_cast<double>(c.Mass));
			writer->Key("restitution");
			writer->Double(static_cast<double>(c.restitution));
			writer->Key("isGravityOn");
			writer->Bool(c.isGravityOn);
			writer->Key("Type");
			if (c.Type == Types::Dynamics)
				writer->String("Dynamics");
			else if (c.Type == Types::Kinametics)
				writer->String("Kinametics");
			else //Type == Types::Static
				writer->String("Static");
			writer->EndObject();
		}
	}
	return true;
}

bool rigidBody2D::OnDeserialize(const rapidjson::Value& data)
{
	SerializeManager sm;

	if (data.HasMember("struct rigidBody2D") &&
		data["struct rigidBody2D"].IsObject())
	{
		const rapidjson::Value& detail = data["struct rigidBody2D"];
		sm.DeserializeFloat("Mass", Mass, detail);
		InvMass = 1.f / Mass;
		InertiaMass = Mass;
		InvInertiaMass = 1.f / InertiaMass;
		sm.DeserializeFloat("restitution", restitution, detail);
		sm.DeserializeBool("isGravityOn", isGravityOn, detail);
		std::string s;
		sm.DeserializeString("Type", s, detail);
		if (!s.compare("Dynamics"))
		{
			Type = Types::Dynamics;
		}
		else if (!s.compare("Static"))
		{
			Type = Types::Static;
		}
		else if (!s.compare("Kinametics"))
		{
			Type = Types::Kinametics;
		}
		return true;
	}
	return false;
}

void rigidBody2D::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void rigidBody2D::inspectorView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<rigidBody2D>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("RigidBody2D##Component", &notToDelete))
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<rigidBody2D>(selected);

			//VELOCITY
			ImGui::Text("Velocity");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How fast the entity is going in arbitrary units");
				ImGui::Text("It is a good idea to decide your units");
				ImGui::Text("early on (i.e. metres, feet, etc...)");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Velocity");
				ImGui::Text("Positive means going to the right");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##VelocityX", &(t.vel.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Velocity");
				ImGui::Text("Positive means going upwards");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f });
			ImGuiHelpers::ImGuiDragFloatSpecial("##VelocityY", &(t.vel.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//ACCELERATION
			ImGui::Text("Accelration");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How fast the entity's velocity is being modified in arbitrary units");
				ImGui::Text("It is a good idea to decide your units");
				ImGui::Text("early on (i.e. metres, feet, etc...)");
				ImGui::EndTooltip();
			}

			ImGui::Indent();
			ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "X");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Horizontal Acceleration");
				ImGui::Text("Positive means going faster to the right");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.4f, 0.2f, 0.2f, 1.0f }); //Red
			ImGuiHelpers::ImGuiDragFloatSpecial("##AccelerationX", &(t.acceleration.x));
			ImGui::PopStyleColor(3);

			ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Y");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Vertical Acceleration");
				ImGui::Text("Positive means going faster upwards");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGuiHelpers::ImGuiPushFrameStyleColors(ImVec4{ 0.2f, 0.4f, 0.2f, 1.0f }); //Green
			ImGuiHelpers::ImGuiDragFloatSpecial("##AccelerationY", &(t.acceleration.y));
			ImGui::PopStyleColor(3);

			ImGui::Unindent();

			//ROTATIONAL VELOCITY
			ImGui::Text("Rotational Velocity");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How fast the entity is rotating");
				ImGui::Text("Positive means rotating counter-clockwise");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();

			ImGuiHelpers::ImGuiDragFloatSpecial("##RotationalVelocity", &(t.rot_vel));

			//FORCES
			//A table of forces acting on the entity perhaps?
			ImGui::Text("Forces (TODO)");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("The forces acting on the entity");
				ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "More implementation required");
				ImGui::EndTooltip();
			}
			ImGui::Text("Force Sum: (%.3f, %.3f)", t.forceManager.getForce().x, t.forceManager.getForce().y);

			//MASS
			ImGui::Text("Mass");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How heavy the entity is");
				ImGui::Text("Defines how much influence forces have on it");
				ImGui::Text("Should be positive");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			//TODO check again the maximum value
			ImGuiHelpers::ImGuiDragFloatSpecial("##EntityMass", &(t.Mass), 0.01f, true, 0.0f, 100000000000.0f);

			//Inertial mass
			ImGui::Text("Inertial Mass");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("How much influence rotational forces have on it");
				ImGui::Text("Presently matches mass");
				ImGui::Text("Should be positive");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			//TODO check again the maximum value
			ImGuiHelpers::ImGuiDragFloatSpecial("##EntityInertialMass", &(t.InertiaMass), 0.01f, true, 0.0f, 100000000000.0f);

			//GRAVITY
			ImGui::Text("Gravity");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Whether gravity is applied on this entity");
				ImGui::Text("A constant downward force");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGui::Checkbox("##EntityGravity", &(t.isGravityOn));

			//FREEZES
			ImGui::Text("Freeze X");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("If the horizontal positioning of the entity");
				ImGui::Text("is to be fixed");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGui::Checkbox("##FreezeX", &(t.freezeX));

			ImGui::Text("Freeze Y");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("If the vertical positioning of the entity");
				ImGui::Text("is to be fixed");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGui::Checkbox("##FreezeY", &(t.freezeY));

			ImGui::Text("Freeze Rotation");
			//Tooltip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("If the rotation of the entity");
				ImGui::Text("is to be fixed");
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGui::Checkbox("##FreezeRot", &(t.freezeR));

			//Rigid body type
			//ImGui::Text("Rigidbody type: %s", t.textureId.c_str());
			std::string entityType;
			switch (t.Type)
			{
			case(Types::Dynamics):
				entityType = "Dynamics";
				break;
			case(Types::Kinametics):
				entityType = "Kinametics";
				break;
			case(Types::Static):
				entityType = "Static";
				break;
			default:
				break;
			}
			ImGui::Text("Type: %s", entityType.c_str());
			if (ImGui::Button("Change Type"))
				ImGui::OpenPopup("RB TypeList");
			if (ImGui::BeginPopup("RB TypeList"))
			{
				if (ImGui::Selectable("Dynamics"))
					t.Type = Types::Dynamics;
				if (ImGui::Selectable("Kinametics"))
					t.Type = Types::Kinametics;
				if (ImGui::Selectable("Static"))
					t.Type = Types::Static;
				ImGui::EndPopup();
			}
		}
		else
		{
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("To be implemented");
				ImGui::EndTooltip();
			}
		}
		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<rigidBody2D>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void rigidBody2D::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<rigidBody2D>(selected))
	{
		if (ImGui::Selectable("RigidBody 2D##componentList"))
		{
			//Add component
			ecs.addComponent<rigidBody2D>(ecs.whichEntityIsSelected(),
				rigidBody2D{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("To be implemented");
			ImGui::EndTooltip();
		}
	}
}