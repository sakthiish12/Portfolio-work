
#include "pch.h"
#include "UIToggle.h"

#include <Serialisation/BananaSerialisation.h>

#include "../../AudioManager/AudioManager.h"
#include "../../InputController/InputController.h"
#include "../UIManager.h"
#include <Systems/RendererSystem/RendererSystem.h>

namespace BE
{
	UIToggle::UIToggle(CanvasID _cid, UIElementID _uid) : UIElement(UIElementType::toggle, _cid, _uid), UIInteractable()
	{
		m_uiElementName = "New Toggle";
	}

	UIToggle::~UIToggle()
	{
	}

	void UIToggle::InitUI()
	{
		UIElement::InitUI();
	}

	void UIToggle::UpdateUI(double dt)
	{
		//If UI Element is inactive, then just return
		if (!GetUIElementFlag(UIElementFlags::isActive))
			return;

		//Update
		UIElement::UpdateUI(dt);

		//If UI Element disabled, then no need to do any checks for button pressing
		if (GetUIElementFlag(UIElementFlags::isDisabled))
			return;

		bool isCursorInsideWindow = true;
		//If mouse cursor is outside the window then don't need to do anything
#ifdef USE_EDITOR
		const auto& [screenW, screenH] = Debugger->GetEditorWindowSize();
		const auto& [mousePosX, mousePosY] = Debugger->GetEditorMousePos();
		if (mousePosX < 0 || mousePosX > screenW || mousePosY < 0 || mousePosY > screenH)
			isCursorInsideWindow = false;

#else
		const auto& [screenW, screenH] = RendererSystem::GetInstance()->GetBEWindow()->getExtent();
		if (Input->GetMousePosX() < 0 || Input->GetMousePosX() > (float)screenW || Input->GetMousePosY() < 0 || Input->GetMousePosY() > (float)screenH)
			isCursorInsideWindow = false;
#endif

#ifdef USE_EDITOR
		UpdateReadOnlyTransform();

		//If mouse cursor is within the button
		if (isCursorInsideWindow && !UI->GetIsInteractingWithUI() &&				//No other UI elements are being interacted
			(mousePosX >= GetUIScreenPos().x - GetUIScreenScale().x / 2.0f) &&		//Left bound
			(mousePosX <= GetUIScreenPos().x + GetUIScreenScale().x / 2.0f) &&		//Right bound
			(mousePosY >= GetUIScreenPos().y - GetUIScreenScale().y / 2.0f) &&		//Top bound
			(mousePosY <= GetUIScreenPos().y + GetUIScreenScale().y / 2.0f))		//Bottom bound															
#else
		//If mouse cursor is within the button
		if (isCursorInsideWindow && !UI->GetIsInteractingWithUI() &&							//No other UI elements are being interacted
			(Input->GetMousePosX() >= GetUIScreenPos().x - GetUIScreenScale().x / 2.0f) &&		//Left bound
			(Input->GetMousePosX() <= GetUIScreenPos().x + GetUIScreenScale().x / 2.0f) &&		//Right bound
			(Input->GetMousePosY() >= GetUIScreenPos().y - GetUIScreenScale().y / 2.0f) &&		//Top bound
			(Input->GetMousePosY() <= GetUIScreenPos().y + GetUIScreenScale().y / 2.0f))		//Bottom bound
#endif
		{
			//Tell UI Manager that this UI Element is being interacted right now, so don't interact underlying ones
			UI->SetIsInteractingWithUI();

			//If previously wasn't hovering
			if (m_uiFlags[(size_t)UIElementFlags::isHovering] == false)
			{
				m_uiFlags[(size_t)UIElementFlags::isHovering] = true;
				setHasTexturesChanged(true);

				LogCore("Hovering toggle");

				//If there's audio file linked
				if (m_audioQues[(size_t)UIAudioQueType::hovered])
				{
					AudioEvent ae(m_canvasID, m_uiID, m_audioQues[(size_t)UIAudioQueType::hovered], UIAudioQueType::hovered);
					AudioProperties ap(m_pitch, m_volume, m_priority, false, false, MIN_AUDIO_DISTANCE, MAX_AUDIO_DISTANCE);
					AudioMgr->PlayAudio(ae, ap);
				}
			}

			if (Input->IsMousePressed(Mouse::MOUSE_LEFT))
			{
				//If mouse button is pressed then show the pressed colour tint
				m_uiFlags[(size_t)UIElementFlags::isPressed] = true;
				m_currentStatus = UIInteractStatus::pressed;
				setHasTexturesChanged(true);
			}
			else if (Input->IsMouseReleased(Mouse::MOUSE_LEFT))
			{
				//Upon releasing mouse, trigger the action
				m_uiFlags[(size_t)UIElementFlags::isPressed] = false;
				m_currentStatus = UIInteractStatus::hovered;
				setHasTexturesChanged(true);

				m_isEnabled = !m_isEnabled;

				if (m_isEnabled)
				{
					for (const ScriptPtr& script : m_script.GetAllScripts())
						script->OnToggleEnabled();
				}
				else
				{
					for(const ScriptPtr & script : m_script.GetAllScripts())
						script->OnToggleDisabled();
				}

				LogCore("Pressed toggle");
				//If there's audio file linked
				if (m_audioQues[(size_t)UIAudioQueType::pressed])
				{
					AudioEvent ae(m_canvasID, m_uiID, m_audioQues[(size_t)UIAudioQueType::pressed], UIAudioQueType::pressed);
					AudioProperties ap(m_pitch, m_volume, m_priority, false, false, MIN_AUDIO_DISTANCE, MAX_AUDIO_DISTANCE);
					AudioMgr->PlayAudio(ae, ap);
				}
			}
			else
			{
				//Mouse cursor is hovering if no LMB is pressed
				m_currentStatus = UIInteractStatus::hovered;
			}
		}
		else
		{
			//If button was previously hovered
			if (m_uiFlags[(size_t)UIElementFlags::isHovering])
			{
				//Disable hover/pressed flag, and set tint colour or texture back to normal
				m_uiFlags[(size_t)UIElementFlags::isPressed] = false;
				m_uiFlags[(size_t)UIElementFlags::isHovering] = false;
				m_currentStatus = UIInteractStatus::normal;
				setHasTexturesChanged(true);
			}
		}
	}

