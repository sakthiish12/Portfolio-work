
#pragma once
#ifndef UISLIDER_H
#define UISLIDER_H
#include "../UIElement.h"
#include "../UIInteractable.h"

namespace BE
{
	class UISlider : public UIElement, public UIInteractable
	{
		//UIInteractable::m_textures stores the texture ID of the slider knob at different states
		//such as pressed, hovered, disabled, normal

		//Asset ID of the slider bar background and foreground
		AssetID m_sliderBarBGTexture{ BEID{0} };
		AssetID m_sliderBarFGTexture{ BEID{0} };

		//For slider knob
		float m_percentage{ 0.5f };							//Between 0 and 1
		Vec3f m_sliderKnobScale{ 0.5f, 0.5f, 0.5f };		//The scale of the slider knob, by pixel

	public:
		UISlider(CanvasID _cid, UIElementID _uid);
		~UISlider();

		UIELEMENT_FUNCTIONS;

		//For slider knobs only, position depends on the slider percentage
		Vec3f GetSliderKnobScreenPos() const;	//Position of the slider knob by screen coords (0 to screen size)
		Vec3f GetSliderKnobScreenScale() const;	//Scale of the slider knob by screen size
		Vec3f GetSliderKnobToDrawPos() const;	//Position of the slider knob normalised (between 0 and 1)
		inline Vec3f GetSliderKnobToDrawScale() const					{ return m_sliderKnobScale; }

		//UI Transforms for the slider knobs (Slider bar use base class's UI Transforms)
		inline const Vec3f& GetSliderKnobScale() const					{ return m_sliderKnobScale; }
		inline void SetSliderKnobScale(const Vec3f& _newScale)			{ m_sliderKnobScale = _newScale; }
		
		//Slider Percentage
		inline void SetSliderPercentage(float _percent)					{ m_percentage = BEMath::Clamp(0.0f, 1.0f, _percent); }
		inline float GetSliderPercentage() const						{ return m_percentage; }

		//Get the slider bar texture
		inline AssetID GetSliderBarBackgroundTexture() const			{ return m_sliderBarBGTexture; }
		inline AssetID GetSliderBarForegroundTexture() const			{ return m_sliderBarFGTexture; }

		//Set the texture ID for slider bar texture, set _aID to 0 to remove texture
		void SetSliderBarBackgroundTexture(AssetID _aID);
		void SetSliderBarForegroundTexture(AssetID _aID);


		virtual void SetUIElementFlag(UIElementFlags _f, bool _b) override;

		/*
			RTTR Stuff
		*/
		RTTR_ENABLE(UIElement, UIInteractable);
	};
}
#endif
