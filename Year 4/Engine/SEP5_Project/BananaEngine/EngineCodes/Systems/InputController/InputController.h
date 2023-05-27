#pragma once
#ifndef	INPUT_CONTROLLER_H
#define INPUT_CONTROLLER_H

#include <Maths/Vector3D.h>
#include "../ISystem.h"
#include "KeyCodes.h"

namespace BE
{
	//Please use this class if you want to check for inputs

	class InputController : public Singleton<InputController>, public ISystem
	{
		//Mapping from GLFW key codes to SH Key codes
		static std::unordered_map<int, Key> m_KeyCodeMap;
		static std::unordered_map<int, Mouse> m_MouseButtonMap;

		//Key/Mouse states - Are they pressed? triggered? released?
		std::bitset<static_cast<size_t>(Key::KEY_LAST)> m_pressedKey;
		std::bitset<static_cast<size_t>(Key::KEY_LAST)> m_triggeredKey;
		std::bitset<static_cast<size_t>(Key::KEY_LAST)> m_releasedKey;

		std::bitset<static_cast<size_t>(Mouse::MOUSE_LAST)> m_pressedMouse;
		std::bitset<static_cast<size_t>(Mouse::MOUSE_LAST)> m_triggeredMouse;
		std::bitset<static_cast<size_t>(Mouse::MOUSE_LAST)> m_releasedMouse;

		//Mouse positions
		Vec3f m_currMousePos{};
		Vec3f m_prevMousePos{};					//Helper variable to calculate mouse delta

		//Scroll wheel
		Vec3f m_scrollOffset{};

		//Double click
		double m_leftClickTimeRemaining{};		//Time much time left since a left click before it no longer count as double click

		/*
			Helper variables - not to be used by client
		*/
		enum class InputCtrlFlag
		{
			cursorMoving = 0,		//Helper variable for single frame update of the previous mouse position
			isScrolling,			//Helper variable for single frame update of the scroll offset
			justDoubleClicked,		//Whether there was a double click happened

			last
		};
		std::bitset<static_cast<size_t>(InputCtrlFlag::last)> m_inputCtrlFlags{ 0 };

		/*
			Constructor and destructor
		*/
		InputController();
		~InputController();

	public:

		/*
			Input checking
		*/
		//For checking keyboard buttons being pressed/triggered/released
		inline bool IsKeyPressed(Key k)	const			{ return m_pressedKey[static_cast<size_t>(k)]; }
		inline bool IsKeyTriggered(Key k) const			{ return m_triggeredKey[static_cast<size_t>(k)]; }
		inline bool IsKeyReleased(Key k) const			{ return m_releasedKey[static_cast<size_t>(k)]; }

		//For checking if ctrl, alt, shift are pressed
		inline bool IsCtrlPressed()	const				{ return m_pressedKey[static_cast<size_t>(Key::LEFT_CONTROL)] || m_pressedKey[static_cast<size_t>(Key::RIGHT_CONTROL)]; }
		inline bool IsAltPressed() const				{ return m_pressedKey[static_cast<size_t>(Key::LEFT_ALT)] || m_pressedKey[static_cast<size_t>(Key::RIGHT_ALT)]; }
		inline bool IsShiftPressed() const				{ return m_pressedKey[static_cast<size_t>(Key::LEFT_SHIFT)] || m_pressedKey[static_cast<size_t>(Key::RIGHT_SHIFT)]; }

		//For checking mouse buttons being pressed/triggered/released
		inline bool IsMousePressed(Mouse m)	const		{ return m_pressedMouse[static_cast<size_t>(m)]; }
		inline bool IsMouseTriggered(Mouse m) const		{ return m_triggeredMouse[static_cast<size_t>(m)]; }
		inline bool IsMouseReleased(Mouse m) const		{ return m_releasedMouse[static_cast<size_t>(m)]; }

		//Get the current mouse position (screen pos)
		inline float GetMousePosX()	const				{ return m_currMousePos.x; }
		inline float GetMousePosY()	const				{ return m_currMousePos.y; }
		inline Vec3f GetMousePos() const				{ return m_currMousePos; }

		//Get the change in mouse position (screen pos)
		inline float GetMouseDeltaX() const				{ return m_currMousePos.x - m_prevMousePos.x; }
		inline float GetMouseDeltaY() const				{ return m_currMousePos.y - m_prevMousePos.y; }
		inline Vec3f GetMouseDelta() const				{ return m_currMousePos - m_prevMousePos; }

		//Get the amount of scrolling
		inline float GetHorizontalScroll()	const		{ return m_scrollOffset.x; }
		inline float GetVerticalScroll()	const		{ return m_scrollOffset.y; }

		//Check whether there is double left click
		inline bool HasDoubleClick() const				{ return m_inputCtrlFlags.test((size_t)InputCtrlFlag::justDoubleClicked); }

		//Set cursor enabled/disabled
		void SetEnableCursor(bool _b) const;
		bool IsCursorEnabled() const;

		/*
			System functions
		*/
		ISYSTEM_FUNCTIONS;

		/*
			Event handling
		*/
		void KeyButtonHandling(IEvent& e);
		void MouseButtonHandling(IEvent& e);
		void MouseMoveHandling(IEvent& e);
		void MouseScrollHandling(IEvent& e);

		/*
			Used in application.cpp
		*/
		void passGLFWwindow(GLFWwindow* newWindow);

	private:
		friend class Singleton<InputController>;
		GLFWwindow* window;
	};

#define Input InputController::GetInstance()
}

#endif