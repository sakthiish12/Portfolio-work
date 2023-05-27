/******************************************************************************/
/*!
\file       UndoSystem.h
\par        Project: 
\author     Pham Thao Mien
\date       12 November 2021
\brief      

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef UNDO_SYSTEM_H
#define UNDO_SYSTEM_H

#include <BananaEngine.h>
#ifdef USE_EDITOR

namespace BE
{
	class ICommand
	{
	public:
		virtual void execute() = 0;
		virtual void undo() = 0;
	};

	typedef std::deque<std::shared_ptr<ICommand> > commandStack_t;
	class CommandManager
	{
		commandStack_t mUndoStack;
	public:
		void executeCmd(std::shared_ptr<ICommand> command);
		void clearUndoStack();
		void undo();
	};
};
#endif
#endif