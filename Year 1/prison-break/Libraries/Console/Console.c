#include <windows.h>/*ConsoleHandle, Char_Info*/
#include <assert.h>/*Error diagnostics*/
#include <stdbool.h>/*Boolean in C*/
#include <stdio.h>/*wchar_t string*/
/*#include <stdlib.h>*/
#include "Console.h"


/*********************************************************************************
								LOCAL VARIABLES
*********************************************************************************/

static HANDLE		sConsoleHandle = 0x00; /*Main Handle for rendering*/
static CHAR_INFO*	sRenderBuffer = 0x00; /*Place where all display value is stored*/
static int			sRenderBufferSize = 0; /*Acts like count for array sRenderBuffer*/
static COORD		sConsoleSize; /*The size of console screen*/
static const COORD ORIGIN = { 0,0 }; /*Origin of the window*/
static bool			sConsoleInitialized = false;
static float		sConsoleDisplayRatio = 4.f / 3.f; /*FONT SIZE RATIO*/

static unsigned short bufferAttribute = WHITE|BLACK|DEFAULTLVB; /*Foreground Color, BackGround Color, Othereffects*/
HWND consoleHWNDTrack;
RECT consoleRectTrack;

/**********************************************************************************
								CONTROL FUNCTIONS
**********************************************************************************/

void Console_HandleValidity_Assert()
{
	/* Crash here: you haven't called Console_Init() in your code before using the library*/
	assert(sConsoleHandle != 0x00);/*Display an error message stderr and abort program*/
}

/**********************************************************************************
								RENDER BUFFER HANDLING
**********************************************************************************/

void Console_FreeRenderBuffer()
{
	/*if nothing new to render this frame, skip*/
	if (sRenderBuffer == 0)
		return;

	/*deallocates the memory previously allocated by a call to calloc, malloc, or realloc.*/
	free(sRenderBuffer);
	sRenderBuffer = 0;
	sRenderBufferSize = 0;
}

void Console_CreateRenderBuffer()
{
	/*Reset Render Buffer if exist already*/
	if (sRenderBuffer != 0)
		Console_FreeRenderBuffer();

	/*CHAR_INFO = https://docs.microsoft.com/en-us/windows/console/char-info-str */
	sRenderBufferSize = Console_GetHeight() * Console_GetWidth() * sizeof(CHAR_INFO);

	sRenderBuffer = malloc(sRenderBufferSize);/*Allocates the requested memory and returns a pointer to it*/
}

void Console_ClearRenderBuffer()
{
	if (sRenderBuffer == 0)
		return;

	memset(sRenderBuffer, 0, sRenderBufferSize);/*Fill the array|Pointer with 0... Start address, Value to fill, number of byte to fill*/

}

void Console_SwapRenderBuffer()
{
	if (sRenderBuffer == 0)
		return;

	/* Copy the local rendering buffer to the ConsoleHandle Buffer*/
	SMALL_RECT writeRegion = { 0,0, 0, 0};
	writeRegion.Right = sConsoleSize.X;
	writeRegion.Bottom = sConsoleSize.Y;
 	if (!WriteConsoleOutputA(sConsoleHandle, sRenderBuffer, sConsoleSize, ORIGIN, &writeRegion))/*Check if able to write new buffer onto existing*/
	{
		DWORD error = GetLastError();
		assert(1);/*Display error message*/
	}
}

/**********************************************************************************
						RENDER BUFFER HANDLING - Writing
**********************************************************************************/

void Console_SetRenderBuffer_Char(int x, int y, char c)
{
#ifdef _DEBUG	/*for gcc with the -D flag*/
	Console_HandleValidity_Assert();
#endif
	const int index = x + y * sConsoleSize.X;/*Render buffer arranged by horizontal rows, followed by next one*/

	if ((index >= sRenderBufferSize) || (index < 0)) /*If string is too big to be displayed*/
		return;


	sRenderBuffer[index].Char.AsciiChar = c;
	sRenderBuffer[index].Attributes = bufferAttribute;
	//FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
// FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
}

void Console_SetRenderBuffer_String(int x, int y, const char* s)
{
#ifdef _DEBUG	/*for gcc with the -D flag*/
	Console_HandleValidity_Assert();
#endif
 	const int size = (int)strlen(s);	/*Calculate string length*/
	int index = x + y * sConsoleSize.X; /*Render buffer arranged by horizontal rows, followed by next one*/
	for (int i = 0; i < size; ++i)
	{
		if ((index >= sRenderBufferSize) || (index < 0)) /*If string is too big to be displayed*/
			return;

		/*Print char one by one*/
		sRenderBuffer[index].Char.AsciiChar = s[i];
		sRenderBuffer[index].Attributes = bufferAttribute;
		//FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN; /*Bitwise Or (if either one is 1 then output 1)
		++index;
	}
}

