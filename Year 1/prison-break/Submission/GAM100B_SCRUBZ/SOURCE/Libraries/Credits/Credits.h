/*********************************************************************************
* \copyright	Copyright (c) 2019 DigiPen Institute of Technology. Reproduction
				or disclosure of this file or its contents without the prior
				written consent of DigiPen Institute of Technology is prohibited.
**********************************************************************************/
#ifndef CREDITS_H
#define CREDITS_H

/*State Management*/
void Credits_EnterState();
void Credits_ExitState();

/*Credits Page - Keyboard Input*/
void Credits_ProcessInput();
/*Credits Page - Logic*/
void Credits_Update();
/*Credits Page - Rendering*/
void Credits_Render();

#endif // CREDITS_H
