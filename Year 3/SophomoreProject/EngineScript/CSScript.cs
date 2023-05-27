// /******************************************************************************/
// /*!
// \file         CSScript.cs
// \project name Engine
// \author(s)    Sakthiish
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the BatchRenderer 
// \lines        55
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

//For using cpp function
using System.Runtime.CompilerServices;

using System.Runtime.InteropServices;



namespace EngineScript
{
   
    public class Script 
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static int callCPP_function();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void callCPP_function_withPrimitiveParam(int a);
        
        public void init()
        {
            //Console.WriteLine("script init ");
        }

        public void update() 
        {
			//Console.WriteLine("script update ");
            //callCPP_function();
            int a = callCPP_function();
            callCPP_function_withPrimitiveParam(a);
        }

        public void destory()
        {
            //Console.WriteLine("script destory ");
        }

        public static Script GetInstance() 
        {
            return new Script();
        }
    }
}
