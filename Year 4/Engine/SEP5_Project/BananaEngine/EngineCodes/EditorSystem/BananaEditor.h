/******************************************************************************/
/*!
\file		BananaEditor.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2022
\brief		Editor Manager, calling all ImGui windows

\copyright	Copyright (C) 2022 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef BANANA_EDITOR_H
#define BANANA_EDITOR_H

#ifdef USE_EDITOR
#include "ImGuiWindow/ImGuiWindow.h"
#include "EditorSystem/EditorSystem.h"
#include "Menu_Window/Menu_Window.h"
#include "Console_Window/Console_Window.h"
#include "Hierarchy_Window/Hierarchy_Window.h"
#include "Inspector_Window/Inspector_Window.h"
#include "Tool_Bar/Tool_Bar.h"
#include "UndoSystem/UndoSystem.h"
#include "EntityTag_Window/EntityTag_Window.h"
#include "Resources_Window/Resources_Window.h"
#include "Assets_Window/Assets_Window.h"
#include "Graphics_Window/Graphics_Window.h"
#endif

#endif