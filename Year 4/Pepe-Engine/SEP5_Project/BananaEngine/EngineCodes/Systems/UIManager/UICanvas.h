#pragma once
#ifndef UICANVAS_H
#define UICANVAS_H

#include "UIElements/UIButton.h"
#include "UIElements/UIFontText.h"
#include "UIElements/UIImage.h"
#include "UIElements/UISlider.h"
#include "UIElements/UIToggle.h"
#include "UIElements/UIProgressBar.h"

namespace BE
{
	enum class UICanvasFlag
	{
		isActive = 0,		//whether canvas is active
		// ^ Only use these *

		destroy,
		uiCanvasFlagLast
	};

	class UICanvas
	{
		//Name of the UI Canvas
		std::string m_uiCanvasName{"New Canvas"};

		//Data of all the UI Elements, packed array
		std::vector<std::shared_ptr<UIElement>> m_allUIElementData{};

		//Keeps track of which UI IDs are in use - will be sorted based on UI Element's sort layer (stored in m_allUIElementData)
		std::vector<UIElementID> m_allUIElementIDs{};

		//Maps the UI Element ID to the index of the UI Element data array
		std::unordered_map<UIElementID, size_t> m_canvasIDToIndexMap{};

		//Maps from index of UI Element data array to the CanvasID
		std::unordered_map<size_t, UIElementID> m_IndexToCanvasIDMap{};

		//Whether canvas is active or not
		std::bitset<(size_t)UICanvasFlag::uiCanvasFlagLast> m_flags{};


		//Sort layer of the canvas - lower number draws first
		int m_sortLayer{ 0 };

		//The positional offset for all UI elements, based on Draw Pos
		Vec3f m_positionOffset{};

		//Canvas ID
		const CanvasID m_canvasID;

	public:
		//Constructors/Destructors
		UICanvas(CanvasID _cid);
		~UICanvas();

		//Update loop
		void InitCanvas();					//1 time initialisation after scene is loaded
		void UpdateCanvas(double dt);		//Called every frame
		void UninitCanvas();				//1 time call when scene unloaded

		/*---------------------------------- Canvas Info/Functions ----------------------------------------*/

		//Getting and setting Canvas name
		inline const std::string& GetCanvasName() const			{ return m_uiCanvasName; }
		inline void SetCanvasName(std::string&& _newName)		{ m_uiCanvasName = std::move(_newName); }

		//Access which UI elements are active, sorted by sort layer
		inline const auto& GetAllUIElements() const				{ return m_allUIElementIDs; }

		//Get how many UI Elements are created for this canvas
		inline size_t GetUIElementCount() const					{ return m_allUIElementIDs.size(); }

		//Sort Layer - Set sort layer via the UI Manager
		inline int GetSortLayer() const							{ return m_sortLayer; }

		//Getting and Setting canvas to be active/inactive - inactive means won't draw/update
		inline bool GetIsActive() const							{ return GetCanvasFlag(UICanvasFlag::isActive); }
		inline void SetIsActive(bool _b)						{ SetCanvasFlag(UICanvasFlag::isActive, _b); }

		//Getting and Setting canvas flags
		inline bool GetCanvasFlag(UICanvasFlag _f) const		{ return m_flags.test((size_t)_f); }
		inline void SetCanvasFlag(UICanvasFlag _f, bool _b)		{ m_flags.set((size_t)_f, _b); }

		//Getting and Setting UI element positional offsets
		inline const Vec3f& GetUIOffset() const					{ return m_positionOffset; }
		void SetUIOffset(const Vec3f& _offset);

		void ResetOffset();

		/*---------------------------------- UI Element Functions ----------------------------------------*/

		//Create a new UI Element by UI Type class name, optionally take in an ID
		template <typename UIType>
		UIElementID CreateUI(UIElementID _uid = UIElementID{});

		//Create a new UI Element by enum, optionally take in an ID
		UIElementID CreateUI(UIElementType _type, UIElementID _uid = UIElementID{});

