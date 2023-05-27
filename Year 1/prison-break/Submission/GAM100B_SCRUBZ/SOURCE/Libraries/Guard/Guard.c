/*********************************************************************************
Guard.c
Author: Xie Hao Zhe (Statemachine, look around, detection, Astar pathfinding)
Code Lines: 597
Author: Chong Jun Yi (Movement, collision, FOV graphics ,Additional states such as investigate, shooting)
Code Lines: 282
Brief: Guard functionality in the game.
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#include <windows.h>			/*COORD*/
#include <math.h>
#include "Guard.h"
#include "../Global/Global.h"
#include "../States/Game.h"
#include "../Map/Map.h"
#include "../Console/Console.h"						/*Rendering FOV*/
#include "Pathfind.h"
#include "../Clock/Clock.h"							/*DeltaTime*/
#include "../Human/Human.h"
#include "../DifficultySystem/DifficultySystem.h"	/*Modify guard variables*/
#include "../Sounds/GameSounds.h"


/*********************************************************************************
Variables and struct for guards
Author: Chong Jun Yi (38) and Xie Hao Zhe (39) 
Code Lines: 77
Brief: Struct and variables that controls the guard AI
**********************************************************************************/
/*Tom is a basic patrol guard*/
struct GUARD Guard1 =
{
	"GuardTom",			/*Name of Guard*/
	1,				    /*Graphic Index*/
	{0},		    	/*Positon on map*/
	{0},			    /*Float position*/
	{-1,-1},		    /*6 Waypoints*/
	{-1,-1},		    /*Start Point*/
	{-1,-1},		    /*Destination Waypoint*/
	0,				    /*Curr Waypoint AKA starting*/
	0,				    /*Total number of waypoints*/
	2,				    /*Movespeed*/
	DIR_RIGHT,		    /*Facing direction*/
	{0},			    /*Inventory*/
	0.0f,			    /*CooldownTime*/
	0.0f,               /*Float of direction*/
	{-1,-1},		    /*Detected Location*/
	{0,0},		        /*Guard PathList list*/
	0,				    /*The current step of pathlist*/
	Patrol_Idle_State,  /*Current state*/
	Patrol_Idle_State,  /*Previous state*/
	Guard_No_Event		/*Event Changes*/
};

/*Ricky has a AOE detection */
struct GUARD Guard2 =
{
	"GuardRicky",		/*Name of Guard*/
	157,				    /*Graphic Index*/		
	{0},		    	/*Positon on map*/
	{0},			    /*Float position*/
	{-1,-1},		    /*6 Waypoints*/
	{-1,-1},		    /*Start Point*/
	{-1,-1},		    /*Destination Waypoint*/
	0,				    /*Curr Waypoint AKA starting*/
	0,				    /*Total number of waypoints*/
	2,				    /*Movespeed*/
	DIR_RIGHT,		    /*Facing direction*/
	{0},			    /*Inventory*/
	0.0f,			    /*CooldownTime*/
	0.0f,               /*Float of direction*/
	{-1,-1},		    /*Detected Location*/
	{0,0},		        /*Guard PathList list*/
	0,				    /*The current step of pathlist*/
	Patrol_Idle_State,  /*Current state*/
	Patrol_Idle_State,  /*Previous state*/
	Guard_No_Event		/*Event Changes*/
};

/*Henry has a gun*/
struct GUARD Guard3 =
{
	"GuardHenry",		/*Name of Guard*/
	234,				    /*Graphic Index*/
	{0},		    	/*Positon on map*/
	{0},			    /*Float position*/
	{-1,-1},		    /*6 Waypoints*/
	{-1,-1},		    /*Start Point*/
	{-1,-1},		    /*Destination Waypoint*/
	0,				    /*Curr Waypoint AKA starting*/
	0,				    /*Total number of waypoints*/
	2,				    /*Movespeed*/
	DIR_RIGHT,		    /*Facing direction*/
	{0},			    /*Inventory*/
	0.0f,			    /*CooldownTime*/
	0.0f,               /*Float of direction*/
	{-1,-1},		    /*Detected Location*/
	{0,0},		        /*Guard PathList list*/
	0,				    /*The current step of pathlist*/
	Patrol_Idle_State,  /*Current state*/
	Patrol_Idle_State,  /*Previous state*/
	Guard_No_Event		/*Event Changes*/
};

/*Array to keep track of active guards in the level*/
struct GUARD activeGuardList[10] = { 0 };
int activeGuardCount = 0;

/*Global guard variables*/
int guardRange = 10; /*3*/
int guardRange2 = 7; /*3*/
int guardMoveSpeed_patrol = 2;
int guardMoveSpeed_chase = 10;
int guardMoveSpeed_invest = 6;
int guardPathLimit = 10;
float guardDetectDelay = 0.4f;
float guardLookArdPatrol = 4.0f;
float guardLookArdChase = 2.0f;
float guardRepathDetect = 1.0f;
float guardShootingReload = 1.0f;
float guardShootingStationary = 0.4f;

/*********************************************************************************
/								Initalisation Functions
/*********************************************************************************
/*********************************************************************************
Spawn_Guard1(),  
Author: Xie Hao Zhe
Code Lines: 19
Brief: Spawn the guards, Initalise pos, 6 waypoints pos, start & dest pts, push them to active tracking array
Spawns the normal melee guards
**********************************************************************************/
void Spawn_Guard1(short x, short y, short x0, short y0, short x1, short y1, short x2, short y2,
	short x3, short y3, short x4, short y4, short x5, short y5, short totalWp)
{
	Guard1.pos.X = x;
	Guard1.pos.Y = y;
	Guard1.fpos.X = (float)x;
	Guard1.fpos.Y = (float)y;

	Guard1.wayPts[0].X = x0;	Guard1.wayPts[0].Y = y0;
	Guard1.wayPts[1].X = x1;	Guard1.wayPts[1].Y = y1;
	Guard1.wayPts[2].X = x2;	Guard1.wayPts[2].Y = y2;
	Guard1.wayPts[3].X = x3;	Guard1.wayPts[3].Y = y3;
	Guard1.wayPts[4].X = x4;	Guard1.wayPts[4].Y = y4;
	Guard1.wayPts[5].X = x5;	Guard1.wayPts[5].Y = y5;
	Guard1.totalWayPt = totalWp;
	Guard1.startPt.X = x;	Guard1.startPt.Y = y;
	Guard1.endPt.X = Guard1.wayPts[0].X;	Guard1.endPt.Y = Guard1.wayPts[0].Y;
	Guard1.currWayPt = 0;
	activeGuardList[activeGuardCount] = Guard1;
	++activeGuardCount;
}

