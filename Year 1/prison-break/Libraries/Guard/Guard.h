/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef GUARD_H
#define GUARD_H
#include "../Global/Global.h"
#include "../Human/Human.h"

/*********************************************************************************
/					Guard global enums, structs and functions
/*********************************************************************************/
/*********************************************************************************
Guards enum, struct and statemachine
Author: Xie Hao Zhe (Statemachine, look around, detection, Astar pathfinding)
Code Lines: 45 (approx)
Brief: Guards enum, struct and statemachine
**********************************************************************************/

/*Different state of Guard*/
typedef enum
{
	Patrol_Idle_State,
	Patrol_Alert_State,
	Chase_Delay_State,
	Chase_Alert_State,
	Chase_Idle_State,
	Chase_ReturnPatrol_State,
	Look_Around_State,
	Chase_LookArd_State,
	Investigate_Alert_State,
	Investigating_State,
	Investigating_ReturnPatrol_State
}eGuardState;

/*Different Guard events*/
typedef enum
{
	Guard_No_Event,
	Guard_Reset_Event,
	Guard_Detected_Event,
	Guard_DetectedDelay_Event,
	Guard_StartChase_Event,
	Guard_MidChase_Event,
	Guard_ReturnPatrol_Event,
	Guard_StaticLookArd_Event,
	Guard_ChaseLookArd_Event,
	Guard_Investigate_Event,
	Guard_InvestigateComfirm_Event,
	Guard_InvestigateEnd_Event
}eGuardEvent;

typedef struct GUARD
{
	char name[20];			/*Name of human: Guard, Player, NPC*/
	char graphicIndex;		/*The index from graphics array*/
	COORD pos;				/*Current DISPLAY position of human*/
	fCOORD fpos;			/*Current EXACT position of human*/
	COORD wayPts[6];		/*Max 6 Waypoints for AI*/
	COORD startPt;			/*Starting Waypoint*/
	COORD endPt;			/*Destination Waypoint*/
	int currWayPt;			/*Index of Starting Waypoint*/
	int totalWayPt;			/*Total number of Waypoint*/
	int moveSpeed;			/*Movement Speed per realtime sec of human*/
	int direction;			/*Sets the facing direction using global enum*/
	Item inventory[5];		/*Inventory*/
	float cooldownTime;		/*CooldownTime for guards*/
	float fdirection;		/*Float direction for deltatime*/
	COORD detectedLoc;		/*Detected Location*/
	COORD pathList[10];		/*Guard PathList list*/
	int pathIndex;			/*The current step of pathlist*/
	eGuardState eCurrState;
	eGuardState ePrevState;
	eGuardEvent eNewEvent;
}Guard;


/*Declare all the guard variables*/
struct GUARD guard1;
struct GUARD guard2;
struct GUARD guard3;

/*Array to keep track of active guards in the level*/
struct GUARD activeGuardList[10];
int activeGuardCount;

/*Some Global guard variables*/
int guardRange;
int guardRange2;
int guardMoveSpeed_patrol;
int guardMoveSpeed_chase;
int guardMoveSpeed_invest;
int guardPathLimit;
float guardDetectDelay;
float guardLookArdPatrol;
float guardLookArdChase;
float guardRepathDetect;
float guardShootingReload;
float guardShootingStationary;


/*Spawn the guard, Initalise pos, 6 waypoints pos, start & dest pts, push them to active tracking array*/
void Spawn_Guard1(short x, short y, short x0, short y0, short x1, short y1, short x2, short y2,
	short x3, short y3, short x4, short y4, short x5, short y5, short totalWp);

void Spawn_Guard2(short x, short y, short x0, short y0, short x1, short y1, short x2, short y2,
	short x3, short y3, short x4, short y4, short x5, short y5, short totalWp);

void Spawn_Guard3(short x, short y, short x0, short y0, short x1, short y1, short x2, short y2,
	short x3, short y3, short x4, short y4, short x5, short y5, short totalWp);

void ClearAllGuard();

COORD GetFacingLocationGuard(struct GUARD* human, int distance);

/*Guard pathfinding movement*/
int Guard_Movement(struct GUARD* guard, int endX, int endY);

void Guard_PathfindingMove(struct GUARD* guard);

int CheckCollisionWithPlayer(struct GUARD* guard, struct HUMAN* playerChar);

void ResetGuardPath(struct GUARD* guard);


/*Main update for guard, checking statemachine called from each game frame*/
void Guard_Loop_Start(struct GUARD* guard);

/*Detection logic*/
int DetectPlayer(struct GUARD* guard);

int DetectPlayer2(struct GUARD* guard);

int DetectRock(struct GUARD* guard);

/*One time action to shoot a bullet at player*/
void Guard_Shooting(struct GUARD* guard);

void RenderGuardFOV(struct GUARD* guard, COORD localMapOffset);

void RenderGuardFOV2(struct GUARD* guard, COORD localMapOffset);

void PrisonAlarmEvent(int proximity, int numOfGuard);

#endif // !GUARD_H