/******************************************************************************/
/*!
\file		Assets_Window.h
\par        Project:
\author		Pham Thao Mien
\date   	October 7, 2021
\brief		Get all game resources into the engine

\copyright	Copyright (C) 2021 DigiPen Institute of Technology.
			Reproduction or disclosure of this file or its contents without the
			prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/
#pragma once
#ifndef ASSETS_WINDOW_H
#define ASSETS_WINDOW_H

#include "../ImGuiWindow/ImGuiWindow.h"
#ifdef USE_EDITOR
#include <AssetManager/AssetManager.h>


namespace BE
{
	class Assets_Window : public ImGuiWindow
	{
	private:
		static Assets_Window* p_asset;

		//BE::AudioMode _mode;

		static std::string prefabs_filepath;
	public:	
		Assets_Window();
		~Assets_Window();

		void RemoveAsset_Popup(BE::AssetID _assetID);

		static inline Assets_Window* Get() { return p_asset; }

		static std::string GetPrefabsPath() { return prefabs_filepath; };

		Assets_Window* init();
		void update() override;
		void Shutdown() override;
	};
};
#endif
#endif