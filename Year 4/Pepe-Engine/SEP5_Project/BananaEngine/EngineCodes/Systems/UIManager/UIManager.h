#pragma once
#ifndef	UIMANAGER_H
#define UIMANAGER_H

//Different types of UI here

#include "UICanvas.h"
#include "../ISystem.h"

namespace BE
{
	class UIManager : public Singleton<UIManager>, public ISystem
	{
		//Data of all the UI Canvases, packed array
		std::vector<std::unique_ptr<UICanvas>> m_allUICanvasData;

		//Keeps track of which UI Canvas IDs are in use - will be sorted based on UI Canvas's sort layer (stored in m_allUICanvasData)
		std::vector<CanvasID> m_allUICanvasIDs{};

		//Maps the CanvasID to the index of the UI Element data array
		std::unordered_map<CanvasID, size_t> m_canvasIDToIndexMap{};

		//Maps from index of UI Canvas data array to the CanvasID
		std::unordered_map<size_t, CanvasID> m_IndexToCanvasIDMap{};

		//All the fonts and their save states
		std::vector<std::pair<CanvasID, UIElementID>> m_fontIDs{};
		std::vector<bool> m_fontActiveStates{};

		//Whether the player is currently hovering any button, for safeguarding against overlapping UI
		bool m_isInteractingWithUI{ false };

		UIManager();
		~UIManager();

	public:
		/*-------------------------- Canvas Functions ----------------------------------*/
		//Create a new canvas, with an optional ID
		CanvasID CreateCanvas(int _sortOrder = 0, CanvasID _cid = CanvasID{});

		//Flag a canvas ready for destroying
		void DestroyCanvas(CanvasID _cid);

	private:
		//Actually destroy a canvas
		void EraseCanvas(CanvasID _cid);

	public:
		/* -------------------------------- Get a canvas ------------------------------------*/
		std::unique_ptr<UICanvas>& GetCanvas(CanvasID _cid);
		const std::unique_ptr<UICanvas>& GetCanvas(CanvasID _cid) const;

		//Set UI Canvas sort layer, lower number draw first
		void SetUICanvas_SortLayer(CanvasID _cid, int _newLayer);

		/* -------------------------------- Get a UI element ---------------------------------*/
		std::shared_ptr<UIElement>& GetUI(CanvasID _cid, UIElementID _uid);
		const std::shared_ptr<UIElement>& GetUI(CanvasID _cid, UIElementID _uid) const;

		template <typename UIType>
		std::shared_ptr<UIType> GetUI(CanvasID _cid, UIElementID _uid);

		template <typename UIType>
		std::shared_ptr<UIType> GetUI(CanvasID _cid, UIElementID _uid) const;



		/*--------------------------- Show and hide font ------------------------------------*/

		//Save the font states and hide all the fonts
		void SaveFontState();

		//Reload the font states
		void RestoreFontState();


		/*--------------------------- UI Manager info ------------------------------------*/

		//Get all the canvases
		inline const auto& GetAllCanvases() const						{ return m_allUICanvasIDs; }

		//Get the number of canvases loaded
		inline size_t GetUICanvasCount() const							{ return m_allUICanvasIDs.size(); }

		//Checks if there is any interaction with UI Elements
		inline bool GetIsInteractingWithUI() const						{ return m_isInteractingWithUI; }
		inline void SetIsInteractingWithUI()							{ m_isInteractingWithUI = true; }

		/*------------------------------- Check if exists functions ----------------------*/

		//Checks if the canvas exists
		bool DoesUICanvasExist(CanvasID _cid) const;

		//Checks if the UI element exists in the given canvas
		bool DoesUIElementExist(CanvasID _cid, UIElementID _uid) const;

		/*------------------------------ Finding functions -------------------------------*/

		//Search for all UI Canvases that has _substr in their name, case sensitive 
		//No. of UI Canvases IDs limited by count, optionally filtered by active status
		std::vector<CanvasID> FindUICanvasesBySubstr(const std::string& _substr, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		//Search for all UI Canvases that is called _name, case sensitive 
		//No. of UI Canvases IDs limited by count, optionally filtered by active status
		std::vector<CanvasID> FindUICanvasesByName(const std::string& _name, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		//Search for all UI Elements that is called _name, case sensitive, pair.first is the CanvasID it is in, pair.second is the ElementID 
		//No. of UI Canvases/Element ID pairs limited by count, optionally filtered by active status
		std::vector<std::pair<CanvasID, UIElementID>> FindUIElementByName(const std::string& _name, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		//Search for all UI Element that has _substr in their name, case sensitive 
		//No. of UI Canvases/Element ID pairs limited by count, optionally filtered by active status
		std::vector<std::pair<CanvasID, UIElementID>> FindUIElementBySubstr(const std::string& _substr, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		//Sorting the UI Canvases
		void SortUICanvases();

		/*--------------------------------- System functions -----------------------------*/
		ISYSTEM_FUNCTIONS;

		/*--------------------------------- Event Handler --------------------------------*/

		//Unload all entities in the current scene, then need to load loading scene entities
		void OnSceneUnload(IEvent& e);
		void OnToggleFullScreen(IEvent& e);
		void OnWindowResize(IEvent& e);

		friend class Singleton<UIManager>;
	};

	template <typename UIType>
	std::shared_ptr<UIType> UIManager::GetUI(CanvasID _cid, UIElementID _uid)
	{
		return GetCanvas(_cid)->GetUI<UIType>(_uid);
	}

	template <typename UIType>
	std::shared_ptr<UIType> UIManager::GetUI(CanvasID _cid, UIElementID _uid) const
	{
		return GetCanvas(_cid)->GetUI<UIType>(_uid);
	}



#define UI UIManager::GetInstance()
}

#endif