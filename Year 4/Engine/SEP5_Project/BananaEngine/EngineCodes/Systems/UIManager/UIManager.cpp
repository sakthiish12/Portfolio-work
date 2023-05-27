#include "pch.h"
#include "UIManager.h"
#include "../SceneManager/SceneManager.h"
#include <Serialisation/BananaSerialisation.h>
#include <AssetManager/AssetManager.h>

namespace BE
{
	UIManager::UIManager()
	{
		m_allUICanvasData.reserve(MAX_UI_COUNT);
		m_allUICanvasIDs.reserve(MAX_UI_COUNT);

		//When assets have been loaded, it needs to load the UI
		EventMgr->SubscribeWPrio(UnloadSceneEvent, this, UIManager::OnSceneUnload, 1);
		EventMgr->SubscribeWPrio(SceneRestartingEvent, this, UIManager::OnSceneUnload, 1);
		EventMgr->SubscribeWPrio(WindowResizeEvent, this, UIManager::OnWindowResize, 1);

		LogCore("Created UI Manager");
	}

	UIManager::~UIManager()
	{
		LogCore("Destroyed UI Manager");
	}

	CanvasID UIManager::CreateCanvas(int _sortOrder, CanvasID _cid)
	{
		AssertCore(_cid != NULL_UICANVAS, "Cannot create UI Canvas with ID 0");
		AssertCore(m_canvasIDToIndexMap.count(_cid) == 0, "UI Canvas with ID %llu already exists", _cid);

		//Update the mapping from UI Canvas to the index where its data is actually stored
		size_t sz = m_allUICanvasData.size();
		m_canvasIDToIndexMap[_cid] = sz;
		m_IndexToCanvasIDMap[sz] = _cid;

		//Put the new UI Canvas into the active UI Canvas array
		m_allUICanvasIDs.push_back(_cid);

		//Then create a pointer to that UI Canvas
		m_allUICanvasData.push_back(std::make_unique<UICanvas>(_cid));

		//Sort the UI Elements
		SortUICanvases();

		//Returns the ID to the entity
		return _cid;
	}

	void UIManager::DestroyCanvas(CanvasID _cid)
	{
		AssertCore(m_canvasIDToIndexMap.count(_cid) != 0, "UI Canvas with ID %llu was not created", _cid);
		m_allUICanvasData[m_canvasIDToIndexMap.at(_cid)]->SetCanvasFlag(UICanvasFlag::destroy, true);
	}


	void UIManager::EraseCanvas(CanvasID _cid)
	{
		AssertCore(m_canvasIDToIndexMap.count(_cid) != 0, "UI Canvas with ID %llu was not created", _cid);

		//Determine the last UI that will take over the memory position of the UI
		size_t indexToRemove = m_canvasIDToIndexMap.at(_cid);		//Index of the UI Canvas data to be removed
		size_t indexToLast = m_allUICanvasData.size() - 1;		//Index of the last canvas of the UI Canvases
		UIElementID lastUI = m_IndexToCanvasIDMap.at(indexToLast);		//The last Canvases that is going to take over the deleted slot

		//Move the indexToLast to indexToRemove
		m_allUICanvasData[indexToRemove] = std::move(m_allUICanvasData[indexToLast]);

		//Update the mapping
		m_canvasIDToIndexMap[lastUI] = indexToRemove;				//Update the mapping for the last canvas
		m_IndexToCanvasIDMap[indexToRemove] = lastUI;				//Update the mapping for the newly occupied slot

		//Remove the deleted UI Element ID
		m_canvasIDToIndexMap.erase(_cid);
		m_IndexToCanvasIDMap.erase(indexToLast);

		//Remove the UI Element ID and data
		auto it = std::find(m_allUICanvasIDs.begin(), m_allUICanvasIDs.end(), _cid);
		m_allUICanvasIDs.erase(it);
		m_allUICanvasData.pop_back();
	}

