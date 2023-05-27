#include "pch.h"
#include "UICanvas.h"
#include <Serialisation/BananaSerialisation.h>


namespace BE
{
	UICanvas::UICanvas(CanvasID _cid) : m_canvasID(_cid)
	{
		m_allUIElementData.reserve(MAX_UI_COUNT);
		m_allUIElementIDs.reserve(MAX_UI_COUNT);
		m_flags.set((size_t)UICanvasFlag::isActive);
		LogCore("Created UI Canvas");
	}

	UICanvas::~UICanvas()
	{
		LogCore("Destroyed UI Canvas");
	}

	void UICanvas::InitCanvas()
	{
		SortUIElements();

		//Call init for UI Elements
		for (UIElementID& id : m_allUIElementIDs)
			GetUI(id)->InitUI();
	}

	void UICanvas::UpdateCanvas(double dt)
	{
		//Call init for UI Elements
		for (auto it = m_allUIElementIDs.rbegin(); it != m_allUIElementIDs.rend(); ++it)
		{
			if(GetUI(*it)->GetUIElementFlag(UIElementFlags::isActive))
				GetUI(*it)->UpdateUI(dt);
		}
	}

	void UICanvas::UninitCanvas()
	{
		//Call unload for UI Elements
		for (UIElementID& id : m_allUIElementIDs)
			GetUI(id)->UninitUI();

		ClearCanvas();
	}

	void UICanvas::SetUIOffset(const Vec3f& _offset)
	{
		m_positionOffset = _offset;

		//Also update the position of all the children
		for (auto& ui : m_allUIElementData)
			ui->UpdateReadOnlyTransform();
	}

	void UICanvas::ResetOffset()
	{ 
		m_positionOffset = Vec3f{}; 

		//Also update the position of all the children
		for (auto& ui : m_allUIElementData)
			ui->UpdateReadOnlyTransform();
	}




	//Create a new UI Element by enum, optionally take in an ID
	UIElementID UICanvas::CreateUI(UIElementType _type, UIElementID _uid)
	{
		switch (_type)
		{
		case UIElementType::button:
			CreateUI<UIButton>(_uid);
			break;
		case UIElementType::fontText:
			CreateUI<UIFontText>(_uid);
			break;
		case UIElementType::image:
			CreateUI<UIImage>(_uid);
			break;
		case UIElementType::slider:
			CreateUI<UISlider>(_uid);
			break;
		case UIElementType::toggle:
			CreateUI<UIToggle>(_uid);
			break;
		case UIElementType::progressBar:
			CreateUI<UIProgressBar>(_uid);
			break;
		default:
			LogCoreError("Attempting to create an unknown UI Type");
			return NULL_UIELEMENT;
		}

		return _uid;
	}

	void UICanvas::DestroyUI(UIElementID _uid)
	{
		AssertCore(m_canvasIDToIndexMap.count(_uid) != 0, "UI Element with ID %llu was not created", _uid);
		m_allUIElementData[m_canvasIDToIndexMap.at(_uid)]->SetUIElementFlag(UIElementFlags::destroy, true);
	}

	void UICanvas::EraseUI(UIElementID _uid)
	{
		AssertCore(m_canvasIDToIndexMap.count(_uid) != 0, "UI Element with ID %llu was not created", _uid);

		//Determine the last UI that will take over the memory position of the UI
		size_t indexToRemove = m_canvasIDToIndexMap.at(_uid);			//Index of the UI Element data to be removed
		size_t indexToLast = m_allUIElementData.size() - 1;		//Index of the last element of the active UI elements
		UIElementID lastUI = m_IndexToCanvasIDMap.at(indexToLast);		//The last UI that is going to take over the deleted slot

		//Move the indexToLast to indexToRemove
		m_allUIElementData[indexToRemove] = std::move(m_allUIElementData[indexToLast]);
		
		//Update the mapping
		m_canvasIDToIndexMap[lastUI] = indexToRemove;				//Update the mapping for the last UI
		m_IndexToCanvasIDMap[indexToRemove] = lastUI;				//Update the mapping for the newly occupied slot

		//Remove the deleted UI Element ID
		m_canvasIDToIndexMap.erase(_uid);
		m_IndexToCanvasIDMap.erase(indexToLast);

		//Remove the UI Element ID and data
		auto it = std::find(m_allUIElementIDs.begin(), m_allUIElementIDs.end(), _uid);
		m_allUIElementIDs.erase(it);
		m_allUIElementData.pop_back();
	}

