// /******************************************************************************/
// /*!
// \file         InputMgr.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Contains InputMgr that updates virtual key, contains MouseInput 
// and KeyInput that retrieve from InputMgr
// \lines        154 Lines
// \lines        198 Lines (seg)
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "InputMgr.h"
#include "../ECS/component.h"
#include "../ECS/ECS.h"

//bit 31 of lParam in window message determine if a key is pressed repeatedly
#define REPEATED_BIT 0b01000000000000000000000000000000
//#define REPEATED_BIT 0x40000000

//#define HI_BIT_GETKEYSTATE 0b10000000
#define HI_BIT_GETKEYSTATE 0x80
#define LOW_BIT_GETKEYSTATE 0b00000001

//In a Win32 context lParam is 32 bits.  
//"Word" is 16 bits, so two words can fit into a 32 bit location at the same time.
//The "low" word is the least significant 16 bits.The "high" word is the most significant 16 bits.
#define LOW_WORD_PARAM 0xffff
//#define HIGH_WORD_PARAM 0b11111111'11111111'00000000'00000000


////////////////////////////////////////////


//Uncomment cout every key pressed
#define INPUT_DEBUG

#ifdef INPUT_DEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif
#include "../Mono/Mono.h"

//Skip processing of win proc call back (since input is handled directly in SaveKeyboardState()
//Uncomment it to avoid proccessing the message, Might want to uncomment for debugging purposes
#define DO_NOT_PROCESS_INPUT_MESSAGE


//static InputMgr* instance;
void InputMgr::SaveKeyboardState()
{
	//prevData = data;
	memcpy(prevData, data, sizeof(data)); 
	if (GetKeyboardState(data) == false)
	{
		std::cout << "GetKeyboardState failed" << std::endl;
		//error, function failed
	}
}

//temp
InputMgr::InputMgr()
{
	SaveKeyboardState();
}

InputMgr::~InputMgr()
{
}

void InputMgr::update(float deltaTime)
{
	SaveKeyboardState();
	//This is a temp fix, it fixes, but havent thought of a better way atm, TODO
	if (last_mouse_scroll)
		last_mouse_scroll = false;
	else
	{
		zDelta = 0;
		mouse_changes = 0;
	}

	//for the mouse click enable
	if (mouseClickDisabled) {
		currentTimer -= deltaTime;
		mouseClickEnabled = false;
		if (currentTimer <= 0.f) {
			mouseClickEnabled = true;
			currentTimer = 2.f;
			mouseClickDisabled = false;
		}
	}
}

InputMgr& InputMgr::GetInstance()
{
	static InputMgr instance;
	return instance;
}

