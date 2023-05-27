// /******************************************************************************/
// /*!
// \file         WinMgr.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         13rd Apr, 2022
// \brief        WindowsManage creates windows and create device context for renderer
// \brief        This file also handle custom cursor, icon, and drag and drop of files
// \lines        378 Lines (episode)
// \lines        684 Lines (gold)
// 
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "WinMgr.h"
#include "../InputMgr/InputMgr.h"
#include "Resource/Resource.h"
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_PlatformWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "../EditorStateManager/gameStateManager.h"
#include "../Dependencies/Vendor/imgui/imgui_impl_win32.h"
#include "../Serializer/Settings/WindowSetting.h"
#include "../Serializer/SerializeManager.h"
#include "../../Graphics/Source/PostProcessing.h"
#include "../../Graphics/Source/GraphicsApplication.h"	//for the graphics system 
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//#pragma warning( disable : C28251 )
//Uncomment this for debugging
//#define COUT_FOR_WINDOWS

//#include "../Sound/SoundManager.h"

//define the size xy for the custom cursor
//#define CCursorSize 32
#define CCursorSizeX 32
#define CCursorSizeY 32

WindowSettings windowSettings;
SerializeManager serializeManager;

#ifdef COUT_FOR_WINDOWS
	#include <iostream>
	using std::cout;
	using std::endl;
#endif

constexpr const wchar_t* CustomIconFile = L".\\Resources\\textures\\CustomIcon\\KettleHero.ico";
constexpr const char* CustomCursorNormalFile= ".\\Resources\\textures\\CustomCursor\\Spatula128x128Normal.cur";
constexpr const char* CustomCursorClickedFile= ".\\Resources\\textures\\CustomCursor\\Spatula128x128Clicked.cur";

wchar_t* ConvertCharP_TO_WCharP(const char* value)
{
	size_t newsize = strlen(value) + 1;
	//wchar_t* = path;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, value, _TRUNCATE);

	// Display the result and indicate the type of string that it is.
	//std::wcout << wcstring << _T(" (wchar_t *)") << std::endl;
	return wcstring;
}

WinMgr::WinMgr() :
	hInstance{ GetModuleHandle(NULL) },
	nCmdShow{ SW_NORMAL },
	szWindowClass{ _T("DesktopApp") },
	szTitle{ _T("PANdemic Application") },
	hWnd{ nullptr },
	width{ 1280 },
	height{ 800 },
	width_f{ static_cast<float>(width) },
	height_f{ static_cast<float>(height) },
	currentRect{},
	fullscreen_state{},
	prevFSRect{},
	prevFSStyle{},
	title{"Gam200"}
{
	serializeManager.DeserializeFromFile("Config/WindowSettings.json", windowSettings);

	//probably need to do setter for width and height and auto update...
	//do that so that can read in
	width = windowSettings.m_windowSize.x;
	height = windowSettings.m_windowSize.y;
	width_f = static_cast<float>(width);
	height_f =static_cast<float>(height);
	title = windowSettings.m_windowTitle;
}

WinMgr::~WinMgr()
{
	//saving to file;
	serializeManager.SerializeToFile("Config/WindowSettings.json", windowSettings);
	
	if (m_wglDC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			//issue
		}
	}

	if (!wglDeleteContext(m_wglDC))
	{
		//issue
	}

	m_wglDC = NULL;

	if (m_windowDC && !ReleaseDC(hWnd, m_windowDC))
	{
		m_windowDC = NULL;
	}
	UnregisterClass(szWindowClass, hInstance);
	if (hIcon != nullptr)
	{
		//"Unuse" the icon then destory
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, NULL);
		SendMessage(hWnd, WM_SETICON, ICON_BIG, NULL);

		DestroyIcon(reinterpret_cast<HICON>(hIcon));
	}
}

