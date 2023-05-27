
#pragma once
#ifndef UIBUTTON_H
#define UIBUTTON_H
#include "../UIElement.h"
#include "../UIInteractable.h"

namespace BE
{
	class UIButton : public UIElement, public UIInteractable
	{
		//UIInteractable::m_textures stores the texture ID for each button state
		//such as pressed, hovered, disabled, normal

	public:
		UIButton(CanvasID _cid, UIElementID _uid);
		~UIButton();

		UIELEMENT_FUNCTIONS;

		virtual void SetUIElementFlag(UIElementFlags _f, bool _b) override;

		/*
			RTTR Stuff
		*/
		RTTR_ENABLE(UIElement, UIInteractable);
	};
}
#endif