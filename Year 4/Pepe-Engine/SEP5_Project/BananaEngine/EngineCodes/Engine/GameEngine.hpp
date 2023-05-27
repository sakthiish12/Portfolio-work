#pragma once

namespace BE
{
	template<typename T_Sys, typename... CtorParam>
	void GameEngine::CreateSystem(CtorParam&& ...args)
	{
		static_assert(std::is_base_of<ISystem, T_Sys>::value, "T_Sys must inherit from ISystem");

		//Get an instance of the pointer
		typename Singleton<T_Sys>::SingletonPtr& ptr = T_Sys::CreateInstance(std::forward<CtorParam>(args)...);

		//Check if such instance already exists
		for (const auto& sys : m_systems)
		{
			if (ptr.get() == sys)
			{
				LogCoreError("Already created an instance of %s", typeid(T_Sys).name());
				return;
			}
		}

		//Successfully created the system
		m_systems.emplace_back(ptr.get());
		LogCore("Created engine system: %s", typeid(T_Sys).name());
	}
}