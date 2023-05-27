#pragma once
#ifndef UIIMAGE_H
#define UIIMAGE_H
#include "../UIElement.h"

namespace BE
{
	class UIImage : public UIElement
	{
		AssetID m_imageID{ BEID{0} };
		float m_uiRotationRadian{0.0f};

	public:
		UIImage(CanvasID _cid, UIElementID _uid);
		~UIImage();

		UIELEMENT_FUNCTIONS;

		//Get the texture ID for the image
		inline AssetID GetTexture() const		{ return m_imageID; }

		//Set the texture ID for the image, set _aID to 0 to remove image
		void SetTexture(AssetID _aID);

		//Check whether a texture is loaded for this image
		inline bool HasTexture() const			{ return m_imageID != NULL_ASSET; }

		//Rotation for this UI
		inline float GetUIRot() const			{ return m_uiRotationRadian; }
		void SetUIRotDeg(float _newRotation);
		void SetUIRotRad(float _newRotation);
	
		/*
			RTTR Stuff
		*/
		RTTR_ENABLE(UIElement);
	};
}
#endif