int WinMgr::Init()
{
	custom_cursor.LoadCustomCursor(CustomCursorNormalFile, 0, 0);
	custom_cursor.LoadCustomCursorClicked(CustomCursorClickedFile, 0, 0);

	/*Window Class Creation*/
	WNDCLASSEX wcex = { 0 };

	
	wcex.cbSize = sizeof(WNDCLASSEX); //size of this class 
	wcex.style = CS_HREDRAW | CS_VREDRAW; //style 
	wcex.lpfnWndProc = WndProc; //pointer to window procedure (handles events) 
	wcex.cbClsExtra = 0; //Additional bytes to alloc after the window-class structure
	wcex.cbWndExtra = 0; //Additional bytes to alloc after the window instance
	wcex.hInstance = hInstance; //handle to instance that contain this window procedure 
	//wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_TESTCURSORPROJECT)); //handle to class icon 
	//wcex.hCursor = LoadCursor(NULL, IDC_ARROW); //seems to be default cursor.. possibly
	wcex.hCursor = custom_cursor.GetCustomCursor(); //handle to cursor
	//wcex.hCursor = (HCURSOR)LoadImage(NULL, L".\\sampleCursor.ico", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //handle to background brush
	wcex.lpszMenuName = NULL; //pointer to resources name of menu
	wcex.lpszClassName = szWindowClass; //this class name
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));//handle to icon
	//wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Error in WNDCLASSEX"), NULL); 
		return 1; // error
	}

	//WS_CLIPCHILDREN: Excludes the area occupied by child windows when drawing occurs within the parent window
	//WS_CLIPSIBLINGS: Same for child windows - relative to each other.
	//DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	dwStyle &= ~WS_SIZEBOX;
	//dwStyle &= ~WS_BORDER;
	//dwStyle &= ~WS_MAXIMIZEBOX;

	RECT rect = { 0, 0, (LONG)(width), (LONG)(height) };
	//GetClientRect();
	//The AdjustWindowRect sets the exact client area without the title bar and all the extra pixels
	//This will give us the exact resolution for the white rectangular area
	//std::cout << "rect_start " << rect.left - rect.right << ", " << rect.bottom - rect.top << std::endl;
	//AdjustWindowRectEx(&rect, dwStyle, FALSE, WS_EX_APPWINDOW);
	AdjustWindowRectEx(&rect, dwStyle, FALSE, NULL);
	//std::cout << "rect_after_adjust window rect ex " << rect.left - rect.right << ", " << rect.bottom - rect.top << std::endl;
	//hWnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW, szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL );
	hWnd = CreateWindowEx(NULL, szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL );

	if (!hWnd) {
		MessageBox(NULL, _T("Call to CreateWindowEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}
	/*Window Class Creation END*/

	//GetClientRect(hWnd, &prevFSRect);
	//GetWindowRect(hWnd, &prevFSRectWindowed);
	//std::cout << "GetWindowRect " << prevFSRectWindowed.left - prevFSRectWindowed.right << ", " << prevFSRectWindowed.bottom - prevFSRectWindowed.top << std::endl;
	//std::cout << "GetClientRect " << prevFSRect.left - prevFSRect.right << ", " << prevFSRect.bottom - prevFSRect.top << std::endl;

	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);
	prevFSStyle = GetWindowLong(hWnd, GWL_STYLE);
	//Hmm what does this do??
	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENGLTUTORIAL01));

	hIcon = LoadImage(NULL, CustomIconFile, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hIcon) {
		//Change both icons to the same icon handle.
		LPARAM hIcon_lparam = reinterpret_cast<LPARAM>(hIcon);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, hIcon_lparam);
		SendMessage(hWnd, WM_SETICON, ICON_BIG, hIcon_lparam);

		//This will ensure that the application icon gets changed too.
		SendMessage(GetWindow(hWnd, GW_OWNER), WM_SETICON, ICON_SMALL, hIcon_lparam);
		SendMessage(GetWindow(hWnd, GW_OWNER), WM_SETICON, ICON_BIG, hIcon_lparam);
	}
	return 0;
}

