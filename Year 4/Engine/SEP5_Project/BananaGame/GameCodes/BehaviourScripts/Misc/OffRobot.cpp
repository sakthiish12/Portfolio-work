#include "OffRobot.h"
#include "../Character/CharacterScript.h"
namespace TH
{
	const float deathTimer = 1.5f;
	const float endFallTimer = 1.0f;
	using namespace BE;
	void TH::OffRobot::Init()
	{
		CharacterScript& cs = dynamic_cast<CharacterScript&>(*ECS->GetComponent<GameLogic>(id).GetScript< CharacterScript>());
		model = ECS->GetComponent<Transform>(id).GetChildren().front();
		timer = 0.0f;
		cs.SetModelID(model);
		cs.SetDestroyDuration(deathTimer);
	}
	void OffRobot::Update(double dt)
	{
		if (dynamic_cast<CharacterScript&>(*ECS->GetComponent<GameLogic>(id).GetScript< CharacterScript>()).IsDead())
		{
			timer += static_cast<float>(dt);
			float interlop = std::min(timer / (endFallTimer), 1.0f);
			static const auto eulerRotation = ECS->GetComponent<Transform>(model).GetEulerRotation();
			if (interlop < 1.0f)
			{
				ECS->GetComponent<Transform>(model).SetPosition(ECS->GetComponent<Transform>(model).GetPosition() - Vec3f::Up() * static_cast<float>(dt));
				ECS->GetComponent<Transform>(model).SetEulerRotation(Vec3f(eulerRotation.x - interlop * 90.0f, eulerRotation.y, eulerRotation.z));
			}
		}
	}
}

