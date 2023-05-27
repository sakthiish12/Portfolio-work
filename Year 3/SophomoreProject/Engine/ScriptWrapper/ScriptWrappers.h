// /******************************************************************************/
// /*!
// \file         ScriptWrappers.h
// \project name Engine
// \author(s)    Sakthiish
// \date         5th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        12
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

extern "C"
{
    typedef struct _MonoString MonoString;
    typedef struct _MonoArray MonoArray;
}

namespace PartyPantry 
{
    namespace Script {
        bool CSharp_Input_IsKeyPressed(char key);
    }

    namespace CSharpScriptSample
    {
        int cppcallCPP_function();

        void cppcallCPP_function_withPrimitiveParam(int a);
    }
    
}