int WinMgr::Update(float dt)
{
	if (MouseManager.isButtonTriggered(MouseButton::LEFT_BUTTON) == true)
	{
		SetCursor(WindowsManager.custom_cursor.hCursorClicked);
	}
	if (MouseManager.isButtonReleased(MouseButton::LEFT_BUTTON) == true)
	{
		SetCursor(custom_cursor.GetCustomCursor());
	}
	
	//std::cout << "width" << width_f << std::endl;
	//std::cout << "height" << height_f << std::endl;
	if (GetActiveWindow())
	{
		isWindowIsFocus = true;
	}
	else
	{
		isWindowIsFocus = false;
	}
	//Update title -> !!!If can receive update of debug mode/editor mode
	//update this once, try
	updateTitle(dt);

	MSG msg;
	//Use PeekMessage w/ PM_REMOVE to update without waiting as GetMessage is a blocking call
	//Also, PeakMessage/GetMessage removing a message from its queue
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		//Translates virtual-key messages into character messages.
		TranslateMessage(&msg);
		//Forward message to window procedure
		DispatchMessage(&msg);
#ifdef COUT_FOR_WINDOWS
		cout << "looping - message dispatched" << endl;
#endif
	}

	return (int)msg.wParam;
}

void WinMgr::setTitle(const char* newTitle)
{
	//Update string
	title = newTitle;
}

void WinMgr::setFullscreenState(bool newState)
{
	if (newState != fullscreen_state)
	{
		fullscreen_state = newState;
		if (fullscreen_state)
		{
			Fullscreen();
		}
		else
		{
			ReturnToWindowed();
		}
	}
}

int WinMgr::getWidth() const
{
	return width;
}

int WinMgr::getHeight() const
{
	return height;
}

float WinMgr::getWidthF() const
{
	return width_f;
}

float WinMgr::getHeightF() const
{
	return height_f;
}

HWND WinMgr::getHWnd() const
{
	return HWND(hWnd);
}

HINSTANCE WinMgr::getHInstance() const
{
	return HINSTANCE(hInstance);
}

int WinMgr::getNCmdShow() const
{
	return nCmdShow;
}

bool WinMgr::getFullscreenState() const
{
	return fullscreen_state;
}

size_t WinMgr::GetMonitorWidthResolution() const
{
	return static_cast<size_t>(GetSystemMetrics(SM_CXSCREEN));
}

size_t WinMgr::GetMonitorHeightResolution() const
{
	return static_cast<size_t>(GetSystemMetrics(SM_CYSCREEN));
}

float WinMgr::GetAspectRatio() const
{
	return static_cast<float>(height) / width;
}



bool WinMgr::InitializeRenderingEnvironment()
{
	//create rendering window
	m_windowDC = GetDC(getHWnd());

	DEVMODE devMode = { 0 };
	devMode.dmSize = sizeof(DEVMODE);
	BOOL b = EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devMode);
	if (b == 0)
		return false;

	//drawing surface format
	PIXELFORMATDESCRIPTOR pfdesc;
	memset(&pfdesc, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfdesc.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Always sizeof(PIXELFORMATDESCRIPTOR)
	pfdesc.nVersion = 1; //Always 1
	pfdesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER; //Style
	pfdesc.iPixelType = PFD_TYPE_RGBA;
	pfdesc.cColorBits = (BYTE)devMode.dmBitsPerPel;//32; //24 bit color for front and back buffer
	pfdesc.cDepthBits = 24;//24 bit depth buffer - not used in this demo
	pfdesc.cStencilBits = 8; //8 bit stencil buffer - not used in this demo

	int pf = ChoosePixelFormat(m_windowDC, &pfdesc);//checks if the graphics card can support the pixel format requested
	if (pf == 0)
	{
		ReleaseDC(getHWnd(), m_windowDC);
		return false;
	}

	BOOL ok = SetPixelFormat(m_windowDC, pf, &pfdesc);
	if (!ok)
	{
		ReleaseDC(getHWnd(), m_windowDC);
		return false;
	}
	DescribePixelFormat(m_windowDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfdesc);
	//set the OpenGL context
	m_wglDC = wglCreateContext(m_windowDC);
	if (!m_wglDC)
	{
		ReleaseDC(getHWnd(), m_windowDC);
		return false;
	}

	ok = wglMakeCurrent(m_windowDC, m_wglDC);
	if (!ok)
	{
		wglDeleteContext(m_wglDC);
		ReleaseDC(getHWnd(), m_windowDC);
		return false;
	}
	ShowWindow(hWnd, nCmdShow);

	return true;
}

