#include "ObjectivesController.h"

namespace TH
{
	void ObjectivesController::Init()
	{
		Assert(m_objectivesTopLeftAssets.size() == m_objectivesMiddleAssets.size(),
			"Number of middle obj UI must match the number of top left obj UI");

		//Reference to the UI
		m_objTopLeft_UIID = BE::UI->FindUIElementByName("ObjectiveTopLeft", 1).front();
		m_objMiddle_UIID = BE::UI->FindUIElementByName("ObjectiveMiddle", 1).front();

		bigObjShowTimeRemaining = 0.0f;
	}

	void ObjectivesController::Update(double dt)
	{
		//If no objectives is being shown then don't need to do anything
		if (!IsObjectivesShowing())
			return;

		//Reduce the time remaining to show the big objective
		if (bigObjShowTimeRemaining < bigObjShowTime)
		{
			bigObjShowTimeRemaining += (float)dt;

			//Reached the time needed to show the big obj, set the texture to 0
			if (bigObjShowTimeRemaining >= bigObjShowTime)
			{
				std::shared_ptr<BE::UIImage> middleObj = BE::UI->GetCanvas(m_objMiddle_UIID.first)->GetUI<BE::UIImage>(m_objMiddle_UIID.second);
				middleObj->SetTexture(BE::BEID(0));
			}
		}
	}

	//Starting the dialogue at the given index
	void ObjectivesController::StartObjectives(size_t _objSet)
	{
		Assert(_objSet < m_objectivesTopLeftAssets.size(), "Objective %llu does not exist", _objSet);
		
		//Set the UI and time remaining
		m_currObj = _objSet;
		bigObjShowTimeRemaining = 0.0f;

		//Change the UI Texture
		std::shared_ptr<BE::UIImage> topLeftObj = BE::UI->GetCanvas(m_objTopLeft_UIID.first)->GetUI<BE::UIImage>(m_objTopLeft_UIID.second);
		std::shared_ptr<BE::UIImage> middleObj  = BE::UI->GetCanvas(m_objMiddle_UIID.first)-> GetUI<BE::UIImage>(m_objMiddle_UIID.second);
		
		topLeftObj->SetTexture(m_objectivesTopLeftAssets[_objSet]);
		middleObj ->SetTexture(m_objectivesMiddleAssets [_objSet]);
	}

	//Whether any dialogue to run
	bool ObjectivesController::IsObjectivesShowing() const
	{
		return m_currObj < m_objectivesMiddleAssets.size();
	}

	//Clear the current objectives
	void ObjectivesController::ClearObjectives()
	{
		m_currObj = std::numeric_limits<size_t>::max();
		bigObjShowTimeRemaining = 0.0f;

		//Set texture back to 0
		std::shared_ptr<BE::UIImage> topLeftObj = BE::UI->GetCanvas(m_objTopLeft_UIID.first)->GetUI<BE::UIImage>(m_objTopLeft_UIID.second);
		std::shared_ptr<BE::UIImage> middleObj = BE::UI->GetCanvas(m_objMiddle_UIID.first)->GetUI<BE::UIImage>(m_objMiddle_UIID.second);

		topLeftObj->SetTexture(BE::BEID(0));
		middleObj-> SetTexture(BE::BEID(0));
	}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<ObjectivesController>(RegistrationNameCreator<ObjectivesController>())
		.constructor()
		.property("ObjTopLeftAssets", &ObjectivesController::m_objectivesTopLeftAssets)
		.property("ObjMiddleAssets", &ObjectivesController::m_objectivesMiddleAssets);
}