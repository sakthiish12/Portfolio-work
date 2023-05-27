#include "pch.h"
#include "InputController.h"
#include "../RendererSystem/RendererSystem.h"

namespace BE
{
	std::unordered_map<int, Key> InputController::m_KeyCodeMap;
	std::unordered_map<int, Mouse> InputController::m_MouseButtonMap;
	
	InputController::InputController()
	{
		//Subscribe to GLFW input polling
		EventMgr->Subscribe(KeyButtonEvent, this, InputController::KeyButtonHandling);
		EventMgr->Subscribe(MouseButtonEvent, this, InputController::MouseButtonHandling);
		EventMgr->Subscribe(MouseMoveEvent, this, InputController::MouseMoveHandling);
		EventMgr->Subscribe(MouseScrollEvent, this, InputController::MouseScrollHandling);

		//Map key code and mouse button from GLFW to our version (compact)
#pragma region Key and Mouse Codes
		//Initialise the mouse button mapping
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_1]	= Mouse::MOUSE_1;		//Left button
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_2]	= Mouse::MOUSE_2;		//Right button
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_3]	= Mouse::MOUSE_3;		//Middle button
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_4]	= Mouse::MOUSE_4;
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_5]	= Mouse::MOUSE_5;
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_6]	= Mouse::MOUSE_6;
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_7]	= Mouse::MOUSE_7;
		m_MouseButtonMap[GLFW_MOUSE_BUTTON_8]	= Mouse::MOUSE_8;

		//Converts from GLFW Mouse button value to our enum class version of mouse button value (safer)
		m_KeyCodeMap[GLFW_KEY_SPACE]			= Key::SPACE;			
		m_KeyCodeMap[GLFW_KEY_APOSTROPHE]		= Key::APOSTROPHE;		
		m_KeyCodeMap[GLFW_KEY_COMMA]			= Key::COMMA;			
		m_KeyCodeMap[GLFW_KEY_MINUS]			= Key::MINUS;			
		m_KeyCodeMap[GLFW_KEY_PERIOD]			= Key::PERIOD;			
		m_KeyCodeMap[GLFW_KEY_SLASH]			= Key::SLASH;			
		m_KeyCodeMap[GLFW_KEY_0]				= Key::_0;				
		m_KeyCodeMap[GLFW_KEY_1]				= Key::_1;				
		m_KeyCodeMap[GLFW_KEY_2]				= Key::_2;				
		m_KeyCodeMap[GLFW_KEY_3]				= Key::_3;				
		m_KeyCodeMap[GLFW_KEY_4]				= Key::_4;				
		m_KeyCodeMap[GLFW_KEY_5]				= Key::_5;				
		m_KeyCodeMap[GLFW_KEY_6]				= Key::_6;				
		m_KeyCodeMap[GLFW_KEY_7]				= Key::_7;				
		m_KeyCodeMap[GLFW_KEY_8]				= Key::_8;				
		m_KeyCodeMap[GLFW_KEY_9]				= Key::_9;				
		m_KeyCodeMap[GLFW_KEY_SEMICOLON]		= Key::SEMICOLON;		
		m_KeyCodeMap[GLFW_KEY_EQUAL]			= Key::EQUAL;			
		m_KeyCodeMap[GLFW_KEY_A]				= Key::_A;				
		m_KeyCodeMap[GLFW_KEY_B]				= Key::_B;				
		m_KeyCodeMap[GLFW_KEY_C]				= Key::_C;				
		m_KeyCodeMap[GLFW_KEY_D]				= Key::_D;				
		m_KeyCodeMap[GLFW_KEY_E]				= Key::_E;				
		m_KeyCodeMap[GLFW_KEY_F]				= Key::_F;				
		m_KeyCodeMap[GLFW_KEY_G]				= Key::_G;				
		m_KeyCodeMap[GLFW_KEY_H]				= Key::_H;				
		m_KeyCodeMap[GLFW_KEY_I]				= Key::_I;				
		m_KeyCodeMap[GLFW_KEY_J]				= Key::_J;				
		m_KeyCodeMap[GLFW_KEY_K]				= Key::_K;				
		m_KeyCodeMap[GLFW_KEY_L]				= Key::_L;				
		m_KeyCodeMap[GLFW_KEY_M]				= Key::_M;				
		m_KeyCodeMap[GLFW_KEY_N]				= Key::_N;				
		m_KeyCodeMap[GLFW_KEY_O]				= Key::_O;				
		m_KeyCodeMap[GLFW_KEY_P]				= Key::_P;				
		m_KeyCodeMap[GLFW_KEY_Q]				= Key::_Q;				
		m_KeyCodeMap[GLFW_KEY_R]				= Key::_R;				
		m_KeyCodeMap[GLFW_KEY_S]				= Key::_S;				
		m_KeyCodeMap[GLFW_KEY_T]				= Key::_T;				
		m_KeyCodeMap[GLFW_KEY_U]				= Key::_U;				
		m_KeyCodeMap[GLFW_KEY_V]				= Key::_V;				
		m_KeyCodeMap[GLFW_KEY_W]				= Key::_W;				
		m_KeyCodeMap[GLFW_KEY_X]				= Key::_X;				
		m_KeyCodeMap[GLFW_KEY_Y]				= Key::_Y;				
		m_KeyCodeMap[GLFW_KEY_Z]				= Key::_Z;				
		m_KeyCodeMap[GLFW_KEY_LEFT_BRACKET]		= Key::LEFT_BRACKET;	
		m_KeyCodeMap[GLFW_KEY_BACKSLASH]		= Key::BACKSLASH;		
		m_KeyCodeMap[GLFW_KEY_RIGHT_BRACKET]	= Key::RIGHT_BRACKET;	
		m_KeyCodeMap[GLFW_KEY_GRAVE_ACCENT]		= Key::GRAVE_ACCENT;	
		m_KeyCodeMap[GLFW_KEY_WORLD_1]			= Key::WORLD_1;			
		m_KeyCodeMap[GLFW_KEY_WORLD_2]			= Key::WORLD_2;			
		m_KeyCodeMap[GLFW_KEY_ESCAPE]			= Key::ESCAPE;			
		m_KeyCodeMap[GLFW_KEY_ENTER]			= Key::ENTER;			
		m_KeyCodeMap[GLFW_KEY_TAB]				= Key::TAB;				
		m_KeyCodeMap[GLFW_KEY_BACKSPACE]		= Key::BACKSPACE;		
		m_KeyCodeMap[GLFW_KEY_INSERT]			= Key::INSERT;			
		m_KeyCodeMap[GLFW_KEY_DELETE]			= Key::DEL;			
		m_KeyCodeMap[GLFW_KEY_RIGHT]			= Key::RIGHT;			
		m_KeyCodeMap[GLFW_KEY_LEFT]				= Key::LEFT;			
		m_KeyCodeMap[GLFW_KEY_DOWN]				= Key::DOWN;			
		m_KeyCodeMap[GLFW_KEY_UP]				= Key::UP;				
		m_KeyCodeMap[GLFW_KEY_PAGE_UP]			= Key::PAGE_UP;			
		m_KeyCodeMap[GLFW_KEY_PAGE_DOWN]		= Key::PAGE_DOWN;		
		m_KeyCodeMap[GLFW_KEY_HOME]				= Key::HOME;			
		m_KeyCodeMap[GLFW_KEY_END]				= Key::END;				
		m_KeyCodeMap[GLFW_KEY_CAPS_LOCK]		= Key::CAPS_LOCK;		
		m_KeyCodeMap[GLFW_KEY_SCROLL_LOCK]		= Key::SCROLL_LOCK;		
		m_KeyCodeMap[GLFW_KEY_NUM_LOCK]			= Key::NUM_LOCK;		
		m_KeyCodeMap[GLFW_KEY_PRINT_SCREEN]		= Key::PRINT_SCREEN;	
		m_KeyCodeMap[GLFW_KEY_PAUSE]			= Key::PAUSE;			
		m_KeyCodeMap[GLFW_KEY_F1]				= Key::F1;				
		m_KeyCodeMap[GLFW_KEY_F2]				= Key::F2;				
		m_KeyCodeMap[GLFW_KEY_F3]				= Key::F3;				
		m_KeyCodeMap[GLFW_KEY_F4]				= Key::F4;				
		m_KeyCodeMap[GLFW_KEY_F5]				= Key::F5;				
		m_KeyCodeMap[GLFW_KEY_F6]				= Key::F6;				
		m_KeyCodeMap[GLFW_KEY_F7]				= Key::F7;				
		m_KeyCodeMap[GLFW_KEY_F8]				= Key::F8;				
		m_KeyCodeMap[GLFW_KEY_F9]				= Key::F9;				
		m_KeyCodeMap[GLFW_KEY_F10]				= Key::F10;				
		m_KeyCodeMap[GLFW_KEY_F11]				= Key::F11;				
		m_KeyCodeMap[GLFW_KEY_F12]				= Key::F12;				
		m_KeyCodeMap[GLFW_KEY_F13]				= Key::F13;				
		m_KeyCodeMap[GLFW_KEY_F14]				= Key::F14;				
		m_KeyCodeMap[GLFW_KEY_F15]				= Key::F15;				
		m_KeyCodeMap[GLFW_KEY_F16]				= Key::F16;				
		m_KeyCodeMap[GLFW_KEY_F17]				= Key::F17;				
		m_KeyCodeMap[GLFW_KEY_F18]				= Key::F18;				
		m_KeyCodeMap[GLFW_KEY_F19]				= Key::F19;				
		m_KeyCodeMap[GLFW_KEY_F20]				= Key::F20;				
		m_KeyCodeMap[GLFW_KEY_F21]				= Key::F21;				
		m_KeyCodeMap[GLFW_KEY_F22]				= Key::F22;				
		m_KeyCodeMap[GLFW_KEY_F23]				= Key::F23;				
		m_KeyCodeMap[GLFW_KEY_F24]				= Key::F24;				
		m_KeyCodeMap[GLFW_KEY_F25]				= Key::F25;				
		m_KeyCodeMap[GLFW_KEY_KP_0]				= Key::KP_0;			
		m_KeyCodeMap[GLFW_KEY_KP_1]				= Key::KP_1;			
		m_KeyCodeMap[GLFW_KEY_KP_2]				= Key::KP_2;			
		m_KeyCodeMap[GLFW_KEY_KP_3]				= Key::KP_3;			
		m_KeyCodeMap[GLFW_KEY_KP_4]				= Key::KP_4;			
		m_KeyCodeMap[GLFW_KEY_KP_5]				= Key::KP_5;			
		m_KeyCodeMap[GLFW_KEY_KP_6]				= Key::KP_6;			
		m_KeyCodeMap[GLFW_KEY_KP_7]				= Key::KP_7;			
		m_KeyCodeMap[GLFW_KEY_KP_8]				= Key::KP_8;			
		m_KeyCodeMap[GLFW_KEY_KP_9]				= Key::KP_9;			
		m_KeyCodeMap[GLFW_KEY_KP_DECIMAL]		= Key::KP_DECIMAL;		
		m_KeyCodeMap[GLFW_KEY_KP_DIVIDE]		= Key::KP_DIVIDE;		
		m_KeyCodeMap[GLFW_KEY_KP_MULTIPLY]		= Key::KP_MULTIPLY;		
		m_KeyCodeMap[GLFW_KEY_KP_SUBTRACT]		= Key::KP_SUBTRACT;		
		m_KeyCodeMap[GLFW_KEY_KP_ADD]			= Key::KP_ADD;			
		m_KeyCodeMap[GLFW_KEY_KP_ENTER]			= Key::KP_ENTER;		
		m_KeyCodeMap[GLFW_KEY_KP_EQUAL]			= Key::KP_EQUAL;		
		m_KeyCodeMap[GLFW_KEY_LEFT_SHIFT]		= Key::LEFT_SHIFT;		
		m_KeyCodeMap[GLFW_KEY_LEFT_CONTROL]		= Key::LEFT_CONTROL;	
		m_KeyCodeMap[GLFW_KEY_LEFT_ALT]			= Key::LEFT_ALT;		
		m_KeyCodeMap[GLFW_KEY_LEFT_SUPER]		= Key::LEFT_SUPER;		
		m_KeyCodeMap[GLFW_KEY_RIGHT_SHIFT]		= Key::RIGHT_SHIFT;		
		m_KeyCodeMap[GLFW_KEY_RIGHT_CONTROL]	= Key::RIGHT_CONTROL;	
		m_KeyCodeMap[GLFW_KEY_RIGHT_ALT]		= Key::RIGHT_ALT;		
		m_KeyCodeMap[GLFW_KEY_RIGHT_SUPER]		= Key::RIGHT_SUPER;		
