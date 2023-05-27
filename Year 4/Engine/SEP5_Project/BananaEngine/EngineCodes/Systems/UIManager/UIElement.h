#pragma once
#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <Precompiled/pch.h>
#include <Maths/Vector3D.h>
#include <AssetManager/AssetManager.h>
#include "UIScript.h"		//For IScript class
#include "UIManagerConfig.h"

namespace BE
{
	enum class UIElementType
	{
		none = 0,
		button,
		fontText,		//Use free type to render the text
		image,			//Renders an image				
		slider,
		toggle,
		progressBar,
	};

	enum class UIHorizontalAlignment
	{
		left = 0,
		middle,
		right
	};

	//Where will the Y Position be on the UI Element
	enum class UIVerticalAlignment
	{
		top = 0,
		middle,
		bottom
	};

	//UI Flags
	enum class UIElementFlags
	{
		//For all UI
		isActive = 0,		//Whether to update & draw this element?
		isUsingTexture,		//Whether to use texture (true) or coloured tint (false)

		//For Interactable UI such as button
		isHovering,			//Whether mouse is hovering over the UI element
		isPressed,			//Whether the UI element is being pressed
		isDisabled,			//Whether UI is disabled (click does nothing)

		//Used by UI Manager for destroying, do not use
		destroy,
		uiElementFlagLast
	};

	//Base class for all UI Elements
	class UIElement
	{
	protected:
		//What type of UI is this?
		const UIElementType m_type;

		//the canvas and UI ID of this UI Element
		const CanvasID m_canvasID;
		const UIElementID m_uiID;
		
		//UI Flags
		std::bitset<(size_t)UIElementFlags::uiElementFlagLast> m_uiFlags;

		//UI Alignment - Where the UI should be aligned around the given UI Position
		UIHorizontalAlignment m_horiAlign{ UIHorizontalAlignment::middle };
		UIVerticalAlignment m_vertAlign{ UIVerticalAlignment::middle };

		//Sort layer - lower number draw first
		int m_sortLayer{ 0 };

		//Name of the UI Element
		std::string m_uiElementName;

		//Controls the scripting behaviour of this UI Element
		UIScript m_script{};

		//Rect transform of the UI
		Vec3f m_uiPos{0.5f,0.5f,0.5f};			//Where the anchor position of the UI will be, normalise between 0 and 1
		Vec3f m_uiScale{0.5f,0.5f,0.5f};		//At what scale to draw the UI at, normalised between 0 and 1
		//float m_uiRotationRadian{ 0.0f };		//Rotation of the UI element in Radian

		//Read only transform of the UI
		Vec3f m_drawPos{};			//At what normalised coordinate (0 to 1) to draw this UI, factoring alignment
		Vec3f m_screenPos{};		//At what screen coordinate to draw this UI, between 0 and screen size
		Vec3f m_screenScale{};		//At what screen size to draw this UI, between 0 and screen size

		//Donavern - for rendering
		std::vector<VkDescriptorSet> currentSet; //To use to draw
		std::vector<VkWriteDescriptorSet> writes; //to use to update what to draw
		bool hasTexturesChanged{true}; //to know when to update for draw
		float brightnessFactor{ 1.0f }; //using
		float opacity{ 1.0f }; //extra who knows maybe we might need it
		bool hasInitialized{}; //using
		glm::vec4 colorIntensityFactor{1.0f,1.0f,1.0f,1.0f};
		glm::vec4 flickerColor{ 0.0f,0.0f,0.0f,1.0f };
		bool useCustomColorAndOpacity{};
	public:
		//Donavern - For rendering
		std::vector<VkDescriptorSet>& getDescriptorSets();
		std::vector<VkWriteDescriptorSet>& getWrites();
		bool getHasTexturesChanged() const;
		float getBrightnessFactor() const;
		float getOpacity() const;
		bool getHasInitialized() const;
		const glm::vec4& getColorIntensityFactor() const;
		const glm::vec4& getFlickerColor() const;
		bool getUseCustomColor() const;

