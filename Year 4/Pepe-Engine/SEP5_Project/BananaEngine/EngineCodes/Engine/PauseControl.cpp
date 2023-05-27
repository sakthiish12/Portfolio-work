#include "pch.h"
#include "PauseControl.h"
#include "../EventDispatcher/EventDispatcher.h"

namespace BE
{
	bool PauseControl::pause = false;

	void PauseControl::SetPause(bool _toPause) 
	{ 
		pause = _toPause;
		GamePauseEvent gpe(_toPause);
		EventMgr->Dispatch(gpe);
	}

	bool PauseControl::GetPause() 
	{
		return pause; 
	}
}