	void UIToggle::UninitUI()
	{
		UIElement::UninitUI();
	}

	void UIToggle::Serialise()
	{
		UIElement::Serialise();
		// Shaun: serialise derived class only
	}

	void UIToggle::Deserialise()
	{
		UIElement::Deserialise();
		// Shaun: serialise derived class only
	}


	//Set the texture ID for checkmark texture, set _aID to 0 to remove texture
	void UIToggle::SetCheckmarkTexture(AssetID _aID)
	{
		//Either be null asset, or the given asset must have been loaded
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<Texture>()), 
			"Attempting to assign non-existing asset ID (%llu) to a UI Element, or the asset ID is not a texture", _aID);
		m_checkmarkTexture = _aID;
		setHasTexturesChanged(true);
	}

	void UIToggle::SetUIElementFlag(UIElementFlags _f, bool _b)
	{
		UIElement::SetUIElementFlag(_f, _b);

		//Also update interaction status
		if (_f == UIElementFlags::isDisabled)
		{
			//If disabling, then current status is now disabled. Otherwise current status is normal
			m_currentStatus = _b ? UIInteractStatus::disabled : UIInteractStatus::normal;
			setHasTexturesChanged(true);
		}
	}

	void UIToggle::SetToggle(bool _b)
	{
		//If toggle is disabled, then don't do anything
		//if (m_uiFlags.test((size_t)UIElementFlags::isDisabled))
		//	return;

		m_isEnabled = _b;
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<UIToggle>(RegistrationNameCreator<UIToggle>())
		.constructor<CanvasID, UIElementID>()
		.property("Checkmark Texture ID", &UIToggle::GetCheckmarkTexture, &UIToggle::SetCheckmarkTexture)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Toggled On", &UIToggle::GetToggle, &UIToggle::SetToggle)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}