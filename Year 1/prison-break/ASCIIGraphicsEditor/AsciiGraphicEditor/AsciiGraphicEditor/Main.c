#include <Windows.h>
#include <stdio.h>/*whereX*/
#include <conio.h>/*whereX*/
#include <direct.h>
#include "Console.h"
#include "Main.h"

int bProgramRunning = 1;

static COORD asciiBrushPanelOrig = {1, 1};
static COORD asciiBrushPanelSize = {8, 32};
static COORD colorPanelOrig = { 1, 35};
static COORD colorPanelOrig2 = { 1, 45 };
static COORD colorPanelSize = { 8, 8};
static COORD currBrushPanelOrig = { 1, 54 };
static COORD currBrushPanelSize = { 20, 8 };
static COORD paintingGridOrig = {10, 1};
static COORD paintingGrid ={88, 51 };
static COORD loadAndSaveOrig = { 80, 53 };
static COORD loadAndSaveSize = { 8, 8 };

int canvasAsciiData[88][51][3];

int currFGColor = BLACK;
int currBGColor = WHITE;
int prevAsciiChar = 63; /*Question Mark*/
int currAsciiChar = 63; /*Question Mark*/

POINT myCursorPos;
COORD screenCursorPos = { 0,0 };
char chMousePos[5];
int bMouseInputRecieved = 0;

int bIsPaintingBrush = 0;
COORD paintingCoord = { 0,0 };

int saveIndex = 1;
FILE *fptr;
char saveFileString[FILENAME_MAX];
char saveFileName[20];
char loadFileName[20];
int loadIndex = 1;
bReloadCanvas = 0;
int bLMouseUp = 1;

int main() {
	/**********************************************************************************************
										GAME INITALISATION
	***********************************************************************************************/
	Console_Init(); /*Initalise Console Features*/
	Console_SetSquareFont(14); /*makes the windows square, since width of character < than height*/
	Console_SetWindowedMode(100,60,false);
	Console_ConsoleTrackInit();
	Console_SetTitle("Ascii Graphic Editor - By Chong Jun Yi");/*Set Console Title Text*/
	Console_SetCursorVisibility(1);

	InitToolKit();

	while (bProgramRunning) {
		ProcessInput();
		Update();
		Rendering();	
		
	}
	return 0;
}

/*Initalise Toolkit such as brushes, Color palette, save and load*/
void InitToolKit() {
	Console_ClearRenderBuffer();
	UI_AsciiCharacterBrush();
	UI_ColorPallete();
	UI_CurrBrushInfo();
	UI_DefinePaintArea();
	UI_StationarySet();
	UI_LoadAndSave();
	Console_SwapRenderBuffer();
}

void UI_AsciiCharacterBrush() {
	int i = 0;
	int j = 0;
	COORD onScreenPos = { 0,0 };

	Console_SetRenderBuffer_Attribute(LIGHTRED, WHITE, GRID_FULL);
	for (i = 0; i < asciiBrushPanelSize.X; i++) {
		for (j = 0; j < asciiBrushPanelSize.Y; j++) {
			onScreenPos.X = asciiBrushPanelOrig.X + i;
			onScreenPos.Y = asciiBrushPanelOrig.Y + j;
			Console_SetRenderBuffer_Char(onScreenPos.X, onScreenPos.Y, i + j * asciiBrushPanelSize.X);
		}
	}
}

