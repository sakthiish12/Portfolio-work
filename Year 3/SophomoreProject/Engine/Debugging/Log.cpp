// /******************************************************************************/
// /*!
// \file         log.cpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         09th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        10 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../pch.h"
#include "Log.h"

void Log::LogToConsole(bool _isDisplaying)
{
   
    if (_isDisplaying)
    {
        //system("Color 40");
        std::cout << m_ss.str();
    }
    m_consoleLog.push_back(m_ss.str());
    
    m_ss.str(std::string());
   
}