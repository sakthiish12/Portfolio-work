/*********************************************************************************
TxtFileReader.c
Author: Chong Jun Yi
Code Lines: 166 (30 + 13 + 45 + 38 + 8 + 14 + 18)
Brief: Read text files from target folder to be used for game graphics. Binary text 
file generated from custom graphic editor. File path is recognised from a content 
page and sorted in to array.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <stdio.h>/*FILE*/
#include <Windows.h> /*errno*/
#include <direct.h> /* _getcwd*/
#include "TxtFileReader.h"
#include "../Global/Global.h"
#include "../Console/Console.h"

/****Overall File Reader variable****/
FILE *fptr; /*Targeted text file*/
char resourceFilePath[FILENAME_MAX]; /*Path of resource folder*/
char resourceContentPgPath[FILENAME_MAX]; /*Path of content page*/

/****Content Page variable****/
char readC = ' '; /*To Read Content page char by char*/
char readLine[100] = {0}; /*Max length of each line of content page*/
int bReadContentTitle = 0; /*Check for content header*/
int bErrorDetected[3] = {1}; /*Error from missing {starting title, category title, folder missing}*/

/****Type of assets in the game****/
char listOfCategory[6][20] = 
{ 
	{ "#LevelMap#" }, 
	{ "#CharSprite#" },
	{ "#ItemMap#" },
	{ "#ItemSprite#" },
	{ "#UIData#" },
	{ "#SplashScreen#" }
};
/****Content Page and Sorting****/
char pathOfCategory[36][FILENAME_MAX]; /*All the text file path location to be loaded into game*/
/*This is mainly for debuging and info*/
float pathOfCatCounter = 0.0f; /*int value is total files, decimal value is category fillers*/ 
int contentPageProgress = 0; /*Counter for category read*/
int bCategoryActive = 0; /*Make sure all the right files fall into the correct category*/
char tempPath[FILENAME_MAX]; /*Temp storage for overall text file length*/

/****List of Storage Array****/
int levelGraphicData[4][240][240][3] = {0};/*[Num of maps][storage width][storage height][Ascii,FG,BG]*/
int charGraphicData[1][1][1][3] = { 0 }; /*[Num of characters][storage width][stirage[height][Ascii,FG,BG]*/
int itemGraphicData[30][3][3][3] = { 0 };
int uIGraphicData[1][15][28][3] = { 0 }; /*[Num of Items][storage width][storage height][Ascii,FG,BG]*/
int uISplashScreenData[9][60][60][3] = { 0 };

/*********************************************************************************
ReadFromResource
Author: Chong Jun Yi 
Code Lines: 13
Brief: Called by game to start reading text resources
	1) Target the resource folder and content page txts
	2) Read the content page to gather all the individual text file path
	3) Call another function to read individual text file and save to array
	4) Comfirm global variable that assets are loaded
**********************************************************************************/
void ReadFromResource() {
	/*Get resource folder*/
	(void)_getcwd(resourceFilePath, FILENAME_MAX); /*Get the current directory*/
	//strcat_s(resourceFilePath, FILENAME_MAX, "\\Libraries\\TxtResource"); /*Get the resource directory*/
	strcat_s(resourceFilePath, FILENAME_MAX, "\\TxtResource"); /*RELEASE MODE*/

	/*Get content page*/
	sprintf_s(resourceContentPgPath, FILENAME_MAX, resourceFilePath);
	strcat_s(resourceContentPgPath, FILENAME_MAX, "\\ContentPage.txt");/*Get the content page path*/
	
	/*Read Content Page - Check for error before reading*/
	if (errno = fopen_s(&fptr, resourceContentPgPath, "r") || !fptr)
	{
		strerror_s(resourceContentPgPath, 50, errno);
		printf("Oh dear, something went wrong with read()! %s\n", resourceContentPgPath);
		bErrorDetected[2] = 1;
		/*Terminate upon error opening*/
		return;
	}
	else
	{
		ReadFromContentPg();
		//LoadToArray(10, 10, &fptr);
		//fread(canvasAsciiData, sizeof(int), 13300, fptr);
		bErrorDetected[2] = 0;
		fclose(fptr);
	}

	/*Start opening files and pass their value to array*/
	ReadFromIndividualTxt();
	Global_AssetsAreLoaded();
}