//might remove if not needed in the future
//You typically do not need the information in lParam. One flag that might be useful is bit 30, the "previous key state" flag, which is set to 1 for repeated key-down messages.
LRESULT InputMgr::Inputs(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifndef DO_NOT_PROCESS_INPUT_MESSAGE
//	wchar_t msg[32];
//	switch (message)
//	{
//	/*Alt key related*/
//	case WM_SYSKEYDOWN:
//#ifdef INPUT_DEBUG
//		swprintf_s(msg, L"WM_SYSKEYDOWN: 0x%x\n", static_cast<int>(wParam));
//		OutputDebugString(msg);
//#endif
//		return DefWindowProc(hWnd, message, wParam, lParam); //Must return sys key to windows
//		break;
//
//	case WM_SYSCHAR:
//#ifdef INPUT_DEBUG
//		swprintf_s(msg, L"WM_SYSCHAR: %c\n", (wchar_t)wParam);
//		OutputDebugString(msg);
//#endif
//		return DefWindowProc(hWnd, message, wParam, lParam); //Must return sys key to windows
//		break;
//
//	case WM_SYSKEYUP:
//#ifdef INPUT_DEBUG
//		swprintf_s(msg, L"WM_SYSKEYUP: 0x%x\n", static_cast<int>(wParam));
//		OutputDebugString(msg);
//#endif
//		return DefWindowProc(hWnd, message, wParam, lParam); //Must return sys key to windows
//		break;
//
//	/*Normal key related*/
//	case WM_KEYDOWN:
//#ifdef INPUT_DEBUG
//		swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", static_cast<int>(wParam));
//		OutputDebugString(msg);
//		if ((REPEATED_BIT & lParam) != REPEATED_BIT)
//		{
//			cout << "Trigger: " << wParam << endl;
//		}
//#endif
//		break;
//
//	case WM_KEYUP:
//#ifdef INPUT_DEBUG
//		swprintf_s(msg, L"WM_KEYUP: 0x%x\n", static_cast<int>(wParam));
//		OutputDebugString(msg);
//		if ((REPEATED_BIT & lParam) == REPEATED_BIT)
//		{
//			cout << "RELEASED: " << wParam << endl;
//		}
//#endif
//		break;
//
//	case WM_CHAR:
//#ifdef INPUT_DEBUG
//		swprintf_s(msg, L"WM_CHAR: %c\n", (wchar_t)wParam);
//		OutputDebugString(msg);
//		//Check if key is repeatedly pressed, bit 31
//		if ((REPEATED_BIT & lParam) == REPEATED_BIT)
//		{
//			cout << "REPEATEDLY PRESSED: " << wParam << endl;
//		}
//#endif
//		break;
//
//	/*Mouse*/
//	// If left mouse button is pressed
//	case WM_LBUTTONDOWN:
//	
//	
//	break;
//	// If right mouse button is pressed
//	case WM_RBUTTONDOWN:
//	
//	
//	break;
//	// If left mouse button is released
//	case WM_LBUTTONUP:
//	
//	
//	break;
//	// If right mouse button is released
//	case WM_RBUTTONUP:
//	
//	
//	break;
//	// If the mouse is moving
//	case WM_MOUSEMOVE:
//	
//
//	break;
//
//		/* Handle other messages (not shown) */
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//	return 0;
#else
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		//IF THIS IS CAUSING ISSUE, JUST INCLUDE WINMGR TO GET THE HWND, try not to use foreground window coz think its refering to any active(even if not this window)
		RECT client_dimension{};
		GetClientRect(GetActiveWindow(), &client_dimension);
		//std::cout << "x: " << client_dimension.right << " y: " << client_dimension.bottom << endl;
		
		//need include windows.h if using this
		//float x = GET_X_LPARAM(lParam);
		//float y = GET_Y_LPARAM(lParam));
		int x = lParam & LOW_WORD_PARAM;
		int y = ((lParam >> 16) & LOW_WORD_PARAM);

		if (x < 0)
			x = 0;
		if (x > client_dimension.right)
			x = client_dimension.right;

		if (y < 0)
			y = 0;
		if (y > client_dimension.bottom)
			y = client_dimension.bottom;

		MouseManager.updateMousePosition_windowSpace(static_cast<float>(x), static_cast<float>(y));
		//std::cout << "x: " << x << " y: " << y << endl;

		//change to normal 2d axis space
		y = client_dimension.bottom - y;
		MouseManager.updateMousePosition_BL(static_cast<float>(x), static_cast<float>(y));
	}
	break;
	case WM_MOUSEWHEEL:
	{
		mouse_changes = GET_WHEEL_DELTA_WPARAM(wParam);
		zDelta += mouse_changes;
		mouse_changes /= WHEEL_DELTA;

		//std::cout << zDelta << std::endl;
		//std::cout << mouse_changes << std::endl;
		//std::cout << MouseManager.getMouseScroll() << std::endl;

		last_mouse_scroll = true;
	}
	break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
#endif
}

void InputMgr::CheckKeyState(char key, bool& PressedState, bool& ToggleState)
{
	//https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
	//short result = GetKeyState(key);
	short result = data[key];
	if ((result & HI_BIT_GETKEYSTATE) == HI_BIT_GETKEYSTATE)
		//Key is pressed
		PressedState = true;
	else
		PressedState = false;

	if ((result & LOW_BIT_GETKEYSTATE) == LOW_BIT_GETKEYSTATE)
		//toggle is on
		ToggleState = true;
	else
		ToggleState = false;
}

bool InputMgr::isKeyTriggered(char key, bool charKey, bool disableUntilUpdated)
{
	if (charKey)
	{
		if (key >= 'a' && key < 'z')
			key -= 32;
	}
	//current on, and prev off
	bool result = (isKeyPressed(key) && (prevData[key] & HI_BIT_GETKEYSTATE) != HI_BIT_GETKEYSTATE) && WindowsManager.getWindowIsFocus();
	if (result && disableUntilUpdated)
		prevData[key] = false;
	return result;
}

