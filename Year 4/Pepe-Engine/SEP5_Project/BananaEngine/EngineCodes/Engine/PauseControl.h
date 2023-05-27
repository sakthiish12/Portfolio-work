#pragma once
namespace BE
{
	class PauseControl
	{
	private:
		static bool pause;
	public:
		static void SetPause(bool _toPause);
		static bool GetPause();
	};



}
