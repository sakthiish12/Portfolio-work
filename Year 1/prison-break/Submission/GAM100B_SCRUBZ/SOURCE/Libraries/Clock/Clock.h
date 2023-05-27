/**********************************************************************************
* \file			Clock.h
* \brief		A C wrapper for Clock and Frame control system
* \author		Yannick Gerber
* \version		1.0
* \date			2019
*
*	The clock uses C++ to allow high precision timing. It is used for Frame time, 
*	DeltaTime calculation, Frame count and Frame Lock mechanism
*	All calculation are done in Microseconds (us)
*
* \note			Course: GAM100
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/

#ifndef CLOCK_H
#define CLOCK_H

#include <stdbool.h>

/**
 * \brief To be called at the beginning of your Game Loop
 * 
 * \note Used to calculate DeltaTime between 2 Loop ticks, and FrameTime
 */
void Clock_GameLoopStart();


/**
 * \brief Returns the Time in Milliseconds between 2 frames
 * 
 * \return DeltaTime in Milliseconds
 */
float Clock_GetDeltaTime();


/**
 * \brief Returns the ElapsedTime in Us (Microseconds) since the first call of Clock_GameLoopStart()
 * \return Elapsed Time in Us
 */
unsigned long Clock_GetElapsedTimeUs();


/**
 * \brief Returns the ElapsedTime in Ms (Milliseconds) since the first call of Clock_GameLoopStart()
 * \return Elapsed Time in Ms
 */
unsigned long Clock_GetElapsedTimeMs();


/**
 * \brief Returns the number of elapsed Frames
 * 
 * \return Number of elapsed Frame since the first Clock_GameLoopStart() call
 */
unsigned long Clock_GetFrameCount();

/**
 * \brief True if the current Loop is a Frame for the DesiredFPS
 * 
 * \return True for a new Frame
 * 
 * \note To use for Frame-Locking a Game
 */
bool Clock_IsAFrame();


/**
 * \brief Set the Desired Frame Per Seconds (FPS) if you want to create a frame locked Game
 * 
 * \param desiredFPS the desired Frame Per Seconds, 60 by default 
 */
void Clock_SetLockedFPS(int desiredFPS);

#endif // CLOCK_H