#pragma endregion
		LogCore("Created Input controller");
	}


	InputController::~InputController()
	{
		LogCore("Destroyed Input controller");
	}

	void InputController::Init()
	{
		//Init the arrays of keyboard key/mouse pressed
		m_pressedKey	 = { 0 };
		m_triggeredKey	 = { 0 };
		m_releasedKey	 = { 0 };
		m_pressedMouse	 = { 0 };
		m_triggeredMouse = { 0 };
		m_releasedMouse	 = { 0 };

		m_currMousePos = Vec3f{};
		m_prevMousePos = Vec3f{};
		m_scrollOffset = Vec3f{};
		m_leftClickTimeRemaining = 0.0;

		m_inputCtrlFlags.reset();
	}

	void InputController::Awake()
	{
		Init();
	}

	void InputController::Update(double dt)
	{
		//Handles keyboard keys - making sure triggered/released are single frame, and updates pressed status
		for (size_t i = 0; i < m_pressedKey.size(); ++i)
		{
			//If the button wasn't pressed but is triggered, then set pressed to true
			if (!m_pressedKey[i] && m_triggeredKey[i])
				m_pressedKey[i] = true;
			//If the button was pressed and triggered, then set triggered to false
			else if (m_pressedKey[i] && m_triggeredKey[i])
				m_triggeredKey[i] = false;

			//If the button was pressed and released, then set pressed to false
			if (m_pressedKey[i] && m_releasedKey[i])
				m_pressedKey[i] = false;
			//If the button wasn't pressed but is released, then set released to false
			else if (!m_pressedKey[i] && m_releasedKey[i])
				m_releasedKey[i] = false;

			//If a button is triggered and released at the same time, reset it's state
			//We'll treat it as if the button isn't pressed
			if (m_triggeredKey[i] && m_releasedKey[i])
			{
				m_pressedKey[i] = false;
				m_triggeredKey[i] = false;
				m_releasedKey[i] = false;
			}
		}

		//Handles mouse buttons - making sure triggered/released are single frame, and updates pressed status
		for (size_t i = 0; i < m_pressedMouse.size(); ++i)
		{
			//If the button wasn't pressed but is triggered, then set pressed to true
			if (!m_pressedMouse[i] && m_triggeredMouse[i])
				m_pressedMouse[i] = true;
			//If the button was pressed and triggered, then set triggered to false
			else if (m_pressedMouse[i] && m_triggeredMouse[i])
				m_triggeredMouse[i] = false;

			//If the button was pressed and released, then set pressed to false
			if (m_pressedMouse[i] && m_releasedMouse[i])
				m_pressedMouse[i] = false;
			//If the button wasn't pressed but is released, then set released to false
			else if (!m_pressedMouse[i] && m_releasedMouse[i])
				m_releasedMouse[i] = false;

			//If a button is triggered and released at the same time, reset it's state
			//We'll treat it as if the button isn't pressed
			if (m_triggeredMouse[i] && m_releasedMouse[i])
			{
				m_pressedMouse[i] = false;
				m_triggeredMouse[i] = false;
				m_releasedMouse[i] = false;
			}
		}

		//This is used for single frame update of the previous mouse cursor position
		//If the cursor is moving, then set to false so in the next frame, the else part can run
		//But if the mouse is still moving, the event handling function will set cursor is moving to true
		//Only when the mouse no longer moves, in the next frame then set the prev mouse position to be current mouse position
		if (m_inputCtrlFlags[(size_t)InputCtrlFlag::cursorMoving])
			m_inputCtrlFlags[(size_t)InputCtrlFlag::cursorMoving] = false;
		else
			m_prevMousePos = m_currMousePos;

		//This is used for single frame update of the mouse scroll wheel offset.
		//If the scroll wheel is rolling, then set isScrolling to false so in the next frame, the else part can run
		//But if the scroll wheel keeps rolling, the event handling function will set is scrolling to true
		//Only when the scroll wheel is no longer rolling, in the next frame, then set the offset to be 0,0,0.
		if (m_inputCtrlFlags[(size_t)InputCtrlFlag::isScrolling])
			m_inputCtrlFlags[(size_t)InputCtrlFlag::isScrolling] = false;
		else
			m_scrollOffset = Vec3f{};

		//Handling of double clicks
		if (m_leftClickTimeRemaining > 0.0 && m_inputCtrlFlags[(size_t)InputCtrlFlag::justDoubleClicked])
		{
			//Reset the time remaining for a double click
			m_leftClickTimeRemaining = 0.0;
		}
		else if (m_leftClickTimeRemaining <= 0.0 && m_inputCtrlFlags[(size_t)InputCtrlFlag::justDoubleClicked])
		{
			//Disable double click after it was true for 1 frame
			m_inputCtrlFlags[(size_t)InputCtrlFlag::justDoubleClicked] = false;
		}
		else if (m_leftClickTimeRemaining > 0.0 && !m_inputCtrlFlags[(size_t)InputCtrlFlag::justDoubleClicked])
		{
			//If no double click but there's time remaining, subtract that time by dt
			m_leftClickTimeRemaining -= dt;
		}

		//Handling of fullscreen
		if (m_triggeredKey[(size_t)m_KeyCodeMap[GLFW_KEY_F11]] == true)
		{
			int fullscreen = glfwGetWindowMonitor(window) != nullptr;
			GLFWmonitor* primary = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primary);

			if (fullscreen)
			{
				glfwSetWindowMonitor(window, NULL, 20,20, mode->width, mode->height, mode->refreshRate);
			}
			else
			{
				glfwSetWindowMonitor(window, primary, 0, 0, mode->width, mode->height, mode->refreshRate);
			}

			GameEngine::GetInstance()->SetIsFullScreen(!fullscreen);

			WindowResizeEvent wre(mode->width,  mode->height);
			EventMgr->Dispatch(wre);
		}

		// For locking mouse within window
		double mouseX{}, mouseY{};
		glfwGetCursorPos(window, &mouseX, &mouseY);

		int xPos{}, yPos{};
		glfwGetWindowPos(window, &xPos, &yPos);

		int width{}, height{};
		glfwGetWindowSize(window, &width, &height);

		if (mouseX < 0)
			mouseX = 0;
		if (mouseY < 0)
			mouseY = 0;
		if (mouseX > width)
			mouseX = width;
		if (mouseY > height)
			mouseY = height;

		glfwSetCursorPos(window,static_cast<double>(mouseX), static_cast<double>(mouseY));	
	}

	void InputController::Reload()
	{
		Init();
	}

	void InputController::Shutdown()
	{

	}

	void InputController::KeyButtonHandling(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::keyButton, "Wrong event! Expected KeyButtonEvent, given \"%s\"", e.GetEventName());
		
		KeyButtonEvent& kb = dynamic_cast<KeyButtonEvent&>(e);

		//If it's a repeat event, then just stop
		if (kb.GetAction() == GLFW_REPEAT)
			return;

		if (kb.GetAction() == GLFW_PRESS)
			m_triggeredKey[(size_t)m_KeyCodeMap[kb.GetKey()]] = true;
		else if (kb.GetAction() == GLFW_RELEASE)
			m_releasedKey[(size_t)m_KeyCodeMap[kb.GetKey()]] = true;
		else
			LogCoreError("Unknown key input action detected");
	}

	void InputController::MouseButtonHandling(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::mouseButton, "Wrong event! Expected MouseButtonEvent, given \"%s\"", e.GetEventName());

		MouseButtonEvent& mb = dynamic_cast<MouseButtonEvent&>(e);

		//If it's a repeat event, then just stop
		if (mb.GetAction() == GLFW_REPEAT)
			return;

		if (mb.GetAction() == GLFW_PRESS)
		{
			m_triggeredMouse[(size_t)m_MouseButtonMap[mb.GetKey()]] = true;

			//If it's left click, then also process double click event
			if (m_MouseButtonMap[mb.GetKey()] == Mouse::MOUSE_LEFT)
			{
				//First click
				if (m_leftClickTimeRemaining <= 0.0)
					m_leftClickTimeRemaining = DOUBLE_CLICK_TIME;
				//Second click within the time frame
				else
					m_inputCtrlFlags[(size_t)InputCtrlFlag::justDoubleClicked] = true;
			}
		}
		else if (mb.GetAction() == GLFW_RELEASE)
			m_releasedMouse[(size_t)m_MouseButtonMap[mb.GetKey()]] = true;
		else
			LogCoreError("Unknown mouse input action detected");
	}

	void InputController::MouseMoveHandling(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::mouseMove, "Wrong event! Expected MouseMoveEvent, given \"%s\"", e.GetEventName());

		MouseMoveEvent& mm = dynamic_cast<MouseMoveEvent&>(e);
		m_prevMousePos = m_currMousePos;
		m_currMousePos = mm.GetNewPos();
		m_inputCtrlFlags[(size_t)InputCtrlFlag::cursorMoving] = true;
	}

	void InputController::MouseScrollHandling(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::mouseScroll, "Wrong event! Expected MouseScrollEvent, given \"%s\"", e.GetEventName());

		MouseScrollEvent& ms = dynamic_cast<MouseScrollEvent&>(e);

		m_scrollOffset = ms.GetOffset();
		m_inputCtrlFlags[(size_t)InputCtrlFlag::isScrolling] = true;
	}

	void InputController::passGLFWwindow(GLFWwindow* newWindow)
	{
		window = newWindow;
	}

	void InputController::SetEnableCursor(bool _b) const
	{
		glfwSetInputMode(RendererSystem::GetInstance()->GetGLFWWindow(), GLFW_CURSOR, _b ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	bool InputController::IsCursorEnabled() const
	{
		return glfwGetInputMode(RendererSystem::GetInstance()->GetGLFWWindow(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
	}
}