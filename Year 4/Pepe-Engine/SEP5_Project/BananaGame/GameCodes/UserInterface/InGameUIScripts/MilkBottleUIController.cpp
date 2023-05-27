#include "MilkBottleUIController.h"

namespace TH
{
	void MilkBottleUIController::Awake()
	{
		//Find the milk bottle UI font text
		m_milkBottleUI = BE::UI->FindUIElementBySubstr("MilkBottleCountText", 1).front();

		
	}

	void MilkBottleUIController::Init()
	{
		//Reset milk bottle count
		m_milkBottleCountLeft = 0;
	}

	void MilkBottleUIController::AddMilkBottleCount()
	{
		//If already at max, then don't do anything
		if (m_milkBottleCountLeft == m_maxMilkBottleCount)
			return;

		++m_milkBottleCountLeft;

		//Update the value
		BE::UI->GetCanvas(m_milkBottleUI.first)
			  ->GetUI<BE::UIFontText>(m_milkBottleUI.second)
			  ->SetText(std::to_string(m_milkBottleCountLeft));

	}

	void MilkBottleUIController::SubtractMilkBottleCount()
	{
		//If already at 0, then don't do anything
		if (m_milkBottleCountLeft == 0)
			return;

		--m_milkBottleCountLeft;

		//Update the value
		BE::UI->GetCanvas(m_milkBottleUI.first)
			  ->GetUI<BE::UIFontText>(m_milkBottleUI.second)
			  ->SetText(std::to_string(m_milkBottleCountLeft));

	}

	void MilkBottleUIController::SetMilkBottleCount(int _val)
	{
		//If no change in value, then don't do anything
		if (m_milkBottleCountLeft == _val)
			return;

		//clamp this value to be between 0 and size of the vector
		_val = BEMath::Clamp<int>(0, m_maxMilkBottleCount, _val);

		//If new value is higher than the current milk bottle count left, then we add milk bottle count
		if (_val > m_milkBottleCountLeft)
		{
			for (int i = m_milkBottleCountLeft; i < _val; ++i)
				AddMilkBottleCount();
		}
		//Otherwise we subtract milk bottle count
		else
		{
			for (int i = m_milkBottleCountLeft; i > _val; --i)
				SubtractMilkBottleCount();
		}
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;

	registration::class_<MilkBottleUIController>(RegistrationNameCreator<MilkBottleUIController>())
		.constructor();
}