void UI_ColorPallete() {
	int i = 0;
	int j = 0;
	int colorHexCode = 0x0000;

	/*Foreground Color*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(1, 34, "FOREGRD");

	for (i = 0; i < colorPanelSize.X; i+=2) {
		for (j = 0; j < colorPanelSize.Y; j += 2) {
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, GRID_HORIZONTAL|GRID_LVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig.X +i, colorPanelOrig.Y + j, 0);
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, UNDERSCORE| GRID_LVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig.X + i, colorPanelOrig.Y + j + 1, 0);
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, GRID_HORIZONTAL| GRID_RVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig.X + i+1, colorPanelOrig.Y + j, 0);
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, UNDERSCORE | GRID_RVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig.X + i + 1, colorPanelOrig.Y + j+1, 0);
			colorHexCode += 1;
		}
	}
	
	/*Background Color*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(1, 44, "BACKGRD");

	for (i = 0; i < colorPanelSize.X; i += 2) {
		for (j = 0; j < colorPanelSize.Y; j += 2) {
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, GRID_HORIZONTAL | GRID_LVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig2.X + i, colorPanelOrig2.Y + j, 0);
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, UNDERSCORE | GRID_LVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig2.X + i, colorPanelOrig2.Y + j + 1, 0);
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, GRID_HORIZONTAL | GRID_RVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig2.X + i + 1, colorPanelOrig2.Y + j, 0);
			Console_SetRenderBuffer_Attribute(WHITE, colorHexCode, UNDERSCORE | GRID_RVERTICAL);
			Console_SetRenderBuffer_Char(colorPanelOrig2.X + i + 1, colorPanelOrig2.Y + j + 1, 0);
			colorHexCode += 1;
		}
	}
	
}

void UI_CurrBrushInfo() {
	/*Right Mouse = Fore + Back*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(currBrushPanelOrig.X+3, currBrushPanelOrig.Y, "ASCII");
	Console_SetRenderBuffer_String(currBrushPanelOrig.X + 3, currBrushPanelOrig.Y+1, "BRUSH - LEFT MOUSE");
	Console_SetRenderBuffer_Attribute(currFGColor, currBGColor, DEFAULTLVB);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X, currBrushPanelOrig.Y, currAsciiChar);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X, currBrushPanelOrig.Y + 1, 0);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X+1, currBrushPanelOrig.Y, 0);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X+1, currBrushPanelOrig.Y + 1, 0);

	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(currBrushPanelOrig.X+3, currBrushPanelOrig.Y+3, "BG ONLY");
	Console_SetRenderBuffer_String(currBrushPanelOrig.X+3, currBrushPanelOrig.Y + 4, "BRUSH - RIGHT MOUSE");
	Console_SetRenderBuffer_Attribute(currFGColor, currBGColor, DEFAULTLVB);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X, currBrushPanelOrig.Y+3, 'B');
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X, currBrushPanelOrig.Y + 4, 0);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X + 1, currBrushPanelOrig.Y+3, 'G');
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X + 1, currBrushPanelOrig.Y + 4, 0);
}

void UI_DefinePaintArea() {
	int i = 0;

	Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);
	/*Define Corner*/
	Console_SetRenderBuffer_Char(paintingGridOrig.X, paintingGridOrig.Y, 201);
	Console_SetRenderBuffer_Char(paintingGridOrig.X, paintingGridOrig.Y + paintingGrid.Y, 200);
	Console_SetRenderBuffer_Char(paintingGridOrig.X + paintingGrid.X, paintingGridOrig.Y, 187);
	Console_SetRenderBuffer_Char(paintingGridOrig.X + paintingGrid.X, paintingGridOrig.Y + paintingGrid.Y, 188);
	/*Horizonatal*/
	for (int i = 0; i < paintingGrid.X - 1; i++) {
		Console_SetRenderBuffer_Char(paintingGridOrig.X + 1 + i, paintingGridOrig.Y, 205);
		Console_SetRenderBuffer_Char(paintingGridOrig.X + 1 + i, paintingGridOrig.Y + paintingGrid.Y, 205);
	}

	for (int i = 0; i < paintingGrid.Y - 1; i++) {
		Console_SetRenderBuffer_Char(paintingGridOrig.X, paintingGridOrig.Y +1+i, 186);
		Console_SetRenderBuffer_Char(paintingGridOrig.X + paintingGrid.X, paintingGridOrig.Y + 1+i, 186);
	}
	
}

void UI_StationarySet() {
	/*Eraser*/

	/*Fill*/
}

void UI_LoadAndSave() {
	Console_SetRenderBuffer_Attribute(LIGHTRED, WHITE, DEFAULTLVB);
	Console_SetRenderBuffer_String(loadAndSaveOrig.X, loadAndSaveOrig.Y, "      ");
	Console_SetRenderBuffer_String(loadAndSaveOrig.X, loadAndSaveOrig.Y+1, " SAVE ");
	Console_SetRenderBuffer_String(loadAndSaveOrig.X, loadAndSaveOrig.Y+2, "      ");

	Console_SetRenderBuffer_String(loadAndSaveOrig.X, loadAndSaveOrig.Y+4, "      ");
	Console_SetRenderBuffer_String(loadAndSaveOrig.X, loadAndSaveOrig.Y + 5, " LOAD ");
	Console_SetRenderBuffer_String(loadAndSaveOrig.X, loadAndSaveOrig.Y + 6, "      ");
}