	std::unique_ptr<UICanvas>& UIManager::GetCanvas(CanvasID _cid)
	{
		return const_cast<std::unique_ptr<UICanvas>&>(static_cast<const UIManager*>(this)->GetCanvas(_cid));
	}

	const std::unique_ptr<UICanvas>& UIManager::GetCanvas(CanvasID _cid) const
	{
		AssertCore(m_canvasIDToIndexMap.count(_cid) != 0, "UI Canvas with ID %llu was not created", _cid);
		return m_allUICanvasData[m_canvasIDToIndexMap.at(_cid)];
	}

	void UIManager::SetUICanvas_SortLayer(CanvasID _cid, int _newLayer)
	{
		AssertCore(m_canvasIDToIndexMap.count(_cid) != 0, "UI Canvas with ID %llu was not created", _cid);
		GetCanvas(_cid)->SetSortLayer(_newLayer);
		SortUICanvases();
	}

	std::shared_ptr<UIElement>& UIManager::GetUI(CanvasID _cid, UIElementID _uid)
	{
		return GetCanvas(_cid)->GetUI(_uid);
	}

	const std::shared_ptr<UIElement>& UIManager::GetUI(CanvasID _cid, UIElementID _uid) const
	{
		return GetCanvas(_cid)->GetUI(_uid);
	}

	//Save the font states and hide all the fonts
	void UIManager::SaveFontState()
	{
		for (size_t i = 0; i < m_fontIDs.size(); ++i)
		{
			const auto& [cid, uid] = m_fontIDs[i];
			m_fontActiveStates[i] = BE::UI->GetCanvas(cid)->GetUI(uid)->GetUIElementFlag(BE::UIElementFlags::isActive);
			BE::UI->GetCanvas(cid)->GetUI(uid)->SetUIElementFlag(BE::UIElementFlags::isActive, false);
		}
	}

	//Reload the font states
	void UIManager::RestoreFontState()
	{
		for (size_t i = 0; i < m_fontIDs.size(); ++i)
		{
			const auto& [cid, uid] = m_fontIDs[i];
			BE::UI->GetCanvas(cid)->GetUI(uid)->SetUIElementFlag(BE::UIElementFlags::isActive, m_fontActiveStates[i]);
		}
	}

	//Checks if the canvas exists
	bool UIManager::DoesUICanvasExist(CanvasID _cid) const
	{
		return std::find(m_allUICanvasIDs.begin(), m_allUICanvasIDs.end(), _cid) != m_allUICanvasIDs.end();
	}

	//Checks if the UI element exists in the given canvas
	bool UIManager::DoesUIElementExist(CanvasID _cid, UIElementID _uid) const
	{
		const auto& uiE = GetCanvas(_cid)->GetAllUIElements();
		return std::find(uiE.begin(), uiE.end(), _uid) != uiE.end();

	}

	std::vector<CanvasID> UIManager::FindUICanvasesBySubstr(const std::string& _substr, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<CanvasID> result;
		result.reserve(m_allUICanvasIDs.size());

		for (CanvasID cid : m_allUICanvasIDs)
		{
			//Limit the result to this number of UI
			if (result.size() >= count)
				break;

			//Retrieve the name of the UI
			const std::string& name = this->GetCanvas(cid)->GetCanvasName();
			bool isActive = this->GetCanvas(cid)->GetIsActive();

			//If canvas's name contains _substr, and
			//either the isActive is not specfied, or it matches the requested active status
			if (name.find(_substr) < name.size() && (!_isActive || isActive == *_isActive))
				result.push_back(cid);
		}

		return result;
	}