bool InputMgr::isKeyPressed(char key, bool charKey, bool disableUntilUpdated)
{
	if (charKey)
	{
		if (key >= 'a' && key < 'z')
			key -= 32;
	}
	//current on
	bool result = (data[key] & HI_BIT_GETKEYSTATE) == HI_BIT_GETKEYSTATE && WindowsManager.getWindowIsFocus();
	if (result && disableUntilUpdated)
		data[key] = false;
	return result;
}

bool InputMgr::isKeyReleased(char key, bool charKey, bool disableUntilUpdated)
{
	if (charKey)
	{
		if (key >= 'a' && key < 'z') {
			key -= 32;
			
		}
			
		
	}
	//current off, and prev on
	bool result = (isKeyPressed(key) == false) && (prevData[key] & HI_BIT_GETKEYSTATE) == HI_BIT_GETKEYSTATE && WindowsManager.getWindowIsFocus();
	if (result && disableUntilUpdated)
		prevData[key] = false;
	return result;
}

vec2 MouseInput::getMousePosition_WindowSpace() const
{

	return mouse_position_TL;
}

Vec2 MouseInput::getMousePosition_BL() const
{
	return mouse_position_BL;
}

Vec2 MouseInput::getWorldPosition()
{
	//might need to be concern with editor mode
	if (activeCameraTransform)
	{
		//if using a camera
		float windowWidthHalfed = WindowsManager.getWidthF() * 0.5f;
		float windowHeightHalfed = WindowsManager.getHeightF() * 0.5f;
		
		Matrix3 centerWindow_to_worldPos{
			activeCameraData->height* activeCameraData->aspectRatio / (windowWidthHalfed), 0.f, activeCameraTransform->pos.x,
			0.f, activeCameraData->height / (windowHeightHalfed), activeCameraTransform->pos.y,
			0.f, 0.f, 1.f
		};

		//BL to center
		//window center scale down and then scale up to cam center, 
		// in centerWindow_to_worldPos possibly its cam * 2 / window (used cam / window / 2 for a bit of optimisation. The 2 is possibly because of NDC, that is -1 to 1 instead of -0.5 to 0.5 
		//then move to cam pos
		vec3 mouseWorldPos = centerWindow_to_worldPos * vec3{ mouse_position_BL.x - windowWidthHalfed , mouse_position_BL.y - windowHeightHalfed, 1.f };

		//std::cout << mouseWorldPos.x << " , " << mouseWorldPos.y << std::endl;
		return mouseWorldPos;
	}
	else
	{
		Vec3 Result = { mouse_position_BL.x, mouse_position_BL.y, 1.0f };
		//std::cout << Result.x << " , " << Result.y << std::endl;
		return Vec2{ Result.x, Result.y };
	}
}

void MouseInput::updateMousePosition_windowSpace(float x, float y)
{
	mouse_position_TL = vec2{ x, y };
}

void MouseInput::updateMousePosition_BL(float x, float y)
{
	mouse_position_BL = vec2{ x, y };
}

bool MouseInput::isButtonTriggered(MouseButton button, bool disableUntilUpdated)
{
	return InputManager.isKeyTriggered(static_cast<char>(button), false, disableUntilUpdated);
}

bool MouseInput::isButtonPressed(MouseButton button, bool disableUntilUpdated)
{
	return InputManager.isKeyPressed(static_cast<char>(button), false, disableUntilUpdated);
}

bool MouseInput::isButtonReleased(MouseButton button, bool disableUntilUpdated)
{
	return InputManager.isKeyReleased(static_cast<char>(button), false, disableUntilUpdated);
}

int MouseInput::getMouseScroll()
{
	return InputManager.mouse_changes;
}

MouseInput& MouseInput::GetInstance()
{
	static MouseInput instance;
	return instance;
}

bool KeyInput::isKeyTriggered(char key, bool charKey, bool disableUntilUpdated)
{
	return InputManager.isKeyTriggered(key, charKey, disableUntilUpdated);
}

bool KeyInput::isKeyPressed(char key, bool charKey, bool disableUntilUpdated)
{
	return InputManager.isKeyPressed(key, charKey, disableUntilUpdated);
}

bool KeyInput::isKeyReleased(char key, bool charKey, bool disableUntilUpdated)
{
	return InputManager.isKeyReleased(key, charKey, disableUntilUpdated);
}

KeyInput& KeyInput::GetInstance()
{
	static KeyInput instance;
	return instance;
}
