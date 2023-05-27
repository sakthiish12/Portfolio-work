#include "pch.h"
#include <Debugger/Debugger.h>
#include "../../Systems/ECS/ECS.h"
#include "ScriptManager.h"
#include "GameLogicSystem.h"

namespace BE
{
	//Add a new script by calling the manager
	void ScriptManager::AddScript(const std::string& type, ScriptPtr& sptr, EntityID id)
	{
		//Lookup inside script factory
		auto itr = ScriptKeyMap.find(type);
		if (itr == ScriptKeyMap.end())
		{
			std::stringstream ss;
			ss << "ADDING SCRIPT OF TYPE : " << type << " WHEN IT WAS NOT REGISTERED";
			AssertCore(false, ss.str().c_str());
		}
		auto itr2 = ScriptMakerMap.find(itr->second);
		if (itr2 == ScriptMakerMap.end())
		{
			AssertCore(false, "Script Maker could not be found with the given key.");
		}
		//Checks if system has already awaken and intialized other behaviours
		sptr = itr2->second();
		if (GLS->loaded)
		{
			sptr->AssignId(id);
		}
	}


	//Add a new script by calling the manager
	void ScriptManager::AddScript(const std::string& type, ScriptPtr& sptr, const std::pair<CanvasID, UIElementID>& uiID)
	{
		//Lookup inside script factory
		auto itr = ScriptKeyMap.find(type);
		if (itr == ScriptKeyMap.end())
		{
			std::stringstream ss;
			ss << "ADDING SCRIPT OF TYPE : " << type << " WHEN IT WAS NOT REGISTERED";
			AssertCore(false, ss.str().c_str());
		}
		auto itr2 = ScriptMakerMap.find(itr->second);
		if (itr2 == ScriptMakerMap.end())
		{
			AssertCore(false, "Script Maker could not be found with the given key.");
		}
		//Checks if system has already awaken and intialized other behaviours
		sptr = itr2->second();
		if (GLS->loaded)
		{
			sptr->AssignId(uiID.first, uiID.second);
		}
	}


	// get a vector of all the script names in that have been registered
	std::vector<std::string> ScriptManager::GetAllScriptNames()
	{
		std::vector<std::string> a;
		for (auto const& itr : ScriptKeyMap)
			a.emplace_back(itr.first);
		return a;
	}

	// get a script name with a script ID
	std::string ScriptManager::GetScriptName(ScriptID Key)
	{
		auto itr = std::find_if(ScriptKeyMap.begin(), ScriptKeyMap.end(), [=](auto const& a) {return a.second == Key; });
		if (itr == ScriptKeyMap.end()) {
			AssertCore(false, "Getting a script name that is not registered");
			return std::string();
		}
		return itr->first;
	}

	// get a script ID with a script name
	ScriptID ScriptManager::GetScriptID(std::string const& Name)
	{
		auto itr = ScriptKeyMap.find(Name);
		if (itr == ScriptKeyMap.end()) {
			AssertCore(false, "Getting a script ID that is not registered");
			return NULL_SCRIPT;
		}
		return itr->second;
	}

}