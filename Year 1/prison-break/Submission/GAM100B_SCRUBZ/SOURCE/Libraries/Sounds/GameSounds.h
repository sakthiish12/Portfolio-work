/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef GAMESOUNDS_H
#define GAMESOUNDS_H


/*Initalise FMOD SYSTEM (ONCE PER GAME)*/
void InitFMod();

/*Create all sound data and channel*/
void FmodCreateSound();

/*Play sound when needed, all sound will be released in sound release*/
void FmodPlayGameSound(int index);

/*Update for FMOD engine*/
void FmodUpdate();

/*Release memory for sound*/
void FmodSoundRelease();

/*Release memory for FMOD SYSTEM*/
void FmodClose();
#endif // !GAMESOUNDS_H
