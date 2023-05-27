// /******************************************************************************/
// /*!
// \file         InputMgr.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        64 Lines
// \lines        69 Lines (seg)
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef __INPUTMGR_H__
#define __INPUTMGR_H__

#include <windows.h>
#include "../Math/Vector/Vector2.h"

/*
Note:
	In this InputManager, has to call InputMgr's update at every frame, usually at gamestate update.

	Then use MouseManager, or KeyManager to check the state.

	change the fn name for key and mouse to be more similar for easier usuage
*/

using namespace Mathf;

class MouseInput;
class KeyInput;
class InputMgr
{
	unsigned char data[256]; //current virtual key state
	unsigned char prevData[256]; // previous virtual key state

	 // mouse scrolling, going to negative means scroll down, going to positive means scroll up
	int zDelta = 0; //raw value from windows
	int mouse_changes = 0; //rate of change per WHEEL_DELTA
	bool last_mouse_scroll = 0; //temporary fix to not resetting to 0 after scroll

	void SaveKeyboardState();
	InputMgr();
	//Use this three fn to check for key state
	/*
	*	NOTE: USE MouseManager && KeyManager respectively instead
	*	
	* 
	*	Sample usage;
	*	isKeyTriggered(VK_ESCAPE);
	*	isKeyTriggered('A');                 -> Use capital letter if possible
	*	isKeyTriggered('a', true);
	*
	*	For list of inputs: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	*	Or find "WinUser.h" or simply peak on VK_ESCAPE.
	*/
	bool isKeyTriggered(char key, bool charKey = false, bool disableUntilUpdated = false);
	bool isKeyPressed(char key, bool charKey = false, bool disableUntilUpdated = false);
	bool isKeyReleased(char key, bool charKey = false, bool disableUntilUpdated = false);
public:
	~InputMgr();
	void update(float deltaTime);

	static InputMgr& GetInstance();
	bool mouseClickDisabled = false;
	bool mouseClickEnabled = true; //tracks if the mouse click is enabled 
	float currentTimer = 2.f;

	//Handle other message from window not handled in WinMgr WinProcedure, which include inputs
	LRESULT CALLBACK Inputs(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//For getting toggle stat ->might delete away
	void CheckKeyState(char key, bool& PressedState, bool& ToggleState);

	//for access to isKeyTriggered, pressed and released
	friend class MouseInput;
	friend class KeyInput;
};

enum class MouseButton
{
	LEFT_BUTTON = VK_LBUTTON,
	RIGHT_BUTTON = VK_RBUTTON,
	MIDDLE_BUTTON = VK_MBUTTON,
	X_BUTTON_1 = VK_XBUTTON1,
	X_BUTTON_2 = VK_XBUTTON2,
	NONE
};

class MouseInput
{
	vec2 mouse_position_TL;
	vec2 mouse_position_BL;

	

	MouseInput() = default;
	
public:
	~MouseInput() = default;
	//Client's window space (Origin: TOP LEFT)
	Vec2 getMousePosition_WindowSpace() const;
	//Normal space Origin: BOTTOM LEFT
	Vec2 getMousePosition_BL() const;
	Vec2 getWorldPosition();

	void updateMousePosition_windowSpace(float x, float y);
	void updateMousePosition_BL(float x, float y);

	bool isButtonTriggered(MouseButton button, bool disableUntilUpdated = false);
	bool isButtonPressed(MouseButton button, bool disableUntilUpdated = false);
	bool isButtonReleased(MouseButton button, bool disableUntilUpdated = false);

	//mouse scroll 
	int getMouseScroll(); // 0 means no change, greater 0 means up, smaller 0 means down

	static MouseInput& GetInstance();
};

class KeyInput
{
	KeyInput() = default;
public:
	~KeyInput() = default;

	//Use this three fn to check for key state
	/*
	*	Sample usage;
	*	isKeyTriggered(VK_ESCAPE);
	*	isKeyTriggered('A');                 -> Use capital letter if possible
	*	isKeyTriggered('a', true);
	*
	*	For list of inputs: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	*	Or find "WinUser.h" or simply peak on VK_ESCAPE.
	*/
	bool isKeyTriggered(char key, bool charKey = false, bool disableUntilUpdated = false);
	bool isKeyPressed(char key, bool charKey = false, bool disableUntilUpdated = false);
	bool isKeyReleased(char key, bool charKey = false, bool disableUntilUpdated = false);

	static KeyInput& GetInstance();
};

#define InputManager ::InputMgr::GetInstance()
#define MouseManager ::MouseInput::GetInstance()
#define KeyManager ::KeyInput::GetInstance()

#endif
