// /******************************************************************************/
// /*!
// \file         PhysicsDebugging.cpp
// \project name Engine
// \author(s)    Poh Chun Ren
// \date         8th Oct, 2021
// \brief        Drawing collision boundary using graphic debugging
// \lines        21 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "PhysicsDebugging.h"

#include "../../Graphics/Source/BatchRenderer2D.h"
#include "../../Graphics/Source/GraphicsDebugging.h"
#include "../../Graphics/Source/EditorCamera.h"

#define GREEN_R 0.f
#define GREEN_G 1.f
#define GREEN_B 0.f

void PhysicsDebugging::drawDebugVelocity(const transform2D& t, const rigidBody2D& rb, bool useBatchRenderer)
{
	vec2 velEnd = vec2(t.pos) + rb.vel;
	if (useBatchRenderer)
	{
		BatchRenderer2D::GetInstance().AddRotateLine(t.pos.x, t.pos.y, velEnd.x, velEnd.y, GREEN_R, GREEN_G, GREEN_B);
	}
	else
	{
		GraphicsDebugging::GetInstance().drawLine(t.pos, velEnd, Mathf::Vector4(1.0f, 0.f, 1.f, 1.0f), 2);
	}
}

void PhysicsDebugging::drawDebugAABB(const transform2D& t, const AABB& aabb, bool isEditorMode, bool useBatchRenderer)
{
	if (isEditorMode == true)
	{
		//use imgui camera
	}
	if (useBatchRenderer)
	{
		BatchRenderer2D::GetInstance().AddRotateLine(aabb.min.x, aabb.min.y, aabb.min.x, aabb.max.y, GREEN_R, GREEN_G, GREEN_B);
		BatchRenderer2D::GetInstance().AddRotateLine(aabb.min.x, aabb.max.y, aabb.max.x, aabb.max.y, GREEN_R, GREEN_G, GREEN_B);
		BatchRenderer2D::GetInstance().AddRotateLine(aabb.max.x, aabb.max.y, aabb.max.x, aabb.min.y, GREEN_R, GREEN_G, GREEN_B);
		BatchRenderer2D::GetInstance().AddRotateLine(aabb.max.x, aabb.min.y, aabb.min.x, aabb.min.y, GREEN_R, GREEN_G, GREEN_B);
	}
	else
	{
		Mathf::Vector2 bbcenter{ t.pos.x + aabb.offset_pos.x, t.pos.y + aabb.offset_pos.y };
		GraphicsDebugging::GetInstance().drawLineRect(bbcenter, aabb.scale, Mathf::Vector4(1.0f, 0.f, 1.f, 1.0f));
	}
	
}

void PhysicsDebugging::drawDebugPolygon(const transform2D& t, const Poly& poly, bool isEditorMode, bool useBatchRenderer)
{
	static_cast<void>(isEditorMode);//unreferenced variable 
	static_cast<void>(t);//unreferenced variable 
	if (useBatchRenderer)
	{
		for (size_t i = 0; i < poly.points.size(); ++i)
		{
			size_t j = (i + 1) % poly.points.size();
			BatchRenderer2D::GetInstance().AddRotateLine(poly.points[i].x, poly.points[i].y, poly.points[j].x, poly.points[j].y, GREEN_R, GREEN_G, GREEN_B);
		}
	}
	else
	{
		for (size_t i = 0; i < poly.points.size(); ++i)
		{
			size_t j = (i + 1) % poly.points.size();
			GraphicsDebugging::GetInstance().drawLine(poly.points[i], poly.points[j], Mathf::Vector4(1.0f, 0.f, 1.f, 1.0f), 2);
		}
	}
}

void PhysicsDebugging::drawRay(RayCast& r, bool useBatchRenderer)
{
	//UNREFERENCED_PARAMETER(useBatchRenderer);
	(void)useBatchRenderer;
	drawRayLine(r);
	drawRayPoint(r);
}

