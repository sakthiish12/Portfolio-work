/******************************************************************************/
/*!
\file		Console_Window.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Debug Console

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef CONSOLE_WINDOW_H
#define CONSOLE_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"

#ifdef USE_EDITOR

namespace BE
{
	class Console_Window : public ImGuiWindow
	{
	private:
		static Console_Window* p_console;

	public:	
		Console_Window();
		~Console_Window();

		static inline Console_Window* Get() { return p_console; }

		Console_Window* init();
		void update() override;
		void Shutdown() override;
	};
};

#endif
#endif