/*********************************************************************************
ReadFromResource
Author: Chong Jun Yi
Code Lines: 45
Brief: Gather the names of all the text files to be read based on their category
	1a) Read char by char (printf was for debugging)
	1b) Ignore blank spaces when saving into all paths array
	1c) Clear readLine when encounter newline
	1d) Otherwise, add the read char to readLine
	2) Based on data from readLine, we get the following information
	2a) ContentPage Main Title
	2b) Asset Category title
	2c) Asset File Name and therefore their paths
**********************************************************************************/
void ReadFromContentPg()
{
	readC = (char)fgetc(fptr);
	while (readC != EOF)
	{	
		/*Check what kind of char was read*/
		if (readC == ' ' || readC == '\t') {
			/*printf("%s", &readC);*/

			/*Read next char*/
			readC = (char)fgetc(fptr);
			continue;
		}
		else if (readC == '\n') 
		{
			/*Do not push \n to readline....but still continue the loop*/
			/*printf("%s", &readC);*/
			/*Specially for indivudal file paths*/
			/*printf("%s", readLine);*/ 
		}
		else
		{
			strcat_s(readLine, 100, &readC);
		}
		
		/*Pass information to be recorded*/
		if (readC =='#') {
			if (bReadContentTitle)
			{
				/*Reading Content Header Title*/
				if (strcmp(readLine, "#StartContent#") == 0)
				{
					bErrorDetected[0] = 0;/*Remove the error since file is correct*/
					/*printf("%s", readLine);*/
					readLine[0]= '\0';
				}
				else if (strcmp(readLine, "#EndContent#") == 0)
				{
					strcat_s(pathOfCategory[(int)pathOfCatCounter], FILENAME_MAX, readLine);
					pathOfCatCounter += 1.1f;
					/*printf("%s\n", readLine);*/
					readLine[0] = '\0';
					break;
				}
				else 
				{
					/*Reading Category of txt files*/
					if (strcmp(listOfCategory[contentPageProgress], readLine) == 0)
					{	
						bCategoryActive = 1;
						strcat_s(pathOfCategory[(int)pathOfCatCounter], FILENAME_MAX, readLine);
						pathOfCatCounter += 1.1f;
						/*printf("%s", readLine);*/
						readLine[0] = '\0';
					}
					else
					{
						bErrorDetected[1] = 1;
						//ERROR! category title not in order, or name is corrupted
					}
				}
				/*Stop Reading Title*/
				bReadContentTitle = 0;
			}
			else {
				/*Start Reading Title*/
				bReadContentTitle = 1;
				/*If encounter new category*/
				if (bCategoryActive) 
				{
					++contentPageProgress;
					bCategoryActive = 0;
				}
			}	
		}
		else if (readC == '\n') 
		{
			if (bErrorDetected[0] == 1)
			{
				//ERROR! Did not detect #StartContent#
			}
			
			/*Check if we have successful read a txtfile name*/
 			if (!bReadContentTitle && bCategoryActive) 
			{	
				/*After # there could be a new line, so must be the second newline encoutnered*/
				if (bCategoryActive == 2) {
					/*Reading file path and storing in array*/
					tempPath[0] = '\0';
					strcat_s(tempPath, FILENAME_MAX, resourceFilePath);
					strcat_s(tempPath, FILENAME_MAX, readLine);
					strcat_s(pathOfCategory[(int)pathOfCatCounter], FILENAME_MAX, tempPath);

					pathOfCatCounter += 1.0;
					bCategoryActive = 1;
				}	
				++bCategoryActive;
			}
  			readLine[0] = '\0';		
		}

		/*Read next char*/
		readC = (char)fgetc(fptr);
	}
}

/*********************************************************************************
ReadFromIndividualTxt
Author: Chong Jun Yi
Code Lines: 38
Brief: Start opening files based on path Array
	1) When encounter category title, save to different storage array
	2) Check if file can be open, if not skip 
**********************************************************************************/
void ReadFromIndividualTxt() {
	int i = 0;/*for loop index*/
	int fileIndex = 0; /*To sort the text file into their storage array*/

	contentPageProgress = -1;
	while (1)
	{
		if (pathOfCategory[i][0] != '#') {
			/*Check if file can be Open successfully*/
			if (errno = fopen_s(&fptr, pathOfCategory[i], "r") || !fptr)
			{
				strerror_s(pathOfCategory[i], 50, errno);
				printf("The freaking txt file does not exist! %s\n", pathOfCategory[i]);
			}
			else
			{
				/*Lets read the file data*/
				switch (contentPageProgress)
				{
				case 0:/*Level Data*/
					LoadToGraphicsArray(240, 240, &levelGraphicData[fileIndex][0][0][0]);
					break;
				case 1:/*Character Data*/
					LoadToGraphicsArray(1, 1, &charGraphicData[fileIndex][0][0][0]);
					break;
				case 2:/*Item Map*/
					MarkItemZoneOnMap(240, 240, &levelGraphicData[fileIndex][0][0][0]);
					break;
				case 3:/*Item Sprite*/
					/*First part is doors*/
					LoadToGraphicsArray2(3, 3, &itemGraphicData[fileIndex][0][0][0]);
					break;
				case 4:/*UI Data*/
					LoadToGraphicsArray(15, 28, &uIGraphicData[fileIndex][0][0][0]);
					break;
				case 5:	/*Splash Screen*/
					LoadToGraphicsArray(60, 60, &uISplashScreenData[fileIndex][0][0][0]);
					break;
				default:
					break;
				}
				fclose(fptr);
				++fileIndex; /*Move on to save in next storage space of the same category*/
			}
		}
		else if (pathOfCategory[i][0] == '#') 
		{
			/*Next Category or EOF*/
			if (strcmp(pathOfCategory[i], "#EndContent#") == 0)
			{
				break;
			}
			++contentPageProgress; /*Move on to next category*/
			fileIndex = 0; /*reset the storage index*/
		}
		++i; /*Read next file*/
	}
	/*printf("All files successfuly\n");*/
}	
	
