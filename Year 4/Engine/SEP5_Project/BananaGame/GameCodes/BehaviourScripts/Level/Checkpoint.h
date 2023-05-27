#pragma once
#include <BananaEngine.h>

namespace TH
{
	//Attached to the game manager to handle all the checkpoint and respawning of enemies
	class CheckpointManager : public BE::IScript
	{
		//All the enemy entities
		std::vector<BE::EntityID> m_enemyEntities{};

		//Last checkpoint triggered
		BE::EntityID m_lastCheckpointTriggered = BE::NULL_ENTITY;

		//Gameover UI reference - to disable when restarting
		BE::CanvasID m_gameoverCanvas;

		//Reference to baby monkey
		BE::EntityID baby_monkey;

		//Reference to boss
		BE::EntityID boss;

	public:

		void Awake() override;

		//For setting checkpoint
		void SetCheckpoint(BE::EntityID _cp);

		//For restoring back to last checkpoint
		void RestoreCheckpoint();

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


	//Attached to the checkpoint panel
	class Checkpoint : public BE::IScript
	{
		//Reference to the game manager
		BE::EntityID m_gameMgr = BE::NULL_ENTITY;

		bool m_isUsed = false;

	public:
		void Awake() override;
		void Init() override;

		void Update(double dt) override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}