void ProcessInput() {
	bMouseInputRecieved = 0;
	if (GetCursorPos(&myCursorPos)) {
		screenCursorPos.X = (short)(myCursorPos.x - Console_GetWindowPosX()) / 14;
		screenCursorPos.Y = (short)(((myCursorPos.y - Console_GetWindowPosY()) / 14) - 2);/*Minus 2 for the title bar*/
		Console_SetCursorPos(screenCursorPos.X, screenCursorPos.Y);
		bMouseInputRecieved = 1;
	}

	/*Handle all mouse inputs*/
	if (bMouseInputRecieved) {
		if (GetAsyncKeyState(VK_LBUTTON)) {
			/*Ascii Key selection*/
			if (screenCursorPos.X > 0 && screenCursorPos.X < 9 && screenCursorPos.Y > 0 && screenCursorPos.Y < 33) {
				prevAsciiChar = currAsciiChar;
				currAsciiChar = (screenCursorPos.X-1) + (screenCursorPos.Y-1)* (asciiBrushPanelSize.X);
			}
			/*ForeGround Color Selection*/
			else if (screenCursorPos.X > 0 && screenCursorPos.X < 9 && screenCursorPos.Y > 33 && screenCursorPos.Y <  43) {
				currFGColor = (screenCursorPos.Y - 34)/2 + ((screenCursorPos.X - 1)/2 * colorPanelSize.Y/2) ;
			}
			/*BackGround Color Selection*/
			else if (screenCursorPos.X > 0 && screenCursorPos.X < 9 && screenCursorPos.Y > 43 && screenCursorPos.Y < 53) {
				currBGColor = (screenCursorPos.Y - 44) / 2 + ((screenCursorPos.X - 1) / 2 * colorPanelSize.Y / 2);
			}
			/*Painting!*/
			else if (screenCursorPos.X > 10 && screenCursorPos.X < 98 && screenCursorPos.Y > 1 && screenCursorPos.Y < 52) {
				Painting(1, screenCursorPos.X, screenCursorPos.Y);
			}
			else if (screenCursorPos.X > (loadAndSaveOrig.X-2) && screenCursorPos.X < loadAndSaveOrig.X + loadAndSaveSize.X
				&& screenCursorPos.Y > (loadAndSaveOrig.Y-1) && screenCursorPos.Y < loadAndSaveOrig.Y + loadAndSaveSize.Y) {
				
				if (screenCursorPos.Y < loadAndSaveOrig.Y + (loadAndSaveSize.Y/2)) {
					/*Save*/
					if (!bLMouseUp) {
						Save();
					}
					
				}
				else {
					/*Load*/
					if (!bLMouseUp) {
						Load();
					}
				}
			}
			bLMouseUp = 1;
		}
		else {
			/*If Left Mouse Up*/
			if (bLMouseUp) {
				bLMouseUp = 0;
			}
		}
		if (GetAsyncKeyState(VK_RBUTTON)) {
			if (screenCursorPos.X > 10 && screenCursorPos.X < 98 && screenCursorPos.Y > 1 && screenCursorPos.Y < 52) {
				Painting(2, screenCursorPos.X, screenCursorPos.Y);
			}
		}
	}
}

void Painting(int brushType, int posX , int posY) { /*1 is foregrd + backgrd, 2 is backgrd only*/
	if (brushType == 1) {
		canvasAsciiData[posX-11][posY-2][0] = currAsciiChar;
		canvasAsciiData[posX-11][posY-2][1] = currFGColor;
		canvasAsciiData[posX-11][posY-2][2] = currBGColor;
		paintingCoord.X = posX;
		paintingCoord.Y = posY;
	}
	else if (brushType == 2) {
		canvasAsciiData[posX][posY][2] = currBGColor;
		paintingCoord.X = posX;
		paintingCoord.Y = posY;
	}
	
	bIsPaintingBrush = brushType;
}

void Save() {
	int errno;

	if (saveIndex>=100) {
		/*ERROR, too many files*/
		return;
	}
	(void)_getcwd(saveFileString, FILENAME_MAX);
	strcat_s(saveFileString, FILENAME_MAX, saveFileName);
	
	if (errno != fopen_s(&fptr, saveFileString, "w") || !fptr)
	{
		/*fptr = fopen_s(&fptr, saveFileString, "w");
		strerror_s(saveFileString, 13, errno);*/
		printf("Oh dear, something went wrong with read()! %s\n", saveFileString);
		return;
	}
	else {
		fwrite(canvasAsciiData, sizeof(int), 13300,fptr);
		//fprintf(fptr,"%s", "HELLO");
		printf("Success");
		//fputs("fopen example", fptr);

		fclose(fptr);
	}

	saveIndex++;
}
void Load() {
	(void)_getcwd(saveFileString, FILENAME_MAX);
	strcat_s(saveFileString, FILENAME_MAX, loadFileName);

	if (errno = fopen_s(&fptr, saveFileString, "r") || !fptr)
	{
		strerror_s(saveFileString, 13, errno);
		printf("Oh dear, something went wrong with read()! %s\n", saveFileString);
		return;
	}
	else {
		fread(canvasAsciiData, sizeof(int), 13300, fptr);
		printf("Success");

		fclose(fptr);
	}
	bReloadCanvas = 1;
	loadIndex++;
}