	std::vector<CanvasID> UIManager::FindUICanvasesByName(const std::string& _name, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<CanvasID> result;
		result.reserve(m_allUICanvasIDs.size());

		for (CanvasID cid : m_allUICanvasIDs)
		{
			//Limit the result to this number of UI
			if (result.size() >= count)
				break;

			//Retrieve the name of the UI
			const std::string& name = this->GetCanvas(cid)->GetCanvasName();
			bool isActive = this->GetCanvas(cid)->GetIsActive();

			//If canvas's name is _name, and
			//either the isActive is not specfied, or it matches the requested active status
			if (name == _name && (!_isActive || isActive == *_isActive))
				result.push_back(cid);
		}

		return result;
	}


	std::vector<std::pair<CanvasID, UIElementID>> UIManager::FindUIElementByName(const std::string& _name, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<std::pair<CanvasID, UIElementID>> result;
		result.reserve(count);

		for (CanvasID cid : m_allUICanvasIDs)
		{
			if (result.size() >= count)
				break;
		
			const auto& canvas = GetCanvas(cid);

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				if (result.size() >= count)
					break;

				//Retrieve the name of the UI
				const std::string& name = this->GetCanvas(cid)->GetUI(uid)->GetName();
				bool isActive = this->GetCanvas(cid)->GetUI(uid)->GetUIElementFlag(UIElementFlags::isActive);

				//If UI Element contains _substr, and
				//either the isActive is not specfied, or it matches the requested active status
				if (name == _name && (!_isActive || isActive == *_isActive))
					result.push_back(std::make_pair(cid, uid));

			}
		}