bool WinMgr::GetIsQuiting() const {
	return toQuit;//returns if windows is quiting 
}

bool WinMgr::getWindowIsFocus()
{
	return isWindowIsFocus;
}

void WinMgr::enableDragAndDrop()
{
	dragAndDrop = true;
	DragAcceptFiles(hWnd, TRUE);
}

void WinMgr::disableDragAndDrop()
{
	dragAndDrop = false;
	DragAcceptFiles(hWnd, FALSE);
}

bool WinMgr::getisDragAndDropped()
{
	return isDragAndDropped;
}

void WinMgr::setisDragAndDropped(bool state)
{
	isDragAndDropped = state;
}

bool WinMgr::getIsWindowActive()
{
	return windowIsActive;
}

void WinMgr::setIsWindowActive(bool state)
{
	windowIsActive = state;
}

void WinMgr::SetIsQuiting(bool toggle) {
	toQuit = toggle;//set the status 
}

void WinMgr::ClearRenderingEnviroment()
{
	if (m_wglDC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			//issue
		}
	}

	if (!wglDeleteContext(m_wglDC))
	{
		//issue
	}
	m_wglDC = NULL;

	if (m_windowDC && !ReleaseDC(getHWnd(), m_windowDC))
	{
		m_windowDC = NULL;
	}
}

HDC WinMgr::getWindowDC() const
{
	return m_windowDC;
}

void WinMgr::HideConsole()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
}

void WinMgr::ShowConsole()
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);
}

bool WinMgr::IsConsoleVisible()
{
	return IsWindowVisible(GetConsoleWindow()) != FALSE;
}

bool WinMgr::isMinimised()
{
	return IsIconic(hWnd);
}

void WinMgr::Minimise()
{
	ShowWindow(hWnd, SW_MINIMIZE);
}

void WinMgr::Fullscreen()
{
	//save windowed style
	prevFSStyle = GetWindowLong(hWnd, GWL_STYLE);
	//prevFSStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	//GetWindowRect(hWnd, &prevFSRect);
	
	GetClientRect(hWnd, &prevFSRect);
	GetWindowRect(hWnd, &prevFSRectWindowed);
	//std::cout << "GetWindowRect " << prevFSRectWindowed.left - prevFSRectWindowed.right << ", " << prevFSRectWindowed.bottom - prevFSRectWindowed.top << std::endl;
	//std::cout << "GetClientRect " << prevFSRect.left - prevFSRect.right << ", " << prevFSRect.bottom - prevFSRect.top << std::endl;
	
	//std::cout << std::endl; std::cout << std::endl; std::cout << std::endl; std::cout << std::endl;

	// Set new window style and size.
	SetWindowLong(hWnd, GWL_STYLE, prevFSStyle & ~(WS_CAPTION | WS_THICKFRAME | WS_BORDER));

	ResizeWindow(0, 0, static_cast<int>(GetMonitorWidthResolution()), static_cast<int>(GetMonitorHeightResolution()));
}

