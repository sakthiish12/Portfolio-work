
#pragma once
#ifndef	UIPROGRESSBAR_H
#define UIPROGRESSBAR_H
#include "../UIElement.h"

namespace BE
{
	//Parts of the progress bar
	// For progress bar
	enum class UIProgressBarParts
	{
		background = 0,		//The background of the progress bar
		foreground,			//The foreground that will shrink and grow based on %
		border,				//The surrounding of the progress bar

		uiProgressBarPartsLast

	};

	//Type of progress bar
	enum class UIProgressBarType
	{
		horizontal = 0,			//Shrinks left/right
		vertical,				//Shrinks up/down
		circular,				//goes in a circle

		uiProgressBarTypeLast
	};


	class UIProgressBar : public UIElement
	{
		//The texture IDs of the UI Button corresponding to the UIInteractStatus - For when UIElementFlags::isUsingTexture is false
		std::array<AssetID, (size_t)UIProgressBarParts::uiProgressBarPartsLast> m_textures;

		//Progress Bar info
		float m_percentage{};					//The value used to determine the progress bar size

		//Bar flickering
		float m_flickerTimesPerSecond{1.0f};	//How many times per second
		float m_flickerThreshold{0.5f};			//At what % will the bar starts to flicker, between 0 and 1
		float m_flickerTimeElapsed{};			//How much time has passed for flickering
		bool  m_isFlickering{};					//Whether the bar should flicker or not
		bool  m_reverseThreshold{};				//Whether to flicker when above the threshold instead?

		//Vertical or horizontal - TO BE REMOVED
		bool m_isVertical{};					//Whether the bar will be horizontal or vertical

		UIProgressBarType m_progressbarType{ UIProgressBarType::horizontal };

	public:
		UIProgressBar(CanvasID _cid, UIElementID _uid);
		~UIProgressBar();

		UIELEMENT_FUNCTIONS;

		/*
			Textures
		*/
		//Get the texture ID for the given progress bar parts
		inline AssetID GetTexture(UIProgressBarParts _progBarPart) const				{ return m_textures[(size_t)_progBarPart]; }

		//Get all textureID
		inline const auto& GetAllTextures() const										{ return m_textures; }

		//Set the texture ID for the given progress bar parts, set _aID to 0 to remove texture
		void SetTexture(UIProgressBarParts _progBarPart, AssetID _aID);


		/*
			Other properties
		*/

		//Values - between 0 and 1
		inline float GetPercentage() const							{ return m_percentage; }
		inline void SetPercentage(float _newPercent)				{ m_percentage = BEMath::Clamp(0.0f, 1.0f, _newPercent); }

		//Flickering
		float GetFlickerOpacity() const;
		void SetFlickering(float _flickerRatePerSecond, float _flickerThreshold);

		inline void SetIsFlickering(bool _b)						{ m_isFlickering = _b; }
		inline void SetFlickerRate(float _flickerRatePerSecond)		{ m_flickerTimesPerSecond = _flickerRatePerSecond;}
		inline void SetFlickerThreshold(float _flickerThreshold)	{ m_flickerThreshold = BEMath::Clamp(0.0f, 1.0f, _flickerThreshold); }

		inline bool  GetIsFlickering() const						{ return m_isFlickering; }
		inline float GetFlickerRate() const							{ return m_flickerTimesPerSecond; }
		inline float GetFlickerThreshold() const					{ return m_flickerThreshold; }

		//TO BE REMOVED
		inline bool GetIsVertical() const							{ return m_isVertical; }
		inline void SetIsVertical(bool _b)							{ m_isVertical = _b; }

		inline UIProgressBarType GetProgBarType() const				{ return m_progressbarType; }
		inline void SetProgBarType(UIProgressBarType _type)			{ m_progressbarType = _type; }

		inline bool GetIsReverseThreshold() const					{ return m_reverseThreshold; }
		inline void SetIsReverseThreshold(bool _b)					{ m_reverseThreshold = _b; }

		/*
			RTTR Stuff
		*/
		RTTR_ENABLE(UIElement);
		RTTR_REGISTRATION_FRIEND;
	};
}

#endif