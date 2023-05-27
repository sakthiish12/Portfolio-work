//MOVED TO PARTY_PANTRY PROJECT

//Author: All Programmers

//Entry point for the entire program
//TODO more documentation

//#include <iomanip>
//#include <iostream>
//#include <chrono> //Benchmarking time taken for simulation
//#include "EditorStateManager/EditorStates/gameState.h"
//#include "EditorStateManager/gameStateManager.h"
//#include "WinMgr/WinMgr.h"
//#include "pch.h"
//#include "Graphics/Source/FontRendering.h"//font rendering
//
////for the graphics system 
//#include "./Graphics/Source/GraphicsApplication.h"	
//
////for the editor 
//#include "Editor/Editor.h"
//
////for the memory debugging
//#define _CRTDBG_MAP_ALLOC
//#include <cstdlib>
//#include <crtdbg.h>
//
//#ifdef _DEBUG
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//#else
//#define DBG_NEW new
//#endif


//
//int main()
//{
//	///////////////////////////////////////////////////////////////////////////
//	// DEBUGGER			(2 /2)
//	// Enable run-time memory check for debug builds.
//	#if defined(DEBUG) | defined(_DEBUG)
//		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
//		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	#endif
//	// END OF DEBUGGER	(2 /2)
//	///////////////////////////////////////////////////////////////////////////
//
//
//	//Chrono values for simulation duration.
//	std::chrono::duration<float> simulationDuration;
//	auto simulationStartTime = std::chrono::high_resolution_clock::now();
//
//	//Make floating point numbers have formatting.
//	std::cout << std::fixed << std::setprecision(7);
//
//
//	//Initialise vital systems here (audio, openGL, fonts, etc...)
//	//Init windows, graphics and imgui
//	WindowsManager.Init();
//	WindowsManager.InitializeRenderingEnvironment();
//	Graphic_Application::GetInstance().initalize_graphics();
//	Editor::GetInstance().initailize_editor(); // Setup ImGui binding
//	Editor::GetInstance().load_test_scenes(); //loading the test scenes 
//
//
//	//Initialise Game State Manager with the first Game State (i.e. Main Menu)
//	//Game State Manager Loop
//	//Keep the game running as long as the current game state is to not quit)
//	//gameStateManager::getInstance().run();
//	
//	//gameStateManager::run(&g_testState);//orignal testing for the imgui 
//
//	//gameStateManager::run(&g_imguiTestState);//for testing of the editor
//	
//	//gameStateManager::run(&g_renderTestState); //for testing of rendering system 
//
//	//gameStateManager::run(&g_officialTest); //OFFICIAL 
//	
//	//gameStateManager::run(&g_physicTestState); //for testing 
//
//	//Terminate vital systems here before ending the game
//
//	//Checking time taken for simulation to run.
//	auto simulationEndTime = std::chrono::high_resolution_clock::now();
//	simulationDuration = simulationEndTime - simulationStartTime;
//	std::cout << "SIMULATING: " << std::endl;
//	std::cout << "SIMULATION TOOK: " << simulationDuration.count()
//		<< " SECONDS. " << std::endl;
//
//	//clear editor & graphics 
//	Editor::GetInstance().clear_editor();//deleting context of editor 
//	//_CrtDumpMemoryLeaks();
//}