/*********************************************************************************
Spawn_Guard2(),
Author: Xie Hao Zhe
Code Lines: 18
Brief: Spawn the guards, Initalise pos, 6 waypoints pos, start & dest pts, push them to active tracking array
Spawns the AOE sight guard
**********************************************************************************/
void Spawn_Guard2(short x, short y, short x0, short y0, short x1, short y1, short x2, short y2,
	short x3, short y3, short x4, short y4, short x5, short y5, short totalWp)
{
	Guard2.pos.X = x;
	Guard2.pos.Y = y;
	Guard2.fpos.X = (float)x;
	Guard2.fpos.Y = (float)y;

	Guard2.wayPts[0].X = x0;	Guard2.wayPts[0].Y = y0;
	Guard2.wayPts[1].X = x1;	Guard2.wayPts[1].Y = y1;
	Guard2.wayPts[2].X = x2;	Guard2.wayPts[2].Y = y2;
	Guard2.wayPts[3].X = x3;	Guard2.wayPts[3].Y = y3;
	Guard2.wayPts[4].X = x4;	Guard2.wayPts[4].Y = y4;
	Guard2.wayPts[5].X = x5;	Guard2.wayPts[5].Y = y5;
	Guard2.totalWayPt = totalWp;
	Guard2.startPt.X = x;	Guard2.startPt.Y = y;
	Guard2.endPt.X = Guard2.wayPts[0].X;	Guard2.endPt.Y = Guard2.wayPts[0].Y;
	Guard2.currWayPt = 0;
	activeGuardList[activeGuardCount] = Guard2;
	++activeGuardCount;
}

/*********************************************************************************
Spawn_Guard3(),
Author: Xie Hao Zhe
Code Lines: 18
Brief: Spawn the guards, Initalise pos, 6 waypoints pos, start & dest pts, push them to active tracking array
Spawns the shooting guard
**********************************************************************************/
void Spawn_Guard3(short x, short y, short x0, short y0, short x1, short y1, short x2, short y2,
	short x3, short y3, short x4, short y4, short x5, short y5, short totalWp)
{
	Guard3.pos.X = x;
	Guard3.pos.Y = y;
	Guard3.fpos.X = (float)x;
	Guard3.fpos.Y = (float)y;

	Guard3.wayPts[0].X = x0;	Guard3.wayPts[0].Y = y0;
	Guard3.wayPts[1].X = x1;	Guard3.wayPts[1].Y = y1;
	Guard3.wayPts[2].X = x2;	Guard3.wayPts[2].Y = y2;
	Guard3.wayPts[3].X = x3;	Guard3.wayPts[3].Y = y3;
	Guard3.wayPts[4].X = x4;	Guard3.wayPts[4].Y = y4;
	Guard3.wayPts[5].X = x5;	Guard3.wayPts[5].Y = y5;
	Guard3.totalWayPt = totalWp;
	Guard3.startPt.X = x;	Guard3.startPt.Y = y;
	Guard3.endPt.X = Guard3.wayPts[0].X;	Guard3.endPt.Y = Guard3.wayPts[0].Y;
	Guard3.currWayPt = 0;
	activeGuardList[activeGuardCount] = Guard3;
	++activeGuardCount;
}

/*********************************************************************************
ClearAllGuard()
Author: Xie Hao Zhe
Code Lines: 4
Brief: Clear all guards when moving to different levels
**********************************************************************************/
void ClearAllGuard() 
{
	for (int i = activeGuardCount - 1; i >= 0; --i) 
	{
		activeGuardList[i] = (struct GUARD){ 0 };
	}
	activeGuardCount = 0;
}

/*********************************************************************************
ClearAllGuard()
Author: Chong Jun Yi
Code Lines: 17
Brief:Get a facing coordinate a variable distance away from guard
**********************************************************************************/
COORD GetFacingLocationGuard(struct GUARD* human, int distance)
{
	COORD facingCoord;

	facingCoord = human->pos;
	switch (human->direction)
	{
	case DIR_RIGHT:
		facingCoord.X += (short)distance;
		break;
	case DIR_UP:
		facingCoord.Y -= (short)distance;
		break;
	case DIR_LEFT:
		facingCoord.X -= (short)distance;
		break;
	case DIR_DOWN:
		facingCoord.Y += (short)distance;
		break;
	}
	return facingCoord;
}