void Console_SetRenderBuffer_Attribute(enum colorCode colorCodeFg, enum colorCode colorCodeBg, enum colorCode specialLVB) {
	bufferAttribute = (short)(colorCodeFg + (colorCodeBg << 4) + specialLVB);
}

/**********************************************************************************
								CONSOLE & WINDOW
**********************************************************************************/

void Console_Init()
{
	sConsoleInitialized = true; /*After this code the console is initalised*/

	/* Main buffer in normal mode, Backbuffer in double buffer*/
	sConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	/* Disable the Mouse quick edit mode for the console*/
	HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD inputFlags;
	GetConsoleMode(inputHandle, &inputFlags);
	SetConsoleMode(inputHandle, ENABLE_EXTENDED_FLAGS | (inputFlags & ~ENABLE_QUICK_EDIT_MODE)); /*Bitwise complement for quick edit mode*/

	/* Get the Current Font Ratio*/
	CONSOLE_FONT_INFO fontInfo;
	GetCurrentConsoleFont(sConsoleHandle, FALSE, &fontInfo);
	sConsoleDisplayRatio = fontInfo.dwFontSize.X / (float)fontInfo.dwFontSize.Y;
}

/*Default is 8x8*/
void Console_SetSquareFont(short fontSizeX, short fontSizeY)
{
	/* CONSOLE_FONT_INFOEX https://docs.microsoft.com/en-us/windows/console/console-font-infoex */
	/* Flattens the font, Terminal and 8x8*/
	CONSOLE_FONT_INFOEX fontInfo = { 0 };
	fontInfo.cbSize = sizeof(fontInfo); /*The size of this structure*/
	/*Change the font type here!*/
	wcscpy_s(fontInfo.FaceName, _countof(fontInfo.FaceName), L"Terminal"); /*Copy Terminal to The name of the typeface*/
	fontInfo.dwFontSize.X = fontSizeX;
	fontInfo.dwFontSize.Y = fontSizeY;
	fontInfo.FontWeight = FW_NORMAL;
	sConsoleDisplayRatio = fontInfo.dwFontSize.X / (float)fontInfo.dwFontSize.Y;

	SetCurrentConsoleFontEx(sConsoleHandle, FALSE, &fontInfo);
}

void Console_IncreaseFontSize(COORD origFontSize, COORD *fontSize, int mutlipler,int origScreenSizeX, int origScreenSizeY)
{
	/*Find the next largest font size that consist of nice number.. no decimal place*/
	int increment = 1;
	COORD overalscreenSize = {0}; 
	overalscreenSize.X = (short)origScreenSizeX * origFontSize.X;
	overalscreenSize.Y = (short)origScreenSizeY * origFontSize.Y;

	for (increment = origFontSize.X* mutlipler; (increment + fontSize->X) < (int)overalscreenSize.X; increment+= origFontSize.X* mutlipler)
	{
		if (overalscreenSize.X % (fontSize->X + increment) == 0)
		{
			fontSize->X += (short)increment;
			break;
		}
	}
	/*Using 10x20 ratio*/
	fontSize->Y = fontSize->X*2;

	CONSOLE_FONT_INFOEX fontInfo = { 0 };
	fontInfo.cbSize = sizeof(fontInfo); /*The size of this structure*/
	/*Change the font type here!*/
	wcscpy_s(fontInfo.FaceName, _countof(fontInfo.FaceName), L"Terminal"); /*Copy Terminal to The name of the typeface*/
	fontInfo.dwFontSize.X = fontSize->X;
	fontInfo.dwFontSize.Y = fontSize->Y;
	fontInfo.FontWeight = FW_NORMAL;
	sConsoleDisplayRatio = fontInfo.dwFontSize.X / (float)fontInfo.dwFontSize.Y;

	SetCurrentConsoleFontEx(sConsoleHandle, FALSE, &fontInfo);

	Console_ForceSize(origFontSize, *fontSize, origScreenSizeX, origScreenSizeY);
}

float Console_GetDisplayRatio()
{
	return sConsoleDisplayRatio;
}

