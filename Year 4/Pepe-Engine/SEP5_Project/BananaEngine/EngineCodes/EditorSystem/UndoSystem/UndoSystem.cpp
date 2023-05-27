/******************************************************************************/
/*!
\file       UndoSystem.cpp
\par        Project:
\author     Pham Thao Mien
\date       12 November 2021
\brief      This header file contains the handling of changing of scene state
			when the tool bar functions are being clicked.

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "UndoSystem.h"
#ifdef USE_EDITOR

namespace BE
{
	void CommandManager::executeCmd(std::shared_ptr<ICommand> command)
	{
		//mRedoStack = commandStack_t(); // clear the redo stack
		command->execute();
		mUndoStack.push_back(command);
	}

	void CommandManager::undo()
	{
		if (mUndoStack.size() <= 0)
		{
			return;
		}
		mUndoStack.back()->undo();          // undo most recently executed command

		mUndoStack.pop_back();                  // remove top entry from undo stack
	}

	void CommandManager::clearUndoStack()
	{
		while (!mUndoStack.empty())
		{
			mUndoStack.pop_back();
		}
	}
}
#endif