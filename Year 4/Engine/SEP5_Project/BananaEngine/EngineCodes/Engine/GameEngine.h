#pragma once
#ifndef	GAMEENGINE_H
#define GAMEENGINE_H

#include <Systems/AllSystems.h>
#include <AssetManager/AssetManager.h>
#include "SpeedControl.h"

namespace BE
{
	struct EngineConfig
	{
		double		m_maxFrameRate = 60.0;				//what desired frame rate to run the engine in?
		int			m_windowWidth = 1920;				//What window width to open up with (non-fullscreen)?
		int			m_windowHeight = 1080;				//What window height to open up with (non-fullscreen)?
		bool		m_isFullScreen = false;			//Whether the application will run full screen?
		int 		m_audioChannels = 16;				//Number of audio channels to use
		float		m_musicVolume = 1.0f;				//The global volume of the music (for audio)
		float		m_soundVolume = 1.0f;				//The global volume of the sound effects (for audio)
	};
	class GameEngine : public Singleton<GameEngine>
	{
		//All the engine's system
		std::vector<ISystem*> m_systems;

		//Times and framerates
		double m_desiredFPS;		//What frames per second should the engine aim for
		double m_deltaTime;			//DT of the engine
		size_t m_frameCount;		//How many frames have passed by
		double m_cappedDt;			//If DT exceeds this value, then cap DT to be this value

		double m_startTime;			//The time at the start of the current frame loop, for DT calculation
		double m_endTime;			//The time at the end of the current frame loop, for DT calculation

		//Fixed DT stuff for physics
		double m_fixedDT;			//Fixed DT of the engine (1/desiredFPS) meant for physics calculation
		double m_accumulatedTime;	//How much time passed since start of this frame
		int    m_physSteps;			//Number of steps to go through for physics calculation (timeSteps)

		//Whether to run the application
		bool m_run;

		//Temporary variable for engine config, will move to window class when ready
		int m_windowWidth;
		int m_windowHeight;
		bool m_isFullScreen;
		std::string m_windowName;

		//Creates an engine with the given configFile name. Leave blank for default config
		GameEngine(const std::string& in);
		~GameEngine();

		//For creating a new engine system, to be put into m_systems - Warning: Do not create the same system twice!
		template<typename T_Sys, typename... CtorParam>
		void CreateSystem(CtorParam&& ...args);

		//Donavern's
		GLFWwindow* window;
	public:
		//Functions to run
		/*------------- Client side to include these functions in their application class --------------------*/
		void Init();		//1 time initialisation of the system after constructor is called
		void Awake();		//To be called when editor goes into play mode, or change to new scene
		void Update();		//The usual update loop
		void Reload();		//To be called when editor goes into pause mode, or going to close the scene
		void Shutdown();	//1 time uninitalisation of the system before destructor is called
		/*----------------------------------------------------------------------------------------------------*/

		//Tell the engine to quit
		inline void Quit()										{ m_run = false; }

		//Getters
		inline bool GetIsRunning() const						{ return m_run; }
		inline double GetDesiredFPS() const						{ return m_desiredFPS; }
		inline double GetDeltaTime() const						{ return m_deltaTime; }
		inline double GetFixedDeltaTime() const					{ return m_fixedDT; }
		inline double GetTimeElapsed() const					{ return glfwGetTime(); }
		inline double GetFramerate() const						{ return 1.0/m_deltaTime; }
		inline double GetFixedFramerate() const					{ return 1.0/m_fixedDT; }
		inline size_t GetCurrFrame() const						{ return m_frameCount; }
		inline int	  GetPhysSteps() const						{ return m_physSteps; }
		inline double GetAccumulatedTime() const				{ return m_accumulatedTime;}

		//Temporary getters for window attribute
		int GetWindowWidth() const								{ return m_windowWidth; }
		int GetWindowHeight() const								{ return m_windowHeight; }
		bool GetIsFullScreen() const							{ return m_isFullScreen; }
		const std::string& GetWindowName() const				{ return m_windowName; }
		
		//------------------------------------------ Event handling -------------------------------------------------

		void OnSceneUnload(IEvent& e);		//Called when scene going to start unloading, called Reload on all ISystem
		void OnSceneLoaded(IEvent& e);		//Called when scene finished loading, called Awake on all ISystem
		void OnQuit(IEvent& e);				//Called when window going to close, call Shutdown on all ISystem
		
		//------------------------------------------ Engine configuration -------------------------------------------

		

		//Engine configuration
		EngineConfig LoadConfig();					//Load an engine configuration. Leave blank for default config
		void SaveConfig();		//Save the engine configuration

		//-----------------------------------------------------------------------------------------------------------
		
		//Donavern's
		void passGLFWwindow(GLFWwindow* newWindow) { window = newWindow; };
		void SetIsFullScreen(bool b) { m_isFullScreen = b; };

		friend class Singleton<GameEngine>;
	};

	#define Engine			GameEngine::GetInstance()
}

#include "GameEngine.hpp"

#endif