
#include "EnemyManagers.h"

namespace TH
{
	using namespace BE;

	void ScientistManager::Init() 
	{
		//Find all the enemies of given type and assign the count number
		auto all_scientist = ECS->FindEntitiesBysubstr("Enemy_Scientist");
		mEnemy_Count = static_cast<int>(all_scientist.size());
		std::move(all_scientist.begin(), all_scientist.end(), std::inserter(mEntities, mEntities.begin()));

		if (mShare_stats)
		{
			for (auto eid : mEntities)
			{
				auto scientist_script = ECS->GetComponent<GameLogic>(eid).GetScript<EnemyScientist>();
				EnemyScientist* scientist_ptr = dynamic_cast<EnemyScientist*>(scientist_script.get());
				if (scientist_ptr) {
					CopyStats<EnemyScientist>(*scientist_ptr, mShared_scientist);
				}

				auto character_script = ECS->GetComponent<GameLogic>(eid).GetScript<CharacterScript>();
				CharacterScript* character_ptr = dynamic_cast<CharacterScript*>(character_script.get());
				if (character_ptr) {
					CopyStats<CharacterScript>(*character_ptr, mShared_character);
				}
			}
		}
	}

	void RobotManager::Init() 
	{
		//Find all the enemies of given type and assign the count number
		auto all_robot = ECS->FindEntitiesBysubstr("Enemy_Robot");
		mEnemy_Count = static_cast<int>(all_robot.size());
		std::move(all_robot.begin(), all_robot.end(), std::inserter(mEntities, mEntities.begin()));

		if (mShare_stats)
		{
			for (auto eid : mEntities)
			{
				auto robot_script = ECS->GetComponent<GameLogic>(eid).GetScript<EnemyRobot>();
				EnemyRobot* robot_ptr = dynamic_cast<EnemyRobot*>(robot_script.get());
				if (robot_ptr) {
					CopyStats<EnemyRobot>(*robot_ptr, mShared_robot);
				}

				auto character_script = ECS->GetComponent<GameLogic>(eid).GetScript<CharacterScript>();
				CharacterScript* character_ptr = dynamic_cast<CharacterScript*>(character_script.get());
				if (character_ptr) {
					CopyStats<CharacterScript>(*character_ptr, mShared_character);
				}
			}
		}
	}

	//void TurretManager::Init()
	//{

	//}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<ScientistManager>(RegistrationNameCreator<ScientistManager>())
		.constructor()
		.property("Use Common Stats", &ScientistManager::mShare_stats)
		.property("Shared Character", &ScientistManager::mShared_character)
		.property("Shared Scientist", &ScientistManager::mShared_scientist);
	registration::class_<RobotManager>(RegistrationNameCreator<RobotManager>())
		.constructor()
		.property("Use Common Stats", &BaseEnemyManager::mShare_stats)
		.property("Shared Character", &RobotManager::mShared_character)
		.property("Shared Robot", &RobotManager::mShared_robot);
}