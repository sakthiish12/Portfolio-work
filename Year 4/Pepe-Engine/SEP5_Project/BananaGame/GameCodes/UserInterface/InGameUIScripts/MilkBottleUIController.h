#pragma once
#include <BananaEngine.h>

namespace TH
{
	class MilkBottleUIController : public BE::IScript
	{
		//The font text for the milk bottle UI
		std::pair<BE::CanvasID, BE::UIElementID> m_milkBottleUI;

		//Number of milk bottles that are "active"
		const int m_maxMilkBottleCount = 4;
		int m_milkBottleCountLeft = 0;

	public:
		void Awake() override;
		void Init() override;

		void AddMilkBottleCount();
		void SubtractMilkBottleCount();
		void SetMilkBottleCount(int _val);

		//Returns the number of milk bottle left to use
		inline int GetMilkBottleCount() const { return m_milkBottleCountLeft; }

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};
}