/*********************************************************************************
/							Movement/Patrol/Pathfinding functions
/*********************************************************************************
/*********************************************************************************
Guard_Movement()
Author: Xie Hao Zhe 
Code Lines: 77
Brief: Guard movement without pathfinding, Handles state management as well 
**********************************************************************************/
int Guard_Movement(struct GUARD* guard, int endX, int endY)
{
	/*Get Manhatten Distance vector*/
	int pathDeltaX = guard->pos.X - endX;
	int pathDeltaY = guard->pos.Y - endY;

	if (guard == NULL || !bGameInitalised || Clock_GetDeltaTime() > 3000.0f)
	{
		return 0;
	}
	
	/*Determine what direction to move based on distance vector to DESTs*/
	/*No diagonal movments*/
	if (pathDeltaY > 0)
	{
		guard->direction = DIR_UP;
		guard->fpos.Y -= (guard->moveSpeed) * 0.001f * Clock_GetDeltaTime();
		guard->pos.Y = (short)roundf(guard->fpos.Y);
	}
	else if (pathDeltaY < 0)
	{
		guard->direction = DIR_DOWN;
		guard->fpos.Y += (guard->moveSpeed) * 0.001f * Clock_GetDeltaTime();
		guard->pos.Y = (short)roundf(guard->fpos.Y);
	}
	else if (pathDeltaX > 0)
	{
		guard->direction = DIR_LEFT;
		guard->fpos.X -= (guard->moveSpeed) * 0.001f * Clock_GetDeltaTime();
		guard->pos.X = (short)roundf(guard->fpos.X);
	}
	else if (pathDeltaX < 0)
	{
		guard->direction = DIR_RIGHT;
		guard->fpos.X += (guard->moveSpeed) * 0.001f * Clock_GetDeltaTime();
		guard->pos.X = (short)roundf(guard->fpos.X);
	}

	/*Check if guard has arrived at DEST waypoint and perform actions*/
	if (guard->pos.X == endX && guard->pos.Y == endY)  
	{
		/*NORMAL PATROL*/
		if (guard->eCurrState == Patrol_Idle_State) {
			guard->detectedLoc.X = -1;
			guard->detectedLoc.Y = -1;
				
			/******************		WAY POINT MANAGEMENT	******************/
			/*Reached the 5th waypoint, loop back to first waypoint */
			if (guard->currWayPt == guard->totalWayPt - 1){
				guard->startPt = guard->wayPts[guard->totalWayPt - 1];
				guard->endPt = guard->wayPts[0];
			}
			else{
				/*Reset the currWayPt to 0 after returning to first waypoint */
				if (guard->currWayPt == guard->totalWayPt)
				{
					guard->currWayPt = 0;
				}
				/*Move on to next set of waypoints*/
				guard->startPt = guard->wayPts[guard->currWayPt];
				guard->endPt = guard->wayPts[guard->currWayPt + 1];
			}
			++guard->currWayPt;
		}
		/*CHASING PLAYER*/
		else if (guard->eCurrState == Chase_Alert_State) {
			if (guard->pathIndex < guardPathLimit - 1 &&		/*Check if path has ended if next is {0,0}*/
				guard->pathList[guard->pathIndex + 1].X != 0 && guard->pathList[guard->pathIndex + 1].Y != 0)
			{
				guard->pathIndex += 1;		/*Continue the chase*/
			}
			else {
				//Reach end of path -- 
				guard->eNewEvent = Guard_MidChase_Event;
				ResetGuardPath(guard);
			}
		}
		/*RETURNING TO PATROL AFTER LOST PLAYER , means arrived at patrol way point*/
		else if (guard->eCurrState == Chase_ReturnPatrol_State) {
			/*Check if guard has arrived at DEST waypoint and perform actions*/
			/*Will waypoint clash with pathfinding??*/
			if (guard->pos.X == guard->detectedLoc.X && guard->pos.Y == guard->detectedLoc.Y)
			{
				guard->detectedLoc.X = 0;
				guard->detectedLoc.Y = 0;
				/*Go back to patrol mode*/
				guard->eNewEvent = Guard_Reset_Event;		
				/******************		WAY POINT MANAGEMENT	******************/
				/*Reached the 5th waypoint, loop back to first waypoint */
				if (guard->currWayPt == guard->totalWayPt - 1)
				{
					guard->startPt = guard->wayPts[guard->totalWayPt - 1];
					guard->endPt = guard->wayPts[0];
				}
				else
				{
					/*Reset the currWayPt to 0 after returning to first waypoint */
					if (guard->currWayPt == guard->totalWayPt)
					{
						guard->currWayPt = 0;
					}
					/*Move on to next set of waypoints*/
					guard->startPt = guard->wayPts[guard->currWayPt];
					guard->endPt = guard->wayPts[guard->currWayPt + 1];
				}
				++guard->currWayPt;
			}
			/*Check if path has ended if next is {0,0}*/
			else if (guard->pathIndex< guardPathLimit - 1 &&		
					 guard->pathList[guard->pathIndex + 1].X != 0 && guard->pathList[guard->pathIndex + 1].Y != 0)
			{
				guard->pathIndex += 1;
			}
			else 
			{
				/*Reach end of path*/
				guard->eNewEvent = Guard_MidChase_Event;
				ResetGuardPath(guard);
			}
		}
		else if (guard->eCurrState == Investigating_State) {
			/*Reached investigative spot without spotting player*/
			if (guard->pos.X == guard->detectedLoc.X && guard->pos.Y == guard->detectedLoc.Y)
			{
				guard->detectedLoc.X = 0;
				guard->detectedLoc.Y = 0;
				ResetGuardPath(guard);
				guard->eNewEvent = Guard_InvestigateEnd_Event;
				CreateNotification(8, 0, -1);

			}
			/*Check if path has ended if next is {0,0}*/
			else if (guard->pathIndex < guardPathLimit - 1 && 
				guard->pathList[guard->pathIndex + 1].X != 0 && guard->pathList[guard->pathIndex + 1].Y != 0)
			{
				/*Continue investigating*/
				guard->pathIndex += 1;	
			}
			else
			{
				/*Reach end of path, recalcuate the route to investigation*/
				ResetGuardPath(guard);
				/*Pathfind algorithm here to pathfind again*/
				StartPathfinding(guard, currLevel); 
				guard->pathIndex = 0;
			}
		}
	}

	/*Just for clarity since above if statement is too long*/
	/*Function will return value based on whether guards have reached destination*/
	if (guard->pos.X == endX && guard->pos.Y == endY) 
	{
		/*Destination Reached*/
		return 1;	
	}
	/*Still on route*/
	return 0;		
}

/*********************************************************************************
Guard_PathfindingMove()
Author: Xie Hao Zhe 
Code Lines: 7
Brief: Guard movement with pathfinding nodes stored in a array 
**********************************************************************************/
void Guard_PathfindingMove(struct GUARD* guard) 
{
	/*Check if pathfinding has ended or does not exist*/
	if ((guard->pathList[guard->pathIndex].X == 0 && guard->pathList[guard->pathIndex].Y == 0) || guard->pathIndex >= guardPathLimit) {
		ResetGuardPath(guard);
		guard->eNewEvent = Guard_MidChase_Event;
		if (guard->eCurrState == Chase_ReturnPatrol_State) { /*The guard is already standing on the way point*/
			guard->eNewEvent = Guard_Reset_Event;
		}
		return;
	}
	Guard_Movement(guard, guard->pathList[guard->pathIndex].X, guard->pathList[guard->pathIndex].Y);
}

/*********************************************************************************
Guard_PathfindingMove()
Author: Xie Hao Zhe
Code Lines: 4
Brief: Guard movement with pathfinding nodes stored in a array
**********************************************************************************/
int CheckCollisionWithPlayer(struct GUARD* guard, struct HUMAN* playerChar) 
{
	if (guard->pos.X == playerChar->pos.X && guard->pos.Y == playerChar->pos.Y) 
	{
		/*Player collision*/
		return 1;
	}
	/*No collision*/
	return 0; 
}

/*********************************************************************************
ResetGuardPath()
Author: Chong Jun Yi
Code Lines: 6
Brief: Clear pathfinding list
**********************************************************************************/
void ResetGuardPath(struct GUARD* guard) {
	/*Reset the path index*/
	int i = 0;
	for (i = 0; i < guardPathLimit; ++i) {
		guard->pathList[i].X = 0;
		guard->pathList[i].Y = 0;
	}
	guard->pathIndex = 0;
}

