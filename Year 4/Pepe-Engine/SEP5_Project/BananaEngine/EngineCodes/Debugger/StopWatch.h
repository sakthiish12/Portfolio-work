#pragma once
#include <Debugger/Debugger.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <sstream>
namespace BE
{
	using namespace std::chrono;
	//How To Use?
	//BE::StopWatch watch(<insert name of watch or leave it blank>)
	//Either let the stopwatch go out of scope or call the stop function to stop the timing, will write to log out
	struct StopWatch
	{
		high_resolution_clock::time_point start;
		std::string name;
		bool stopped = false;
		StopWatch()
			:start{ high_resolution_clock::now() }, name{""}, stopped{false} {}
		StopWatch(const std::string& _name)
			:start{ high_resolution_clock::now() }, name{_name}, stopped{ false } {}
		~StopWatch()
		{
			if (!stopped)
			{
				double timing = duration_cast<duration<double, std::milli>>(high_resolution_clock::now() - start).count();
				std::stringstream ss;
				ss << timing << " ms taken since start of stopwatch " << name;
				LogCore(ss.str().c_str());
			}
		}
		void Stop(){ 
			double timing = duration_cast<duration<double, std::milli>>(high_resolution_clock::now() - start).count();
			std::stringstream ss;
			ss << timing << " ms taken since start of stopwatch " << name;
			LogCore(ss.str().c_str());
			stopped = true;
		}

	};
}