void WinMgr::ReturnToWindowed()
{
	SetWindowLong(hWnd, GWL_STYLE, prevFSStyle);
	//GetClientRect(hWnd, &prevFSRect);
	//GetWindowRect(hWnd, &prevFSRectWindowed);
//	std::cout << "GetWindowRect " << prevFSRectWindowed.left - prevFSRectWindowed.right << ", " << prevFSRectWindowed.bottom - prevFSRectWindowed.top << std::endl;
//	std::cout << "GetClientRect " << prevFSRect.left - prevFSRect.right << ", " << prevFSRect.bottom - prevFSRect.top << std::endl;
	AdjustWindowRectEx(&prevFSRect, prevFSStyle, FALSE, NULL);
	//ResizeWindow(prevFSRect.left, prevFSRect.top,
	//	prevFSRect.right - prevFSRect.left,
	//	(prevFSRect.bottom - prevFSRect.top));
	//ResizeWindow(RectToAccountForTitle.left, RectToAccountForTitle.top,
	//	RectToAccountForTitle.right - RectToAccountForTitle.left + 1,
	//	(RectToAccountForTitle.bottom - RectToAccountForTitle.top) + 1);
	ResizeWindow(prevFSRectWindowed.left, prevFSRectWindowed.top,
		prevFSRect.right - prevFSRect.left,
		(prevFSRect.bottom - prevFSRect.top));
}

void WinMgr::setIsMinimised(bool newState)
{
	isWindowMinimised = newState;
}

void WinMgr::ResizeWindow(int x, int y, int w, int h)
{
	UINT flags = 0;
	if (fullscreen_state)
	{
		flags = SWP_SHOWWINDOW;
	}
	SetWindowPos(hWnd, HWND_TOP, x, y, w, h, flags);
	RECT currentClientRect{ 0 };
	GetClientRect(hWnd, &currentClientRect);
	width = currentClientRect.right - currentClientRect.left;
	height = currentClientRect.bottom - currentClientRect.top;
	width_f = static_cast<float>(width);
	height_f = static_cast<float>(height);
}

bool WinMgr::getIsMinimised()
{
	return isWindowMinimised;
}

//Might need to re-work on this fn, probably can be cleaner also
void WinMgr::updateTitle(float dt)
{
	std::wstring convert_str_to_wstr(title.begin(), title.end());
	const WCHAR* wchar_ptr = convert_str_to_wstr.c_str();
	std::wstring finalText;
	LPCWSTR finalTextPtr;

	bool printFPS = false;

	if (printFPS)
	{
		std::wstring fps = L"           FPS: ";
		//1second divided by dt to get fps
		finalText = wchar_ptr + fps + std::to_wstring(1.f / dt);
		finalTextPtr = finalText.c_str();
	}
	else
	{
		finalText = wchar_ptr;
		finalTextPtr = finalText.c_str();
	}
	SetWindowText(hWnd, finalTextPtr);
}

WinMgr& WinMgr::GetInstance()
{
	static WinMgr instance;
	return instance;
}

