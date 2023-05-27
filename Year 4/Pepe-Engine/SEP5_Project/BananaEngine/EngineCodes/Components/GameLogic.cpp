#include "pch.h"
#include "GameLogic.h"
#include <Systems/GameLogicSystem/GameLogicSystem.h>
#include <Systems/GameLogicSystem/ScriptManager.h>
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	void IScript::Sleep()
	{
		if (state == ScriptState::NotAwake)
		{
			Awake();
			state = ScriptState::Awake;
		}
		if (state == ScriptState::Awake)
		{
			Init();
		}
		state = ScriptState::Sleep;
		UponSleep();
	}

	void IScript::Wake()
	{
		state = ScriptState::Initialized;
		UponWake();
	}

	GameLogic& GameLogic::operator=(GameLogic const& rhs)
	{
		this->scripts = rhs.scripts;
		this->IDs = rhs.IDs;
		eid = rhs.eid;
		return *this;
	}

	GameLogic::GameLogic(GameLogic&& rhs)
	{
		this->scripts = std::move(rhs.scripts);
		this->IDs = std::move(rhs.IDs);
		eid = rhs.eid;
		rhs.eid = NULL_ENTITY;
	}

	GameLogic& GameLogic::operator=(GameLogic&& rhs)
	{
		this->scripts = std::move(rhs.scripts);
		this->IDs = std::move(rhs.IDs);
		eid = rhs.eid;
		rhs.eid = NULL_ENTITY;
		return *this;
	}

	std::vector<std::string> GameLogic::GetNameOfAllScripts() const
	{
		std::vector<std::string> temp(IDs.size());
		for (size_t i = 0; i < temp.size(); i++)
		{
			temp[i] = ScriptAPI->GetScriptName(IDs[i]);
		}
		return temp;
	}

	void GameLogic::SetScriptsByName(std::vector<std::string> in)
	{
		for (size_t i = 0; i < in.size(); i++)
		{
			AddScript(in[i]);
		}
	}

	GameLogic::ScriptDataMap GameLogic::GetScriptsMapWithData()
	{
		ScriptDataMap result;
		for (const auto& scriptName : GetNameOfAllScripts())
		{
			result[scriptName] = scripts[FindScript(scriptName)];
		}
		return result;
	}

	void GameLogic::SetScriptsMapWithData(ScriptDataMap in)
	{
		for (const auto& scriptNamePtr : in)
		{
			CloneObject(rttr::instance(scriptNamePtr.second.get()),
						rttr::instance(scripts[FindScript(scriptNamePtr.first)]),
						rttr::type::get_by_name(scriptNamePtr.first));
		}
	}

	GameLogic::ScriptMap const& GameLogic::GetAllScripts() const
	{
		return scripts;
	}

	void GameLogic::SleepAllScripts() const
	{
		for (auto& s: scripts){
			s->Sleep();
		}
	}

	void GameLogic::WakeAllScripts() const
	{
		for (auto& s : scripts) {
			s->Wake();
		}
	}

	void GameLogic::ReloadAllScripts()
	{
		std::vector<std::string> names;
		for (auto id : IDs)
			names.emplace_back(ScriptAPI->GetScriptName(id));
		for (auto const& name : names)
			ReloadScript(name);
	}

	size_t GameLogic::FindScript(const std::string& scriptname) const
	{
		ScriptID id = ScriptAPI->GetScriptID(scriptname);
		auto itr = std::find(IDs.begin(), IDs.end(), id);
		if (itr == IDs.end()) return scripts.size();
		return std::distance(IDs.begin(), itr);
	}

	ScriptPtr GameLogic::GetScript(const std::string& scriptname) const
	{
		auto i = FindScript(scriptname);
		if (i < scripts.size()) {
			return scripts[i];
		}
		//AssertCore(false,"Script to get is not found in the GameLogic Component.");
		return nullptr;
	}

	bool GameLogic::HasScript(const std::string& scriptname) const
	{
		ScriptID id = ScriptAPI->GetScriptID(scriptname);
		if (std::find(IDs.begin(), IDs.end(), id) != IDs.end()) {
			return true;
		}
		return false;
	}

	void GameLogic::AddScript(const std::string& scriptname)
	{
		ScriptID id = ScriptAPI->GetScriptID(scriptname);
		if (HasScript(scriptname)) {
			std::stringstream ss;
			ss << "Adding the same script: " << scriptname << " twice into GameLogic Component";
			AssertCore(false, ss.str().c_str());
		}
		ScriptPtr ptr;
		/*if (eid == NULL_ENTITY) {

		}*/
		ScriptAPI->AddScript(scriptname, ptr, eid);
		scripts.emplace_back(ptr);
		IDs.emplace_back(id);
	}

	void GameLogic::RemoveScript(const std::string& scriptname)
	{
		auto i = FindScript(scriptname);
		if (i < scripts.size()) {
			scripts[i]->Cleanup();
			scripts.erase(std::next(scripts.begin(), i));
			IDs.erase(std::next(IDs.begin(), i));
		}
	}

	void GameLogic::ReloadScript(const std::string& scriptname)
	{
		RemoveScript(scriptname);
		AddScript(scriptname);
	}

}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<GameLogic>(RegistrationNameCreator<GameLogic>())
		.constructor()
		.property("Scripts", &GameLogic::GetNameOfAllScripts, &GameLogic::SetScriptsByName)
		(
			metadata(MetaDataKey::HiddenFromEditor, true)
		)
		.property("Scripts Data", &GameLogic::GetScriptsMapWithData, &GameLogic::SetScriptsMapWithData)
		(
			metadata(MetaDataKey::HiddenFromEditor, true)
		);
}