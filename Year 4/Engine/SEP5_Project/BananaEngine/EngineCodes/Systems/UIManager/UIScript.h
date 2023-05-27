#pragma once
#ifndef	UI_SCRIPT_H
#define UI_SCRIPT_H

#include "../../Components/GameLogic.h"	//For IScript class
#include "UIManagerConfig.h"

namespace BE
{
	class UIScript
	{
		using ScriptMap = std::vector<ScriptPtr>;
		using ScriptDataMap = std::unordered_map<std::string, std::shared_ptr<IScript>>;

	private:
		ScriptMap scripts;
		std::vector<ScriptID> IDs;

		//Canvas and UI Element
		std::pair<CanvasID, UIElementID> uiID = std::pair<CanvasID, UIElementID>(0, 0);


		void SetScriptsByName(std::vector<std::string> in);
		ScriptDataMap GetScriptsMapWithData();
		void SetScriptsMapWithData(ScriptDataMap in);
	public:
		UIScript() = default;
		~UIScript() = default;
		UIScript(UIScript const& rhs) = default;
		UIScript& operator=(UIScript const& rhs);
		UIScript(UIScript&& rhs);
		UIScript& operator=(UIScript&& rhs);

		void SetID(CanvasID cid, UIElementID uid) { if (uiID.first == BEID(0) && uiID.second == BEID(0)) uiID = std::make_pair(cid, uid); }
		std::vector<std::string> GetNameOfAllScripts() const;
		ScriptMap const& GetAllScripts() const;
		void SleepAllScripts() const;
		void WakeAllScripts() const;
		void ReloadAllScripts();
		ScriptPtr GetScript(const std::string& scriptname) const;
		bool HasScript(const std::string& scriptname) const;
		void AddScript(const std::string& scriptname);
		void RemoveScript(const std::string& scriptname);
		void ReloadScript(const std::string& scriptname);

		template <typename Script> void AddScript();
		template <typename Script> ScriptPtr GetScript();
		template <typename Script> bool HasScript();
		template <typename Script> void RemoveScript();
		template <typename Script> void ReloadScript();
		RTTR_REGISTRATION_FRIEND
	private:
		size_t FindScript(const std::string& scriptname) const;
	};

	template <typename Script>
	void UIScript::AddScript()
	{
		std::string name = typeid(Script).name();
		AddScript(RemoveNamespace(name));
	}

	template <typename Script>
	ScriptPtr UIScript::GetScript()
	{
		std::string name = typeid(Script).name();
		return GetScript(RemoveNamespace(name));
	}

	template <typename Script>
	bool UIScript::HasScript()
	{
		std::string name = typeid(Script).name();
		return HasScript(RemoveNamespace(name));
	}

	template <typename Script>
	void UIScript::RemoveScript()
	{
		std::string name = typeid(Script).name();
		return RemoveScript(RemoveNamespace(name));
	}

	template <typename Script>
	void UIScript::ReloadScript()
	{
		std::string name = typeid(Script).name();
		return ReloadScript(RemoveNamespace(name));
	}

}
#endif