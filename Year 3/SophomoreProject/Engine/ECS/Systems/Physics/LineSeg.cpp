// /******************************************************************************/
// /*!
// \file         LineSeg.cpp
// \project name Engine
// \author(s)    Poh Chun Ren, 100%
// \date         12th Nov, 2021
// \brief        Physics system for ecs that handles movement and collision
// \lines        24 Lines (seg)
//
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "LineSeg.h"
#include "../../../Math/Matrix/Matrix3.h"

LineSeg::LineSeg() = default;

LineSeg::LineSeg(Mathf::vec2 _start, Mathf::vec2 _end) :
	start{ _start },
	end{ _end }
{

	(void)_start;
	(void)_end;
}

LineSeg::LineSeg(float _radian) :
	start{},
	end{}
{
	(void)_radian;
}

void LineSeg::switchEndPoint()
{
}

LineSeg LineSeg::rotate(float _radian)
{
	(void)_radian;
	return LineSeg();
}
