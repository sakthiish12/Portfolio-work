#include "CutsceneController.h"
#include "../UserInterface/InGameUIScripts/InGameUIScripts.h"
#include "../MusicPlayer/MusicPlayer.h"

namespace TH
{
	void CutsceneController::Awake()
	{
		BGMPlayer = BE::ECS->FindEntitiesByName("BGMPlayer", 1).front();
	}


	void CutsceneController::Init()
	{
#ifdef USE_DEBUGGER
		//Assert that the cutscene canvas and the vector of fade in times have the
		//same size
		Assert(m_cutsceneSets.size() == m_cutscenePanelFadeInDelay.size(),
			"m_cutsceneCanvases must be equal to m_cutscenePanelFadeInDelay");

		//Assert that the UI canvas exist
		for (size_t i = 0; i < m_cutsceneSets.size(); ++i)
		{
			//Assert the UI exists
			Assert(BE::UI->DoesUICanvasExist(m_cutsceneSets[i]),
				"UI Canvas %llu does not exist", m_cutsceneSets[i]);
		}

		//Assert that the size of m_cutscenePanelFadeInDelay matches the number of
		//ui elements in the canvas
		for (size_t i = 0; i < m_cutsceneSets.size(); ++i)
		{
			size_t uiElementCount = BE::UI->GetCanvas(m_cutsceneSets[i])->GetUIElementCount();
			Assert(uiElementCount == m_cutscenePanelFadeInDelay[i].size() + 1,
				"uiElementCount must be equal to m_cutscenePanelFadeInDelay[i].size()");
		}
#endif
		m_timeElapsed = 0;
		m_cutsceneBGUI = BE::UI->FindUIElementByName("CutsceneBG", 1).front();
		m_cutscenePressSpaceUI = BE::UI->FindUICanvasesByName("CutSceneContinueUI", 1).front();
	}

	void CutsceneController::Update(double dt)
	{
		//If no cutscene running then don't do anything
		if (!HasCutsceneRunning())
			return;

		//If there still is panels to show
		if (HasPanelsRemaining())
		{
			if (!BE::Input->IsMousePressed(BE::Mouse::MOUSE_LEFT))
				m_timeElapsed += (float)dt;
			else
				m_timeElapsed += (float)dt * m_fastScrollMultiplier;
			

			//If time elapsed exceeded the fading time
			if (m_timeElapsed > m_cutscenePanelFadeInDelay[m_cutsceneSet][m_currentPanel])
			{
				m_timeElapsed -= m_cutscenePanelFadeInDelay[m_cutsceneSet][m_currentPanel];
				++m_currentPanel;
			}	
		}
		
		//Slowly increase the opacity of the current panel
		auto panel = BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->GetUI<BE::UIImage>(m_cutscenePanels[m_currentPanel]);

		//Slowly increase opacity of the panel
		panel->setOpacity(BEMath::Min(panel->getOpacity() + (1.0f / fadeTime) * (float)dt, 1.0f));


		//If the last panel's draw position is not below half way on the screen, 
		//then keep moving the canvas offset
		if (BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->GetUI(m_cutscenePanels.back())->GetUIDrawPos().y > 0.5f)
		{
			//Slowly move down the canvas
			BE::Vec3f offset = BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->GetUIOffset();

			if(!BE::Input->IsMousePressed(BE::Mouse::MOUSE_LEFT))
				offset.y += m_verticalScrollSpeed * (float)dt;
			else
				offset.y += m_verticalScrollSpeed * m_fastScrollMultiplier * (float)dt;
			
			BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->SetUIOffset(offset);

			//Skip to the end if the player presses space bar
			if (BE::Input->IsKeyTriggered(BE::Key::SPACE))
			{
				//Set opacity of all UI to be 1
				for (BE::UIElementID uid : m_cutscenePanels)
				{
					BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->GetUI<BE::UIImage>(uid)->setOpacity(1.0f);
				}

				m_currentPanel = m_cutscenePanelFadeInDelay[m_cutsceneSet].size();

				//Set the Canvas offset to the end
				BE::Vec3f uiPos = BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->GetUI(m_cutscenePanels.back())->GetUIPos();
				uiPos.x = 0.0f;
				(uiPos.y *= -1.0f) += 0.5f;
				BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->SetUIOffset(uiPos);
			}
		}
		else
		{
			//Cloes the canvas when the thing is done
			if (BE::Input->IsKeyTriggered(BE::Key::SPACE))
			{
				//Close this canvas
				BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->ResetOffset();
				BE::UI->GetCanvas(m_cutsceneSets[m_cutsceneSet])->SetIsActive(false);
				BE::UI->GetCanvas(m_cutscenePressSpaceUI)->SetIsActive(false);
				BE::UI->GetCanvas(m_cutsceneBGUI.first)->SetIsActive(false);
				BE::UI->RestoreFontState();

				m_cutsceneSet = (size_t)-1;
				BE::PauseControl::SetPause(false);

				//Stop the Cutscene music and resume in game music
				BE::ECS->GetComponent<BE::GameLogic>(BGMPlayer).GetScriptDerived<IGMusicPlayer>()->StartFadingOutCutscene();
			}
		}
	}
		

	//Start running the cutscene
	void CutsceneController::StartCutscene(size_t _cutsceneSet)
	{
		Assert(_cutsceneSet < m_cutsceneSets.size(), "Cutscene Set %llu does not exist", _cutsceneSet);

		//Clear the old set of cutscene
		m_cutscenePanels.clear();

		//Get all the panels under this cutscene
		m_cutscenePanels = BE::UI->GetCanvas(m_cutsceneSets[_cutsceneSet])->GetAllUIElements();

		m_timeElapsed = 0.0f;
		m_currentPanel = 0;
		m_cutsceneSet = _cutsceneSet;

		//Set all their opacity to 0
		for (BE::UIElementID uid : m_cutscenePanels)
		{
			BE::UI->GetCanvas(m_cutsceneSets[_cutsceneSet])->GetUI<BE::UIImage>(uid)->setOpacity(0.0f);
		}

		//Set the canvas to be visible
		BE::UI->GetCanvas(m_cutsceneSets[_cutsceneSet])->ResetOffset();
		BE::UI->GetCanvas(m_cutsceneSets[_cutsceneSet])->SetIsActive(true);
		BE::UI->GetCanvas(m_cutscenePressSpaceUI)->SetIsActive(true);
		BE::UI->GetCanvas(m_cutsceneBGUI.first)->SetIsActive(true);
		BE::UI->GetCanvas(m_cutsceneBGUI.first)->GetUI<BE::UIImage>(m_cutsceneBGUI.second)->setOpacity(1.0f);
		BE::UI->SaveFontState();
		BE::PauseControl::SetPause(true);

		//Stop the Cutscene music and resume in game music
		BE::ECS->GetComponent<BE::GameLogic>(BGMPlayer).GetScriptDerived<IGMusicPlayer>()->StartFadingInCutscene();
	}

	//Whether any cutscene is running
	bool CutsceneController::HasPanelsRemaining() const
	{
		return m_cutsceneSet < m_cutsceneSets.size() && m_currentPanel < m_cutscenePanelFadeInDelay[m_cutsceneSet].size();
	}

	//Whether there is still cutscenes running
	bool CutsceneController::HasCutsceneRunning() const
	{
		return m_cutsceneSet < m_cutsceneSets.size();
	}
}


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<CutsceneController>(RegistrationNameCreator<CutsceneController>())
		.constructor()
		.property("CutsceneSets", &CutsceneController::m_cutsceneSets)
		.property("CutsceneFadeTimes", &CutsceneController::m_cutscenePanelFadeInDelay);
}