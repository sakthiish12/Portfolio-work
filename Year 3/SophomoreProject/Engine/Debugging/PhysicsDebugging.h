// /******************************************************************************/
// /*!
// \file         PhysicsDebugging.h
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Drawing collision boundary using graphic debugging
// \lines        10 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#ifndef _PHYSICSDEBUGGING_H_
#define _PHYSICSDEBUGGING_H_

#include "../ECS/component.h"
#include "../ECS/Systems/Physics/Ray.h"

#define USE_BATCH_RENDERING true

namespace PhysicsDebugging
{
	void drawDebugVelocity(const transform2D& t, const rigidBody2D& rb, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawDebugAABB(const transform2D& t, const AABB& aabb, bool isEditorMode = false, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawDebugPolygon(const transform2D& t, const Poly& poly, bool isEditorMode = false, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawRay(RayCast& r, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawRayLine(RayCast& r, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawRayPoint(RayCast& r, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawArcRange(float angleFromPlayer, const transform2D& t, float arc_angle, float mag = 100.f, bool useBatchRenderer = USE_BATCH_RENDERING);
	void drawSuctionLimit(const transform2D& t, vec2& targetPos, bool useBatchRenderer = USE_BATCH_RENDERING);
}

#endif //_PHYSICSDEBUGGING_H_