#include "pch.h"
#include <Debugger/Debugger.h>
#include "../../Engine/PauseControl.h"
#include "../../Engine/SpeedControl.h"
#include "../ECS/ECS.h"
#include "../UIManager/UIManager.h"
#include "../RendererSystem/RendererSystem.h"
#include "GameLogicSystem.h"

namespace BE
{
	void GameLogicSystem::Init()
	{
	}

#if 0
#define BEHAVIOUR_MEASUREMENTS
#endif
	//Calls update on all behaviours
	void GameLogicSystem::Update(double dt)
	{
#ifdef BEHAVIOUR_MEASUREMENTS
		using namespace std::chrono;
		high_resolution_clock::time_point t1;
		std::stringstream ss;
#endif // BEHAVIOUR_MEASUREMENTS

#ifdef USE_EDITOR
		//If IMGUI is pausing, then don't update
		if(Debug::GetInstance()->GetDebugFlag(Debug::DebugFlags::imguiPause))
			return;
#endif
		auto& entities = ECS->GetTracker<GameLogicTracker>()->GetTrackedEntities();
		for (size_t i = 0; i< entities.size(); i++)
		{
#ifdef BEHAVIOUR_MEASUREMENTS

			ss = std::stringstream();
			t1 = high_resolution_clock::now();
#endif

			//If entity is inactive, then skip
			if (!ECS->GetComponent<EntityInfo>(entities[i]).HasFlag(EntityFlag::active))
				continue;

			//Don't update game logic when pausing
			if (PauseControl::GetPause() && !ECS->GetComponent<EntityInfo>(entities[i]).HasFlag(EntityFlag::updateWhenPaused))
				continue;

			//Don't update game logic when too far away
			/*if (!ECS->GetComponent<EntityInfo>(entities[i]).HasFlag(EntityFlag::updateWhenNotVisible) &&
				ECS->GetComponent<Transform>(entities[i]).GetPosition().SqDistBetween(RendererSystem::GetInstance()->GameCamera.tf.GetPosition()) > MAX_UPDATE_SQ_DIST)
				continue;*/

			float final_dt = ECS->GetComponent<EntityInfo>(entities[i]).HasFlag(EntityFlag::updateWhenPaused) ?
				static_cast<float>(dt) : static_cast<float>(dt) * EngineSpeed->GetEngineSpeedMultiplier();

			for (auto& script : ECS->GetComponent<GameLogic>(entities[i]).GetAllScripts()) 
			{
				if (script->GetState() == IScript::ScriptState::Initialized)
				{
					script->Update(final_dt);
				}
				else if (script->GetState() == IScript::ScriptState::Sleep)
				{
					if (script->SelfWakeCondition()) script->Wake();
				}
				else
				{
					if (script->GetState() == IScript::ScriptState::Awake)
					{
						script->Init();
						script->SetState(IScript::ScriptState::Initialized);
						script->Update(final_dt);
					}
					if (script->GetState() == IScript::ScriptState::NotAwake)
					{
						script->Awake();
						script->SetState(IScript::ScriptState::Awake);
					}
					
				}
			}
#ifdef BEHAVIOUR_MEASUREMENTS
			duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - t1);
			double time = time_span.count() * 1000;
			std::string name = ECS->GetComponent<EntityInfo>(id).GetName();
			ss << "Causing lag :" << id << " of amount" << time << " with name " << name;
			if (time > 2.0) std::cout << ss.str() << "\n";
			/*	Debug::Log(ss.str());*/
#endif
		}
	}

	//On load try to awake and initialize all behaviours
	void GameLogicSystem::Awake()
	{
		const auto& entities = ECS->GetTracker<GameLogicTracker>()->GetTrackedEntities();
		//Awake the Entities
		for (size_t j = 0; j < entities.size(); ++j)
		{
			auto& vectScripts = ECS->GetComponent<GameLogic>(entities[j]).GetAllScripts();
			for (size_t i = 0; i < vectScripts.size(); i++)
			{
				vectScripts[i]->AssignId(entities[j]);
				if (vectScripts[i]->GetState() == IScript::ScriptState::NotAwake)
				{
					vectScripts[i]->Awake();
					vectScripts[i]->SetState(IScript::ScriptState::Awake);
				}
			}
		}

		//Init the entities
		for (size_t j = 0; j < entities.size(); ++j)
		{
			auto& vectScripts = ECS->GetComponent<GameLogic>(entities[j]).GetAllScripts();
			for (size_t i = 0; i < vectScripts.size(); i++)
			{
				if (vectScripts[i]->GetState() == IScript::ScriptState::Awake)
				{
					vectScripts[i]->Init();
					vectScripts[i]->SetState(IScript::ScriptState::Initialized);
				}
			}
		}
		
		loaded = true;
	}

	void GameLogicSystem::Reload()
	{
		const auto& entities = ECS->GetTracker<GameLogicTracker>()->GetTrackedEntities();
		for (size_t j = 0; j < entities.size(); ++j)
		{
			for (auto& itr : ECS->GetComponent<GameLogic>(entities[j]).GetAllScripts()) {
				itr->Cleanup();
			}
		}
	}

	//Clear the values inside the behaviour systemt to prepare for new scene
	void GameLogicSystem::Shutdown()
	{
		const auto& entities = ECS->GetTracker<GameLogicTracker>()->GetTrackedEntities();

		for (size_t j = 0; j < entities.size(); ++j)
		{
			for (auto& itr : ECS->GetComponent<GameLogic>(entities[j]).GetAllScripts()) {
				itr->Cleanup();
			}
		}
		loaded = false;
	}

}