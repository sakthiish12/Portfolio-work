#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum GameState
{
	State_SplashScreen,
	State_Default,
	State_MainMenu,
	State_Game,
	State_Game_Loading,
	State_Game_Cutscene,
	State_Instructions,
	State_Settings,
	State_Highscore,
	State_Credits,
	State_GameOver
} GameState;


void StateMachine_StartFrame();
void StateMachine_ChangeState(GameState newState);
void StateMachine_ProcessInput();
void StateMachine_Update();
void StateMachine_Render();

#endif // MAINMENU_H