void Update() {
	sprintf_s(saveFileName, 20, "//Ascii%d.txt", saveIndex);
	sprintf_s(loadFileName, 20, "//Ascii%d.txt", loadIndex);
	
}

void Rendering() {
	int i = 0; int j = 0;
	/*Update Mouse Cursor Information UI*/
	Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, DEFAULTLVB);

	if (screenCursorPos.X < 100 && screenCursorPos.Y < 60) {
		Console_SetRenderBuffer_String(currBrushPanelOrig.X + 10, currBrushPanelOrig.Y - 1, "MouseX");
		_itoa_s(screenCursorPos.X, chMousePos, 5, 10);
		Console_SetRenderBuffer_String(currBrushPanelOrig.X + 17, currBrushPanelOrig.Y - 1, chMousePos);
		if (screenCursorPos.X < 10 && screenCursorPos.X >= 0)
			Console_SetRenderBuffer_Char(currBrushPanelOrig.X + 18, currBrushPanelOrig.Y - 1, 0);

		Console_SetRenderBuffer_String(currBrushPanelOrig.X + 20, currBrushPanelOrig.Y - 1, "MouseY");
		_itoa_s(screenCursorPos.Y, chMousePos, 5, 10);
		Console_SetRenderBuffer_String(currBrushPanelOrig.X + 27, currBrushPanelOrig.Y - 1, chMousePos);
		if (screenCursorPos.Y < 10 && screenCursorPos.Y >= 0)
			Console_SetRenderBuffer_Char(currBrushPanelOrig.X + 28, currBrushPanelOrig.Y - 1, 0);
	}

	/*Current Ascii Selection Graphic Update*/   
	Console_SetRenderBuffer_Attribute(LIGHTRED, WHITE, GRID_FULL);
	Console_SetRenderBuffer_Char((prevAsciiChar % asciiBrushPanelSize.X) + 1, (prevAsciiChar / asciiBrushPanelSize.X) + 1, prevAsciiChar);
	Console_SetRenderBuffer_Attribute(LIGHTRED, BLACK, GRID_FULL);
	Console_SetRenderBuffer_Char((currAsciiChar % asciiBrushPanelSize.X) +1, (currAsciiChar / asciiBrushPanelSize.X) +1, currAsciiChar);
	
	/*Current BRUSH - ForeGround and BackGround Graphic Update*/
	Console_SetRenderBuffer_Attribute(currFGColor, currBGColor, DEFAULTLVB);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X, currBrushPanelOrig.Y, currAsciiChar);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X, currBrushPanelOrig.Y+1, 0);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X + 1, currBrushPanelOrig.Y, 0);
	Console_SetRenderBuffer_Char(currBrushPanelOrig.X + 1, currBrushPanelOrig.Y + 1, 0);

	if (bReloadCanvas) {
		for (i = 0; i < paintingGrid.X-1;i++) {
			for (j = 0; j < paintingGrid.Y-1; j++) {
				Console_SetRenderBuffer_Attribute(canvasAsciiData[i][j][1], canvasAsciiData[i][j][2], DEFAULTLVB);
				Console_SetRenderBuffer_Char(i+11, j+2, canvasAsciiData[i][j][0]);
			}
		}
		bReloadCanvas = 0;
	}

	/*Painting On Canvas*/
	if (bIsPaintingBrush) {
		if (bIsPaintingBrush == 1) {
			Console_SetRenderBuffer_Attribute(currFGColor, currBGColor, DEFAULTLVB);
			Console_SetRenderBuffer_Char(paintingCoord.X, paintingCoord.Y, currAsciiChar);
		}
		else if (bIsPaintingBrush == 2) {
			if (canvasAsciiData[paintingCoord.X][paintingCoord.Y][0] != 0) {
				Console_SetRenderBuffer_Attribute(canvasAsciiData[paintingCoord.X][paintingCoord.Y][1], currBGColor, DEFAULTLVB);
				Console_SetRenderBuffer_Char(paintingCoord.X, paintingCoord.Y, canvasAsciiData[paintingCoord.X][paintingCoord.Y][0]);
			}
			else {
				Console_SetRenderBuffer_Attribute(currFGColor, currBGColor, DEFAULTLVB);
				Console_SetRenderBuffer_Char(paintingCoord.X, paintingCoord.Y, 0);
			}
		}	
		bIsPaintingBrush = 0;
	}

	/*Save File Name*/
	Console_SetRenderBuffer_Attribute(LIGHTMAGENTA, WHITE, DEFAULTLVB);
	Console_SetRenderBuffer_String(loadAndSaveOrig.X+7, loadAndSaveOrig.Y, saveFileName);
	Console_SetRenderBuffer_String(loadAndSaveOrig.X + 7, loadAndSaveOrig.Y+5, loadFileName);
	Console_SwapRenderBuffer();
}