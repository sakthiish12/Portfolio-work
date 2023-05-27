#include <Windows.h>
#include <stdio.h>/*whereX*/
#include <conio.h>/*whereX*/
#include <direct.h>
#include "Console.h"
#include "Main.h"
#include <string.h>/*Strlen*/

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
static COORD layerModeOrig = { 60,53 };
static COORD layerModeSize = { 18,2 };
static COORD itemCodeInputOrig = { 60, 56 };
static COORD itemCodeInputSize = { 4, 3 };
static COORD toolBoxOrig = { 28, 55 };

int canvasAsciiData[88][51][3];
int canvasOnionData[88][51][3];

int currFGColor = WHITE;
int currBGColor = BLACK;
int prevAsciiChar = 63; /*Question Mark*/
int currAsciiChar = 63; /*Question Mark*/

POINT myCursorPos;
COORD screenCursorPos = { 0,0 };
char chMousePos[5];
int bMouseInputRecieved = 0;

int bIsPaintingBrush = 0;
COORD paintingCoord = { 0,0 };

int saveIndex = 1;
int saveIndexOnion = 1;
FILE *fptr;
char saveFileString[FILENAME_MAX];
char saveFileName[20];
char loadFileName[20];
int loadIndex = 1;
int loadIndexOnion = 1;
bReloadCanvas = 0;
int bLMouseUp = 1;

/*Item Code Input*/
int bIsModifyItemCode = 0;/*Whether modifyng*/
int currItemCode = 0; /*Read as Hexa by users*/
char itemCodeInput[3] = { 0 };
int inputCounter = 0;
int selectItemCode = 00;/*the item code selected on canvas*/

float screenPixelX = 0.0f, screenPixelY = 0.0f;
int layerMode = 0; /*0 = Level Editor, 1 = Item Editor*/

/*Stationary*/
int toolIndex = 1; /*1 = brush, 2 = eraser, 3 = select and move, 4 = crop*/
char toolInfo[7] = "BRUSH";
int bSelectAreaActive = 0;
COORD selectAreaOrig = {0,0};
COORD selectAreaEnd = { 0,0};
COORD startDrag = { 0,0 };
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
	UI_LayerMode();
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
	Console_SetRenderBuffer_Attribute(LIGHTRED, WHITE, DEFAULTLVB);
	Console_SetRenderBuffer_String(toolBoxOrig.X, toolBoxOrig.Y + 1, "1:B 2:E 3:S 4:C");
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

