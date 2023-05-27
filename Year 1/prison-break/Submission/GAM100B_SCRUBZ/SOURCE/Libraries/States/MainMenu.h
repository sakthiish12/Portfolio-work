/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef MAINMENU_H
#define MAINMENU_H

void MainMenu_EnterState();
void MainMenu_ExitState();

void MainMenu_ProcessInput();
void MainMenu_Update();
void MainMenu_Render();

void ChangeNavigationElement(int offset);

int IsNavSelect(int i);
#endif // MAINMENU_H

