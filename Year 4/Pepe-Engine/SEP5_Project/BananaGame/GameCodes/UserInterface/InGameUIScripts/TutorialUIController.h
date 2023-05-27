#pragma once
#include <BananaEngine.h>

namespace TH
{
	class TutorialUIController : public BE::IScript
	{
		
		//Reference to the whole canvas
		BE::CanvasID m_tutorialCanvas;

		//Reference to each of the tutorial UI text
		std::vector<std::pair<BE::CanvasID, BE::UIElementID>> m_tutorialUIText;

		//Reference to each of the tutorial UI Toggle
		std::vector<std::pair<BE::CanvasID, BE::UIElementID>> m_tutorialUICheckbox;

		//Number of tutorials finished
		int completedCount = 0;

	public:
		static TutorialUIController* inst;
		enum class TutorialIndex
		{
			MovementCheckbox = 0,
			JumpCheckBox = 1,
			CrateInteractCheckbox = 2,
			MeleeAttackCheckbox = 3,
			ChangeWeaponCheckBox = 4,
			RangeAttackCheckBox = 5
		};
		void Awake() override;
		void Init() override;
		void Update(double dt) override;
		void SetTutorialComplete(size_t _index, bool _b);
		inline void SetTutorialComplete(TutorialIndex in)		{ SetTutorialComplete(static_cast<size_t>(in), true); }
		void SetTutorialCanvasEnabled(bool _b) const;
		inline int GetTotalCompletedCount() const			{ return completedCount; }
		inline bool GetAllTutorialsCompleted() const		{ return completedCount == (int)m_tutorialUICheckbox.size(); }
	};
}