void Console_SetTitle(const char* title)
{
	/* Lazy conversion from ANSI to Unicode, Limited to 100 character*/
	/* use dynamic memory allocation and not a fixed array to remove the 100 character limitation*/
	wchar_t  lTitle[100]; /*Wide Char Array*/
	swprintf_s(lTitle, 100, L"%hs", title); /*Write to String*/

	SetConsoleTitle(lTitle);
}

void Console_SetSize(int x, int y) 
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	/*const*/ COORD size = { 0, 0 };
	size.X = (short)x;
	size.Y = (short)y;
	/*const*/ SMALL_RECT windowSize = { 0, 0, 0, 0};/*Define Console Window Size*/
	windowSize.Right = size.X - 1;
	windowSize.Bottom = size.Y - 1;

	/* HACK: sometimes the windows is not of the asked size, because the Buffer can never be smaller than the windows*/
	/* FIX: Force a small windows, set buffer, and resize windows to the correct size*/
	const SMALL_RECT smallWindowSize = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(sConsoleHandle, 1, &smallWindowSize);
	/* END HACK*/

	SetConsoleScreenBufferSize(sConsoleHandle, size); /*Set Screen Buffer to be larger than normalised screensize (1 length)*/
	SetConsoleWindowInfo(sConsoleHandle, 1, &windowSize);

	sConsoleSize = size;
}

void Console_ForceSize(COORD origFont, COORD newFont, int x, int y)
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif
	//COORD offsetSB = { (origFont * x) % newFont.X, (origFont * y) % newFont.Y}; /*offset Screenbuffer*/
	COORD newsize = { 0 };
	newsize.X = (short)(origFont.X * x) / newFont.X;
	newsize.Y = (short)(origFont.Y * y) / newFont.Y;
	SMALL_RECT windowSize = { 0, 0, 0, 0 };
	windowSize.Right = newsize.X - 1;/*Define Console Window Size*/
	windowSize.Bottom = newsize.Y - 1;

	/* HACK: sometimes the windows is not of the asked size, because the Buffer can never be smaller than the windows*/
	/* FIX: Force a small windows, set buffer, and resize windows to the correct size*/
	const SMALL_RECT smallWindowSize = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(sConsoleHandle, 1, &smallWindowSize);
	/* END HACK*/

	SetConsoleScreenBufferSize(sConsoleHandle, newsize); /*Set Screen Buffer to be larger than normalised screensize (1 length)*/
	SetConsoleWindowInfo(sConsoleHandle, 1, &windowSize);

    sConsoleSize = newsize;
}

/*Opposite of Windowed mode*/
void Console_SetFullScreen()
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	CONSOLE_SCREEN_BUFFER_INFO  screenBufferInfo; /*Load up default screnbufferinfo*/

	/* Set the console to full Screen and changes its buffer*/
	SetConsoleDisplayMode(sConsoleHandle, CONSOLE_FULLSCREEN_MODE, NULL);/*Set to fullscreen mode*/
	GetConsoleScreenBufferInfo(sConsoleHandle, &screenBufferInfo); /*Parse current screen buffer info*/
	SetConsoleScreenBufferSize(sConsoleHandle, screenBufferInfo.dwMaximumWindowSize); /*Set size to max*/
	sConsoleSize = screenBufferInfo.dwMaximumWindowSize; /*Record max size*/

	/* Create the rendering buffer*/
	Console_CreateRenderBuffer();

	/* Bring the window to the top*/
	BringWindowToTop(GetConsoleWindow());
}


void Console_SetWindowPos(int x, int y)
{
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, x, y, 0, 0, SWP_FRAMECHANGED | SWP_DRAWFRAME | SWP_NOSIZE);
}

