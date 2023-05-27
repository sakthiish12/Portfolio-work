/******************************************************************************/
/*!
\file		Console_Window.cpp
\par        Project: Candy Crusade
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Debug Console

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#include "pch.h"
#include "Console_Window.h"

#ifdef USE_EDITOR
#include "../Menu_Window/Menu_Window.h"
#include <Debugger/Debugger.h>

namespace BE
{
	Console_Window* Console_Window::p_console = nullptr;

	Console_Window::Console_Window()
	{
	}

	Console_Window::~Console_Window()
	{
		
	}

	Console_Window* Console_Window::init()
	{
		p_console = new Console_Window();
		return p_console;
	}

	void Console_Window::update()
	{
		if (Menu_Window::Get()->showConsoleWindow)
		{
			if (!begin("Console", &Menu_Window::Get()->showConsoleWindow))
			{
				end();
			}
			else
			{
				if (ImGui::Button("Clear Console"))
				{
					BE::Debug::GetInstance()->ClearConsole();
				}

				static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

				if (ImGui::BeginTable("table_scrolly", 6, flags))
				{
					ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
					ImGui::TableSetupColumn("LEVEL", ImGuiTableColumnFlags_None);
					ImGui::TableSetupColumn("LOCATION", ImGuiTableColumnFlags_None);
					ImGui::TableSetupColumn("LINE", ImGuiTableColumnFlags_None);
					ImGui::TableSetupColumn("FILE", ImGuiTableColumnFlags_None);
					ImGui::TableSetupColumn("MESSAGE", ImGuiTableColumnFlags_None);
					ImGui::TableSetupColumn("COUNT", ImGuiTableColumnFlags_None);
					ImGui::TableHeadersRow();

					auto log = BE::Debug::GetInstance()->GetRecentLogs();

					for (auto n : log)
					{
						ImGui::TableNextRow();
						for (int column = 0; column < 6; column++)
						{
							ImGui::TableSetColumnIndex(column);

							if (column == 0)
							{
								std::string level = (n.first.level == BE::Debug::LogLevel::info) ? "LOG" : (n.first.level == BE::Debug::LogLevel::warn) ? "WRN" : "ERR";

								if (level == "ERR")
								{
									ImGui::TextColored(ImVec4(1, 0, 0, 1), level.c_str());
								}
								else if (level == "WRN")
								{
									ImGui::TextColored(ImVec4(1, 1, 0, 1), level.c_str());
								}
								else
								{
									ImGui::Text(level.c_str());
								}

							}
							else if (column == 1)
							{
								std::string location = (n.first.location == BE::Debug::LogLocation::engine) ? "COR" : (n.first.location == BE::Debug::LogLocation::editor) ? "EDI" : "APP";
								ImGui::Text(location.c_str());
							}
							else if (column == 2)
							{
								std::string line = std::to_string(n.first.line);
								ImGui::Text(line.c_str());
							}
							else if (column == 3)
							{
								std::string fileName(n.first.file);
								size_t pos = fileName.find_last_of('\\');
								fileName.erase(0, pos + 1);

								ImGui::Text(fileName.c_str());
							}
							else if (column == 4)
							{
								std::string message = n.first.message;
								ImGui::TextWrapped(message.c_str());
							}
							else if (column == 5)
							{
								std::string count = std::to_string(n.second);
								ImGui::Text(count.c_str());
							}
						}
					}
					ImGui::EndTable();
				}

				end();
			}
		}
	}

	void Console_Window::Shutdown()
	{
		delete p_console;
	}
}
#endif