#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"


#include <iostream>
#include "../../Engine/ECS/Systems/Logic.h"
#include "BossMushroomScript.h"
#include "../../InputMgr/InputMgr.h"
#include "../../Engine/ECS/Systems/Physics/CollisionSystem.h"

#pragma warning(disable: 4715)

using std::cout;
using std::endl;

BossMushroomScript::BossMushroomScript(const char* scriptName) :
	scriptBase(scriptName)
{
}

void BossMushroomScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
}

/*
Mushroom Boss behavior:

 - Boss has shield whenever there’s small mushrooms walking around .whenever small mushrooms are all dead. Shield is deactivated 
 - Boss has slow movement speed
 - Deals damage on impact with player
 - drops then shield ability after it’s defeated 


*/

void BossMushroomScript::update(float dt)
{
	updatePlayerID();
	if (m_BossState == BossState::INACTIVE)
	{
		if (checkPlayerRadiusToBoss())
		{
			m_BossState = BossState::ACTIVE;
		}
	}
	else if (m_BossState == BossState::ACTIVE)
	{
		//if there exist collision between any entity with boss
		std::vector<entityID> collidedEntities = collisionSystemsPtr->getEntityCollidedWith(id);
		for (entityID& eID_collided : collidedEntities)
		{
			//Ensures player
			if (ecs.entityHas<Com_type>(eID_collided) && ecs.getComponent<Com_type>(eID_collided).type == Com_type::type::player)
			{
				//Com_type& entityType{ ecs.getComponent<Com_type>(eID_collided) };

				//damage player
				//NOTE: might need to do collide cool down to avoid -1 per frame/update
				if (ecs.entityHas<Com_Health>(eID_collided))
				{
					Com_Health& entityHealth{ ecs.getComponent<Com_Health>(eID_collided) };
					entityHealth.health -= 1;
				}
			}
			else if (ecs.entityHas<Com_type>(eID_collided) && ecs.getComponent<Com_type>(eID_collided).type == Com_type::type::bullet)
			{
				//need to check when getting sucked in

				//check if boss got shield
				if (m_is_shield_activated == true)
				{
					//no damage
				}
				else
				{
					if (ecs.entityHas<Com_Health>(id))
					{
						Com_Health& BossHealth = ecs.getComponent<Com_Health>(id);

						//take damage
						//may need cooldown
						if (BossHealth.health > 1)
						{
							--(BossHealth.health);
						}
						//else if (m_current_wave == 3 && m_is_current_wave_cleared == true)
						//{
						//	//if dying, DO NOT DIE, until wave 3 cleared

						//	//need a way to identify being sucked
						//	//then m_BossState = BossState::DEAD;
						//}
					}
					
				}
			}
			else if (true)
			{
				//how to do the boss getting sucked?
			}
		}

		//handle wave logic
		switch (m_current_wave)
		{
		case 0:
			summonMushroomMinion(minionToSummonPerWave);
			
			break;
		case 1:
			//fall-through coz similar logic
		case 2:
			//might need to do via the editor path finding and not here, see how first
			if (m_is_current_wave_cleared == false)
			{
				chasePlayer(5.f, dt);
				checkIfMinionIsCleared();
			}
			else
			{
				//m_is_current_wave_cleared is true				
				chasePlayer(15.f, dt);

				m_boss_nextsummon_timer += dt;
				if (canBossSummon())
				{
					summonMushroomMinion(minionToSummonPerWave);
					m_boss_nextsummon_timer = 0.f;
				}
			}
			break;
		case 3:
			//might need to do via the editor path finding and not here, see how first
			if (m_is_current_wave_cleared == false)
			{
				chasePlayer(5.f, dt);
				checkIfMinionIsCleared();
			}
			else
			{
				//m_is_current_wave_cleared is true				
				chasePlayer(15.f, dt);

				//DO NOT SUMMON ANYMORE
				/*m_boss_nextsummon_timer += dt;
				if (canBossSummon())
				{
					summonMushroomMinion(minionToSummonPerWave);
					m_boss_nextsummon_timer = 0.f;
				}*/
			}

			
			break;
		default:
			//do nothing
			break;
		}
	}
	else if (m_BossState == BossState::DEAD)
	{
		//do nothing
		return;
	}
}

void BossMushroomScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> BossMushroomScript::createCopy()
{
	return std::make_shared <BossMushroomScript>();
}

std::string BossMushroomScript::getScriptName()
{
	return name;
}