		return result;
	}

	std::vector<std::pair<CanvasID, UIElementID>> UIManager::FindUIElementBySubstr(const std::string& _substr, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<std::pair<CanvasID, UIElementID>> result;
		result.reserve(count);

		for (CanvasID cid : m_allUICanvasIDs)
		{
			if (result.size() >= count)
				break;
			
			const auto& canvas = GetCanvas(cid);

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				if (result.size() >= count)
					break;

				//Retrieve the name of the UI
				const std::string& name = this->GetCanvas(cid)->GetUI(uid)->GetName();
				bool isActive = this->GetCanvas(cid)->GetUI(uid)->GetUIElementFlag(UIElementFlags::isActive);
			
				//If UI Element contains _substr, and
				//either the isActive is not specfied, or it matches the requested active status
				if(name.find(_substr) < name.size() && (!_isActive || isActive == *_isActive))
					result.push_back(std::make_pair(cid, uid));

			}
		}

		return result;
	}



	void UIManager::SortUICanvases()
	{
		std::stable_sort(m_allUICanvasIDs.begin(), m_allUICanvasIDs.end(),
			[this](const CanvasID& lhs, const CanvasID& rhs) -> bool
			{
				return GetCanvas(lhs)->GetSortLayer() < GetCanvas(rhs)->GetSortLayer();
			});
	}



	void UIManager::Init()
	{
		
	}

	void UIManager::Awake()
	{
		//Awake the UI
		for (CanvasID cid : UI->GetAllCanvases())
		{
			auto& canvas = UI->GetCanvas(cid);

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				auto& ui = canvas->GetUI(uid);
				auto& scripts = ui->GetUIScript().GetAllScripts();

				for (size_t i = 0; i < scripts.size(); ++i)
				{
					scripts[i]->AssignId(cid, uid);
					if (scripts[i]->GetState() == IScript::ScriptState::NotAwake)
					{
						scripts[i]->Awake();
						scripts[i]->SetState(IScript::ScriptState::Awake);
					}
				}
			}

		}

		//For each canvas, init the UI
		for (CanvasID cid : m_allUICanvasIDs)
			GetCanvas(cid)->InitCanvas();

		//Go through all the UI and look for font types
		for (BE::CanvasID cid : BE::UI->GetAllCanvases())
		{
			auto& canvas = BE::UI->GetCanvas(cid);

			for (BE::UIElementID uid : canvas->GetAllUIElements())
			{
				auto ui = canvas->GetUI(uid);

				if (ui->GetUIType() == BE::UIElementType::fontText)
				{
					m_fontIDs.push_back(std::make_pair(cid, uid));
					m_fontActiveStates.push_back(false);
				}
			}
		}
	}

	void UIManager::Update(double dt)
	{
#ifdef USE_EDITOR
		//If IMGUI is pausing, then don't update
		if (!Debug::GetInstance()->GetDebugFlag(Debug::DebugFlags::imguiPause))
		{
#endif
			m_isInteractingWithUI = false;

			//Update canvas in reverse order, so the top most canvas will get the interaction first, if any
			for (auto it = m_allUICanvasIDs.rbegin(); it != m_allUICanvasIDs.rend(); ++it)
			{
				//If canvas is active, update canvas
				if (GetCanvas(*it)->GetIsActive())
					GetCanvas(*it)->UpdateCanvas(dt);
			}
#ifdef USE_EDITOR
		}
#endif

		//Destroy all the UI Canvases and Elements that are flagged to be destroyed
		for (size_t i = 0; i < m_allUICanvasIDs.size(); ++i)
		{
			const auto& canvas = GetCanvas(m_allUICanvasIDs[i]);
			//If the canvas is to be destroyed, then just destroy the canvas
			if (canvas->GetCanvasFlag(UICanvasFlag::destroy))
			{
				EraseCanvas(m_allUICanvasIDs[i]);
				--i;
				continue;
			}

			//Now check the UI Elements to see if needs to be destroyed
			for (size_t j = 0; j < canvas->GetUIElementCount(); ++j)
			{
				auto ui = canvas->GetUI(canvas->GetAllUIElements()[j]);

				//If UI Element is to be destroyed, then destroy UI
				if (ui->GetUIElementFlag(UIElementFlags::destroy))
				{
					canvas->EraseUI(canvas->GetAllUIElements()[j]);
					--j;
				}
			}
		}
	}

	void UIManager::Reload()
	{
		//Call clean up on all the scripts
		
		//For each canvas
		for (CanvasID cid : m_allUICanvasIDs)
			GetCanvas(cid)->UninitCanvas();

		m_allUICanvasData.clear();
		m_allUICanvasIDs.clear();
		m_canvasIDToIndexMap.clear();
		m_IndexToCanvasIDMap.clear();
		m_isInteractingWithUI = false;

		m_fontIDs.clear();
		m_fontActiveStates.clear();
	}

	void UIManager::Shutdown()
	{

	}

	void UIManager::OnSceneUnload(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::unloadScene || e.GetEventType() == EventType::restartScene,
			"Wrong event, expected UnloadSceneEvent or SceneRestartingEvent, passed in %s instead.", e.GetEventName());

		//Destroy all canvases backwards
		for (int i = static_cast<int>(GetAllCanvases().size()) - 1; i >= 0; --i)
			EraseCanvas(GetAllCanvases()[i]);
	}

	void UIManager::OnToggleFullScreen(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::toggleFullScreen,
			"Wrong event, expected WindowToggleFullScreenEvent, passed in %s instead.", e.GetEventName());

		//Update the read only transform for all UI Elements
		for (CanvasID cid : m_allUICanvasIDs)
		{
			auto& canvas = GetCanvas(cid);

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				auto& uiElement = GetCanvas(cid)->GetUI(uid);
				uiElement->UpdateReadOnlyTransform();
			}
		}
	}

	void UIManager::OnWindowResize(IEvent& e)
	{
		AssertCore(e.GetEventType() == EventType::windowResize,
			"Wrong event, expected WindowResizeEvent, passed in %s instead.", e.GetEventName());

		//Update the read only transform for all UI Elements
		for (CanvasID cid : m_allUICanvasIDs)
		{
			auto& canvas = GetCanvas(cid);

			for (UIElementID uid : canvas->GetAllUIElements())
			{
				auto& uiElement = GetCanvas(cid)->GetUI(uid);
				uiElement->UpdateReadOnlyTransform();
			}
		}
	}
}