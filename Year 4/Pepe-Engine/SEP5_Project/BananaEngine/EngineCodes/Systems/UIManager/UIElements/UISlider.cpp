#include "pch.h"
#include "UISlider.h"

#include <Serialisation/BananaSerialisation.h>

#include "../../AudioManager/AudioManager.h"
#include "../../InputController/InputController.h"
#include "../UIManager.h"
#include <Engine/GameEngine.h>

namespace BE
{
	UISlider::UISlider(CanvasID _cid, UIElementID _uid) : UIElement(UIElementType::slider, _cid, _uid), UIInteractable()
	{
		m_uiElementName = "New Slider";
	}

	UISlider::~UISlider()
	{
	}

	void UISlider::InitUI()
	{
		UIElement::InitUI();
	}

	void UISlider::UpdateUI(double dt)
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
		//Update the screen pos and scale in case window width gets changed
		UpdateReadOnlyTransform();

		//If mouse cursor is within the slider knob's position, or slider knobis being dragged
		if (isCursorInsideWindow && !UI->GetIsInteractingWithUI() &&		//Not interacting with other UI
			//Mouse is within the slider bar
			(((mousePosX >= GetUIScreenPos().x - GetUIScreenScale().x / 2.0f) &&
			  (mousePosX <= GetUIScreenPos().x + GetUIScreenScale().x / 2.0f) &&
			  (mousePosY >= GetUIScreenPos().y - GetUIScreenScale().y / 2.0f) &&
			  (mousePosY <= GetUIScreenPos().y + GetUIScreenScale().y / 2.0f)) || 
			//Mouse is on the slider knob
			 ((mousePosX >= GetSliderKnobScreenPos().x - GetSliderKnobScreenScale().x / 2.0f) &&
			  (mousePosX <= GetSliderKnobScreenPos().x + GetSliderKnobScreenScale().x / 2.0f) &&
			  (mousePosY >= GetSliderKnobScreenPos().y - GetSliderKnobScreenScale().y / 2.0f) &&
			  (mousePosY <= GetSliderKnobScreenPos().y + GetSliderKnobScreenScale().y / 2.0f))))
#else
		if (isCursorInsideWindow && !UI->GetIsInteractingWithUI() &&		//Not interacting with other UI
			//Mouse is within the slider bar
			(((Input->GetMousePosX() >= GetUIScreenPos().x - GetUIScreenScale().x / 2.0f) &&
			  (Input->GetMousePosX() <= GetUIScreenPos().x + GetUIScreenScale().x / 2.0f) &&
			  (Input->GetMousePosY() >= GetUIScreenPos().y - GetUIScreenScale().y / 2.0f) &&
			  (Input->GetMousePosY() <= GetUIScreenPos().y + GetUIScreenScale().y / 2.0f)) ||
			//Mouse is on the slider knob
		 	 ((Input->GetMousePosX() >= GetSliderKnobScreenPos().x - GetSliderKnobScreenScale().x / 2.0f) &&
			  (Input->GetMousePosX() <= GetSliderKnobScreenPos().x + GetSliderKnobScreenScale().x / 2.0f) &&
			  (Input->GetMousePosY() >= GetSliderKnobScreenPos().y - GetSliderKnobScreenScale().y / 2.0f) &&
			  (Input->GetMousePosY() <= GetSliderKnobScreenPos().y + GetSliderKnobScreenScale().y / 2.0f))))
#endif
		{
			//Tell UI Manager that this UI Element is being interacted right now, so don't interact underlying ones
			UI->SetIsInteractingWithUI();

			//If previously wasn't hovering
			if (m_uiFlags[(size_t)UIElementFlags::isHovering] == false)
			{
				m_uiFlags[(size_t)UIElementFlags::isHovering] = true;
				m_currentStatus = UIInteractStatus::hovered;
				setHasTexturesChanged(true);

				//If there's audio file linked
				if (m_audioQues[(size_t)UIAudioQueType::hovered])
				{
					AudioEvent ae(m_canvasID, m_uiID, m_audioQues[(size_t)UIAudioQueType::hovered], UIAudioQueType::hovered);
					AudioProperties ap(m_pitch, m_volume, m_priority, false, false, MIN_AUDIO_DISTANCE, MAX_AUDIO_DISTANCE);
					AudioMgr->PlayAudio(ae, ap);
				}
			}

			//If mouse button is held down
			if (Input->IsMousePressed(Mouse::MOUSE_LEFT))
			{
				//If previously wasn't hovering
				if (m_uiFlags[(size_t)UIElementFlags::isPressed] == false)
				{
					//If mouse button is pressed then show the pressed colour tint
					m_uiFlags[(size_t)UIElementFlags::isPressed] = true;
					m_currentStatus = UIInteractStatus::pressed;
					setHasTexturesChanged(true);
					
					//If there's audio file linked
					if (m_audioQues[(size_t)UIAudioQueType::pressed])
					{
						AudioEvent ae(m_canvasID, m_uiID, m_audioQues[(size_t)UIAudioQueType::pressed], UIAudioQueType::pressed);
						AudioProperties ap(m_pitch, m_volume, m_priority, false, false, MIN_AUDIO_DISTANCE, MAX_AUDIO_DISTANCE);
						AudioMgr->PlayAudio(ae, ap);
					}
				
				}


				//Set the new screen position of the knob
#ifdef USE_EDITOR
				//Compiled with editor - Set the % based on the mouse pos
				this->SetSliderPercentage( ((mousePosX - GetUIScreenPos().x) / GetUIScreenScale().x) + 0.5f );
#else
				//Compiled without editor - Set the % based on the mouse pos
				this->SetSliderPercentage( ((Input->GetMousePosX() - GetUIScreenPos().x) / GetUIScreenScale().x) + 0.5f );
#endif
				//Call the script to update stuff
				for (const ScriptPtr& script : m_script.GetAllScripts())
					script->OnSliderChanged(m_percentage);
			}
			else
			{
				//Mouse cursor is hovering if no LMB is pressed
				m_uiFlags[(size_t)UIElementFlags::isPressed] = false;
				m_currentStatus = UIInteractStatus::hovered;
				setHasTexturesChanged(true);
			}
		}
		else
		{
			//Disable hover/pressed flag, and set tint colour back to normal
			m_uiFlags[(size_t)UIElementFlags::isHovering] = false;
			m_uiFlags[(size_t)UIElementFlags::isPressed] = false;
			m_currentStatus = UIInteractStatus::normal;
			setHasTexturesChanged(true);
		}

	}

	void UISlider::UninitUI()
	{
		UIElement::UninitUI();
	}

	void UISlider::Serialise()
	{
		UIElement::Serialise();
		// Shaun: serialise derived class only
	}

	void UISlider::Deserialise()
	{
		UIElement::Deserialise();
		// Shaun: serialise derived class only
	}

	Vec3f UISlider::GetSliderKnobToDrawPos() const
	{
		//Between 0 and 1
		return Vec3f{ m_drawPos.x + (m_percentage - 0.5f) * m_uiScale.x,
					  m_drawPos.y,
					  0.0f };
	}

	Vec3f UISlider::GetSliderKnobScreenPos() const
	{
		Vec3f drawPos = GetSliderKnobToDrawPos();
		
		//between 0 and screen sizes
#ifdef USE_EDITOR
		const auto& [editorWidth, editorHeight] = Debugger->GetEditorWindowSize();

		return Vec3f{ drawPos.x * editorWidth,
					  drawPos.y * editorHeight,
					  0.0f};
#else
		return Vec3f{		  drawPos.x  * Engine->GetWindowWidth(),
					  (1.0f - drawPos.y) * Engine->GetWindowHeight(),
					  0.0f};
#endif
	}

	Vec3f UISlider::GetSliderKnobScreenScale() const
	{
		//between 0 and screen sizes
#ifdef USE_EDITOR
		const auto& [editorWidth, editorHeight] = Debugger->GetEditorWindowSize();

		return Vec3f{ m_sliderKnobScale.x * editorWidth,
					  m_sliderKnobScale.y * editorHeight,
					  0.0f };
#else
		return Vec3f{ m_sliderKnobScale.x * Engine->GetWindowWidth(),
					  m_sliderKnobScale.y * Engine->GetWindowHeight(),
					  0.0f };
#endif
	}

	
	void UISlider::SetSliderBarBackgroundTexture(AssetID _aID)
	{
		//Either be null asset, or the given asset must have been loaded
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<Texture>()), 
			"Attempting to assign non-existing asset ID (%llu) to a UI Element, or the asset ID is not a texture", _aID);
		m_sliderBarBGTexture = _aID;
		setHasTexturesChanged(true);
	}

	void UISlider::SetSliderBarForegroundTexture(AssetID _aID)
	{
		//Either be null asset, or the given asset must have been loaded
		AssertCore(_aID == NULL_ASSET || (AssetMgr->IsAssetDataLoaded(_aID) && AssetMgr->GetAssetTypeIDOf(_aID) == AssetMgr->GetAssetTypeID<Texture>()),
			"Attempting to assign non-existing asset ID (%llu) to a UI Element, or the asset ID is not a texture", _aID);
		m_sliderBarFGTexture = _aID;
		setHasTexturesChanged(true);
	}


	void UISlider::SetUIElementFlag(UIElementFlags _f, bool _b)
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
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<UISlider>(RegistrationNameCreator<UISlider>())
		.constructor<CanvasID, UIElementID>()
		.property("SliderKnob Scale", &UISlider::GetSliderKnobScale, &UISlider::SetSliderKnobScale)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Percentage", &UISlider::GetSliderPercentage, &UISlider::SetSliderPercentage)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("SliderBarBackgroundTexture", &UISlider::GetSliderBarBackgroundTexture, &UISlider::SetSliderBarBackgroundTexture)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("SliderBarForegroundTexture", &UISlider::GetSliderBarForegroundTexture, &UISlider::SetSliderBarForegroundTexture)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}