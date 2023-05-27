/******************************************************************************/
/*!
\file		UI_Inspector_Window.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		UI Inspector window for each UI object

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef UI_INSPECTOR_WINDOW_H
#define UI_INSPECTOR_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR

namespace BE
{
	/*enum class Audio_UI
	{
		Button = 0,
		Slider = 1,
		Toggle = 2
	};*/

	class UI_Inspector_Window : public ImGuiWindow
	{
	private:
		static UI_Inspector_Window* p_ui_insp;

		////AudioProperties
		//float _volume;
		//float _pitch;
		//BE::UIAudioQueType _type;

	public:	
		UI_Inspector_Window();
		~UI_Inspector_Window();

		static inline UI_Inspector_Window* Get() { return p_ui_insp; }

		UI_Inspector_Window* init();
		void update() override;
		void Shutdown() override;

		template<class T>
		void UI_Interactable(std::shared_ptr<T> getUIE);
		//void Add_Remove_Audio_Que(std::shared_ptr<SH::UIElement> ptr, Audio_UI UIType);
	};
};

#endif
#endif