		//Flag the UI to be ready for destruction during the UI Manager update loop
		void DestroyUI(UIElementID _uid);

	private:
		//Actually destroy an UI Element
		void EraseUI(UIElementID _uid);

	public:
		//Get a UI Element as base class type
		std::shared_ptr<UIElement>& GetUI(UIElementID _uid);
		const std::shared_ptr<UIElement>& GetUI(UIElementID _uid) const;

		//Get a UI Element as the derived class type, do check if return type is a nullptr before accessing data
		template <typename UIType>
		std::shared_ptr<UIType> GetUI(UIElementID _uid);

		template <typename UIType>
		std::shared_ptr<UIType> GetUI(UIElementID _uid) const;

		//Set the sort layer of the UI Element, will also sort the layer
		void SetUIElement_SortLayer(UIElementID _uid, int _newLayer);

		//Clear the canvas - destroy all UI elements in this canvas
		void ClearCanvas();

		/*-------------------------------- Finding UI Element ------------------------------------------*/

		//Search for all UI Elements that has _substr in their name, case sensitive
		//No. of UI Element IDs limited by count, optionally filtered by active status
		std::vector<UIElementID> FindUIElementBySubstr(const std::string& _substr, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		//Search for all UI Elements that is called _name, case sensitive
		//No. of UI Element IDs limited by count, optionally filtered by active status
		std::vector<UIElementID> FindUIElementByName(const std::string& _name, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		//Find all UI element IDs with a given UI type, case sensitive
		//No. of UI Element IDs limited by count, optionally filtered by active status
		std::vector<UIElementID> FindUIElementByType(UIElementType _type, size_t count = MAX_UI_COUNT, const std::optional<bool>& _isActive = std::nullopt) const;

		/*----------------------------------------------------------------------------------------------*/
		void SortUIElements();
	private:
		//Disabling copy constructor/assignment
		UICanvas& operator=(const UICanvas&) = delete;
		UICanvas(const UICanvas&) = delete;

		//Only UI Manager can access these stuff
		friend class UIManager;

		//Used by UI Manager
		inline void SetSortLayer(int _newLayer)					{ m_sortLayer = _newLayer; }
	
		

	public:
		/*
			RTTR Stuff
		*/
		RTTR_ENABLE();
		RTTR_REGISTRATION_FRIEND;
	};


	template <typename UIType>
	UIElementID UICanvas::CreateUI(UIElementID _uid)
	{
		static_assert(std::is_base_of<UIElement, UIType>::value, "Static assertion failed: UIType must inherit UIElement");
		AssertCore(_uid != NULL_UIELEMENT, "Cannot create UI with ID 0");
		AssertCore(m_canvasIDToIndexMap.count(_uid) == 0, "UI Element with ID %llu already exists", _uid);
		AssertCore(m_allUIElementData.size() < MAX_UI_COUNT, "Too many UI Element created for this canvas, max is %d", MAX_UI_COUNT);

		//Update the mapping from UI Element to the index where its data is actually stored
		size_t sz = m_allUIElementData.size();
		m_canvasIDToIndexMap[_uid] = sz;
		m_IndexToCanvasIDMap[sz] = _uid;

		//Put the new UI Element into the active UI Elements array
		m_allUIElementIDs.push_back(_uid);

		//Then create a pointer to that UI Element
		m_allUIElementData.push_back(std::make_shared<UIType>(m_canvasID, _uid));

		//Sort the UI Elements
		SortUIElements();

		//Returns the ID to the entity
		return _uid;
	}

	template <typename UIType>
	std::shared_ptr<UIType> UICanvas::GetUI(UIElementID _uid)
	{
		return static_cast<const UICanvas*>(this)->GetUI<UIType>(_uid);
	}

	template <typename UIType>
	std::shared_ptr<UIType> UICanvas::GetUI(UIElementID _uid) const
	{
		static_assert(std::is_base_of<UIElement, UIType>::value, "Static assertion failed: UIType must inherit UIElement");
		return std::dynamic_pointer_cast<UIType>(GetUI(_uid));
	}
}

#endif