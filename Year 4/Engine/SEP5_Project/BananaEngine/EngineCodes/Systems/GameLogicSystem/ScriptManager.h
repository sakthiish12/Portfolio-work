#ifndef SCRIPT_MANAGER_HPP
#define SCRIPT_MANAGER_HPP

#include "../../Components/GameLogic.h"
#include "../UIManager/UIManagerConfig.h"

namespace BE
{
	template <typename Script> ScriptPtr MakeScript();

	using ScriptMaker = ScriptPtr(*)();

	class ScriptManager : public Singleton<ScriptManager>
	{
		std::unordered_map<ScriptID, ScriptMaker> ScriptMakerMap;
		std::unordered_map<std::string, ScriptID> ScriptKeyMap;
		ScriptID IDs = 1;

	public:
		//ECS Entity
		void AddScript(const std::string& type, ScriptPtr& in, EntityID id);
		
		//UI Elements
		void AddScript(const std::string& type, ScriptPtr& in, const std::pair<CanvasID, UIElementID>& uiID);

		std::vector<std::string> GetAllScriptNames();
		std::string GetScriptName(ScriptID Key);
		ScriptID GetScriptID(std::string const& Name);

		template <typename Script> void RegisterScript();
		template <typename Script> void AddScript(ScriptPtr& in, EntityID id);
		template <typename Script> ScriptID GetScriptID();
	};

#define ScriptAPI ScriptManager::GetInstance()

	//templated function to create an instance of a behaviour once registered
	template <typename Script>
	ScriptPtr MakeScript()
	{
		return std::make_shared<Script>();
	}

	//Registers script to the script factory to create instances
	template<typename Script>
	inline void ScriptManager::RegisterScript()
	{
		std::string name = RemoveNamespace(std::string(typeid(Script).name()));
		auto itr = ScriptKeyMap.find(name);
		if (itr != ScriptKeyMap.end())
		{
			std::stringstream ss;
			ss << "Double Registration of " << name;
			AssertCore(false, ss.str().c_str());
		}
		ScriptMakerMap.insert({ IDs, MakeScript<Script> });
		ScriptKeyMap.insert({ name, IDs++ });
	}

	//Calls the non templated add script to add a script to a GameLogic component
	template<typename Script>
	inline void ScriptManager::AddScript(ScriptPtr& in, EntityID id)
	{
		std::string name = typeid(Script).name();
		return AddScript(RemoveNamespace(name), in, id);
	}

	template<typename Script>
	inline ScriptID ScriptManager::GetScriptID()
	{
		std::string name = typeid(Script).name();
		return GetScriptID(RemoveNamespace(name));
	}
}

#endif