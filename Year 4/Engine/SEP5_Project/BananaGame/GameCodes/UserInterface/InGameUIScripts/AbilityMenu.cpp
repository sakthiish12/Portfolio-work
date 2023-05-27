#include "AbilityMenu.h"

namespace TH
{

	void AbilityButton::Init() 
	{
		m_meleeButton	= BE::UI->FindUIElementByName("FistAbility", 1).front();
		m_laserButton	= BE::UI->FindUIElementByName("LaserAbility", 1).front();
		m_shieldButton	= BE::UI->FindUIElementByName("ShieldAbility", 1).front();
		m_bananaButton	= BE::UI->FindUIElementByName("BananaAbility", 1).front();
	}

	void AbilityButton::OnButtonPressed()
	{
		auto script_ptr = ECS->GetComponent<GameLogic>(GLS->GetPlayerID()).GetScript<PlayerCharacter>();
		PlayerCharacter* player_ptr = dynamic_cast<PlayerCharacter*>(script_ptr.get());
		if (player_ptr) 
		{
			//Get the current player's skill
			PlayerSkills lastSkill = player_ptr->GetCurrentPlayerSkill();
			player_ptr->SetCurrentPlayerSkill(selectAbility);

			//Disable the button for selected ability and enable button for the last skill
			switch (lastSkill)
			{
				case PlayerSkills::Banana:
				{
					BE::UI->GetCanvas(m_bananaButton.first)
						  ->GetUI<BE::UIButton>(m_bananaButton.second)
						  ->SetUIElementFlag(BE::UIElementFlags::isDisabled, false);
					break;
				}
				case PlayerSkills::Laser:
				{
					BE::UI->GetCanvas(m_laserButton.first)
						  ->GetUI<BE::UIButton>(m_laserButton.second)
						  ->SetUIElementFlag(BE::UIElementFlags::isDisabled, false);
					break;
				}
				case PlayerSkills::Melee:
				{
					BE::UI->GetCanvas(m_meleeButton.first)
						  ->GetUI<BE::UIButton>(m_meleeButton.second)
						  ->SetUIElementFlag(BE::UIElementFlags::isDisabled, false);
					break;
				}
				case PlayerSkills::Shield:
				{
					BE::UI->GetCanvas(m_shieldButton.first)
						  ->GetUI<BE::UIButton>(m_shieldButton.second)
						  ->SetUIElementFlag(BE::UIElementFlags::isDisabled, false);
					break;
				}
			}

			//Disable this button
			BE::UI->GetCanvas(uiID.first)->GetUI<BE::UIButton>(uiID.second)->SetUIElementFlag(BE::UIElementFlags::isDisabled, true);
		}
		
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<AbilityButton>(RegistrationNameCreator<AbilityButton>())
		.constructor()
		.property("SelectAbility", &AbilityButton::selectAbility);
}