	//Get a UI Element as base class type
	std::shared_ptr<UIElement>& UICanvas::GetUI(UIElementID _uid)
	{
		return const_cast<std::shared_ptr<UIElement>&>(static_cast<const UICanvas*>(this)->GetUI(_uid));
	}

	const std::shared_ptr<UIElement>& UICanvas::GetUI(UIElementID _uid) const
	{
		AssertCore(m_canvasIDToIndexMap.count(_uid) != 0, "UI Element with ID %llu was not created", _uid);
		return m_allUIElementData[m_canvasIDToIndexMap.at(_uid)];
	}

	//Set the sort layer of the UI Element, will also sort the layer
	void UICanvas::SetUIElement_SortLayer(UIElementID _uid, int _newLayer)
	{
		AssertCore(m_canvasIDToIndexMap.count(_uid) != 0, "UI Element with ID %llu was not created", _uid);
		GetUI(_uid)->SetSortLayer(_newLayer);
		SortUIElements();
	}

	//Clear the canvas - destroy all UI elements in this canvas
	void UICanvas::ClearCanvas()
	{
		//Release all the shared pointers
		for (size_t i = 0; i < m_allUIElementData.size(); ++i)
			m_allUIElementData[i].reset();

		//Clear the containers
		m_allUIElementIDs.clear();
		m_canvasIDToIndexMap.clear();
		m_IndexToCanvasIDMap.clear();
	}

	void UICanvas::SortUIElements()
	{
		std::stable_sort(m_allUIElementIDs.begin(), m_allUIElementIDs.end(),
			[this](const UIElementID& lhs, const UIElementID& rhs) -> bool
			{
				return GetUI(lhs)->GetSortLayer() < GetUI(rhs)->GetSortLayer();
			});
	}

	std::vector<UIElementID> UICanvas::FindUIElementBySubstr(const std::string& _substr, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<UIElementID> result;
		result.reserve(MAX_UI_COUNT);

		for (UIElementID uid : m_allUIElementIDs)
		{
			//Limit the result to this number of UI
			if (result.size() >= count)
				break;

			//Retrieve the name of the UI
			const std::string& name = this->GetUI(uid)->GetName();
			bool isActive = this->GetUI(uid)->GetUIElementFlag(UIElementFlags::isActive);

			//If UI's name contains _substr, and
			//either the isActive is not specfied, or it matches the requested active status
			if (name.find(_substr) < name.size() && (!_isActive || isActive == *_isActive))
				result.push_back(uid);
		}

		return result;
	}

	std::vector<UIElementID> UICanvas::FindUIElementByName(const std::string& _name, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<UIElementID> result;
		result.reserve(MAX_UI_COUNT);

		for (UIElementID uid : m_allUIElementIDs)
		{
			//Limit the result to this number of UI
			if (result.size() >= count)
				break;

			//Retrieve the name of the UI
			const std::string& name = this->GetUI(uid)->GetName();
			bool isActive = this->GetUI(uid)->GetUIElementFlag(UIElementFlags::isActive);

			//If UI's name is _name, and
			//either the isActive is not specfied, or it matches the requested active status
			if (name == _name && (!_isActive || isActive == *_isActive))
				result.push_back(uid);
		}

		return result;
	}
	
	std::vector<UIElementID> UICanvas::FindUIElementByType(UIElementType _type, size_t count, const std::optional<bool>& _isActive) const
	{
		std::vector<UIElementID> result;
		result.reserve(MAX_UI_COUNT);

		for (UIElementID uid : m_allUIElementIDs)
		{
			//Limit the result to this number of UI
			if (result.size() >= count)
				break;

			//Retrieve the name of the UI
			UIElementType type = this->GetUI(uid)->GetUIType();
			bool isActive = this->GetUI(uid)->GetUIElementFlag(UIElementFlags::isActive);

			//If UI's type matches requested type, and
			//either the isActive is not specfied, or it matches the requested active status
			if (type == _type && (!_isActive || isActive == *_isActive))
				result.push_back(uid);
		}

		return result;
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;

	registration::class_<UICanvas>(RegistrationNameCreator<UICanvas>())
		.constructor<CanvasID>()
		.property("Canvas Name", &UICanvas::m_uiCanvasName)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("IsActive", &UICanvas::GetIsActive, &UICanvas::SetIsActive)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("Sort Layer", &UICanvas::GetSortLayer, &UICanvas::SetSortLayer)
		(
			//Use UI Manager's Set Sort Layer to set this value
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			)
		.property("Offset", &UICanvas::GetUIOffset, &UICanvas::SetUIOffset)
		(
			//Use UI Manager's Set Sort Layer to set this value
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
			);
}