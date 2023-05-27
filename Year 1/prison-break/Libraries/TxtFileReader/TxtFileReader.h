/*********************************************************************************
TxtFileReader.h
Author: Chong Jun Yi
Code Lines: 15
Brief: Read text files from target folder to be used for game graphics. Binary text
file generated from custom graphic editor. File path is recognised from a content
page and sorted in to array.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef TXTFILEREADER_H
#define TXTFILEREADER_H

int bErrorDetected[3];

int levelGraphicData[4][240][240][3];/*[3] -> Ascii,FG,BG*/
int charGraphicData[1][1][1][3];
int itemGraphicData[30][3][3][3];
int uIGraphicData[1][15][28][3];
int uISplashScreenData[9][60][60][3];
/**
 * \brief	Read from resource file
 */
void ReadFromResource();

/**
 * \brief	Read from content page
 */
void ReadFromContentPg();

/**
 * \brief	Read individual file path
 */
void ReadFromIndividualTxt();

/**
 * \brief	Read individual file data
 */
void LoadToGraphicsArray(int width, int height, int *array);

void LoadToGraphicsArray2(int width, int height, int* array);

void MarkItemZoneOnMap(int width, int height, int* array);

#endif // TXTFILEREADER_H


