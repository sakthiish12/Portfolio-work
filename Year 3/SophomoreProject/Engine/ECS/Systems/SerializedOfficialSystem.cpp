// /******************************************************************************/
// /*!
// \file         SerializedOfficialSystem.cpp 
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the definition of the Transform,Camera,Renderer, Sprite system for the serializedofficialteststate 
// \lines        213 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "SerializedOfficialSystem.h"

#include "../../InputMgr/InputMgr.h"//input 
#include "../../Graphics/Source/FontRendering.h"//font rendering 
#include "../../Graphics/Source/BatchRenderer2D.h"//batch rendering 
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Serializer/Settings/AnimationDataSerializer.h"//animation data 
#include "../../Serializer/SerializeManager.h"//serializer manager 

//externs 
std::shared_ptr<SerializedTransform2DSystem> SerializedTransform2DSys;
std::shared_ptr<SerializedSprite2DSystem> SerializedSprite2DSys;

 

void SerializedTransform2DSystem::update(float dt)
{

	static_cast<void>(dt);//unreferenced variable 
}

void SerializedTransform2DSystem::updateMatrix()
{
	for (auto const& e : getEntitiesRegistered())
	{
		auto& oTransform = ecs.getComponent<transform2D>(e);
		(void)oTransform;
		Matrix3 scale, rot, trans;
		//do the 3 matraix then concatenate and keep in transform2d
		//to revisit here after ensuring math library is working fine.
	}
}



void SerializedSprite2DSystem::update(float dt)
{
	static_cast<void>(dt);//unreferenced variable 

	if (gameStateManager::usingFixedDTTimestep)
	{
		dt = static_cast<float>(gameStateManager::fixedDeltaTime);
		//Loop used in systems that have time-based formula
		for (int step = 0; step < gameStateManager::currentNumberOfSteps; ++step)
		{
			updateSteps(dt);
		}
	}
	else
	{
		updateSteps(dt);
	}
}


void SerializedSprite2DSystem::updateSteps(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	for (auto const& e : getEntitiesRegistered())
	{
		//SerializeManager sm;
		AnimationSerializer as;
		Log logger;
		auto& oSprite = ecs.getComponent<spriteCom>(e);
		//first on load 
		if (oSprite.toLoaded && oSprite.loop == true) {

			//sanity check to make sure animation file path is alreaedy loaded 
			if (AssetManager::GetInstance().animationDataGame.find(oSprite.animationFilePath) != AssetManager::GetInstance().animationDataGame.end()) {
				//if already loaded, copy in the data
				oSprite.animationData.insert(AssetManager::GetInstance().animationDataGame.find(oSprite.animationFilePath)->second.begin(), AssetManager::GetInstance().animationDataGame.find(oSprite.animationFilePath)->second.end());//getting animation data 
				std::copy(AssetManager::GetInstance().frameData.find(oSprite.animationFilePath)->second.begin(), AssetManager::GetInstance().frameData.find(oSprite.animationFilePath)->second.end(), std::back_inserter(oSprite.frameData));//copy vector 
				oSprite.toLoaded = false;
			}


			////first check if there are any existing entities with the same animation data loaded 
			//for (auto& x : ecs.getEntityNames()) {
			//	//if they have the same animation data 
			//	if (ecs.getComponent<spriteCom>(x.first).animationFilePath == oSprite.animationFilePath && !ecs.getComponent<spriteCom>(x.first).toLoaded) {
			//		oSprite.animationData.insert(ecs.getComponent<spriteCom>(x.first).animationData.begin(), ecs.getComponent<spriteCom>(x.first).animationData.end());
			//		std::copy(ecs.getComponent<spriteCom>(x.first).frameData.begin(), ecs.getComponent<spriteCom>(x.first).frameData.end(), std::back_inserter(oSprite.frameData));
			//		oSprite.toLoaded = false;
			//		break;
			//	}
			//}


			////sanity check to make sure the texture exist 
			//if (AssetManager::GetInstance().textureMap.find(oSprite.textureId) != AssetManager::GetInstance().textureMap.end() && oSprite.toLoaded) {
			//	sm.DeserializeFromFile(as.animationResourcePath + oSprite.animationFilePath, as);

			//	//copying data into the component 
			//	oSprite.animationData.insert(as.aniData.begin(), as.aniData.end());
			//	std::copy(std::begin(as.uvFramePoints), std::end(as.uvFramePoints), std::back_inserter(oSprite.frameData));
			//	oSprite.toLoaded = false;
			//}
			//else {
			//	//oSprite.loop = false;
			//}
		}

	}
}