void PhysicsDebugging::drawRayLine(RayCast& r, bool useBatchRenderer)
{
	if (useBatchRenderer)
	{
		BatchRenderer2D::GetInstance().AddRotateLine(r.start.x, r.start.y, r.dir.x, r.dir.y, GREEN_R, GREEN_G, GREEN_B);
	}
	else
	{
		GraphicsDebugging::GetInstance().drawLine(r.start, r.dir, Mathf::Vector4(1.0f, 0.f, 1.f, 1.0f), 2);
	}
}

void PhysicsDebugging::drawRayPoint(RayCast& r, bool useBatchRenderer)
{
	if (r.rayHit)
	{
		if (useBatchRenderer)
		{
			//NEED POINT FOR BATCH Renderer
			//BatchRenderer2D::GetInstance().AddRotateLine(r.start.x, r.start.y, r.dir.x, r.dir.y, GREEN_R, GREEN_G, GREEN_B);
		}
		else
		{
			GraphicsDebugging::GetInstance().drawPoint(r.IntersectedPoint, 2, Mathf::Vector4(1.0f, 0.f, 1.f, 1.0f));
		}
	}
}

void PhysicsDebugging::drawArcRange(float angleFromPlayer, const transform2D& t, float arc_angle, float mag, bool useBatchRenderer)
{
	float cosRot = cos(angleFromPlayer + arc_angle);
	float sinRot = sin(angleFromPlayer + arc_angle);
	int linewidth = 2;
	(void)linewidth;
	//draw arc limit
	vec3 arcVec{ 0.f, mag, 0.f };//need to form a vec towards orientation
	Matrix3 arcrotate{
		cosRot, sinRot, 0.f,
		-sinRot, cosRot, 0.f,
		0.f, 0.f, 1.f
	};
	arcVec = t.pos + arcrotate * arcVec;
	vec2 ArcVec{ arcVec.x , arcVec.y };
	//GraphicsDebugging::GetInstance().drawLine(t.pos, ArcVec, Mathf::Vector4(1.0f, 0.5f, 1.f, 1.0f), 2);

	vec3 arcVec2{ 0.f, mag, 0.f };
	cosRot = cos(angleFromPlayer - arc_angle);
	sinRot = sin(angleFromPlayer - arc_angle);
	Matrix3 arcrotate2{
		cosRot, sinRot, 0.f,
		-sinRot, cosRot, 0.f,
		0.f, 0.f, 1.f
	};
	arcVec2 = t.pos + arcrotate2 * arcVec2;
	vec2 ArcVec2{ arcVec2.x , arcVec2.y };
	//GraphicsDebugging::GetInstance().drawLine(t.pos, ArcVec2, Mathf::Vector4(1.0f, 0.5f, 1.f, 1.0f), 2);

	if (useBatchRenderer)
	{
		BatchRenderer2D::GetInstance().AddRotateLine(t.pos.x, t.pos.y, ArcVec.x, ArcVec.y, 1.f, 0.f, 0.f);
		BatchRenderer2D::GetInstance().AddRotateLine(t.pos.x, t.pos.y, ArcVec2.x, ArcVec2.y, 1.f, 0.f, 0.f);
	}
	else
	{
		GraphicsDebugging::GetInstance().drawLine(t.pos, ArcVec, Mathf::Vector4(1.0f, 0.5f, 1.f, 1.0f), 2);
		GraphicsDebugging::GetInstance().drawLine(t.pos, ArcVec2, Mathf::Vector4(1.0f, 0.5f, 1.f, 1.0f), 2);
	}

}

void PhysicsDebugging::drawSuctionLimit(const transform2D& t, vec2& targetPos, bool useBatchRenderer)
{
	if (useBatchRenderer)
	{
		BatchRenderer2D::GetInstance().AddRotateLine(t.pos.x, t.pos.y, targetPos.x, targetPos.y, 1.f, 0.f, 0.f);
	}
	else
	{
		GraphicsDebugging::GetInstance().drawLine(t.pos, targetPos, Mathf::Vector4(1.0f, 0.f, 1.f, 1.0f), 2);
	}
}
