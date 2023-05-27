/******************************************************************************/
/*!
\file		Hierachy_Window.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Show all game objects in the scene

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef HIERARCHY_WINDOW_H
#define HIERARCHY_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR
#include <BananaEngine.h>

namespace BE
{
	//enum class FileStates
	//{
	//	ogState,
	//	failState,
	//	successState,
	//};

	class Hierarchy_Window : public ImGuiWindow
	{
	private:
		static Hierarchy_Window* p_hier;

		// Bool 
		bool inTextbox = false;
		bool duplicate_entity;
		bool isFocused{};
		//// Test
		//FileStates loadState = FileStates::ogState;
		//FileStates saveState = FileStates::ogState;

		static std::vector<BE::EntityID> selected_id;

		//static BE::EntityID selected_id;
		//static std::vector<int> selected_flag;

		//static bool show_selected;

	public:	
		static BE::EntityID id;

		static bool isusing;
		static bool isSet;

		Hierarchy_Window();
		~Hierarchy_Window();

		static inline Hierarchy_Window* Get() { return p_hier; }

		static std::vector<BE::EntityID>& Get_Selected() { return selected_id; }
		//static BE::EntityID& Get_Selected() { return selected_id; }
		//static std::vector<int>& Get_selected_flag() { return selected_flag; }

		static void Clone_Popup();

		//static void Delete_Popup(std::vector<BE::EntityID> vec);
		static void Delete_Popup();
		//static void Multiple_Select(BE::EntityID _x, int _i);

		static void ParentChild_Hierarchy(std::string name, BE::EntityID _x);

		Hierarchy_Window* init();
		void update() override;
		void Shutdown() override;
		bool isHierarchyFocused();
	};
};

#endif
#endif