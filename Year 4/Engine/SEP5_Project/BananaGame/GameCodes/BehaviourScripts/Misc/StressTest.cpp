#include "StressTest.h"
namespace TH
{
	using namespace BE;
	void TH::StressTest::Awake()
	{
	}
	constexpr float speed = 3.0f;
	void TH::StressTest::Init()
	{
		const AssetID& boxAsset = AssetManager::GetInstance()->GetAssetID("../Assets/Models/Props/Box1.fbx");
		const AssetID& texture1 = AssetManager::GetInstance()->GetAssetID("../Assets/Textures/box/albedo.dds");
		const AssetID& texture2 = AssetManager::GetInstance()->GetAssetID("../Assets/Textures/Props/VendingMachine/VMBase.dds");
		entities.reserve(100);
		for (size_t i = 0; i < 100; i++)
		{
			BEID newEntity = ECS->CreateEntity();
			ECS->AddComponent<Renderer>(newEntity);
			std::stringstream ss;
			ss << "StressEntity " << i;
			ECS->GetComponent<EntityInfo>(newEntity).SetName(ss.str());
			ECS->GetComponent<Transform>(newEntity).SetPosition(Vec3f(	BE::Rand::Random<float>(-20.0f,20.0f),
																		BE::Rand::Random<float>(4.0f, 8.0f),
																		BE::Rand::Random<float>(-20.0f, 20.0f)));
			Renderer& rnd = ECS->GetComponent<Renderer>(newEntity);
			rnd.setModelID(boxAsset);
			if(BE::Rand::Random<int>(0,1))
				rnd.setAlbedoAlphaMap(texture1, 0);
			else
				rnd.setAlbedoAlphaMap(texture2, 0);
			ECS->AddComponent<Physics>(newEntity);
			Physics& phy = ECS->GetComponent<Physics>(newEntity);
			phy.SetShape(Shape::Sphere);
			phy.SetMotionType(JPH::EMotionType::Dynamic);
			phy.SetGravityFactor(1.0f);
			phy.SetShapeSize(Vec3f(1.0f, 1.0f, 1.0f));
			phy.SetRestitution(100.0f);
			//phy.AddAngularImpulse(Vec3f(0.0f, BE::Rand::Random<float>(10.0f, 20.0f),0.0f));
			std::pair<BEID, bool> temp{ newEntity,BE::Rand::Random<int>(0,1) % 2 };
			entities.emplace_back(std::move(temp));
		}
	}

	void TH::StressTest::UpdateTimers(float dt)
	{
	}

	void TH::StressTest::Update(double dt)
	{
		//float dtf = static_cast<float>(dt);
		//for (auto& cid:entities)
		//{
		//	Transform& tf = ECS->GetComponent<Transform>(cid.first);
		//	if (cid.second)
		//	{
		//		if (tf.GetPositon().y < 2.0f)
		//			cid.second = !cid.second;
		//		else
		//			tf.SetPosition(tf.GetPositon() + speed * dtf * Vec3f::Down());
		//	}
		//	else
		//	{
		//		if (tf.GetPositon().y > 6.0f)
		//			cid.second = !cid.second;
		//		else
		//			tf.SetPosition(tf.GetPositon() + speed * dtf * Vec3f::Up());
		//	}
		//}
	}

	void TH::StressTest::Cleanup()
	{
	}

	void TH::StressTest::UponSleep()
	{
	}

	void TH::StressTest::UponWake()
	{
	}

	void TH::StressTest::OnCollision(EntityID collidingID)
	{
	}

	void TH::StressTest::OnPersistedCollision(EntityID collidingID)
	{
	}

	void TH::StressTest::OnCollisionRemoved(EntityID collidingID)
	{
	}

}