void WinMgr::openWindowExplorer(const char* path)
{
	std::string str(path);
	std::wstring wstr(str.begin(), str.end());
	//const wchar_t* szName = name.c_str();
	//ShellExecute(NULL, L"open", L".\\Config", NULL, NULL, SW_SHOWDEFAULT); //working
	ShellExecute(NULL, L"open", wstr.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}


//unsigned long long WinMgr::getTotalSystemMemory()
//{
//	MEMORYSTATUSEX status;
//	status.dwLength = sizeof(status);
//	GlobalMemoryStatusEx(&status);
//	return status.ullTotalPhys;
//	//return status.ullTotalVirtual;
//}
//
//unsigned long long WinMgr::getAvailableSystemMemory()
//{
//	MEMORYSTATUSEX status;
//	status.dwLength = sizeof(status);
//	GlobalMemoryStatusEx(&status);
//	return status.ullAvailPhys;
//	//return status.ullAvailVirtual;
//}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (/*using imgui*/true)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

		//Maybe also recognise  key and mouse 
		//pass on to input??
	}
	//Queued message includes input and mouse
	// use GetMessageTime and GetMessagePos if needed

	//TCHAR greeting[] = _T("Hello, Welcome to CSD2400/GAM200!");
	HDC hdc;
	//cout << message << endl;
	switch (message)
	{
	case WM_DROPFILES: 
		//drag and drop
		WindowsManager.setisDragAndDropped(true);
		
		//release memory
		DragFinish(reinterpret_cast<HDROP>(wParam));

		break;
	case WM_COMMAND:
	{
		
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
		    DialogBox(WindowsManager.getHInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		    break;
		case IDM_EXIT:
		    DestroyWindow(hWnd);
		    break;
		default:
		    return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		
		//Window's Client area has changed and must be repainted
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		int user_decision;
		user_decision = MessageBox(WindowsManager.getHWnd(), L"Progress is not automatically saved upon quitting. Unsaved progress will be lost. Quit?", L"Confirm Quit", MB_OKCANCEL | MB_ICONWARNING);
		if (user_decision == IDOK)
		{
			PostQuitMessage(0);
			WindowsManager.SetIsQuiting(true);
			gameStateManager::quitGame();
		}
		else
		{
			EndDialog(WindowsManager.getHWnd(), user_decision);
		}
		break;
	case WM_STYLECHANGED:
		//WindowsManager.prevFSStyle = reinterpret_cast<STYLESTRUCT*>(lParam)->styleOld;
		break;
	case WM_DESTROY:
		
		break;
	case WM_QUIT:
		//Somehow quit - to learn the diff with WM_DESTROY
		break;
	//case WM_SYSKEYDOWN:
	//	//ALT TAB and CTRL ALT DELETE, cannot be detected because its a system thing. So using WM_INACTIVE
	//	if (KeyManager.isKeyTriggered(VK_TAB) /*GetAsyncKeyState(VK_TAB)*/)
	//	{
	//		std::cout << "ALT TAB" << std::endl;
	//		//ALT TAB is pressed
	//		WindowsManager.Minimise();
	//	}
	//	if ((GetAsyncKeyState(VK_DELETE) & 0x80) && (GetAsyncKeyState(VK_LCONTROL) & 0x80 || GetAsyncKeyState(VK_RCONTROL) & 0x80))
	//	{
	//		std::cout << "CTRL ALT DEL" << std::endl;
	//		//CTRL ALT DEL is pressed
	//		WindowsManager.Minimise();
	//	}
	//	break;
	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{	
			WindowsManager.setIsWindowActive(false);
		}
		if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE)
		{
			//activated
			WindowsManager.setIsWindowActive(true);
		}
		break;
	case WM_WINDOWPOSCHANGED:
	{
		//handling this here without passing it to DefWindowProc
		//will result in skipping sending to WM_SIZE and WM_MOVE

		const WINDOWPOS* winPos = reinterpret_cast<WINDOWPOS*>(lParam);
		if (winPos != nullptr)
		{
			WindowsManager.width = winPos->cx;
			WindowsManager.height = winPos->cy;
			WindowsManager.width_f = static_cast<float>(winPos->cx);
			WindowsManager.height_f = static_cast<float>(winPos->cy);

			//if graphics and framebuffer stuff has been initialised successfully
			if (Graphic_Application::GetInstance().graphicsInitialized)
			{
				//updating the frame buffer size, resize the post processing frame and pre processing frame 
				PostProcessing::GetInstance().m_PostProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
				PostProcessing::GetInstance().m_PreProcessingFrame->Resize((UINT32)(WindowsManager.getWidth()), (UINT32)(WindowsManager.getHeight()));
			}
		}

		return DefWindowProc(hWnd, message, wParam, lParam);//to be safe, send for default handling also coz it will pass on to WM_SIZE and WM_MOVE
		break;
	}
	case WM_SIZE:
		if (wParam == SIZE_RESTORED)
		{
			WindowsManager.setIsMinimised(false);
		}
		if (wParam == SIZE_MINIMIZED)
		{
			WindowsManager.setIsMinimised(true);
		}
		break;
	default:
		return InputManager.Inputs(hWnd, message, wParam, lParam);
		//return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}












///DROP MANAGER

ULONG DropManager::AddRef() { return 1; }
ULONG DropManager::Release() { return 0; }

// we handle drop targets, let others know
HRESULT DropManager::QueryInterface(REFIID riid, void** ppvObject)
{
	if (riid == IID_IDropTarget)
	{
		*ppvObject = this;	// or static_cast<IUnknown*> if preferred
		// AddRef() if doing things properly
		// but then you should probably handle IID_IUnknown as well;
		return S_OK;
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
};

void DropManager::initialise()
{
	OleInitialize(NULL);
	HRESULT result = RegisterDragDrop(WindowsManager.getHWnd(), this);
	result;
}

void DropManager::endDragDrop()
{
	RevokeDragDrop(WindowsManager.getHWnd());
}

void DropManager::startDragDrop()
{
	RegisterDragDrop(WindowsManager.getHWnd(), this);
}


void DropManager::copyLastDroppedFile(const TCHAR* targetPath, const TCHAR* source)
{
	BOOL result = false;
	if (source)
	{
		result = CopyFile( source, targetPath, FALSE );
		if (result != 0)
			std::cout << GetLastError() << std::endl;
	}
	else
	{
		result = CopyFile(
			szFile, targetPath, FALSE
		);
	}

}

void DropManager::copyLastDroppedFile(const char* targetPath, const char* source)
{
	std::string targetPathStr(targetPath);
	std::wstring convert_str_to_wstr(targetPathStr.begin(), targetPathStr.end());
	const WCHAR* targetPath_ptr = convert_str_to_wstr.c_str();
	if (source == nullptr)
	{
		copyLastDroppedFile(targetPath_ptr, nullptr);
	}
	else
	{
		std::string sourceStr(source);
		std::wstring convert_str_to_wstr2(sourceStr.begin(), sourceStr.end());
		const WCHAR* sourcePath_ptr = convert_str_to_wstr2.c_str();

		copyLastDroppedFile(targetPath_ptr, sourcePath_ptr);
	}
}

//const wchar_t* DropManager::getdraggedFileNamePath()
//{
//	return &draggedFileNamePath;
//
//}

DropManager::~DropManager()
{
	OleUninitialize();
}

DropManager& DropManager::GetInstance()
{
	static DropManager DMinstance;
	return DMinstance;
}



//Windows interface fn 
//--- implement the IDropTarget parts

// occurs when we drag files into our applications view
HRESULT DropManager::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER(pt);
	UNREFERENCED_PARAMETER(grfKeyState);
	UNREFERENCED_PARAMETER(pDataObj);
	// TODO: check whether we can handle this type of object at all and set *pdwEffect &= DROPEFFECT_NONE if not;
	// do something useful to flag to our application that files have been dragged from the OS into our application
	// trigger MouseDown for button 1 within ImGui
	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

// occurs when we drag files out from our applications view
HRESULT DropManager::DragLeave() { isDragging = false,isDropped = false; return S_OK; }

// occurs when we drag the mouse over our applications view whilst carrying files (post Enter, pre Leave)
HRESULT DropManager::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER(pt);
	UNREFERENCED_PARAMETER(grfKeyState);
	
	isDragging = true;
	// trigger MouseMove within ImGui, position is within pt.x and pt.y
	// grfKeyState contains flags for control, alt, shift etc

	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

// occurs when we release the mouse button to finish the drag-drop operation
HRESULT DropManager::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	UNREFERENCED_PARAMETER(grfKeyState);
	UNREFERENCED_PARAMETER(pt);
	isDropped = true;
	//isDragging = false;
	// grfKeyState contains flags for control, alt, shift etc

	// render the data into stgm using the data description in fmte
	FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgm;

	if (SUCCEEDED(pDataObj->GetData(&fmte, &stgm)))
	{
		HDROP hdrop = (HDROP)stgm.hGlobal; // or reinterpret_cast<HDROP> if preferred
		UINT file_count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

		// we can drag more than one file at the same time, so we have to loop here
		for (UINT i = 0; i < file_count; i++)
		{
			UINT cch = DragQueryFile(hdrop, i, szFile, MAX_PATH);
			if (cch > 0 && cch < MAX_PATH)
			{
				//std::wstring wStr(szFile);
				//std::string strPath(wStr.begin(), wStr.end());
				//draggedFileNamePath = szFile;//passing the file name path that is being dropped 
				std::cout << szFile << std::endl;
				// szFile contains the full path to the file, do something useful with it
				 
				
				// i.e. add it to a vector or something
			}
		}

		// we have to release the data when we're done with it
		ReleaseStgMedium(&stgm);

		// notify our application somehow that we've finished dragging the files (provide the data somehow)
	}

	// trigger MouseUp for button 1 within ImGui

	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

bool DropManager::deleteFile(const char* pathToBeDeleted)
{
	bool result = DeleteFileA(pathToBeDeleted);
	return result;//returns 0 if fail 
}
bool DropManager::deleteFile(std::string pathToBeDeleted)
{
	return deleteFile(pathToBeDeleted.c_str());
}




CursorManager::CursorManager()
{
}

CursorManager::~CursorManager()
{
	DestoryLoadedCursor();
}

void CursorManager::LoadCustomCursor(const char* path, int hotspotX, int hotspotY)
{
	cursorNormalPath = ConvertCharP_TO_WCharP(path);
	hCustomCursor = hCursor = (HCURSOR)LoadImage(NULL, cursorNormalPath, IMAGE_CURSOR, CCursorSizeX, CCursorSizeY, LR_LOADFROMFILE );
	
	//HOTSPOT UPDATE FROM HERE MIGHT NOT WORK
	GetIconInfo(hCursor, &iIcon);
	iIcon.xHotspot = hotspotX;
	iIcon.yHotspot = hotspotY;
	//hCursor = CreateIconIndirect(&iIcon);
}

void CursorManager::LoadCustomCursorClicked(const char* path, int hotspotX, int hotspotY)
{
	cursorClickedPath = ConvertCharP_TO_WCharP(path);
	hCursorClicked = (HCURSOR)LoadImage(NULL, cursorClickedPath, IMAGE_CURSOR, CCursorSizeX, CCursorSizeY, LR_LOADFROMFILE);

	GetIconInfo(hCursorClicked, &iIcon);
	iIcon.xHotspot = hotspotX;
	iIcon.yHotspot = hotspotY;
	//hCursor = CreateIconIndirect(&iIcon);
}

void CursorManager::DestoryLoadedCursor()
{
	if (hCursor != NULL)
	{
		DestroyCursor(hCursor);
		hCursor = nullptr;
	}
	if (hCursorClicked != NULL)
	{
		DestroyCursor(hCursorClicked);
		hCursorClicked = nullptr;
	}
	if (cursorNormalPath != nullptr)
	{
		delete cursorNormalPath;
		cursorNormalPath = nullptr;
	}
	if (cursorClickedPath != nullptr)
	{
		delete cursorClickedPath;
		cursorClickedPath = nullptr;
	}
}

HCURSOR CursorManager::GetCurrentCursor()
{

	return hCursor;
}

HCURSOR CursorManager::GetCustomCursor()
{

	return hCustomCursor;
}

HCURSOR CursorManager::GetPreviousCursor()
{

	return hCursorPrev;
}

void CursorManager::SetCustomCursor()
{
	hCursorPrev = SetCursor(hCursor);
}

HCURSOR CursorManager::GetDefaultCursor()
{
	return LoadCursor(NULL, IDC_ARROW);
}