/*********************************************************************************
LoadToGraphicsArray
Author: Chong Jun Yi
Code Lines: 8
Brief: Pass the data to graphics array
	1) Get int data using fread
	2) Reading from upper left corner, then going vectically downwards, after reach 
	vertical end, start again horizontal top right (1 space)
	3) Dont worry, will save to graphics array in [width][height][type] format
**********************************************************************************/
void LoadToGraphicsArray(int width, int height, int *array)
{
	/*Graphics array will always have 3 component, ASCII, FG and BG*/
	int w = 0, h = 0, rowOffset = 0; /* Count which dimension of the array*/
	int dataWidth = 88, dataHeight = 51;

	/*[width][height][type]*/
	/*How the array is arranged in memory [0][0][1-3] ... [0][1][1-3]*/
	for (w = 0; w < width && w < dataWidth; ++w)
	{
		for (h = 0; h < height*3 && h < dataHeight*3; h+=3)
		{
			rowOffset = width * w * 3; 
			fread_s(array + rowOffset + h + 0, sizeof(int), sizeof(int), 1, fptr);
			fread_s(array + rowOffset + h + 1, sizeof(int), sizeof(int), 1, fptr);
			fread_s(array + rowOffset + h + 2, sizeof(int), sizeof(int), 1, fptr);	
		}

		/*int hello = rowOffset;*/ /*Debugging*/
	}
}

/*********************************************************************************
LoadToGraphicsArray2
Author: Chong Jun Yi
Code Lines: 14
Brief: Variation of first function, for files Lesser than data Width = 88 data Height = 51
	1) Get int data using fread
	2) Reading from upper left corner, then going vectically downwards, after reach
	vertical end, start again horizontal top right (1 space)
	3) Dont worry, will save to graphics array in [width][height][type] format
**********************************************************************************/
void LoadToGraphicsArray2(int width, int height, int* array)
{
	/*Graphics array will always have 3 component, ASCII, FG and BG*/
	int w = 0, h = 0, rowOffset = 0; /* Count which dimension of the array*/
	int dataWidth = 88, dataHeight = 51;
	int tempRead[1];

	/*[width][height][type]*/
	/*How the array is arranged in memory [0][0][1-3] ... [0][1][1-3]*/
	for (w = 0; w < width && w < dataWidth; ++w)
	{
		for (h = 0; h < height * 3 /*&& h < dataHeight * 3*/; h += 3)
		{
			rowOffset = width * w * 3;
			fread_s(array + rowOffset + h + 0, sizeof(int), sizeof(int), 1, fptr);
			fread_s(array + rowOffset + h + 1, sizeof(int), sizeof(int), 1, fptr);
			fread_s(array + rowOffset + h + 2, sizeof(int), sizeof(int), 1, fptr);
		}

		while (h < dataHeight*3) {
			fread_s(tempRead, sizeof(int), sizeof(int), 1, fptr);
			fread_s(tempRead, sizeof(int), sizeof(int), 1, fptr);
			fread_s(tempRead, sizeof(int), sizeof(int), 1, fptr);
			h += 3;
		}
		/*int hello = rowOffset;*/ /*Debugging*/
	}
}

/*********************************************************************************
MarkItemZoneOnMap
Author: Chong Jun Yi
Code Lines: 18
Brief: From item editor file, mark white areas on the overal level data. Mainly
for zone items (larger than 1x1)
**********************************************************************************/
void MarkItemZoneOnMap(int width, int height, int* array)
{
	/*From the item map, mark out zones on the main level map where items are*/
	/*By changing their display ascii to item code (HEX)*/
	int w = 0, h = 0, rowOffset = 0; /* Count which dimension of the array*/
	int dataWidth = 88, dataHeight = 51;
	int tempReciever[1] = { 0 }; /*Recieve Data but do not store them */
	int tempItemCode = 0; /*Store possible item code recieved*/
	for (w = 0; w < width && w < dataWidth; ++w)
	{
		for (h = 0; h < height * 3 && h < dataHeight * 3; h += 3)
		{
			rowOffset = width * w * 3;
			if (fread_s(tempReciever, sizeof(int), sizeof(int), 1, fptr)) {
				//printf("%d", tempReciever[0]);		
			}
			if (tempReciever[0] != 0) {
				/*Store a possible item code*/
				tempItemCode = tempReciever[0];
				fread_s(tempReciever, sizeof(int), sizeof(int), 1, fptr);
				fread_s(tempReciever, sizeof(int), sizeof(int), 1, fptr);
				if (tempReciever[0] == WHITE) {
					/*Comfirm that pixel is a readable item zone*/
					*(array + rowOffset + h + 0) = tempItemCode;
					*(array + rowOffset + h + 2) = tempReciever[0];
				}
			}
			else {
				/*Clear out the text data*/
				fread_s(tempReciever, sizeof(int), sizeof(int), 1, fptr);
				fread_s(tempReciever, sizeof(int), sizeof(int), 1, fptr);
			}
		}
	}
}