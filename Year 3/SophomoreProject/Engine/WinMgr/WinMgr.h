// /******************************************************************************/
// /*!
// \file         WinMgr.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        WinMgr.h creates windows and create device context for renderer
// \lines        60 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef __WINMGR_H__
#define __WINMGR_H__

#include <string>
#include <windows.h>
#include <tchar.h>

//#pragma warning disable : C26495 
#define MAX_LOADSTRING 100

//Window creation guide:
//https://docs.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-160

//TODO
/*
Fn:
Alt Tab handling?? Other possible issue?? HotKeyManagement??

Disabling or enabling of resizing

//For writing text in windows (not sure if will be replaced by opengl)
TextOut(hdc, 5, 5, greeting, _tcslen(greeting));

To find out and be CLEAR of WM_QUIT, WM_PAINT

GetLastError to try and add and use
*/


class CursorManager
{
	HICON hIcon = { 0 };
	ICONINFO iIcon = { 0 };

	HCURSOR hCursor = { 0 }; //current
	
	HCURSOR hCustomCursor = { 0 }; //custom,extra variable incase needed
	HCURSOR hCursorPrev = { 0 }; //previous
	LPWSTR cursorNormalPath = nullptr;
	LPWSTR cursorClickedPath = nullptr;
public:
	HCURSOR hCursorClicked = { 0 }; //when clicked
	float clickedTimeMAX = 1.0f; //Clicked starting
	float clickedTime = 0.f; //once clicked, count down to 0, then revert to unclick 

	CursorManager();
	~CursorManager();

	void LoadCustomCursorClicked(const char* path, int hotspotX = 0, int hotspotY = 0);
	void LoadCustomCursor(const char* path, int hotspotX = 0, int hotspotY = 0);
	void DestoryLoadedCursor();
	
	HCURSOR GetCustomCursor();
	HCURSOR GetCurrentCursor();
	HCURSOR GetPreviousCursor();
	HCURSOR GetDefaultCursor();
	void SetCustomCursor();

};

class WinMgr
{
	bool windowIsActive = true;
	//bool icon_loaded = false;
	HANDLE hIcon = nullptr;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	int nCmdShow = 1;

	TCHAR szWindowClass[MAX_LOADSTRING] = _T("DesktopApp");
	TCHAR szTitle[MAX_LOADSTRING] = _T("Windows Desktop Guided Tour Application");

	HWND hWnd;

	int width;
	int height;
	float width_f;
	float height_f;
	RECT currentRect; // recheck if this is being used
	
	bool fullscreen_state = false;
	RECT prevFSRect;//client
	RECT prevFSRectWindowed;
	DWORD prevFSStyle;

	std::string title;

	HDC m_windowDC;//global Window-device context
	HGLRC m_wglDC;//OpenGL-device context

	bool toQuit;//indicates if windows is closing 
	bool dragAndDrop = false;
	bool isDragAndDropped = false;

	bool isWindowIsFocus = true;
	bool isWindowMinimised = false;
	
//	HDROP hDrop,
//	UINT  iFile,
//	LPSTR lpszFile,
//	UINT  cch


	WinMgr();
public:
	CursorManager custom_cursor;
	~WinMgr();

	bool Load();
	int Init();
	int Update(float dt);

	//setter
	void setTitle(const char* newTitle);
	//set FS on or off
	void setFullscreenState(bool newState);
	//adjust the window, xy relative to monitor pos(top left), width and height for the size
	void ResizeWindow(int x, int y, int w, int h);

	//TODO
	void DisableResizing();//endable also if done

	bool InitializeRenderingEnvironment();

	void ClearRenderingEnviroment();
	HDC getWindowDC() const;

	//Hides console
	void HideConsole();

	void ShowConsole();

	bool IsConsoleVisible();
	bool isMinimised();

	void Minimise();
	//void UnMinimise();

	bool getIsWindowActive();
	void setIsWindowActive(bool state);

	void SetIsQuiting(bool toggle);
	bool GetIsQuiting() const;

	bool getWindowIsFocus();

	//after enable, disable it after used
	void enableDragAndDrop();
	void disableDragAndDrop();

	bool getisDragAndDropped();
	void setisDragAndDropped(bool state);

	//getter
	int getWidth() const;
	int getHeight() const;
	float getWidthF() const; //Use this so no need cast
	float getHeightF() const; //Use this so no need cast
	HWND getHWnd() const;
	HINSTANCE getHInstance() const;
	int getNCmdShow() const;
	bool getFullscreenState() const;
	//get the primary display (monitor) width resolution
	size_t GetMonitorWidthResolution() const;
	//get the primary display (monitor) heght resolution
	size_t GetMonitorHeightResolution() const;
	//Get aspect ratio of current window or Fullscreen if on
	float GetAspectRatio() const;

	static WinMgr& GetInstance();
	void openWindowExplorer(const char* path);

	//test fn
	//unsigned long long getTotalSystemMemory();
	//unsigned long long getAvailableSystemMemory();

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool getIsMinimised();

	//private function
private:
	void updateTitle(float dt);
	//Change to fullscreen
	void Fullscreen();
	//change back to before fullscreen
	void ReturnToWindowed();
	void setIsMinimised(bool);
};

LRESULT CALLBACK WndProc(_In_ HWND   hWnd, _In_ UINT   message, _In_ WPARAM wParam, _In_ LPARAM lParam);

# define WindowsManager ::WinMgr::GetInstance()
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


#include <oleidl.h>

// create a class inheriting from IDropTarget
class DropManager : public IDropTarget
{
public:
	bool isDragging = false;
	bool isDropped = false;
	//const wchar_t* draggedFileNamePath;
	//const wchar_t* draggedFileNamePath[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	//--- implement the IUnknown parts
	// you could do this the proper way with InterlockedIncrement etc,
	// but I've left out stuff that's not exactly necessary for brevity
	ULONG AddRef();
	ULONG Release();

	// we handle drop targets, let others know
	HRESULT QueryInterface(REFIID riid, void** ppvObject);

	void initialise();

	void endDragDrop();
	void startDragDrop();

	//if source is nullptr, then use path name from szFile.
	//Path is starting from the application path
	void copyLastDroppedFile(const TCHAR* targetPath, const TCHAR* source = nullptr);
	void copyLastDroppedFile(const char* targetPath, const char* source = nullptr);
	static bool deleteFile(const char* pathToBeDeleted);
	static bool deleteFile(std::string pathToBeDeleted);
	//const wchar_t* getdraggedFileNamePath();
	~DropManager();

	static DropManager& GetInstance();



	//Windows interface fn 
	//--- implement the IDropTarget parts

	// occurs when we drag files into our applications view
	HRESULT DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// occurs when we drag files out from our applications view
	HRESULT DragLeave();

	// occurs when we drag the mouse over our applications view whilst carrying files (post Enter, pre Leave)
	HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// occurs when we release the mouse button to finish the drag-drop operation
	HRESULT Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
};

#define dropManager ::DropManager::GetInstance()






#endif
