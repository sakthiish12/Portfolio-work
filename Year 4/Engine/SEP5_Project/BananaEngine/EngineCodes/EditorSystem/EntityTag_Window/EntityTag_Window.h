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
#ifndef ENTITY_TAG_H
#define ENTITY_TAG_H

#include "../ImGuiWindow/ImGuiWindow.h"

#ifdef USE_EDITOR

namespace BE
{
	class EntityTag_Window : public ImGuiWindow
	{
	private:
		static EntityTag_Window* p_entityTag;

		//Tag
		int tagID;

	public:
		EntityTag_Window();
		~EntityTag_Window();

		static inline EntityTag_Window* Get() { return p_entityTag; }

		void RenameTag_Popup(std::string old_name);

		EntityTag_Window* init();
		void update() override;
		void Shutdown() override;
	};
};
#endif
#endif