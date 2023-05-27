#pragma once

namespace BE
{
	template <typename T>
	typename Singleton<T>::SingletonPtr Singleton<T>::m_instance = nullptr;

	template <typename T>
	Singleton<T>::Singleton()
	{
		m_instance = nullptr;
	}

	template <typename T>
	Singleton<T>::~Singleton()
	{
		
	}

	template <typename T>
	template <typename... Args>
	typename Singleton<T>::SingletonPtr& Singleton<T>::CreateInstance(Args&& ...args)
	{
		//If no instance of T exists, create 1
		if (!m_instance)
			m_instance = { new T{ std::forward<Args>(args)...} , 
						   [](T* t) { delete t; } };

		return m_instance;
	}

	template <typename T>
	void Singleton<T>::DeleteInstance()
	{
		//Release the pointer if an instance exists
		if (m_instance)
			m_instance.reset();
	}

	template <typename T>
	bool Singleton<T>::IsNull() 
	{
		return m_instance == nullptr;
	}
}