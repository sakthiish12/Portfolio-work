/**********************************************************************************
* \file			Random.h
* \brief		Small Wrapper for the C Random
* \author		Yannick Gerber
* \version		1.0
* \date			2019
*
* \note			Course: GAM100
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/

#ifndef RANDOM_H
#define RANDOM_H

/**
 * \brief Initializes the Random Seed
 */
void Random_Init();


/**
 * \brief Returns a Random number between a specified range
 * \param min_n Minimum Value
 * \param max_n Maximum Value
 * \return Random Number
 * 
 * \note This is not a true accurate random, the usage of modulo lessens the spread
 *		 but it is good enough for a small game
 */
int Random_Range(int min_n, int max_n);


#endif // RANDOM_H
