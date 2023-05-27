// /******************************************************************************/
// /*!
// \file         ScriptWrappers.h
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        15
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "ScriptWrappers.h"
#include <mono/jit/jit.h>
#include <iostream>
#include "../InputMgr/InputMgr.h"

namespace PartyPantry 
{
extern MonoDomain* g_MonoDomain;

	namespace Script {
// Input
		bool CSharp_Input_IsKeyPressed(char key)
		{
		std::cout << "key\n";
		return KeyManager.GetInstance().isKeyPressed(key);
		//.isKeyPressed(key);
		//::IsKeyPressed(key);
		}

	}

    namespace CSharpScriptSample
    {
        int cppcallCPP_function()
        {
            return 5;
            //std::cout << "Calling this function callCPP_function" << std::endl;
        }

        void cppcallCPP_function_withPrimitiveParam(int a)
        {
            static_cast<void>(a);//unref
            //std::cout << "Calling this function callCPP_function_withPrimitiveParam with " << a << " as input" << std::endl;
        }
    }
}