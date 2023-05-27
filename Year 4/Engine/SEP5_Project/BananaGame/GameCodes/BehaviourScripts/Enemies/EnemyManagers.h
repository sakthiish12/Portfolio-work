#pragma once
#ifndef	ENEMY_MANAGERS_H
#define ENEMY_MANAGERS_H

#include <BananaEngine.h>
#include "EnemyRobot.h"
#include "EnemyScientist.h"
#include "../Character/CharacterScript.h"


namespace TH
{
	using namespace BE;
	class BaseEnemyManager
	{
	protected:
		std::set<EntityID> mEntities;
		bool mShare_stats = false;
		int mEnemy_Count = 0;
		RTTR_REGISTRATION_FRIEND
	};

	using namespace BE;
	class ScientistManager : public BaseEnemyManager, public IScript
	{
		CharacterScript mShared_character;
		EnemyScientist mShared_scientist;
	public:
		void Init();

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	class RobotManager : public BaseEnemyManager, public IScript
	{
		CharacterScript mShared_character;
		EnemyRobot mShared_robot;
	public:
		void Init();
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};

	//class TurretManager : public BaseEnemyManager, public IScript
	//{
	//public:
	//	void Init();
	//	RTTR_ENABLE(IScript)
	//	RTTR_REGISTRATION_FRIEND
	//};

	template<typename T>
	inline void CopyStats(T const& toCopyTo, T const& toCopyFrom)
	{
		rttr::instance inst = toCopyTo;
		rttr::instance inst2 = toCopyFrom;
		for (auto& prop2 : inst2.get_type().get_properties())
		{
			auto prop1 = inst.get_type().get_property(prop2.get_name());
			prop1.set_value(inst, prop2.get_value(inst2));
		}
	};
}

#endif