/*********************************************************************************
/								State Machine Functions
/*********************************************************************************
/*********************************************************************************
Guard_Loop_Start()
Author: Xie Hao Zhe (All other states)
Code Lines: 174 (39 + 21 + 7 + 18 + 24 + 23 + 14 + 28 )
Author: Chong Jun Yi (Investigation states)
Code Lines 34
Brief: Main update for guards, do certain action based on state
**********************************************************************************/
/*Main update for guard, checking statemachine called from each game frame*/
void Guard_Loop_Start(struct GUARD* guard)
{	
	int detectRockIndex; /*Will be always one more than actual Index*/

	/*Move to a new state based on event*/
	if (guard->eNewEvent != Guard_No_Event) 
	{
		guard->ePrevState = guard->eCurrState;
		switch (guard->eNewEvent)
		{
		case Guard_Detected_Event:
			guard->eCurrState = Patrol_Alert_State; /*Calculate a pathfind*/
			break;
		case Guard_DetectedDelay_Event:
			guard->eCurrState = Chase_Delay_State; /*Some delay before start chasing*/
			break;
		case Guard_StartChase_Event:
			guard->eCurrState = Chase_Alert_State;
			break;
		case Guard_MidChase_Event:
			guard->eCurrState = Chase_Idle_State;
			break;
		case Guard_ReturnPatrol_Event:
			guard->eCurrState = Chase_ReturnPatrol_State;
			break;
		case Guard_Reset_Event:
			guard->eCurrState = Patrol_Idle_State;
			break;
		case Guard_StaticLookArd_Event:
			guard->eCurrState = Look_Around_State;
			break;
		case Guard_ChaseLookArd_Event:
			guard->eCurrState = Chase_LookArd_State;
			break;
		case Guard_Investigate_Event:
			guard->eCurrState = Investigate_Alert_State;
			break;
		case Guard_InvestigateComfirm_Event:
			guard->eCurrState = Investigating_State;
			break;
		case Guard_InvestigateEnd_Event:
			guard->eCurrState = Investigating_ReturnPatrol_State;
			break;
		}
		/*Reset event so next frame wont call any repeat events*/
		guard->eNewEvent = Guard_No_Event;
	}
	
	/*State Machine for Guard states*/
	switch (guard->eCurrState)
	{
	case Patrol_Idle_State:
		guard->moveSpeed = guardMoveSpeed_patrol;
		/*Handles patrol movements, transition to look around once at waypoint*/
		if (Guard_Movement(guard, guard->endPt.X, guard->endPt.Y))
		{
			if ((strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0)) {
				guard->eNewEvent = Guard_StaticLookArd_Event; /*Look around before moving to next waypoint*/
			}
			else if (strcmp(guard->name, "GuardRicky") == 0){
				guard->eNewEvent = Guard_Reset_Event; /*Just move to next waypoint without pause*/
			}
		}
		/*If the player is detected by the guard*/
		/*Different detection mechanism for different guards*/
		if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
			{
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;	
				CreateNotification(6, 0, -1);
			}
			else if (DetectPlayer2(guard) && strcmp(guard->name, "GuardRicky") == 0)
			{
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(6, 0, -1);
			}
			else if ((detectRockIndex = DetectRock(guard)) != -1) {
				guard->detectedLoc = activeSoundList[detectRockIndex].pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(9, 0, -1);
			}
			break;

		case Patrol_Alert_State:
			StartPathfinding(guard, currLevel); /*Pathfind algorithm here*/
			/*If already seen the player, dont give chance*/
			if (guard->ePrevState != Chase_Idle_State && guard->ePrevState != Chase_LookArd_State) { 
				guard->cooldownTime = guardDetectDelay;
				/*Play detected sound here*/
				FmodPlayGameSound(4);
			}
			guard->eNewEvent = Guard_DetectedDelay_Event;
			break;

		case Chase_Delay_State:
			guard->cooldownTime -= Clock_GetDeltaTime() * 0.001f;	/*give the guard some delay before chasing*/
			if (guard->cooldownTime <=0.001f) {
				guard->cooldownTime = 0;
				if (guard->pathList[0].X != 0 && guard->pathList[0].Y != 0) {
					guard->pathIndex = 0;
					guard->cooldownTime = guardRepathDetect;/*Re Pathfind timer*/
					if (strcmp(guard->name, "GuardHenry") == 0) {
						Guard_Shooting(guard);/*Trigger Happy*/
						guard->cooldownTime = guardShootingReload;
					}
					guard->eNewEvent = Guard_StartChase_Event;
				}
				else {
					/*No possible path to player*/
					guard->detectedLoc = guard->wayPts[guard->currWayPt];
					StartPathfinding(guard, currLevel);
					guard->pathIndex = 0;
					guard->eNewEvent = Guard_ReturnPatrol_Event;
					CreateNotification(8, 0, -1);
				}
			}
			break;
		case Chase_Alert_State:
			/*Set to max movespeed if detected*/
			guard->moveSpeed = guardMoveSpeed_chase;

			/*Handles shooting guard*/
			if (strcmp(guard->name, "GuardHenry") == 0) {
				guard->cooldownTime -= Clock_GetDeltaTime() * 0.001f;
				if (guard->cooldownTime <= 0.0f) {
					Guard_Shooting(guard);
					guard->cooldownTime = guardShootingReload;	
				}
				else if (guard->cooldownTime >= guardShootingReload - guardShootingStationary) {
					return; /*Do not move this frame*/
				}

			} /*Unfortunately shooting guard cannot repathfind*/
			/*Handle re-pathfind logics if player is still within detection window*/
			else if (guard->cooldownTime > 0.0f) {
				guard->cooldownTime -= Clock_GetDeltaTime() * 0.001f;
				if (guard->cooldownTime <= 0.0f) {
					if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
					{
						guard->detectedLoc = PlayerChar.pos;
						StartPathfinding(guard, currLevel); /*Re - Pathfind algorithm here*/
						guard->pathIndex = 0;
					}
					else if (DetectPlayer2(guard) && strcmp(guard->name, "GuardRicky") == 0)
					{
						guard->detectedLoc = PlayerChar.pos;
						StartPathfinding(guard, currLevel); /*Re - Pathfind algorithm here*/
						guard->pathIndex = 0;
					}
					guard->cooldownTime = guardRepathDetect;
				}
			}

			/*Handles movements - similar but under a pathfinding movement category*/
			Guard_PathfindingMove(guard);
			break;

		/*Either end of pathfinding (and need to recalculate) or lost player*/
		case Chase_Idle_State:
			guard->cooldownTime = 0.0f;
			if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
			{
				guard->detectedLoc = PlayerChar.pos;			/*If detected player again*/
				guard->eNewEvent = Guard_Detected_Event;
			}
			else if (DetectPlayer2(guard) && strcmp(guard->name, "GuardRicky") == 0)
			{
				guard->detectedLoc = PlayerChar.pos;			/*If detected player again*/
				guard->eNewEvent = Guard_Detected_Event;
			}
			else if ((detectRockIndex = DetectRock(guard)) != -1) {
				guard->detectedLoc = activeSoundList[detectRockIndex].pos;
				guard->eNewEvent = Guard_Detected_Event;		/*If detected rock again*/
				CreateNotification(9, 0, -1);
			}
			else if(guard->ePrevState == Chase_Alert_State)
			{
				/*Attempt to look for player by looking around*/
				guard->eNewEvent =  Guard_ChaseLookArd_Event;
				CreateNotification(7, 0, -1); 
			}
			else {
				if (guard->currWayPt == guard->totalWayPt) {	/*No new detection, go back to waypoint patrol*/
					guard->currWayPt = 0;
				}
				guard->detectedLoc = guard->wayPts[guard->currWayPt];	
				StartPathfinding(guard, currLevel);
				guard->pathIndex = 0;
				guard->eNewEvent = Guard_ReturnPatrol_Event;
			}
			break;
		case Chase_ReturnPatrol_State:
			guard->moveSpeed = guardMoveSpeed_patrol;
			Guard_PathfindingMove(guard);

			/*While Move back to patrol waypoint, and spot something new*/
			if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
			{
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
			}
			else if (DetectPlayer2(guard) && strcmp(guard->name, "GuardRicky") == 0)
			{
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
			}
			else if ((detectRockIndex = DetectRock(guard)) != -1) {
				guard->detectedLoc = activeSoundList[detectRockIndex].pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(9, 0, -1);
			}
			break;

		case Look_Around_State:
			/*Timer update to know when to change state*/
			guard->fdirection += Clock_GetDeltaTime() * 0.001f;
			guard->cooldownTime += Clock_GetDeltaTime() * 0.001f;

			/*Rotate around a static point to look in different direction*/
			if (guard->fdirection > guardLookArdPatrol/4.0f) {
				guard->direction += 1;	/*Counter clockwise rotation*/
				if (guard->direction >3) {
					guard->direction = 0;
				}
				guard->fdirection = 0.0f;
			}
			/*If seen player while looking around */
			if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
			{
				guard->fdirection = 0;
				guard->cooldownTime = 0;
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(6, 0, -1);
			}
			else if (DetectPlayer2(guard) && strcmp(guard->name, "GuardRicky") == 0)
			{
				guard->fdirection = 0;
				guard->cooldownTime = 0;
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(6, 0, -1);
			}
			else if ((detectRockIndex = DetectRock(guard)) != -1) {
				guard->detectedLoc = activeSoundList[detectRockIndex].pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(9, 0, -1);
			}

			/*Have look around in all directions, time to go*/
			if (guard->cooldownTime > guardLookArdPatrol) {
				guard->fdirection = 0;
				guard->cooldownTime = 0;
				/*Go back to movement patrol*/
				guard->eNewEvent = Guard_Reset_Event;
			}
			break;
		/*Try to find player after lossing him in chase*/
		case Chase_LookArd_State:
			guard->fdirection += Clock_GetDeltaTime() * 0.001f;
			guard->cooldownTime += Clock_GetDeltaTime() * 0.001f;
			/*Rotate around a static point to look in different direction*/
			if (guard->fdirection > guardLookArdChase / 4.0f) {
				guard->direction += 1;	/*Counter clockwise rotation*/
				if (guard->direction > 3) {
					guard->direction = 0;
				}
				guard->fdirection = 0.0f;
			}
			if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
			{
				guard->fdirection = 0;
				guard->cooldownTime = 0;
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(6, 0, -1);
			}
			else if (DetectPlayer2(guard) && strcmp(guard->name, "GuardRicky") == 0)
			{
				guard->fdirection = 0;
				guard->cooldownTime = 0;
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(6, 0, -1);
			}

			/*Looked around in all directions, time to go*/
			if (guard->cooldownTime > guardLookArdChase) {
				guard->fdirection = 0;
				guard->cooldownTime = 0;

				/*Player cannot be seen, go back to waypoint*/
				if (guard->currWayPt == guard->totalWayPt) {
					guard->currWayPt = 0;
				}
				guard->detectedLoc = guard->wayPts[guard->currWayPt];
				StartPathfinding(guard, currLevel);
				guard->pathIndex = 0;
				guard->eNewEvent = Guard_ReturnPatrol_Event;
				CreateNotification(8, 0, -1);
			}
			break;
		/*Investigate a laser alarm or prison alarm*/
		case Investigate_Alert_State:
			/*Track down player exact location*/
			guard->detectedLoc = PlayerChar.pos;
			StartPathfinding(guard, currLevel); /*Pathfind algorithm here*/

			/*Check whether there is a path to player, if not return to previous state*/
			if (guard->pathList[0].X != 0 && guard->pathList[0].Y != 0) 
			{
				guard->pathIndex = 0;
				guard->eNewEvent = Guard_InvestigateComfirm_Event;
				CreateNotification(10, 0, -1);
			}
			else 
			{
				/*No possible path to player*/
				guard->detectedLoc = guard->wayPts[guard->currWayPt];
				StartPathfinding(guard, currLevel);
				guard->pathIndex = 0;
				guard->eNewEvent = Guard_ReturnPatrol_Event;
			}
			break;
		/*Handles movement while investigating*/
		case Investigating_State:
			guard->moveSpeed = guardMoveSpeed_invest;

			if (DetectPlayer(guard) && (strcmp(guard->name, "GuardTom") == 0 || strcmp(guard->name, "GuardHenry") == 0))
			{
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
			}
			else if (DetectPlayer(guard) && strcmp(guard->name, "GuardRicky") == 0)
			{
				guard->detectedLoc = PlayerChar.pos;
				guard->eNewEvent = Guard_Detected_Event;
			}
			else if ((detectRockIndex = DetectRock(guard)) != -1) {
				guard->detectedLoc = activeSoundList[detectRockIndex].pos;
				guard->eNewEvent = Guard_Detected_Event;
				CreateNotification(9, 0, -1);
			}
			/*Handles movements - similar but under a pathfinding movement category*/
			Guard_PathfindingMove(guard);
			break;
		/*Return to patrol after investigating*/
		case Investigating_ReturnPatrol_State:
			guard->detectedLoc = guard->wayPts[guard->currWayPt];
			StartPathfinding(guard, currLevel);
			guard->pathIndex = 0;
			guard->eNewEvent = Guard_ReturnPatrol_Event;
			break;
		default:
			/*Clearly shouldnt be coming here*/
			guard->eNewEvent = Guard_MidChase_Event;
			break;
	}
	/*End of State Machine*/
}

