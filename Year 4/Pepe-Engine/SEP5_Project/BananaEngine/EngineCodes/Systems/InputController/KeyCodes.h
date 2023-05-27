#pragma once
#ifndef	KEY_CODES_H
#define KEY_CODES_H

namespace BE
{
	//2 left clicks must be made within this time to be counted as double click
	#define DOUBLE_CLICK_TIME 0.5

	//Keyboard keys
	enum class Key
	{
		UNKNOWN = -1,
		SPACE = 0,
		APOSTROPHE, /* ' */
		COMMA,      /* , */
		MINUS,      /* - */
		PERIOD,     /* . */
		SLASH,      /* / */
		_0,			/* Top Row Numbers*/
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		_9,
		SEMICOLON,	/* ; */
		EQUAL,		/* = */
		_A,
		_B,
		_C,
		_D,
		_E,
		_F,
		_G,
		_H,
		_I,
		_J,
		_K,
		_L,
		_M,
		_N,
		_O,
		_P,
		_Q,
		_R,
		_S,
		_T,
		_U,
		_V,
		_W,
		_X,
		_Y,
		_Z,
		LEFT_BRACKET,	/* [ */
		BACKSLASH,		/* \ */
		RIGHT_BRACKET,	/* ] */
		GRAVE_ACCENT,	/* ` */
		WORLD_1,		/* non-US #1 */
		WORLD_2,		/* non-US #2 */
		ESCAPE,
		ENTER,
		TAB,
		BACKSPACE,
		INSERT,
		DEL,
		RIGHT,
		LEFT,
		DOWN,
		UP,
		PAGE_UP,
		PAGE_DOWN,
		HOME,
		END,
		CAPS_LOCK,
		SCROLL_LOCK,
		NUM_LOCK,
		PRINT_SCREEN,
		PAUSE,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		KP_0,	/* Key Pad Numbers*/
		KP_1,
		KP_2,
		KP_3,
		KP_4,
		KP_5,
		KP_6,
		KP_7,
		KP_8,
		KP_9,
		KP_DECIMAL,
		KP_DIVIDE,
		KP_MULTIPLY,
		KP_SUBTRACT,
		KP_ADD,
		KP_ENTER,
		KP_EQUAL,
		LEFT_SHIFT,
		LEFT_CONTROL,
		LEFT_ALT,
		LEFT_SUPER,
		RIGHT_SHIFT,
		RIGHT_CONTROL,
		RIGHT_ALT,
		RIGHT_SUPER,		//Window button

		KEY_LAST
	};

	//Mouse buttons
	enum class Mouse
	{
		MOUSE_1 = 0,
		MOUSE_2,
		MOUSE_3,
		MOUSE_4,
		MOUSE_5,
		MOUSE_6,
		MOUSE_7,
		MOUSE_8,
		MOUSE_LAST,

		MOUSE_LEFT = MOUSE_1,
		MOUSE_RIGHT = MOUSE_2,
		MOUSE_MIDDLE = MOUSE_3,

	};
}

#endif