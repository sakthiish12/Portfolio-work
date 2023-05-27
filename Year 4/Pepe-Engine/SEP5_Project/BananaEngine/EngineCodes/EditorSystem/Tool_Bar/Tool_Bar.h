/******************************************************************************/
/*!
\file       Tool_Bar.h
\par        Project: 
\author     Pham Thao Mien
\date       12 November 2021
\brief      This header file contains the declarations of tool bar functions
			such as play-pause button and feature

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef TOOL_BAR_H
#define TOOL_BAR_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR

namespace BE
{
	enum class SceneState
	{
		Pause = 0,
		Play = 1,
		Edit = 2
	};

	class Tool_Bar : public ImGuiWindow
	{
	private:
		/*const std::string play{ ICON_FA_PLAY };
		const std::string pause{ ICON_FA_PAUSE };
		std::string button{ play };*/

		static Tool_Bar* p_toolbar;
		SceneState m_scenestate;

		bool playButton;
		bool pauseButton;
	public:
		Tool_Bar();
		~Tool_Bar();

		static inline Tool_Bar* Get() { return p_toolbar; }
		inline SceneState GetSceneState() { return m_scenestate; }
		void setSceneState(SceneState s) { m_scenestate = s; }
		void resetButtons();
		//void SetSceneState(SceneState state);

		Tool_Bar* init();
		void update() override;
		void Shutdown() override;
	};
};
#endif
#endif