int BossMushroomScript::updatePlayerID()
{
	if (playerComTypeexist)
	{
		if (ecs.entityHas<Com_type>(playerID) && ecs.getComponent<Com_type>(playerID).type == Com_type::type::player)
		{
			return playerID;
		}
		else
		{
			playerID = -1;playerComTypeexist = false;
		}
			
	}
	if (!playerComTypeexist)
	{
		//(attempt to) Get the player entity
		for (auto const& e : ecs.getEntityNames())
		{
			if (ecs.entityHas<Com_type>(e.first) && ecs.getComponent<Com_type>(e.first).type == Com_type::type::player)
			{
				playerID = e.first;
				playerComTypeexist = true;
				return playerID;
			}
		}

		return playerID;
	}
}

bool BossMushroomScript::checkPlayerRadiusToBoss()
{
	if (playerComTypeexist && ecs.entityHas<transform2D>(playerID))
	{
		auto& player_transform = ecs.getComponent<transform2D>(playerID);
		auto& boss_transform = ecs.getComponent<transform2D>(id);
		float x_diff = player_transform.pos.x - boss_transform.pos.x;
		float y_diff = player_transform.pos.y - boss_transform.pos.y;

		//absolute distance
		if (x_diff < 0)
			x_diff *= -1.f;
		if (y_diff < 0)
			y_diff *= -1.f;
		
		if (x_diff < x_diff_activate_trigger && y_diff < y_diff_activate_trigger)
		{
			return true;
		}

	}
	return false;
}

void BossMushroomScript::summonMushroomMinion(int numToSummon)
{
	//Spawn mushroom minion
	for (int i = 0; i < numToSummon; ++i)
	{
		//entityID newID = ecs.createEntityFromFile("Resources/PresetEntities/fireball.json");
		
	}
	
	activeMinion = numToSummon;
	++m_current_wave;
	m_is_current_wave_cleared = false;
	m_is_shield_activated = true;
}

bool BossMushroomScript::checkIfMinionIsCleared()
{
	if (activeMinion <= 0)
	{
		m_is_current_wave_cleared = true;
		m_is_shield_activated = false;
		return true;
	}
	return false;
}

bool BossMushroomScript::canBossSummon()
{
	if (m_boss_nextsummon_timer > boss_inactive_period)
	{
		
		return true;
	}
	return false;
}

void BossMushroomScript::chasePlayer(float bossSpeed, float dt)
{
	auto& playerTransform = ecs.getComponent<transform2D>(playerID);
	auto& enemyTransform = ecs.getComponent<transform2D>(id);

	////CHASE PLAYER
	Mathf::vec2 enemyToPlayer{ playerTransform.pos - enemyTransform.pos };

	Force newForce(Force::ForceType::LINEAR_FORCE, Mathf::Normalize(enemyToPlayer) * bossSpeed, dt);
	
	rigidBody2D* enemyRB = nullptr;
	if (ecs.entityHas<rigidBody2D>(id))
	{
		enemyRB = &ecs.getComponent<rigidBody2D>(id);
	}
	enemyRB->forceManager.setMaxVel(1000.0f);
	enemyRB->forceManager.setLinearDrag(0.08f);
	enemyRB->forceManager.AddLinearForce("movement", newForce);

	////Animation
	//enum class facing { up, right, left, down };
	//facing facingDirection{ facing::up };
	//if (enemyToPlayer.y > 0.0f)
	//{
	//	if (abs(enemyToPlayer.y) > abs(enemyToPlayer.x))
	//		facingDirection = facing::up;
	//	else if (enemyToPlayer.x > 0.0f)
	//		facingDirection = facing::right;
	//	else
	//		facingDirection = facing::left;
	//}
	//else
	//{
	//	if (abs(enemyToPlayer.y) > abs(enemyToPlayer.y))
	//		facingDirection = facing::down;
	//	else if (enemyToPlayer.x > 0.0f)
	//		facingDirection = facing::right;
	//	else
	//		facingDirection = facing::left;
	//}

	////Sprite to display based off facing
	//switch (facingDirection)
	//{
	//case (facing::up):
	//	enemySprite.currentTexCoords = enemySprite.row1[enemySprite.currentFrame];
	//	break;
	//case (facing::left):
	//	enemySprite.currentTexCoords = enemySprite.row2[enemySprite.currentFrame];
	//	break;
	//case (facing::right):
	//	enemySprite.currentTexCoords = enemySprite.row3[enemySprite.currentFrame];
	//	break;
	//case (facing::down):
	//	enemySprite.currentTexCoords = enemySprite.row4[enemySprite.currentFrame];
	//	break;
	//}

	////Animate
	//frameTime -= dt * Mathf::Length(enemyRB.vel);
	//if (frameTime <= 0.0f)
	//{
	//	enemySprite.currentFrame = (enemySprite.currentFrame + 1) % 4;
	//	frameTime = 8.0f;
	//}

}