/*********************************************************************************
/								Detection Functions
/*********************************************************************************
/*********************************************************************************
DetectRock()
Author: Xie Hao Zhe
Code Lines: 122
Brief: Detection Logic for guards when seen a distractino rock
**********************************************************************************/
int DetectRock(struct GUARD* guard)
{
	/*Check vector distance between them*/
	int deltaX;
	int deltaY; 
	int i, j, s; /*for loop index*/
	int bBreakToNext = 0;
	int spread = 0;

	for (s = 0; s < activeSoundCount; ++s ) {
		bBreakToNext = 0;
		spread = activeSoundList[s].numOfUsage - (int)activeSoundList[s].fpos.Y;
		spread > 2 ? spread = 2: spread;
		deltaX = activeSoundList[s].pos.X - guard->pos.X;
		deltaY = activeSoundList[s].pos.Y - guard->pos.Y;

		/*Compensate for sound area*/
		while (spread > 0) {
			if (deltaX < 0) { /*Sound is left*/
				deltaX += 1;
			}
			else if (deltaX > 0) { /*Sound is right*/
				deltaX -= 1;
			}
			if (deltaY < 0) { /*Sound is up*/
				deltaY += 1;
			}
			else if (deltaY > 0) { /*Sound is down*/
				deltaY -= 1;
			}
			--spread;
		}
		

		/*Returns 0 if the distance is more than the range*/
		if (deltaX >= guardRange || deltaX <= -guardRange || deltaY >= guardRange || deltaY <= -guardRange)
		{
			continue;
		}

		/***************************** Detection Chunk here*****************************************/
		/*Check guard direction*/
		switch (guard->direction)
		{
		case DIR_RIGHT:
			if (abs(deltaY) > 2) /*Limit the spread*/
			{
				break; /*Out of Spread Range*/
			}
			else
			{
				if (abs(deltaY) > abs(deltaX))
				{
					break; /*Out of Spread Range*/
				}
			}

			if (deltaX < 0)
			{
				break; /*Behind the guard facing*/
			}
			/*If the player is above the guard*/
			if (deltaY < 0)
			{
				for (i = 1; i < deltaX; ++i)
				{
					for (j = 0; j >= deltaY; --j)  /*DETECTING TOO WELL IN EDGE OF CONE*/
					{
						if (abs(i) <= abs(j)) {
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			/*If the player is below the guard*/
			else if (deltaY > 0)
			{
				for (i = 1; i < deltaX; ++i)
				{
					for (j = 0; j <= deltaY; ++j)
					{
						if (abs(i) <= abs(j)) {
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}

			return s;	/*Rock Detected*/
			break;

		case DIR_LEFT:
			if (abs(deltaY) > 2) /*Limit the spread*/
			{
				break; /*Out of Spread Range*/
			}
			else
			{
				if (abs(deltaY) > abs(deltaX))
				{
					break; /*Out of Spread Range*/
				}
			}

			if (deltaX > 0)
			{
				break;
			}
			if (deltaY <= 0)
			{
				for (i = -1; i > deltaX; --i)
				{
					for (j = 0; j >= deltaY; --j)
					{
						if (abs(i) <= abs(j)) 
						{
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			else if (deltaY > 0)
			{
				for (i = -1; i > deltaX; --i)
				{
					for (j = 0; j <= deltaY; ++j)
					{
						if (abs(i) <= abs(j)) {
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			return s;
			break;
		case DIR_UP:
			if (abs(deltaX) > 2) /*Limit the spread*/
			{
				break; /*out of spread range*/
			}
			else
			{
				if (abs(deltaX) > abs(deltaY))
				{
					break; /*Out of Spread Range*/
				}
			}

			if (deltaY > 0)
			{
				break;
			}
			if (deltaX <= 0)
			{
				for (j = 1; j < deltaY; ++j)
				{
					for (i = 0; i >= deltaX; --i)
					{
						if (abs(j) <= abs(i)) 
						{
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			else if (deltaX > 0)
			{
				for (j = 1; j < deltaY; ++j)
				{
					for (i = 0; i <= deltaX; ++i)
					{
						if (abs(j) <= abs(i)) {
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			return s;
			break;
		case DIR_DOWN:
			if (abs(deltaX) > 2) /*Limit the spread*/
			{
				break; /*Out of Spread Range*/
			}
			else
			{
				if (abs(deltaX) > abs(deltaY))
				{
					break; /*Out of Spread Range*/
				}
			}
			if (deltaY < 0)
			{
				break;
			}
			if (deltaX <= 0)
			{
				for (j = -1; j < deltaY; ++j)
				{
					for (i = -1; i >= deltaX; --i)
					{
						if (abs(j) <= abs(i))
						{
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			else if (deltaX > 0)
			{
				for (j = 0; j < deltaY; ++j)
				{
					for (i = 0; i <= deltaX; ++i)
					{
						if (abs(j) <= abs(i)) 
						{
							continue;
						}
						if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
						{
							break;
						}
					}
				}
			}
			return s;
			break;

		default:
			return -1; /*SHOULD NOT BE COMING HERE*/
			break;
		}
	}
	/*No detection of rocks*/
	return -1;
}

/*********************************************************************************
DetectPlayer()
Author: Xie Hao Zhe
Code Lines: 99
Brief: Detection Logic for guards to see player
**********************************************************************************/
int DetectPlayer(struct GUARD* guard)
{
	/*Check vector distance between them*/
	int deltaX = PlayerChar.pos.X - guard->pos.X;
	int deltaY = PlayerChar.pos.Y - guard->pos.Y;

	/*Returns 0 if the distance is more than the range*/
	if (deltaX > guardRange || deltaX < -guardRange || deltaY > guardRange || deltaY < -guardRange)
	{
		return 0;
	}
	/*Check guard direction*/
	switch (guard->direction)
	{
	case DIR_RIGHT:
		if (abs(deltaY) > 2) /*Limit the spread*/
		{
			return 0; /*Out of Spread Range*/
		}
		else
		{
			if (abs(deltaY) > abs(deltaX))
			{
				return 0; /*Out of Spread Range*/
			}
		}

		if (deltaX < 0)
		{
			return 0; /*Behind the guard facing*/
		}
		/*If the player is above the guard*/
		if (deltaY < 0)
		{
			for (int i = 1; i < deltaX; ++i)
			{
				for (int j = 0; j >= deltaY; --j)  /*DETECTING TOO WELL IN EDGE OF CONE*/
				{
					if (abs(i) < abs(j) && i != 0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		/*If the player is below the guard*/
		else if (deltaY > 0)
		{
			for (int i = 1; i < deltaX; ++i)
			{
				for (int j = 0; j <= deltaY; ++j)
				{
					if (abs(i) < abs(j) && i != 0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		return 1;	/*Player Detected*/
		break;

	case DIR_LEFT:
		if (abs(deltaY) > 2) /*Limit the spread*/
		{
			return 0; /*Out of Spread Range*/
		}
		else
		{
			if (abs(deltaY) > abs(deltaX))
			{
				return 0; /*Out of Spread Range*/
			}
		}

		if (deltaX > 0)
		{
			return 0;
		}
		if (deltaY <= 0)
		{
			for (int i = -1; i > deltaX; --i)
			{
				for (int j = 0; j >= deltaY; --j)
				{
					if (abs(i) < abs(j) && i != 0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		else if (deltaY > 0)
		{
			for (int i = -1; i > deltaX; --i)
			{
				for (int j = 0; j <= deltaY; ++j)
				{
					if (abs(i) < abs(j) && i != 0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		return 1;
		break;
		/*UP DOWN DIRECTION STILL NOT 100 % tested*/
	case DIR_UP:
		if (abs(deltaX) > 2) /*Limit the spread*/
		{
			return 0; /*Out of Spread Range*/
		}
		else
		{
			if (abs(deltaX) > abs(deltaY))
			{
				return 0; /*Out of Spread Range*/
			}
		}

		if (deltaY > 0)
		{
			return 0;
		}
		if (deltaX <= 0)
		{
			for (int j = 1; j < deltaY; ++j)
			{
				for (int i = 0; i >= deltaX; --i)
				{
					if (abs(j) < abs(i) && i !=0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		else if (deltaX > 0)
		{
			for (int j = 1; j < deltaY; ++j)
			{
				for (int i = 0; i <= deltaX; ++i)
				{
					if (abs(j) < abs(i) && i != 0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		return 1;
		break;
	case DIR_DOWN:
		if (abs(deltaX) > 2) /*Limit the spread*/
		{
			return 0; /*Out of Spread Range*/
		}
		else
		{
			if (abs(deltaX) > abs(deltaY))
			{
				return 0; /*Out of Spread Range*/
			}
		}
		if (deltaY < 0)
		{
			return 0;
		}
		if (deltaX <= 0)
		{
			for (int j = -1; j < deltaY; ++j)
			{
				for (int i = 0; i >= deltaX; --i)
				{
					if (abs(j) < abs(i) && i != 0) /*if <= then the corner will detect too well*/
					{
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		else if (deltaX > 0)
		{
			for (int j = 0; j < deltaY; ++j)
			{
				for (int i = 0; i <= deltaX; ++i)
				{
					if (abs(j) < abs(i) && i != 0) { /*if <= then the corner will detect too well*/
						continue;
					}
					if (Map_CheckCollision(guard->pos.X + i, guard->pos.Y + j, currLevel, 1))
					{
						return 0;
					}
				}
			}
		}
		return 1;
		break;

	default:
		return 0; /*SHOULD NOT BE COMING HERE*/
		break;
	}
}

/*********************************************************************************
DetectPlayer2()
Author: Chong Jun Yi
Code Lines: 12
Brief: AOE Detection Logic for guards to see player
**********************************************************************************/
int DetectPlayer2(struct GUARD* guard)
{
	int h = PlayerChar.pos.Y - guard->pos.Y;
	int w = PlayerChar.pos.X - guard->pos.X;
	int yScale = 1, xScale = 1;
	int y = 0, x = 0;

	if (h * h + w * w <= (guardRange2 / 2) * (guardRange2 / 2)) { /*Within radius*/
		/*No obstacles inbetween*/
		xScale = w < 0 ? -1 : 1;
		yScale = h < 0 ? -1 : 1;
		for (y = 0; y <= abs(h); ++y) {
			for (x = 0; x <= abs(w); ++x) {
				if (Map_CheckCollision(guard->pos.X + (x * xScale), guard->pos.Y + (y * yScale), currLevel, 1)) {
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}
/*********************************************************************************
/								Guard Shooting  Functions
/********************************************************************************/

/*********************************************************************************
Guard_Shooting()
Author: Chong Jun Yi
Code Lines: 3
Brief: One time action to shoot at player 
**********************************************************************************/
void Guard_Shooting(struct GUARD*guard)
{
	COORD gunLoc; /*Dont spawn bullet on the guard himself */

	gunLoc = GetFacingLocationGuard(guard, 1);

	SpawnProjectile(&Item_Bullet, gunLoc.X, gunLoc.Y, guard->direction, 99, 12, 7); /*Bullet Speed 7*/
}

/*********************************************************************************
RenderGuardFOV()
Author: Chong Jun Yi
Code Lines: 128
Brief: Render the in -game user interface for where guard can see
**********************************************************************************/
void RenderGuardFOV(struct GUARD* guard, COORD localMap_Offset) {
	int r, c; /*For loop, r is vector parallel to range, while c is column index*/
	int col; /*Column*/
	int bBreakOut = 0;

	col = guardRange * 2;
	if (col > 5) {
		col = 5;
	}
		
	Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);
	switch (guard->direction)
	{
		case DIR_RIGHT:
			/*Center -> bottom*/		/*Parallel to facing direction*/
			for (c = 0; c >= -(col / 2); --c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X + 1 + r, guard->pos.Y + c, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X + 1 + r, guard->pos.Y + c, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X + 1 + r - localMap_Offset.X, guard->pos.Y - localMap_Offset.Y + c, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			/*-> top*/				/*Parallel to facing direction*/
			for (c = 1; c <= (col / 2); ++c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X + 1 + r, guard->pos.Y + c, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X + 1 + r, guard->pos.Y + c, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X + 1 + r - localMap_Offset.X, guard->pos.Y - localMap_Offset.Y + c, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			break;
		case DIR_LEFT:
			/*Center -> bottom*/		/*Parallel to facing direction*/
			for (c = 0; c >= -(col / 2); --c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X - 1 - r, guard->pos.Y + c, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X - 1 - r, guard->pos.Y + c, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X - 1 - r - localMap_Offset.X, guard->pos.Y - localMap_Offset.Y + c, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			/*-> top*/				/*Parallel to facing direction*/
			for (c = 1; c <= (col / 2); ++c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X - 1 - r, guard->pos.Y + c, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X - 1 - r, guard->pos.Y + c, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X - 1 - r - localMap_Offset.X, guard->pos.Y - localMap_Offset.Y + c, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			break;
		case DIR_UP:
			/*Center -> bottom*/		/*Parallel to facing direction*/
			for (c = 0; c >= -(col / 2); --c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X + c, guard->pos.Y - 1 - r, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X + c, guard->pos.Y - 1 - r, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X - localMap_Offset.X + c, guard->pos.Y - 1 - r - localMap_Offset.Y, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			/*-> top*/				/*Parallel to facing direction*/
			for (c = 1; c <= (col / 2); ++c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X + c, guard->pos.Y - 1 - r, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X + c, guard->pos.Y - 1 - r, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X - localMap_Offset.X + c, guard->pos.Y - 1 - r - localMap_Offset.Y, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			break;
		case DIR_DOWN:
			/*Center -> bottom*/		/*Parallel to facing direction*/
			for (c = 0; c >= -(col / 2); --c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X + c, guard->pos.Y + 1 + r, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X + c, guard->pos.Y + 1 + r, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X - localMap_Offset.X + c, guard->pos.Y + 1 + r - localMap_Offset.Y, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			/*-> top*/				/*Parallel to facing direction*/
			for (c = 1; c <= (col / 2); ++c) {
				for (r = abs(c); r < guardRange; ++r) {
					/*Check for walls*/
					if (Map_CheckCollision(guard->pos.X + c, guard->pos.Y + 1 + r, currLevel, 1)) {
						if (c == 0 && r == 0) {
							return; /*Front is blocked*/
						}
						else if (c != 0 && r <= col / 2) {
							bBreakOut = 1; /*Sides is blocked*/
						}
						break; /*Go to next column if wall encountered*/
					}
					/*Check if within play area*/
					if (WithinPlayArea(guard->pos.X + c, guard->pos.Y + 1 + r, 1, -1)) {
						/*Render vision cone if no wall detected*/
						Console_SetRenderBuffer_Char(guard->pos.X - localMap_Offset.X + c, guard->pos.Y + 1 + r - localMap_Offset.Y, 250);
					}
				}
				if (bBreakOut) {
					bBreakOut = 0;
					break;
				}
			}
			break;
	}
}

/*********************************************************************************
RenderGuardFOV()
Author: Chong Jun Yi
Code Lines: 19
Brief: Render the in game user interface for where guard can see (AOE)
**********************************************************************************/
void RenderGuardFOV2(struct GUARD* guard, COORD localMap_Offset) {
	int w = 0, h = 0;	/*Bounding square*/
	int x = 0, y = 0; /*For loop for direction vector*/ /*Direction vector from origin to point*/
	int bBreakOut;
	int xScale = 1, yScale = 1;

	Console_SetRenderBuffer_Attribute(RED, BLACK, DEFAULTLVB);
	/*Check a small bounding square around guard*/
	for (h = -(guardRange2 / 2); h <= (guardRange2 / 2); ++h) {
		for (w = -(guardRange2 / 2); w <= (guardRange2 / 2); ++w) {
			bBreakOut = 0;
			if (h*h + w*w <= (guardRange2 / 2)* (guardRange2 / 2)) { /*Within radius*/
				/*Direction vector check to see if any obstacle in way*/
				xScale = w < 0 ? -1 : 1;
				yScale = h < 0 ? -1 : 1;
				for (y = 0; y <= abs(h); ++y) {
					for (x = 0; x <= abs(w); ++x) {
						if (Map_CheckCollision(guard->pos.X + (x * xScale), guard->pos.Y + (y * yScale), currLevel, 1)) {
							bBreakOut = 1;
							break;
						}
					}
					if (bBreakOut) break;
				}
				if (bBreakOut) continue;

				/*If within the playarea screen*/
				if (WithinPlayArea(guard->pos.X + w, guard->pos.Y + h, 1, -1)) {
					Console_SetRenderBuffer_Char(guard->pos.X + w - localMap_Offset.X, guard->pos.Y + h - localMap_Offset.Y, 250);
				}
			}
		}
	}
}

/*********************************************************************************
PrisonAlarmEvent()
Author: Chong Jun Yi
Code Lines: 24
Brief: SEND Guards to player location when alarm has sound off
**********************************************************************************/
void PrisonAlarmEvent(int proximity, int numOfGuard) {
	int i = 0;
	int guardAssigned = 0;
	int soundPlayed = 0;
	
	/*FORGET ABOUT THIS ->Can have multiple alarm, but only non-investigative guard will come*/
	for (i = 0; i < activeGuardCount; ++i) {
		if (guardAssigned >= numOfGuard) {
			break;	/*Found enough guards*/
		}

		if (proximity != -1) {	/*Alarm by proximity only*/
			if ((proximity* proximity) > (activeGuardList[i].pos.X - PlayerChar.pos.X)* (activeGuardList[i].pos.X - PlayerChar.pos.X)
				+ (activeGuardList[i].pos.Y - PlayerChar.pos.Y) * (activeGuardList[i].pos.Y - PlayerChar.pos.Y)) {
				/*Since guard is within proximity, only assign if he is not busy*/
				if (activeGuardList[i].eCurrState == Patrol_Idle_State || activeGuardList[i].eCurrState == Look_Around_State
					|| activeGuardList[i].eCurrState == Chase_ReturnPatrol_State) 
				{
					activeGuardList[i].eNewEvent = Guard_Investigate_Event;
					++guardAssigned;
					if (soundPlayed == 0) {
						FmodPlayGameSound(5);
						soundPlayed = 1;
					}
				}
			}
		}
		else if (numOfGuard > 0) { /*Go by minimum number of guards*/
			if (activeGuardList[i].eCurrState == Patrol_Idle_State || activeGuardList[i].eCurrState == Look_Around_State
				|| activeGuardList[i].eCurrState == Chase_ReturnPatrol_State) 
			{
				activeGuardList[i].eNewEvent = Guard_Investigate_Event;
				++guardAssigned;
				if (soundPlayed == 0) {
					FmodPlayGameSound(5);
					soundPlayed = 1;
				}
			}
		}
	}

}