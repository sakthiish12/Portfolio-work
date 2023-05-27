// /******************************************************************************/
// /*!
// \file         EmitterComponent.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         3rd Feb, 2022
// \brief        EmitterComponent
// \lines         Lines
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
#include "../../Editor/UI Layer/ImGuiHelpers.h"
#include "../../AssetManager/AssetManager.h"
#include "../Systems/ParticleSystem/ParticleSystem.h"

bool EmitterComponent::OnSerialize
(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
{
	//deactivate for now
	//return true;
	entityID toSave = ecs.whichEntityIsBeingSaved();
	if (ecs.entityHas<EmitterComponent>(toSave))
	{
		//const EmitterComponent& c = ecs.getComponent<EmitterComponent>(toSave);
		writer->Key("struct EmitterComponent");
		{
			writer->StartObject();
			writer->Key("container_size");
			writer->Uint(static_cast<unsigned int>(emitterContainer.size()));
			unsigned int i = 0;
			for (auto x : emitterContainer)
			{
				writer->Key(("EmitterName" + std::to_string(i)).c_str());
				{
					//writer->StartObject();
					//writer->Key("ScriptNameID");
					writer->String(x.first.c_str());
					//writer->EndObject();
				}

				writer->Key(("isUsingTexture" + std::to_string(i)).c_str());
				{
					writer->Bool(x.second->_newParticleProperties.use_texture);
				}

				writer->Key(("TextureId" + std::to_string(i)).c_str());
				{
					writer->String(x.second->_newParticleProperties.textureId.c_str());
				}

				++i;
			}
			writer->EndObject();
		}
	}
	return true;
}
#pragma warning( push )
#pragma warning( disable : 4702)

//needed to cancle win32 stuff for json fn
#undef GetObject

//Deserialize from JSON object into data
//Data read is the entire JSON file for now
bool EmitterComponent::OnDeserialize(const rapidjson::Value& data)
{
	/*
	* Emitter will be set via init() at first, then modify by any serialised value
	*/
	//deactivate for now
	return false;
	SerializeManager sm;

	if (data.HasMember("struct EmitterComponent") &&
		data["struct EmitterComponent"].IsObject())
	{
		const rapidjson::Value& detail = data["struct EmitterComponent"];

		unsigned int container_size;
		sm.DeserializeUInt("emitter_size", container_size, detail);
		emitterContainer.clear();
		
		//For each Emitter to be added...
		for (unsigned int i = 0; i < container_size; ++i)
		{
			std::string sName{};
			//std::string emitter_IDname{}, emitter_path{};
			sm.DeserializeString(("EmitterName" + std::to_string(i)).c_str(), sName, detail);//EmitterName0 //name for the class
			//sm.DeserializeString(("EmitterIDname" + std::to_string(i)).c_str(), emitter_IDname, detail); //EmitterIDname0 //name for emitter data
			//sm.DeserializeString(("EmitterPath" + std::to_string(i)).c_str(), emitter_path, detail); //EmitterPath0 //
			
			////if  emitter not found, attempt to serialise via emitter_path...
			//if (EmitterManager.getEmitter(emitter_IDname) == nullptr)
			//{
			//	//create a new instane
			//	std::shared_ptr<Emitter> temp = EmitterManager.createFromEmitterDatabase(sName.c_str());
			//	
			//	//serialised it
			//		// attempt to open file via path
			//		std::stringstream fileContents;
			//		sm.DeserializeFromFile(emitter_path, fileContents);
			//		rapidjson::Document doc;
			//		if (doc.Parse(fileContents.str().c_str()).HasParseError())
			//			PartyAssert(false, "Cannot parse JSON properly");

			//		// serialised
			//		auto e = doc.MemberBegin();
			//		rapidjson::Value& new_data = e->value.GetObject()["emitter_IDname"];
			//		
			//		if (data.HasMember("struct EmitterComponent") &&
			//			data["struct EmitterComponent"].IsObject())
			//		{
			//		}

			//		//sm.DeserializeUInt("emitter_size", container_size, new_data);
			//		//sm.DeserializeUInt("emitter_size", container_size, new_data);
			//		//sm.DeserializeUInt("emitter_size", container_size, new_data);

			//					
			//	//add to emittermanager map
			//	//EmitterManager.createFromEmitterDatabase(sName);
			//}

			//create instance for this component
			emitterContainer.insert(std::make_pair(sName, EmitterManager.createFromEmitterDatabase(sName.c_str())));
			
			//Deserialised data
			bool isUsingTexture;
			sm.DeserializeBool(("isUsingTexture" + std::to_string(i)).c_str(), isUsingTexture, detail);
			if (isUsingTexture == true)
			{
				emitterContainer[sName]->_newParticleProperties.use_texture = isUsingTexture;
				std::string TextureToBeUsed;
				sm.DeserializeString(("TextureId" + std::to_string(i)).c_str(), TextureToBeUsed, detail);
				emitterContainer[sName]->_newParticleProperties.textureId = TextureToBeUsed;
			}
			else
			{
				//not using texture
			}
			
		}
		return true;
	}
	return false;
}
#pragma warning( pop )
void EmitterComponent::addComponent(uint32_t toEntity)
{
	ecs.addComponent(toEntity, *this);
}

//IMGUI////////////////////////////////////////////////////////////////////////

void EmitterComponent::inspectorView()
{
	////Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Please assess whether the entity has the component
	if (ecs.entityHas<EmitterComponent>(selected))
	{
		bool notToDelete = true; //If component is not to be deleted

		if (ImGui::CollapsingHeader("Emitter##Component", &notToDelete))
		{
			//For checking emitter included
			//for (auto& emi : EmitterManager.getEmitterDatabase())
				//std::cout << emi.first.c_str() << std::endl;

			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Add Emitter to entity");
				ImGui::EndTooltip();
			}

			//Get the component's data
			auto& t = ecs.getComponent<EmitterComponent>(selected);

			//Adding a EmitterComponent
			if (ImGui::Button("Add Emitter##EmitterAdder")) {
				ImGui::OpenPopup("Emitter Selector");
			}
			if (ImGui::BeginPopup("Emitter Selector"))
			{
				for (auto& emi : EmitterManager.getEmitterDatabase())
					if (ImGui::Selectable(emi.first.c_str(), false))
						t.emitterContainer[emi.first.c_str()] = EmitterManager.createFromEmitterDatabase(emi.first.c_str());
				ImGui::EndPopup();
			}

			//removing a Emitter
			if (ImGui::Button("Remove Emitter##EmitterRemover")) {
				ImGui::OpenPopup("Emitter Remover");
			}
			if (ImGui::BeginPopup("Emitter Remover"))
			{
				auto it = t.emitterContainer.begin();
				for (; it != t.emitterContainer.end();)
				{
					if (ImGui::Selectable(it->first.c_str(), false))
						it = t.emitterContainer.erase(it);
					else
						++it;
				}


				/*for (auto& script : scriptContainer)
					if (ImGui::Selectable(script.first.c_str(), false))
						scriptContainer.erase(script.first);*/

				ImGui::EndPopup();
			}

			//List scripts
			ImGui::Text("Current Emitter:");
			ImGui::Separator();
			for (auto& s : t.emitterContainer)
			{
				ImGui::Text(s.first.c_str());
			}
		}
		else
		{
			//shown when header is hidden
			//Component tool tip
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Add emitter to entity");
				ImGui::Text("Emitter data is started via both init() and serialisation");
				ImGui::EndTooltip();
			}
		}

		if (!notToDelete)
		{
			//Remove component if the X is clicked
			//Should it be the last component removed, lose selection
			if (ecs.removeComponent<EmitterComponent>(selected))
				ecs.loseEntitySelection();
		}
	}
}

void EmitterComponent::listView()
{
	//Get the ID of the presently selected entity
	entityID selected = ecs.whichEntityIsSelected();

	//Only if the entity not have this component, would it possible for the
	//inspector to add component
	//ECS does not support entities having multiple components of the same type
	if (!ecs.entityHas<EmitterComponent>(selected))
	{
		if (ImGui::Selectable("Emitter##componentList"))
		{
			//Add component
			ecs.addComponent<EmitterComponent>(ecs.whichEntityIsSelected(),
				EmitterComponent{});
		}
		//Component tool tip
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Contain Emitter for an entity");
			ImGui::EndTooltip();
		}
	}
}