/*Opposite of Fullscreen mode*/
void Console_SetWindowedMode(int sizeX, int sizeY, bool centeredWindow)
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	const COORD maxSize = GetLargestConsoleWindowSize(sConsoleHandle);
	/*Make sure the window does not overflow if size specified is too big*/
	sizeX = sizeX < maxSize.X? sizeX : maxSize.X;

	sizeY = sizeY < maxSize.Y ? sizeY : maxSize.Y;

	SetConsoleDisplayMode(sConsoleHandle, CONSOLE_WINDOWED_MODE, NULL);
	Console_SetSize(sizeX, sizeY); /*Load Screen buffer based on size*/

	/* Center the window on screen*/
	const HWND   consoleHWND = GetConsoleWindow();
	RECT   windowsRect;
	RECT   consoleRect;

	GetWindowRect(consoleHWND, &consoleRect);
	GetWindowRect(GetDesktopWindow(), &windowsRect);

	/* Create the rendering buffer*/
	Console_CreateRenderBuffer();

	/* Bring it on top*/
	BringWindowToTop(consoleHWND);

	/* Removes the resize ability*/
	SetWindowLongPtr(consoleHWND, GWL_STYLE, GetWindowLongPtr(consoleHWND, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

	/* Set the Window to the center*/
	if (centeredWindow)
	{
		const int newX = ((windowsRect.right - windowsRect.left) / 2 - (consoleRect.right - consoleRect.left) / 2);
		const int newY = ((windowsRect.bottom - windowsRect.top) / 2 - (consoleRect.bottom - consoleRect.top) / 2);
		Console_SetWindowPos(newX, newY);
	}
	else
		Console_SetWindowPos(0, 0);
}

/*INBETWEEN OF FULLSCREEN AND WINDOWED*/
void Console_SetWindowedFullScreen()
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	const COORD maxSize = GetLargestConsoleWindowSize(sConsoleHandle);
	Console_SetWindowedMode(maxSize.X, maxSize.Y, false);
}

int Console_GetWidth()
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	return sConsoleSize.X;
}

int Console_GetHeight()
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	return sConsoleSize.Y;
}

void Console_SetCursorVisibility(int visible)
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(sConsoleHandle, &cursorInfo);
	cursorInfo.bVisible = (visible == 0) ? false : true;/*Set cursor visible or not*/
	SetConsoleCursorInfo(sConsoleHandle, &cursorInfo);/*Update in system*/
}

void Console_ConsoleTrackInit() {
	consoleHWNDTrack = GetConsoleWindow();

}

int Console_GetWindowPosY() {
	GetWindowRect(consoleHWNDTrack, &consoleRectTrack);
	return consoleRectTrack.top;
}

int Console_GetWindowPosX() {
	GetWindowRect(consoleHWNDTrack, &consoleRectTrack);
	return consoleRectTrack.left;
}

/*********************************************************************************
			STANDARD USAGE OF CONSOLE : Clear / Print / Putchar
**********************************************************************************/

/*Move the cursor so that the program can printf at a certain position*/
void Console_SetCursorPos(short x, short y)
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	COORD pos = { 0, 0 };
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(sConsoleHandle, pos);
}

/* If you use printF*/
COORD Console_GetCursorPos()
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	CONSOLE_SCREEN_BUFFER_INFO  screenBufferInfo;
	GetConsoleScreenBufferInfo(sConsoleHandle, &screenBufferInfo);

	return screenBufferInfo.dwCursorPosition;
}

/*Only one char, based on cursor position*/
void Console_Putchar(short x, short y, const char s)
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	Console_SetCursorPos(x, y);
	putchar(s);
}

/*string, based on cursor position*/
void Console_Printf(short x, short y, const char* s)
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	Console_SetCursorPos(x, y);
	printf_s(s);
}

void Console_Clear()
{
#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif
	/* Fills the whole console with a space character*/
	/* does not change the color of a cell, use FillConsoleOutputAttribute to clean out properly the colors if you decide to use colors*/
	CONSOLE_SCREEN_BUFFER_INFO screenInfoBuffer;
	GetConsoleScreenBufferInfo(sConsoleHandle, &screenInfoBuffer);

	const COORD firstCell = { 0, 0 };
	const int cellCount = screenInfoBuffer.dwSize.X * screenInfoBuffer.dwSize.Y; /*Count of all the cells in the window*/
	DWORD writtenCells = 0;
	FillConsoleOutputCharacter(sConsoleHandle, ' ', cellCount, firstCell, &writtenCells);/*Fill all the memory with ' '*/
	/*FillConsoleOutputAttribute*/
}

void Console_ClearArea(short x, short y, int width, int height)
{

#ifdef _DEBUG
	Console_HandleValidity_Assert();
#endif

	char buffer[512];
	memset(buffer, ' ', width);/*Full the 512 index with blank space*/
	buffer[width] = '\0'; /*Mark the out of range index with a null*/

	for (short j = y; j < y + height; ++j)
		Console_Printf(x, j, buffer);/*Print null*/
}

void Console_TestAPI()
{
	/*Use to be a testing program, but getchar is causing warnings*/
}


//*********************************************************************************
//									CLEAN UP
//*********************************************************************************

/*Clear up both render buffer method and normal printf*/
void Console_CleanUp()
{
	Console_FreeRenderBuffer();
	Console_Clear();
	DestroyWindow(GetConsoleWindow());
}
