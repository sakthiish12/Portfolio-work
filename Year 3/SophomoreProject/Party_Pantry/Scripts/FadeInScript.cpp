// /******************************************************************************/
// /*!
// \file         FadeInScript.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         8th Dec, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the script of the fading in of the scene 
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "../../Engine/ECS/component.h"
#include "../../Engine/ECS/ECS.h"
#include "../../Engine/Graphics/Source/PostProcessing.h"//post processing 
#include "FadeInScript.h"//fade in and out script 

FadeInScript::FadeInScript(const char* scriptName) :
	scriptBase(scriptName)
{

}

void FadeInScript::init(float dt, unsigned int idToBeSet)
{
	UNREFERENCED_PARAMETER(dt);//unref
	id = idToBeSet;
	state = scriptBase::ScriptState::UPDATE;
	PostProcessing::GetInstance().m_AlphaValue = 0.f;//start from total black 
}

void FadeInScript::update(float dt)
{
	//if it's activated 
	if (active == true) {
		//fade out 
		if (fadeInOut == false) {
			//decrease the alpha of the texture 
			PostProcessing::GetInstance().m_AlphaValue <= 0.f ? PostProcessing::GetInstance().m_AlphaValue = 0.f : PostProcessing::GetInstance().m_AlphaValue -= speedOfTransition * dt;
			if (PostProcessing::GetInstance().m_AlphaValue <= 0.0f) active = false;//reset 
		}
		else {
			//fade in
			PostProcessing::GetInstance().m_AlphaValue >= 1.f ? PostProcessing::GetInstance().m_AlphaValue = 1.f : PostProcessing::GetInstance().m_AlphaValue += speedOfTransition * dt;
			if (PostProcessing::GetInstance().m_AlphaValue >= 1.0f) active = false;//reset 
		}
	}
}

void FadeInScript::end(float dt)
{
	UNREFERENCED_PARAMETER(dt);//unref
}

std::shared_ptr<scriptBase> FadeInScript::createCopy()
{
	return std::make_shared <FadeInScript>();
}

std::string FadeInScript::getScriptName()
{
	return name;
}






