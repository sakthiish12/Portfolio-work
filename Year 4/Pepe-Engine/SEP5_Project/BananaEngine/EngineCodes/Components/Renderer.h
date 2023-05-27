#pragma once
#ifndef	RENDERER_H
#define RENDERER_H

#include "ECSComponent.h"
#include <Systems/Graphics/Be_buffer.hpp>

namespace BE
{
	constexpr const int maxTextureMaps = 7;

	const std::vector<std::string> defaultTextures{ "../Assets/Textures/default/default_albedo.dds" ,"../Assets/Textures/default/default_normal.dds" ,"../Assets/Textures/default/default_ao.dds"
	,"../Assets/Textures/default/default_roughness.dds" ,"../Assets/Textures/default/default_emissive.dds" ,"../Assets/Textures/default/default_metallic.dds",
	"../Assets/Textures/default/default_height.dds" };

	struct MeshInfo
	{
		std::string meshName{};
		bool hasTextures{};
		BEID albedoAlphaMap{ BEID(0) };
		BEID normalMap{ BEID(0) };
		BEID ambientOcclusionMap{ BEID(0) };
		BEID roughnessMap{ BEID(0) };
		BEID emissiveMap{ BEID(0) };
		BEID metallicMap{ BEID(0) };
		BEID heightMap{ BEID(0) };
		float brightnessFactor{ 1.0f };

		std::vector<float> textureStrength;
		std::vector<VkDescriptorSet> currentSet;

		bool hasCustomAlbedoMap{}, hasCustomNormalMap{}, hasCustomAmbientOcclusionMap{}, hasCustomRoughnessMap{}, hasCustomEmissiveMap{}, hasCustomMetallicMap{},
			hasCustomHeightMap{};
		void refreshStatus();
	};

	class Renderer : public IECSComponent
	{
		BEID modelID = BEID{ 0 };
		bool initialized{};
		std::vector<MeshInfo> meshInfo;
		std::vector<VkWriteDescriptorSet> writes;
		std::vector<bool> hasTexturesChanged;
		VkDescriptorBufferInfo uboBufferInfoDefault, uboBufferInfo1, uboBufferInfo2, pickBufferInfo;
		std::vector<std::unique_ptr<BE::BeBuffer>> ownUboBuffer;

		float opacity{ 1.0f };
		bool shouldFlicker{};
		glm::vec4 flickerColor{ 1.0f,1.0f,1.0f,1.0f }; //rttr
		float timer{};
		float flickerDuration{ 0.1f }; //rttr

		bool wireframeMode{};
		glm::vec4 wireframeColor{ 1.0f,1.0f,1.0f,1.0f };

		bool alreadyUsingOwnUbo{};
		bool ignoreLighting{};
		bool reflectSpecular{ false };
		bool checkedForCustomMaps{};
	public:
		Renderer() = default;
		~Renderer() = default;
		Renderer(const Renderer& rhs) noexcept = default;
		Renderer(Renderer&& rhs) noexcept = default;
		Renderer& operator=(const Renderer& rhs) noexcept = default;
		Renderer& operator=(Renderer&& rhs) noexcept = default;

		std::vector<VkDescriptorSet>& getDescriptorSet(int index);
		void initializeComponent(std::unique_ptr<BE::BeBuffer>& pickBuffer, std::vector<std::unique_ptr<BE::BeBuffer>>& uboBuffer,
			BeDevice& device, BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, VkDescriptorImageInfo& blankImage);
		void updateOwnUboAsItHasAnim(int frameIndex, GlobalUbo& ubo, BeDescriptorSetLayout& setLayout);
		void addTexturesToUpdateList(int meshIndex, BeDevice& device, BeDescriptorSetLayout& setLayout, VkDescriptorImageInfo& blankImage, bool assetReloaded);
		void updateDescriptorSet(BeDevice& device);
		void resetToDefaultTextures();
		void flicker();

		//getting id
		inline BEID getModelID();
		inline std::vector<BEID>  getTextureSet(int index);

		//getter string
		std::string const& getModelname();
		std::vector<std::string> getTextureSetName(int index);

		//Setter id
		inline void setModelID(BEID id);
		void setAlbedoAlphaMap(BEID id, int index);
		inline void setNormalMap(BEID id, int index);
		inline void setAmbientOcclusionMap(BEID id, int index);
		inline void setRoughnessMap(BEID id, int index);
		inline void setEmissiveMap(BEID id, int index);
		inline void setMetallicMap(BEID id, int index);
		inline void setHeightMap(BEID id, int index);
		void setBrightnessFactor(float f, int index);

		//setter string
		void setModelIDwithName(std::string const& name);
		void setAlbedoAlphaMapwithName(std::string const& name, int index);
		void setNormalMapwithName(std::string const& name, int index);
		void setAmbientOcclusionMapwithName(std::string const& name, int index);
		void setRoughnessMapwithName(std::string const& name, int index);
		void setEmissiveMapwithName(std::string const& name, int index);
		void setMetallicMapwithName(std::string const& name, int index);
		void setHeightMapwithName(std::string const& name, int index);

		//getter misc
		std::vector<float>& getTextureStrength(int index);

		//setter misc
		void setTextureStrength(float value, int meshIndex, int textureIndex);

		bool getInitialized();
		inline void setInitialized(bool b);

		inline bool getHasTexturesChanged(int index);
		inline void setHasTexturesChanged(bool b, int index);

		std::vector<MeshInfo>& getMeshInfo();
		inline bool getHasTextures(int index);
		inline void setHasTextures(bool, int index);

		inline float getOpacity();
		inline void setOpacity(float f);

		float getBrightnessFactor(int index);

		glm::vec4 const& getFlickerColor();
		void setFlickerColor(glm::vec4 const& v);

		bool getShouldFlicker();
		void setShouldFlicker(bool b);

		float getTimer();
		void setTimer(float f);

		float getFlickerDuration();
		void setFlickerDuration(float f);

		bool& getWireframeModeRef();
		bool getWireframeMode();
		void setWireframeMode(bool b);

		glm::vec4& getWireframeColorRef();
		glm::vec4 getWireframeColor();
		void setWireframeColor(glm::vec4&);

		bool getAlreadyUsingOwnUbo();
		void setAlreadyUsingOwnUbo(bool b);

		bool getIgnoreLighting();
		void setIgnoreLighting(bool b);

		bool getReflectSpecular();
		void setReflectSpecular(bool b);

		bool getMeshCustomAlbedoStatus(int i);
		bool getMeshCustomNormalStatus(int i);
		bool getMeshCustomAmbientOcclusionStatus(int i);
		bool getMeshCustomRoughnessStatus(int i);
		bool getMeshCustomEmissiveStatus(int i);
		bool getMeshCustomMetallicStatus(int i);
		bool getMeshCustomHeightStatus(int i);

		bool getCheckedForCustomMaps();
		void setCheckedForCustomMaps(bool b);
		void checkForCustomMaps();

		void release(BeDevice* device, BeDescriptorPool& pool);
		/*
			Serialisation
		*/
		RTTR_ENABLE(IECSComponent)
			RTTR_REGISTRATION_FRIEND
	};
}


#endif