		void setHasTexturesChanged(bool b);
		void setBrightnessFactor(float f);
		void setOpacity(float f);
		void setHasInitialized(bool b);
		void setColorIntensityFactor(const glm::vec4& v);
		void setFlickerColor(const glm::vec4& v);
		void setUseCustomColor(bool b);

		//Donavern - for rendering
		void InitializeForRendering(BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool,
			VkDescriptorImageInfo& blankImage,BEID textureToUse,
			BEID textureToUse2, BEID textureToUse3);
		void addTexturesToUpdateList(BeDescriptorSetLayout& setLayout, 
			VkDescriptorImageInfo& blankImage,BEID textureToUse,
			BEID textureToUse2, BEID textureToUse3);

		void InitializeForRenderingText(BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool,
			VkDescriptorImageInfo& blankImage, VkDescriptorImageInfo& glyphTexture);

		void updateDescriptorSet(BeDevice& device);

		UIElement(UIElementType _type, CanvasID _cid, UIElementID _uid);
		virtual ~UIElement();

		//Functions to override
		virtual void InitUI() = 0;					//1 time initialisation after scene is loaded
		virtual void UpdateUI(double dt) = 0;		//Called every frame
		virtual void UninitUI() = 0;				//1 time call when scene unloaded

		//UI alignment and sort layer (Set sort layer via the UICanvas!)
		inline int GetSortLayer() const										{ return m_sortLayer; }
		inline UIHorizontalAlignment GetUIHorizontalAlignment() const		{ return m_horiAlign; }
		inline UIVerticalAlignment GetUIVerticalAlignment() const			{ return m_vertAlign; }
		void SetUIHorizontalAlignment(UIHorizontalAlignment _newAlign);
		void SetUIVerticalAlignment(UIVerticalAlignment _newAlign);

		//UI Transform
		inline const Vec3f& GetUIPos() const								{ return m_uiPos; }
		inline const Vec3f& GetUIScale() const								{ return m_uiScale; }
		void SetUIPos(const Vec3f& _newPos);
		void SetUIScale(const Vec3f& _newScale);

		//UI Transform adjusted with alignment, these are read only
		inline const Vec3f& GetUIScreenPos() const							{ return m_screenPos; }
		inline const Vec3f& GetUIScreenScale() const						{ return m_screenScale; }
		inline const Vec3f& GetUIDrawPos() const							{ return m_drawPos; }
		
		//UI Flag and types
		inline UIElementType GetUIType() const								{ return m_type; }
		inline bool GetUIElementFlag(UIElementFlags _f) const				{ return m_uiFlags.test((size_t)_f); }
		inline virtual void SetUIElementFlag(UIElementFlags _f, bool _b)	{ m_uiFlags.set((size_t)_f, _b); }
		inline std::uint32_t GetUIElementFlag_AsUInt() const				{ return (std::uint32_t)m_uiFlags.to_ulong(); }
		void SetUIElementFlag_AsUInt(std::uint32_t _flags);

		//UI Element name
		inline const std::string& GetName() const							{ return m_uiElementName; }
		inline const void SetName(const std::string& _name)					{ m_uiElementName = _name; }

		//Access the behaviour of the UI Element
		inline UIScript& GetUIScript()										{ return m_script; }

		//For serialisation
		virtual void Serialise() = 0;
		virtual void Deserialise() = 0;


	protected:
		//Disabling copy constructor/assignment
		UIElement& operator=(const UIElement&) = delete;
		UIElement(const UIElement&) = delete;

		friend class UICanvas;
		friend class UIManager;

		//Helper function for updating m_drawPos and m_drawScale
		void UpdateReadOnlyTransform();

		//Used by UI Canvas
		inline void SetSortLayer(int _newLayer)							{ m_sortLayer = _newLayer; }

		/*
			RTTR Stuff
		*/
	public:
		RTTR_ENABLE();
		RTTR_REGISTRATION_FRIEND;
	};

	
}

#define UIELEMENT_FUNCTIONS			void InitUI() override;\
									void UpdateUI(double dt) override;\
									void UninitUI() override;\
									void Serialise() override;\
									void Deserialise() override

#endif