void UI_LayerMode() {
	Console_SetRenderBuffer_Attribute(MAGENTA, LIGHTGRAY, DEFAULTLVB);
	Console_SetRenderBuffer_String(layerModeOrig.X + 8, layerModeOrig.Y, "LEVEL");
	Console_SetRenderBuffer_String(layerModeOrig.X + 8, layerModeOrig.Y + 1, "     ");
	Console_SetRenderBuffer_String(layerModeOrig.X + 14, layerModeOrig.Y, "ITEM");
	Console_SetRenderBuffer_String(layerModeOrig.X + 14, layerModeOrig.Y + 1, "    ");
	/*Layer Indicator*/
	Console_SetRenderBuffer_Char(layerModeOrig.X + 10 + (5 * layerMode), layerModeOrig.Y + 1, 30);
	Console_SetRenderBuffer_Attribute(MAGENTA, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(layerModeOrig.X, layerModeOrig.Y, "Editor: ");
	
}

void UI_ItemCodeInput() {
	char itemCodeGraphic1[5] = { 218, 196, 196, 191 ,0};
	char itemCodeGraphic3[5] = { 192, 196, 196, 217 ,0 };
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(itemCodeInputOrig.X, itemCodeInputOrig.Y, itemCodeGraphic1);
	Console_SetRenderBuffer_String(itemCodeInputOrig.X, itemCodeInputOrig.Y+2, itemCodeGraphic3);

	Console_SetRenderBuffer_String(itemCodeInputOrig.X +5, itemCodeInputOrig.Y, itemCodeGraphic1);
	Console_SetRenderBuffer_String(itemCodeInputOrig.X +5, itemCodeInputOrig.Y + 2, itemCodeGraphic3);
	
	/*Display currentItemCode*/
	_itoa_s(currItemCode, itemCodeInput, 3, 16);
	if (currItemCode<16) {
		Console_SetRenderBuffer_Char(itemCodeInputOrig.X + 1, itemCodeInputOrig.Y + 1, '0');
		Console_SetRenderBuffer_String(itemCodeInputOrig.X + 2, itemCodeInputOrig.Y + 1, itemCodeInput);
	}
	else {
		Console_SetRenderBuffer_String(itemCodeInputOrig.X + 1, itemCodeInputOrig.Y + 1, itemCodeInput);
	}
	
}

void ConvertItemInputToInt(char itemCodeInput[], int* currItemCode) {
	int length = (int)strlen(itemCodeInput);
	int i = 0, charToInt = 0, currInt = 0;

	*currItemCode = 0;
	/*Make sure char is hexadecimal compliant otherwise reject*/
	for (i = 0; i < length; ++i) {
		charToInt = itemCodeInput[i];
		currInt = 0;
		/*0-9, A-F, a-f*/
		if (charToInt >= 48 && itemCodeInput[i] <= 57) {
			currInt += (0x0001 << (length - 1 - i)*4) * (charToInt - 48);
		}
		else if (charToInt >= 65 && itemCodeInput[i] <= 70) {
			currInt += (0x0001 << (length - 1 - i) *4) * ((charToInt - 65) + 10);
		}
		else if (charToInt >= 97 && itemCodeInput[i] <= 102) {
			currInt += (0x0001 << (length - 1 - i) *4) * ((charToInt - 97) + 10);
		}
		else{
			/*ignore invalid input*/
		}
		*currItemCode += currInt;
	}
}

void UIClear_ItemCodeInput() {
	char itemCodeGraphic1[10] = {'0','0','0','0','0','0','0','0','0'};

	Console_SetRenderBuffer_Attribute(BLACK, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(itemCodeInputOrig.X, itemCodeInputOrig.Y, itemCodeGraphic1);
	Console_SetRenderBuffer_String(itemCodeInputOrig.X, itemCodeInputOrig.Y + 1, itemCodeGraphic1);
	Console_SetRenderBuffer_String(itemCodeInputOrig.X, itemCodeInputOrig.Y + 2, itemCodeGraphic1);
}

void ReadExistItemCode(int posX, int posY) {
	char readDisplay[3] = {0};

	selectItemCode = canvasOnionData[posX - 11][posY - 2][0];
	_itoa_s(selectItemCode, readDisplay, 3, 16);
	if (selectItemCode < 16) {
		Console_SetRenderBuffer_Char(itemCodeInputOrig.X + 6, itemCodeInputOrig.Y + 1, '0');
		Console_SetRenderBuffer_String(itemCodeInputOrig.X + 7, itemCodeInputOrig.Y + 1, readDisplay);
	}
	else {
		Console_SetRenderBuffer_String(itemCodeInputOrig.X + 6, itemCodeInputOrig.Y + 1, readDisplay);
	}
}

void ChangeTools(int newIndex) {
	if (toolIndex != newIndex) {
		toolIndex = newIndex;

		switch (toolIndex) {
			case 1:/*Brush*/
				strcpy_s(toolInfo, 7, "BRUSH ");
				break;
			case 2:/*Eraser*/
				strcpy_s(toolInfo, 7, "ERASER");
				break;
			case 3:/*Select*/
				strcpy_s(toolInfo, 7, "SELECT");
				break;
			case 4:/*Crop*/
				strcpy_s(toolInfo, 7, "CROP  ");
				break;
		}
	}
}

void Eraser(int mode, int posX, int posY) {
	if (mode == 0) /*Canvas*/
	{
		canvasAsciiData[posX - 11][posY - 2][0] = 0;
		canvasAsciiData[posX - 11][posY - 2][1] = 0;
		canvasAsciiData[posX - 11][posY - 2][2] = 0;
		paintingCoord.X = posX;
		paintingCoord.Y = posY;
	}
	else if (mode == 1) /*Onion*/
	{
		canvasOnionData[posX - 11][posY - 2][0] = 0;
		canvasOnionData[posX - 11][posY - 2][1] = 0;
		canvasOnionData[posX - 11][posY - 2][2] = 0;
		paintingCoord.X = posX;
		paintingCoord.Y = posY;
	}
	bIsPaintingBrush = 4;
}

void SelectState(int layer, int mode, int posX, int posY) {
	/*Layer 0 = canvas, 1 = onion*/
	/*Mode 0 = start drag, 1 = end drag, 2 = move drag??, 3= deselect*/
	int diffX = 0, diffY = 0;

	if (layer == 0) {
		if (mode == 0) {
			selectAreaOrig.X = posX;
			selectAreaOrig.Y = posY;
		}
		else if (mode == 1) {
			selectAreaEnd.X = posX;
			selectAreaEnd.Y = posY;
			bSelectAreaActive = 1;
		}
		else if (mode == 2) {
			if (startDrag.X == 0 && startDrag.Y == 0) {
				startDrag.X = posX;
				startDrag.Y = posY;
			}
			else {
				if (startDrag.X != posX || startDrag.Y != posY) {
					diffX = posX - startDrag.X;
					diffY = posY - startDrag.Y;
					/*Check if move is more than edge of screen*/
					if (selectAreaOrig.X + diffX <=paintingGridOrig.X || selectAreaEnd.X + diffX >= paintingGridOrig.X+paintingGrid.X) {
						diffX = 0;
						posX = startDrag.X;
					}
					if (selectAreaOrig.Y + diffY <= paintingGridOrig.Y || selectAreaEnd.Y + diffY >= paintingGridOrig.Y + paintingGrid.Y) {
						diffY = 0;
						posY = startDrag.Y;
					}
					MoveCanvasData(selectAreaOrig, selectAreaEnd, diffX, diffY);
					
					selectAreaOrig.X += diffX;
					selectAreaOrig.Y += diffY;
					selectAreaEnd.X += diffX;
					selectAreaEnd.Y += diffY;
					startDrag.X = posX;
					startDrag.Y = posY;
					bReloadCanvas = 1;
				}
			}
		}
		else if (mode == 3) {
			selectAreaOrig.X = 0;
			selectAreaOrig.Y = 0;
			selectAreaEnd.X = 0;
			selectAreaEnd.Y = 0;
			startDrag.X = 0;
			startDrag.Y = 0;
			bSelectAreaActive = 0; 
			bReloadCanvas = 1;
		}
	}
	else if (layer == 1) {
		if (mode == 0) {
		}
		else if (mode == 1) {

		}
		else if (mode == 2) {

		}
		else if (mode == 3) {

		}
	}
}

void MoveCanvasData(COORD start, COORD end, int displaceX, int displaceY) {
	//-11 -2
	int w = 0, h = 0;
	if (displaceX <0 || displaceY < 0) {
		for (w = start.X; w <= end.X; ++w) {
			for (h = start.Y; h <= end.Y; ++h) {
				//Console_SetRenderBuffer_Attribute(canvasAsciiData[w-11][h-2][1], canvasAsciiData[w-11][h-2][2], DEFAULTLVB);
				//Console_SetRenderBuffer_Char(w + displaceX, h + displaceY, canvasAsciiData[w-11][h-2][0]);

				canvasAsciiData[w - 11+ displaceX][h - 2 + displaceY][0] = canvasAsciiData[w - 11][h - 2][0];
				canvasAsciiData[w - 11 + displaceX][h - 2 + displaceY][1] = canvasAsciiData[w - 11][h - 2][1];
				canvasAsciiData[w - 11 + displaceX][h - 2 + displaceY][2] = canvasAsciiData[w - 11][h - 2][2];
				canvasAsciiData[w - 11][h - 2][0] = 0;
				canvasAsciiData[w - 11][h - 2][1] = 0;
				canvasAsciiData[w - 11][h - 2][2] = 0;
			}
		}
	}
	else if (displaceX > 0 || displaceY > 0) {
		for (w = end.X; w >= start.X; --w) {
			for (h = end.Y; h >= start.Y; --h) {
				canvasAsciiData[w - 11 + displaceX][h - 2 + displaceY][0] = canvasAsciiData[w - 11][h - 2][0];
				canvasAsciiData[w - 11 + displaceX][h - 2 + displaceY][1] = canvasAsciiData[w - 11][h - 2][1];
				canvasAsciiData[w - 11 + displaceX][h - 2 + displaceY][2] = canvasAsciiData[w - 11][h - 2][2];
				canvasAsciiData[w - 11][h - 2][0] = 0;
				canvasAsciiData[w - 11][h - 2][1] = 0;
				canvasAsciiData[w - 11][h - 2][2] = 0;
			}
		}
	}
	
}

void ProcessInput() {
	bMouseInputRecieved = 0;
	if (GetCursorPos(&myCursorPos)) {
		screenCursorPos.X = (short)((myCursorPos.x - Console_GetWindowPosX()) / screenPixelX);
		screenCursorPos.Y = (short)(((myCursorPos.y - Console_GetWindowPosY()) / screenPixelY) - 2);/*Minus 2 for the title bar*/
		Console_SetCursorPos(screenCursorPos.X, screenCursorPos.Y);
		bMouseInputRecieved = 1;
	}

	/*Handle all mouse inputs*/
	if (bMouseInputRecieved) {
		if (GetAsyncKeyState(VK_LBUTTON)) 
		{
			/*Ascii Key selection*/
			if (screenCursorPos.X > 0 && screenCursorPos.X < 9 && screenCursorPos.Y > 0 && screenCursorPos.Y < 33) {
				prevAsciiChar = currAsciiChar;
				currAsciiChar = (screenCursorPos.X-1) + (screenCursorPos.Y-1)* (asciiBrushPanelSize.X);
			}
			/*ForeGround Color Selection*/
			else if (screenCursorPos.X > 0 && screenCursorPos.X < 9 && screenCursorPos.Y > 33 && screenCursorPos.Y < 43) {

				if (layerMode == 0) {
					currFGColor = (screenCursorPos.Y - 34) / 2 + ((screenCursorPos.X - 1) / 2 * colorPanelSize.Y / 2);
				}
			}
			/*BackGround Color Selection*/
			else if (screenCursorPos.X > 0 && screenCursorPos.X < 9 && screenCursorPos.Y > 43 && screenCursorPos.Y < 53) {
				if (layerMode == 0) {
					currBGColor = (screenCursorPos.Y - 44) / 2 + ((screenCursorPos.X - 1) / 2 * colorPanelSize.Y / 2);
				}
			}
			/*Canvas!*/
			else if (screenCursorPos.X > 10 && screenCursorPos.X < 98 && screenCursorPos.Y > 1 && screenCursorPos.Y < 52) {
				if (layerMode == 0) {
					if (toolIndex == 1) {
						Painting(1, screenCursorPos.X, screenCursorPos.Y);
					}
					else if (toolIndex == 2) {
						Eraser(0, screenCursorPos.X, screenCursorPos.Y);
					}
					else if (toolIndex == 3 ) {
						if (!bSelectAreaActive && !bLMouseUp) {
							SelectState(0, 0, screenCursorPos.X, screenCursorPos.Y);
						}
						else {
							if (screenCursorPos.X >=selectAreaOrig.X && screenCursorPos.Y >= selectAreaOrig.Y
								&& screenCursorPos.X <= selectAreaEnd.X && screenCursorPos.Y <= selectAreaEnd.Y) {
								SelectState(0, 2, screenCursorPos.X, screenCursorPos.Y);
							}
							else if (!bLMouseUp) {
								SelectState(0, 3, screenCursorPos.X, screenCursorPos.Y);
							}
						}	
					}
					else if (toolIndex == 4) {

					}
				}
				else
				{
					if (toolIndex == 1) {
						OnionPainting(screenCursorPos.X, screenCursorPos.Y);
					}
					else if (toolIndex == 2) {
						Eraser(1, screenCursorPos.X, screenCursorPos.Y);
					}
					else if (toolIndex == 3 && !bLMouseUp) {
						SelectState(1, 0, screenCursorPos.X, screenCursorPos.Y);
					}
					else if (toolIndex == 4) {

					}
				}
			}
			/*Save and Load*/
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
			/*Change Layer*/
			else if (screenCursorPos.X >= layerModeOrig.X && screenCursorPos.Y >= layerModeOrig.Y &&
				screenCursorPos.X < layerModeOrig.X + layerModeSize.X && screenCursorPos.Y < layerModeOrig.Y + layerModeSize.Y) 
			{
				if (screenCursorPos.X < layerModeOrig.X + 13 && screenCursorPos.X > layerModeOrig.X + 8) {
					ChangeMode(0);
				}
				else if (screenCursorPos.X < layerModeOrig.X + 18 && screenCursorPos.X > layerModeOrig.X + 14) {
					ChangeMode(1);
				}
			}
			/*Item Code Input*/
			else if (layerMode == 1 && screenCursorPos.X >= itemCodeInputOrig.X+1 && screenCursorPos.Y >= itemCodeInputOrig.Y+1 &&
				screenCursorPos.X < itemCodeInputOrig.X + itemCodeInputSize.X-1 && itemCodeInputOrig.Y < itemCodeInputOrig.Y + itemCodeInputSize.Y-1) 
			{
				if (!bIsModifyItemCode) {
					bIsModifyItemCode = 1;
					strcpy_s(itemCodeInput,3,"");;
				}
			}
			else {
							
			}
			bLMouseUp = 1;
		}
		else {
			/*If Left Mouse Up*/
			if (bLMouseUp) {
				if (toolIndex == 3 && (selectAreaOrig.X != 0 && selectAreaOrig.Y != 0) && !bSelectAreaActive) {
					SelectState(layerMode, 1, screenCursorPos.X, screenCursorPos.Y);
				}
				bLMouseUp = 0;
			}
		}

		if (GetAsyncKeyState(VK_RBUTTON)) {
			if (screenCursorPos.X > 10 && screenCursorPos.X < 98 && screenCursorPos.Y > 1 && screenCursorPos.Y < 52) {
				if (layerMode == 0) {
					Painting(2, screenCursorPos.X, screenCursorPos.Y);
				}
				else
				{
					ReadExistItemCode(screenCursorPos.X, screenCursorPos.Y);
				}
				
			}
		}

		if (GetAsyncKeyState(VK_RETURN)) {
			//if (bIsModifyItemCode == 1) {
			//	UI_ItemCodeInput();
			//	bIsModifyItemCode = 0;
			//}
			
		}
		
		if (GetAsyncKeyState('1')) {
			ChangeTools(1);
		}
		else if (GetAsyncKeyState('2')) {
			ChangeTools(2);
		}
		else if (GetAsyncKeyState('3')) {
			ChangeTools(3);
		}
		else if (GetAsyncKeyState('4')) {
			ChangeTools(4);
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
		if (layerMode == 0) {
			fwrite(canvasAsciiData, sizeof(int), 13300, fptr);
			//fprintf(fptr,"%s", "HELLO");
			printf("Success");
			//fputs("fopen example", fptr);
			saveIndex++;
		}
		else if (layerMode == 1) {
			fwrite(canvasOnionData, sizeof(int), 13300, fptr);
			//fprintf(fptr,"%s", "HELLO");
			printf("Success Onion");
			//fputs("fopen example", fptr);
			saveIndexOnion++;
		}
		

		fclose(fptr);
	}

	
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
		
		if (layerMode == 0) {
			fread(canvasAsciiData, sizeof(int), 13300, fptr);
			//fprintf(fptr,"%s", "HELLO");
			printf("Success");
			//fputs("fopen example", fptr);
			loadIndex++;
		}
		else if (layerMode == 1) {
			fread(canvasOnionData, sizeof(int), 13300, fptr);
			printf("Success Onion");
			++loadIndexOnion;
		}

		fclose(fptr);
	}
	bReloadCanvas = 1;
	
}

void ChangeMode(int mode) {
	if (mode != layerMode) {
		if (mode == 0) {
			layerMode = 0;
			UIClear_ItemCodeInput();
		}
		else if (mode == 1) {
			layerMode = 1;
			currFGColor = BLACK;
			currBGColor = WHITE;
			UI_ItemCodeInput();
		}
		bReloadCanvas = 1;
	}
	
}

void OnionPainting(int posX, int posY)
{
	canvasOnionData[posX - 11][posY - 2][0] = 0;
	if (currItemCode >0) {
		canvasOnionData[posX - 11][posY - 2][0] = currItemCode;
	}
	canvasOnionData[posX - 11][posY - 2][1] = currFGColor;
	canvasOnionData[posX - 11][posY - 2][2] = currBGColor;
	paintingCoord.X = posX;
	paintingCoord.Y = posY;
	bIsPaintingBrush = 3;
}

void Update() {
	int c = 0;
	
	if (layerMode == 0) {
		sprintf_s(saveFileName, 20, "//Ascii%d.txt", saveIndex);
		sprintf_s(loadFileName, 20, "//Ascii%d.txt", loadIndex);
	}
	else if (layerMode == 1) {
		sprintf_s(saveFileName, 20, "//Item%d.txt", saveIndexOnion);
		sprintf_s(loadFileName, 20, "//Item%d.txt", loadIndexOnion);
	}
	

	/*Check Screen Pixel Size*/
	screenPixelX = Console_GetWindowHorizontal() / (float)Console_GetWidth();
	screenPixelY = Console_GetWindowVertical() / (float)(Console_GetHeight()+2);

	/*Check if user is doing any input*/
	if (bIsModifyItemCode) {
		if (fgets(itemCodeInput, 3, stdin) != NULL) {	/**/
			bIsModifyItemCode = 0;
			ConvertItemInputToInt(itemCodeInput, &currItemCode);
			UI_ItemCodeInput();
		}

		
	}
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

	/*Update Current stationary tool*/
	Console_SetRenderBuffer_Attribute(WHITE, BLACK, DEFAULTLVB);
	Console_SetRenderBuffer_String(toolBoxOrig.X , toolBoxOrig.Y, "Tool: ");
	Console_SetRenderBuffer_String(toolBoxOrig.X+6, toolBoxOrig.Y, toolInfo);

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

	/*ReLoad Canvas*/
	
	if (bReloadCanvas) {
		if (layerMode == 0) /*Level Editor*/
		{
			for (i = 0; i < paintingGrid.X - 1; i++) {
				for (j = 0; j < paintingGrid.Y - 1; j++) {
					Console_SetRenderBuffer_Attribute(canvasAsciiData[i][j][1], canvasAsciiData[i][j][2], DEFAULTLVB);
					Console_SetRenderBuffer_Char(i + 11, j + 2, canvasAsciiData[i][j][0]);
				}
			}
		}
		else if (layerMode == 1) /*Item Editor*/
		{
			/*Set an onion skin*/
			Console_SetRenderBuffer_Attribute(YELLOW, BLACK, DEFAULTLVB);
			for (i = 0; i < paintingGrid.X - 1; i++) {
				for (j = 0; j < paintingGrid.Y - 1; j++) {
					
					Console_SetRenderBuffer_Char(i + 11, j + 2, canvasAsciiData[i][j][0]);
				}
			}

			Console_SetRenderBuffer_Attribute(BLACK, WHITE, DEFAULTLVB);
			for (i = 0; i < paintingGrid.X - 1; i++) {
				for (j = 0; j < paintingGrid.Y - 1; j++) {

					if (canvasOnionData[i][j][0] == 0 && canvasOnionData[i][j][2] == WHITE) {
						Console_SetRenderBuffer_Char(i + 11, j+2, 0);
					}
					else if (canvasOnionData[i][j][0] != 0) {
						Console_SetRenderBuffer_Char(i + 11, j+2, 158);
					}
				}
			}
		}

		/*Layer Indicator*/
		/*Manually Clear the previous indications*/
		Console_SetRenderBuffer_Attribute(MAGENTA, LIGHTGRAY, DEFAULTLVB);
		Console_SetRenderBuffer_Char(layerModeOrig.X + 10 + (5 * 0), layerModeOrig.Y + 1, 0);
		Console_SetRenderBuffer_Char(layerModeOrig.X + 10 + (5 * 1), layerModeOrig.Y + 1, 0);
		Console_SetRenderBuffer_Char(layerModeOrig.X + 10 + (5 * layerMode), layerModeOrig.Y + 1, 30);

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
		/*Onion Skin*/
		else if (bIsPaintingBrush == 3) {
			Console_SetRenderBuffer_Attribute(currFGColor, currBGColor, DEFAULTLVB);
			if (canvasOnionData[paintingCoord.X-11][paintingCoord.Y-2][0] == 0) {
				Console_SetRenderBuffer_Char(paintingCoord.X, paintingCoord.Y, 0);
			}
			else if (canvasOnionData[paintingCoord.X - 11][paintingCoord.Y - 2][0] != 0) {
				Console_SetRenderBuffer_Char(paintingCoord.X, paintingCoord.Y, 158);
			}
			
		}
		/*Eraser*/
		else if (bIsPaintingBrush == 4) {
			Console_SetRenderBuffer_Attribute(BLACK, BLACK, DEFAULTLVB);
			Console_SetRenderBuffer_Char(paintingCoord.X, paintingCoord.Y, 0);
		}
		bIsPaintingBrush = 0;
	}

	if (bSelectAreaActive) {
		/*Clear past selection*/
		
		/*Display new selection*/
		for (i = selectAreaOrig.X; i <= selectAreaEnd.X; ++i) {
			Console_SetRenderBuffer_Attribute(BLACK, RED, DEFAULTLVB);
			Console_SetRenderBuffer_Char(i, selectAreaOrig.Y, 8);
			Console_SetRenderBuffer_Char(i, selectAreaEnd.Y, 8);
		}
		for (i = selectAreaOrig.Y; i <= selectAreaEnd.Y; ++i) {
			Console_SetRenderBuffer_Attribute(BLACK, RED, DEFAULTLVB);
			Console_SetRenderBuffer_Char(selectAreaOrig.X, i, 8);
			Console_SetRenderBuffer_Char(selectAreaEnd.X, i, 8);
		}
	}

	/*Save File Name*/
	Console_SetRenderBuffer_Attribute(LIGHTMAGENTA, WHITE, DEFAULTLVB);
	Console_SetRenderBuffer_String(loadAndSaveOrig.X + 7, loadAndSaveOrig.Y, "            ");
	Console_SetRenderBuffer_String(loadAndSaveOrig.X + 7, loadAndSaveOrig.Y+5, "            ");
	Console_SetRenderBuffer_String(loadAndSaveOrig.X + 7, loadAndSaveOrig.Y, saveFileName);
	Console_SetRenderBuffer_String(loadAndSaveOrig.X + 7, loadAndSaveOrig.Y+5, loadFileName);
	Console_SwapRenderBuffer();
}