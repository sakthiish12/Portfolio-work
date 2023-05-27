/******************************************************************************/
/*!
\file		Inspector_Window.h
\par        Project: 
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Inspector window for each game object

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef INSPECTOR_WINDOW_H
#define INSPECTOR_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR

namespace BE
{
	class Inspector_Window : public ImGuiWindow
	{
	private:
		static Inspector_Window* p_insp;

		//std::map <SH::EntityID, int> collider;

		//std::map <int, int> audio;

		//mesh renderer
		//int layer;

		//collider
		//bool DebugDraw;

		//tilemap
		//bool drawGrid;

		////AudioProperties
		//SH::AudioType _type;
		//SH::AudioMode _mode;

		//tile map
		//int map_val[2];
		//int map_size[2];
		//std::string level_file_path;

	public:	
		Inspector_Window();
		~Inspector_Window();

		static inline Inspector_Window* Get() { return p_insp; }
		void DisplayProperties(rttr::instance, rttr::type);
		void DisplayDetails(const rttr::instance& inst, const rttr::property& prop);
		void DisplayDetails123(const rttr::variant& prop, std::string propName);

		Inspector_Window* init();
		void update() override;
		void Shutdown() override;
	};
};

#endif
#endif