// /******************************************************************************/
// /*!
// \file         LineSeg.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _LINESEGMENT_H_
#define _LINESEGMENT_H_

#include "../../../Math/Vector/Vector2.h"

struct LineSeg
{
	Mathf::vec2 start;
	Mathf::vec2 end;

	LineSeg();
	LineSeg(Mathf::vec2 _start, Mathf::vec2 _end);
	LineSeg(float _radian);
	void switchEndPoint();
	LineSeg rotate(float _radian);
};

#endif
