// /******************************************************************************/
// /*!
// \file         log.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         09th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        104 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#define UNUSED(x) x
//3 types of logging level: Info, Warn and Error
class Log
{
public:
    template <typename T = void, typename... Args>
    void Info(bool _isDisplaying, const char* _s, const T& _value, const Args&... _args)
    {
        std::string temp = _s;
        temp.insert(0, "[Info] ");
        LogToFile(temp.c_str(), _value, _args...);

        LogToConsole(_isDisplaying);
    }

    template <typename T = void, typename... Args>
    void Info(bool _isDisplaying, const char* _s)
    {
        std::string temp = _s;
        temp.insert(0, "[Info] ");
        LogToFile(temp.c_str());

        LogToConsole(_isDisplaying);
    }

    template <typename T, typename... Args>
    void Warn(bool _isDisplaying, const char* _s, const T& _value, const Args&... _args)
    {
        std::string temp = _s;
        temp.insert(0, "[Warn] ");
        LogToFile(temp.c_str(), _value, _args...);

        LogToConsole(_isDisplaying);
    }

    template <typename T = void, typename... Args>
    void Warn(bool _isDisplaying, const char* _s)
    {
        std::string temp = _s;
        temp.insert(0, "[Warn] ");
        LogToFile(temp.c_str());

        LogToConsole(_isDisplaying);
    }

    template <typename T, typename... Args>
    void Error(bool _isDisplaying, const char* _s, const T& _value, const Args&... _args)
    {
        std::string temp = _s;
        temp.insert(0, "[Error] ");
        LogToFile(temp.c_str(), _value, _args...);

        LogToConsole(_isDisplaying);
    }

    template <typename T = void, typename... Args>
    void Error(bool _isDisplaying, const char* _s)
    {
        std::string temp = _s;
        temp.insert(0, "[Error] ");
        LogToFile(temp.c_str());

        LogToConsole(_isDisplaying);
    }

    static std::vector<std::string> m_consoleLog;

    //using recursion, unpacks the list of format specifiers, with value representing the actual value of the data type
    template <typename T, typename... Args>
    void LogToFile(const char* _s, const T& _value, const Args&... _args)
    {

        while (*_s)
        {
            if (*_s == '%')
            {
                //double %% prints '%' itself
                if (*(_s + 1) == '%')
                {
                    ++_s;
                    m_ss << *_s;
                }

                //is a format specifier
                else
                {
                    m_ss << _value;
                    ++_s;
                    LogToFile(_s + 1, _args...);
                    return;
                }
            }

            m_ss << *_s;
            ++_s;
        }

        throw std::logic_error("extra arguments provided.");
    }

    //partial specialization. will save the text into a file.
    //this code will run when the variadic template is fully unpacked
    void LogToFile(const char* _s)
    {
        while (*_s)
        {
            if (*_s == '%' && *(_s + 1) == '%')
            {
                ++_s;
                m_ss << *_s;
            }

            m_ss << *_s;
            ++_s;
        }

        //TODO: use a macro of something to enable/disable this
        //save to txt file
        //std::filesystem::create_directories("Log"); // Create a folder
        std::fstream file;
        file.open("Config/Logging.txt", std::fstream::out | std::fstream::app);

        if (file.is_open())
        {
            file << m_ss.str();
        }
        file.close();
    }

    //display the text on the console if the bool is true.
    void LogToConsole(bool _isDisplaying);

    //variable to save the string txt into a txt file
    std::stringstream m_ss;
};