#include <BananaEngine.h>
#include "../../BehaviourScripts/Player/PlayerScript.h"
namespace TH
{

	class AbilityButton : public BE::IScript
	{
		PlayerSkills selectAbility;

		//The 4 ability buttons
		std::pair<BE::CanvasID, BE::UIElementID> m_meleeButton;
		std::pair<BE::CanvasID, BE::UIElementID> m_laserButton;
		std::pair<BE::CanvasID, BE::UIElementID> m_shieldButton;
		std::pair<BE::CanvasID, BE::UIElementID> m_bananaButton;

		void Init() override;
		void OnButtonPressed() override;

		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
	};


}
