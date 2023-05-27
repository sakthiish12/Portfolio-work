#include "pch.h"
#include "UIScript.h"
#include <Systems/GameLogicSystem/ScriptManager.h>
#include <Serialisation/BananaSerialisation.h>

namespace BE
{
	UIScript& UIScript::operator=(UIScript const& rhs)
	{
		this->scripts = rhs.scripts;
		this->IDs = rhs.IDs;
		uiID = rhs.uiID;
		return *this;
	}

	UIScript::UIScript(UIScript&& rhs)
	{
		this->scripts = std::move(rhs.scripts);
		this->IDs = std::move(rhs.IDs);
		uiID = rhs.uiID;
		rhs.uiID = std::make_pair(BEID(0), BEID(0));
	}

	UIScript& UIScript::operator=(UIScript&& rhs)
	{
		this->scripts = std::move(rhs.scripts);
		this->IDs = std::move(rhs.IDs);
		uiID = rhs.uiID;
		rhs.uiID = std::make_pair(BEID(0), BEID(0));
		return *this;
	}

	std::vector<std::string> UIScript::GetNameOfAllScripts() const
	{
		std::vector<std::string> temp(IDs.size());
		for (size_t i = 0; i < temp.size(); i++)
		{
			temp[i] = ScriptAPI->GetScriptName(IDs[i]);
		}
		return temp;
	}

	void UIScript::SetScriptsByName(std::vector<std::string> in)
	{
		for (size_t i = 0; i < in.size(); i++)
		{
			AddScript(in[i]);
		}
	}

	UIScript::ScriptDataMap UIScript::GetScriptsMapWithData()
	{
		ScriptDataMap result;
		for (const auto& scriptName : GetNameOfAllScripts())
		{
			result[scriptName] = scripts[FindScript(scriptName)];
		}
		return result;
	}

	void UIScript::SetScriptsMapWithData(ScriptDataMap in)
	{
		for (const auto& scriptNamePtr : in)
		{
			CloneObject(rttr::instance(scriptNamePtr.second.get()),
				rttr::instance(scripts[FindScript(scriptNamePtr.first)]),
				rttr::type::get_by_name(scriptNamePtr.first));
		}
	}

	UIScript::ScriptMap const& UIScript::GetAllScripts() const
	{
		return scripts;
	}

	void UIScript::SleepAllScripts() const
	{
		for (auto& s: scripts){
			s->Sleep();
		}
	}

	void UIScript::WakeAllScripts() const
	{
		for (auto& s : scripts) {
			s->Wake();
		}
	}

	void UIScript::ReloadAllScripts()
	{
		std::vector<std::string> names;
		for (auto id : IDs)
			names.emplace_back(ScriptAPI->GetScriptName(id));
		for (auto const& name : names)
			ReloadScript(name);
	}

	size_t UIScript::FindScript(const std::string& scriptname) const
	{
		ScriptID id = ScriptAPI->GetScriptID(scriptname);
		auto itr = std::find(IDs.begin(), IDs.end(), id);
		if (itr == IDs.end()) return scripts.size();
		return std::distance(IDs.begin(), itr);
	}

	ScriptPtr UIScript::GetScript(const std::string& scriptname) const
	{
		auto i = FindScript(scriptname);
		if (i < scripts.size()) {
			return scripts[i];
		}
		//AssertCore(false,"Script to get is not found in the UIScript Component.");
		return nullptr;
	}

	bool UIScript::HasScript(const std::string& scriptname) const
	{
		ScriptID id = ScriptAPI->GetScriptID(scriptname);
		if (std::find(IDs.begin(), IDs.end(), id) != IDs.end()) {
			return true;
		}
		return false;
	}

	void UIScript::AddScript(const std::string& scriptname)
	{
		ScriptID id = ScriptAPI->GetScriptID(scriptname);
		if (HasScript(scriptname)) {
			std::stringstream ss;
			ss << "Adding the same script: " << scriptname << " twice into UIScript Component";
			AssertCore(false, ss.str().c_str());
		}
		ScriptPtr ptr;
		ScriptAPI->AddScript(scriptname, ptr, uiID);
		scripts.emplace_back(ptr);
		IDs.emplace_back(id);
	}

	void UIScript::RemoveScript(const std::string& scriptname)
	{
		auto i = FindScript(scriptname);
		if (i < scripts.size()) {
			scripts[i]->Cleanup();
			scripts.erase(std::next(scripts.begin(), i));
			IDs.erase(std::next(IDs.begin(), i));
		}
	}

	void UIScript::ReloadScript(const std::string& scriptname)
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
	registration::class_<UIScript>(RegistrationNameCreator<UIScript>())
		.constructor()
		.property("UIScripts", &UIScript::GetNameOfAllScripts, &UIScript::SetScriptsByName)
		(
			metadata(MetaDataKey::HiddenFromEditor, true)
		)
		.property("UIScripts Data", &UIScript::GetScriptsMapWithData, &UIScript::SetScriptsMapWithData)
		(
			metadata(MetaDataKey::HiddenFromEditor, true)
		);
}