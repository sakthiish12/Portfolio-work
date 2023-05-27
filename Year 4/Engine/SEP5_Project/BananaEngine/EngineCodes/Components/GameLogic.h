#pragma once
#ifndef	SCRIPT_H
#define SCRIPT_H

#include "ECSComponent.h"

namespace BE
{
	class IScript
	{
	public:
		enum class ScriptState
		{
			NotAwake,
			Awake,
			Initialized,
			Sleep
		};
		IScript()
			:state{}, id{ NULL_ENTITY } {};
		
		virtual void Awake()					{}
		virtual void Init()						{}
		virtual void Update(double dt)			{ UNUSED(dt); }
		virtual void Cleanup()					{}
		// calls by System when the behaviour instance is asleep, if returns true, wakes the behaviour itself
		virtual bool SelfWakeCondition()		{ return false;	}
		virtual void UponSleep()				{}
		virtual void UponWake()					{}

		virtual void OnCollision(BEID collidingID)			{}
		virtual void OnPersistedCollision(BEID collidingID) {}
		virtual void OnCollisionRemoved(BEID collidingID)	{}
		virtual void OnMouseTrigger()						{}

		//UI elements
		virtual void OnButtonPressed() {}
		virtual void OnSliderChanged(float _newPercentage) {}
		virtual void OnToggleEnabled() {}
		virtual void OnToggleDisabled() {}

		inline bool GetAwake()						{ return state == ScriptState::Initialized;	}
		inline ScriptState GetState()				{ return state;								}
		inline void SetState(ScriptState in)		{ state = in;								}
		inline BEID GetID()							{ return id;								}
		void Sleep();
		void Wake();
		RTTR_ENABLE()
	protected:
		ScriptState state;
		BEID id = NULL_ENTITY;

		//Canvas ID, UI Element ID
		std::pair<BEID, BEID> uiID = std::pair<BEID, BEID>(0, 0);

	private:
		void AssignId(BEID in) { if (id == NULL_ENTITY) id = in; }
		void AssignId(BEID cid, BEID uid) { if (uiID.first == BEID(0) && uiID.second == BEID(0)) uiID = std::make_pair(cid, uid); }

		friend class GameLogicSystem;
		friend class UIManager;
		friend class ScriptManager;
	};

	using ScriptPtr = std::shared_ptr<IScript>;
	using ScriptID = unsigned int;

	const ScriptID NULL_SCRIPT = 0;

	class GameLogic : public IECSComponent
	{
		//using ScriptMap = std::unordered_map<ScriptID, ScriptPtr>;
		using ScriptMap = std::vector<ScriptPtr>;
		using ScriptDataMap = std::unordered_map<std::string, std::shared_ptr<IScript>>;
	private:
		ScriptMap scripts;
		std::vector<ScriptID> IDs;
		EntityID eid = NULL_ENTITY;
		void SetScriptsByName(std::vector<std::string> in);
		ScriptDataMap GetScriptsMapWithData();
		void SetScriptsMapWithData(ScriptDataMap in);
	public:
		GameLogic() = default;
		~GameLogic() = default;
		GameLogic(GameLogic const& rhs) = default;
		GameLogic& operator=(GameLogic const& rhs);
		GameLogic(GameLogic&& rhs);
		GameLogic& operator=(GameLogic&& rhs);

		inline void SetID(EntityID in) { if (eid == NULL_ENTITY) eid = in; }
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
		template <typename Script> std::shared_ptr<Script> GetScriptDerived();
		template <typename Script> bool HasScript();
		template <typename Script> void RemoveScript();
		template <typename Script> void ReloadScript();
		RTTR_ENABLE(IECSComponent)
		RTTR_REGISTRATION_FRIEND
	private:
		size_t FindScript(const std::string& scriptname) const;
	};

	inline std::string RemoveNamespace(std::string const& name)
	{
		std::string a = name.substr(name.find_last_of(":") + 1);
		return a;
	}

	template <typename Script>
	void GameLogic::AddScript()
	{
		std::string name = typeid(Script).name();
		AddScript(RemoveNamespace(name));
	}

	template <typename Script>
	ScriptPtr GameLogic::GetScript()
	{
		std::string name = typeid(Script).name();
		return GetScript(RemoveNamespace(name));
	}

	template <typename Script>
	std::shared_ptr<Script> GameLogic::GetScriptDerived()
	{
		std::string name = typeid(Script).name();
		return std::dynamic_pointer_cast<Script>(GetScript(RemoveNamespace(name)));
	}

	template <typename Script>
	bool GameLogic::HasScript()
	{
		std::string name = typeid(Script).name();
		return HasScript(RemoveNamespace(name));
	}

	template <typename Script>
	void GameLogic::RemoveScript()
	{
		std::string name = typeid(Script).name();
		return RemoveScript(RemoveNamespace(name));
	}

	template <typename Script>
	void GameLogic::ReloadScript()
	{
		std::string name = typeid(Script).name();
		return ReloadScript(RemoveNamespace(name));
	}
	//GameLogic(GameLogic&& o) noexcept
	//{
	//	scripts.clear();
	//	for (auto& i : o.scripts) {
	//		scripts.insert(std::move(i));
	//	}
	//	o.scripts.clear();
	//}

	//GameLogic& operator=(GameLogic&& o) noexcept {
	//	scripts.clear();
	//	for (auto& i : o.scripts) {
	//		scripts.insert(std::move(i));
	//	}
	//	o.scripts.clear();
	//	return *this;
	//}
}
#endif