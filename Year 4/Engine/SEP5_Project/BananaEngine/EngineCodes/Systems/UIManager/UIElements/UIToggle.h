#pragma once
#ifndef UITOGGLE_H
#define UITOGGLE_H
#include "../UIElement.h"
#include "../UIInteractable.h"

namespace BE
{
	class UIToggle : public UIElement, public UIInteractable
	{
		//UIInteractable::m_textures stores the texture ID of the toggle box at different states
		//such as pressed, hovered, disabled, normal


		//Checkmark Asset ID
		AssetID m_checkmarkTexture{ BEID{0} };

		//Whether the toggle is enabled (shows the checkmark)
		bool m_isEnabled{ false };


	public:
		UIToggle(CanvasID _cid, UIElementID _uid);
		~UIToggle();

		UIELEMENT_FUNCTIONS;

		//Get the checkmark texture
		inline AssetID GetCheckmarkTexture() const							{ return m_checkmarkTexture; }

		//Set the texture ID for checkmark texture, set _aID to 0 to remove texture
		void SetCheckmarkTexture(AssetID _aID);

		
		virtual void SetUIElementFlag(UIElementFlags _f, bool _b) override;

		/*
			Whether the toggle is turned on/off
		*/
		//Get the current toggle state - is it on or off
		inline bool GetToggle() const			{ return m_isEnabled; }

		//Set the current toggle state. Does nothing if disabled
		void SetToggle(bool _b);

		/*
			RTTR Stuff
		*/
		RTTR_ENABLE(UIElement, UIInteractable);
	};
}
#endif