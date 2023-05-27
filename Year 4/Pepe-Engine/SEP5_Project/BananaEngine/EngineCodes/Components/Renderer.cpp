#include "pch.h"
#include "Renderer.h"

#include "../AssetManager/AssetManager.h"
#include "../Systems/Graphics/Be_descriptors.hpp"
#include "../Systems/Graphics/Be_frame_info.hpp"
#include "../Systems/Graphics/Be_swap_chain.hpp"
#include "../Systems/Graphics/Be_buffer.hpp"

#include <Serialisation/BananaSerialisation.h>

#pragma warning ( push )
#pragma warning ( disable: 4456 )
#pragma warning ( disable: 4267 )
#pragma warning ( disable: 4172 )
#pragma warning ( disable: 26812 )
namespace BE
{
	std::vector<VkDescriptorSet>& Renderer::getDescriptorSet(int index)
	{
		return meshInfo[index].currentSet;
	}

	void Renderer::initializeComponent(std::unique_ptr<BE::BeBuffer>& pickBuffer, std::vector<std::unique_ptr<BE::BeBuffer>>& uboBuffer,
		BeDevice& device, BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, VkDescriptorImageInfo& blankImage)
	{
		ownUboBuffer.emplace_back(std::make_unique<BeBuffer>(&device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));
		ownUboBuffer.emplace_back(std::make_unique<BeBuffer>(&device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

		auto pickInfo = pickBuffer->descriptorInfo();

		//Create descriptor set if not created yet
		for (size_t m{}; m < meshInfo.size(); ++m)
		{
			for (size_t i{}; i < meshInfo[m].currentSet.size(); ++i)
			{
				if (meshInfo[m].currentSet[i] == VK_NULL_HANDLE)
				{

					uboBufferInfoDefault = uboBuffer[i]->descriptorInfo();

					std::vector<Texture*> textureToUse;

					for (size_t t{}; t < defaultTextures.size(); ++t)
					{
						if (AssetMgr->GetAssetID(defaultTextures[t]))
						{
							textureToUse.emplace_back(&AssetMgr->GetAssetData<Texture>(defaultTextures[t]));
						}
						else
						{
							textureToUse.emplace_back(nullptr);
						}
					}

					BE::BeDescriptorWriter(setLayout, pool)
						.writeBuffer(0, &uboBufferInfoDefault)
						.writeImage(1, textureToUse[0] ? &textureToUse[0]->getDescriptorImageInfo() : &blankImage)
						.writeImage(2, textureToUse[1] ? &textureToUse[1]->getDescriptorImageInfo() : &blankImage)
						.writeImage(3, textureToUse[2] ? &textureToUse[2]->getDescriptorImageInfo() : &blankImage)
						.writeImage(4, textureToUse[3] ? &textureToUse[3]->getDescriptorImageInfo() : &blankImage)
						.writeImage(5, textureToUse[4] ? &textureToUse[4]->getDescriptorImageInfo() : &blankImage)
						.writeImage(6, textureToUse[5] ? &textureToUse[5]->getDescriptorImageInfo() : &blankImage)
						.writeImage(7, textureToUse[6] ? &textureToUse[6]->getDescriptorImageInfo() : &blankImage)
						.writeBuffer(10, &pickInfo)
						.build(meshInfo[m].currentSet[i]);
				}
			}
		}

		initialized = true;
	}

	void Renderer::updateOwnUboAsItHasAnim(int frameIndex, GlobalUbo& ubo, BeDescriptorSetLayout& setLayout)
	{
		ownUboBuffer[frameIndex]->map();
		ownUboBuffer[frameIndex]->writeToBuffer(&ubo);
		ownUboBuffer[frameIndex]->flush();
		ownUboBuffer[frameIndex]->unmap();

		if (alreadyUsingOwnUbo == false)
		{
			//Update it
			uboBufferInfo1 = ownUboBuffer[0]->descriptorInfo();

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorType = setLayout.getBindings()[0].descriptorType;
			write.dstBinding = 0;
			write.pBufferInfo = &uboBufferInfo1;
			write.descriptorCount = 1;
			for (size_t i{}; i < meshInfo.size(); ++i)
			{
				write.dstSet = meshInfo[i].currentSet[0];
				writes.emplace_back(write);
			}

			uboBufferInfo2 = ownUboBuffer[1]->descriptorInfo();
			write.pBufferInfo = &uboBufferInfo2;
			for (size_t i{}; i < meshInfo.size(); ++i)
			{
				write.dstSet = meshInfo[i].currentSet[1];
				writes.emplace_back(write);
			}

			alreadyUsingOwnUbo = true;
		}
	}

	void Renderer::addTexturesToUpdateList(int meshIndex, BeDevice& device, BeDescriptorSetLayout& setLayout, VkDescriptorImageInfo& blankImage, bool assetReloaded)
	{
		if (hasTexturesChanged[meshIndex] || assetReloaded)
		{
			int xx{};
			for (BEID id : getTextureSet(meshIndex))
			{
				++xx;
				for (int i{}; i < BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
				{
					VkWriteDescriptorSet write{};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.descriptorType = setLayout.getBindings()[xx].descriptorType;
					write.dstBinding = xx;
					if (id != 0)
						write.pImageInfo = &AssetMgr->GetAssetData<Texture>(id).getDescriptorImageInfo(); //this
					else
						write.pImageInfo = &blankImage;
					write.descriptorCount = 1;
					write.dstSet = meshInfo[meshIndex].currentSet[i];
					writes.emplace_back(write); //this
				}
			}
			hasTexturesChanged[meshIndex] = false;
		}
	}

	void Renderer::updateDescriptorSet(BeDevice& device)
	{
		if (writes.size())
		{
			vkUpdateDescriptorSets(device.device(), writes.size(), writes.data(), 0, nullptr); //this
			writes.clear();
		}
	}

	void Renderer::resetToDefaultTextures()
	{
		BEID IDs[7] = { AssetMgr->GetAssetID(defaultTextures[0]),AssetMgr->GetAssetID(defaultTextures[1]) ,AssetMgr->GetAssetID(defaultTextures[2])
		,AssetMgr->GetAssetID(defaultTextures[3]) ,AssetMgr->GetAssetID(defaultTextures[4]) ,AssetMgr->GetAssetID(defaultTextures[5]),
		AssetMgr->GetAssetID(defaultTextures[6]) };

		if (IDs[0] && IDs[1] && IDs[2] && IDs[3] && IDs[4] && IDs[5] && IDs[6])
		{
			for (int i{}; i < meshInfo.size(); ++i)
			{
				setAlbedoAlphaMap(IDs[0], i);
				setNormalMap(IDs[1], i);
				setAmbientOcclusionMap(IDs[2], i);
				setRoughnessMap(IDs[3], i);
				setEmissiveMap(IDs[4], i);
				setMetallicMap(IDs[5], i);
				setHeightMap(IDs[6], i);
			}
		}
	}

	void Renderer::flicker()
	{
		shouldFlicker = true;
	}

	BEID Renderer::getModelID()
	{
		return modelID;
	}

	std::vector<BEID> Renderer::getTextureSet(int index)
	{
		std::vector<BEID> newSet(maxTextureMaps, BEID(0));
		newSet[0] = (meshInfo[index].albedoAlphaMap);
		newSet[1] = (meshInfo[index].normalMap);
		newSet[2] = (meshInfo[index].ambientOcclusionMap);
		newSet[3] = (meshInfo[index].roughnessMap);
		newSet[4] = (meshInfo[index].emissiveMap);
		newSet[5] = (meshInfo[index].metallicMap);
		newSet[6] = meshInfo[index].heightMap;

		return newSet;
	}

	std::string const& Renderer::getModelname()
	{
		return AssetMgr->GetAssetFileName(modelID);
	}

	std::vector<std::string> Renderer::getTextureSetName(int index)
	{
		std::vector<std::string> newSet(maxTextureMaps, "");

		if (meshInfo[index].albedoAlphaMap != 0)
		{
			newSet[0] = AssetMgr->GetAssetFileName(meshInfo[index].albedoAlphaMap);
		}
		if (meshInfo[index].normalMap != 0)
		{
			newSet[1] = AssetMgr->GetAssetFileName(meshInfo[index].normalMap);
		}
		if (meshInfo[index].ambientOcclusionMap != 0)
		{
			newSet[2] = AssetMgr->GetAssetFileName(meshInfo[index].ambientOcclusionMap);
		}
		if (meshInfo[index].roughnessMap != 0)
		{
			newSet[3] = AssetMgr->GetAssetFileName(meshInfo[index].roughnessMap);
		}
		if (meshInfo[index].emissiveMap != 0)
		{
			newSet[4] = AssetMgr->GetAssetFileName(meshInfo[index].emissiveMap);
		}
		if (meshInfo[index].metallicMap != 0)
		{
			newSet[5] = AssetMgr->GetAssetFileName(meshInfo[index].metallicMap);
		}
		if (meshInfo[index].heightMap != 0)
		{
			newSet[6] = AssetMgr->GetAssetFileName(meshInfo[index].heightMap);
		}

		return newSet;
	}

	void Renderer::setModelID(BEID id)
	{
		modelID = id;
		const auto& meshesRef = AssetMgr->GetAssetData<BeModel>(modelID).getMeshes();
		meshInfo.resize(meshesRef.size());
		hasTexturesChanged.resize(meshInfo.size());

		for (int i{}; i < hasTexturesChanged.size(); ++i)
		{
			hasTexturesChanged[i] = true;
		}

		for (int i{}; i < meshInfo.size(); ++i)
		{
			meshInfo[i].meshName = meshesRef[i].meshName;
			meshInfo[i].textureStrength.resize(maxTextureMaps - 1);
			meshInfo[i].currentSet.resize(BeSwapChain::MAX_FRAMES_IN_FLIGHT);

			for (int ts{}; ts < meshInfo[i].textureStrength.size(); ++ts)
			{
				meshInfo[i].textureStrength[ts] = 1.0f;
			}

			for (int cs{}; cs < meshInfo[i].currentSet.size(); ++cs)
			{
				meshInfo[i].currentSet[cs] = VK_NULL_HANDLE;
			}
			meshInfo[i].albedoAlphaMap = AssetMgr->GetAssetID(defaultTextures[0]);
			meshInfo[i].normalMap = AssetMgr->GetAssetID(defaultTextures[1]);
			meshInfo[i].ambientOcclusionMap = AssetMgr->GetAssetID(defaultTextures[2]);
			meshInfo[i].roughnessMap = AssetMgr->GetAssetID(defaultTextures[3]);
			meshInfo[i].emissiveMap = AssetMgr->GetAssetID(defaultTextures[4]);
			meshInfo[i].metallicMap = AssetMgr->GetAssetID(defaultTextures[5]);
			meshInfo[i].heightMap = AssetMgr->GetAssetID(defaultTextures[6]);

			meshInfo[i].hasCustomAlbedoMap = false;
			meshInfo[i].hasCustomNormalMap = false;
			meshInfo[i].hasCustomAmbientOcclusionMap = false;
			meshInfo[i].hasCustomRoughnessMap = false;
			meshInfo[i].hasCustomEmissiveMap = false;
			meshInfo[i].hasCustomMetallicMap = false;
			meshInfo[i].hasCustomHeightMap = false;
		}
		initialized = false;
		alreadyUsingOwnUbo = false;
		checkedForCustomMaps = false;
	}

	void Renderer::setAlbedoAlphaMap(BEID id, int index)
	{
		meshInfo[index].albedoAlphaMap = id;
		hasTexturesChanged[index] = true;
	}

	void Renderer::setNormalMap(BEID id, int index)
	{
		meshInfo[index].normalMap = id;
		hasTexturesChanged[index] = true;
	}

	void Renderer::setAmbientOcclusionMap(BEID id, int index)
	{
		meshInfo[index].ambientOcclusionMap = id;
		hasTexturesChanged[index] = true;
	}

	void Renderer::setRoughnessMap(BEID id, int index)
	{
		meshInfo[index].roughnessMap = id;
		hasTexturesChanged[index] = true;
	}

	void Renderer::setEmissiveMap(BEID id, int index)
	{
		meshInfo[index].emissiveMap = id;
		hasTexturesChanged[index] = true;
	}

	void Renderer::setMetallicMap(BEID id, int index)
	{
		meshInfo[index].metallicMap = id;
		hasTexturesChanged[index] = true;
	}

	inline void Renderer::setHeightMap(BEID id, int index)
	{
		meshInfo[index].heightMap = id;
		hasTexturesChanged[index] = true;
	}

	void Renderer::setBrightnessFactor(float f, int index)
	{
		meshInfo[index].brightnessFactor = f;
		if (meshInfo[index].brightnessFactor < 0.0f)
		{
			meshInfo[index].brightnessFactor = 0.0f;
		}
	}

	void Renderer::setModelIDwithName(std::string const& name)
	{
		modelID = AssetMgr->GetAssetID(name);
		const auto& meshesRef = AssetMgr->GetAssetData<BeModel>(modelID).getMeshes();
		meshInfo.resize(meshesRef.size());
		hasTexturesChanged.resize(meshInfo.size());

		for (int i{}; i < hasTexturesChanged.size(); ++i)
		{
			hasTexturesChanged[i] = true;
		}

		for (int i{}; i < meshInfo.size(); ++i)
		{
			meshInfo[i].meshName = meshesRef[i].meshName;
			meshInfo[i].textureStrength.resize(maxTextureMaps - 1);
			meshInfo[i].currentSet.resize(BeSwapChain::MAX_FRAMES_IN_FLIGHT);

			for (int ts{}; ts < meshInfo[i].textureStrength.size(); ++ts)
			{
				meshInfo[i].textureStrength[ts] = 1.0f;
			}

			for (int cs{}; cs < meshInfo[i].currentSet.size(); ++cs)
			{
				meshInfo[i].currentSet[cs] = VK_NULL_HANDLE;
			}
			meshInfo[i].albedoAlphaMap = AssetMgr->GetAssetID(defaultTextures[0]);
			meshInfo[i].normalMap = AssetMgr->GetAssetID(defaultTextures[1]);
			meshInfo[i].ambientOcclusionMap = AssetMgr->GetAssetID(defaultTextures[2]);
			meshInfo[i].roughnessMap = AssetMgr->GetAssetID(defaultTextures[3]);
			meshInfo[i].emissiveMap = AssetMgr->GetAssetID(defaultTextures[4]);
			meshInfo[i].metallicMap = AssetMgr->GetAssetID(defaultTextures[5]);
			meshInfo[i].heightMap = AssetMgr->GetAssetID(defaultTextures[6]);


			meshInfo[i].hasCustomAlbedoMap = false;
			meshInfo[i].hasCustomNormalMap = false;
			meshInfo[i].hasCustomAmbientOcclusionMap = false;
			meshInfo[i].hasCustomRoughnessMap = false;
			meshInfo[i].hasCustomEmissiveMap = false;
			meshInfo[i].hasCustomMetallicMap = false;
			meshInfo[i].hasCustomHeightMap = false;
		}
		initialized = false;
		alreadyUsingOwnUbo = false;
		checkedForCustomMaps = false;
	}

	void Renderer::setAlbedoAlphaMapwithName(std::string const& name, int index)
	{
		meshInfo[index].albedoAlphaMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[0])
		{
			meshInfo[index].hasCustomAlbedoMap = true;
		}
		else
		{
			meshInfo[index].hasCustomAlbedoMap = false;
		}
	}

	void Renderer::setNormalMapwithName(std::string const& name, int index)
	{
		meshInfo[index].normalMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[1])
		{
			meshInfo[index].hasCustomNormalMap = true;
		}
		else
		{
			meshInfo[index].hasCustomNormalMap = false;
		}
	}

	void Renderer::setAmbientOcclusionMapwithName(std::string const& name, int index)
	{
		meshInfo[index].ambientOcclusionMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[2])
		{
			meshInfo[index].hasCustomAmbientOcclusionMap = true;
		}
		else
		{
			meshInfo[index].hasCustomAmbientOcclusionMap = false;
		}
	}

	void Renderer::setRoughnessMapwithName(std::string const& name, int index)
	{
		meshInfo[index].roughnessMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[3])
		{
			meshInfo[index].hasCustomRoughnessMap = true;
		}
		else
		{
			meshInfo[index].hasCustomRoughnessMap = false;
		}
	}

	void Renderer::setEmissiveMapwithName(std::string const& name, int index)
	{
		meshInfo[index].emissiveMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[4])
		{
			meshInfo[index].hasCustomEmissiveMap = true;
		}
		else
		{
			meshInfo[index].hasCustomEmissiveMap = false;
		}
	}

	void Renderer::setMetallicMapwithName(std::string const& name, int index)
	{
		meshInfo[index].metallicMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[5])
		{
			meshInfo[index].hasCustomMetallicMap = true;
		}
		else
		{
			meshInfo[index].hasCustomMetallicMap = false;
		}
	}

	void Renderer::setHeightMapwithName(std::string const& name, int index)
	{
		meshInfo[index].heightMap = AssetMgr->GetAssetID(name);
		hasTexturesChanged[index] = true;

		if (name != defaultTextures[6])
		{
			meshInfo[index].hasCustomHeightMap = true;
		}
		else
		{
			meshInfo[index].hasCustomHeightMap = false;
		}
	}


	std::vector<float>& Renderer::getTextureStrength(int index)
	{
		return meshInfo[index].textureStrength;
	}

	void Renderer::setTextureStrength(float value, int meshIndex, int textureIndex)
	{
		meshInfo[meshIndex].textureStrength[textureIndex] = value;
	}

	bool Renderer::getInitialized()
	{
		return initialized;
	}

	void Renderer::setInitialized(bool b)
	{
		initialized = b;
	}

	bool Renderer::getHasTexturesChanged(int index)
	{
		return hasTexturesChanged[index];
	}

	void Renderer::setHasTexturesChanged(bool b, int index)
	{
		hasTexturesChanged[index] = b;
	}

	std::vector<MeshInfo>& Renderer::getMeshInfo()
	{
		return meshInfo;
	}

	bool Renderer::getHasTextures(int index)
	{
		return meshInfo[index].hasTextures;
	}

	void Renderer::setHasTextures(bool b, int index)
	{
		meshInfo[index].hasTextures = b;
	}

	float Renderer::getOpacity()
	{
		return opacity;
	}

	float Renderer::getBrightnessFactor(int index)
	{
		return meshInfo[index].brightnessFactor;
	}

	glm::vec4 const& Renderer::getFlickerColor()
	{
		return flickerColor;
	}

	void Renderer::setFlickerColor(glm::vec4 const& v)
	{
		flickerColor = v;
	}

	bool Renderer::getShouldFlicker()
	{
		return shouldFlicker;
	}

	void Renderer::setShouldFlicker(bool b)
	{
		shouldFlicker = b;
	}

	float Renderer::getTimer()
	{
		return timer;
	}

	void Renderer::setTimer(float f)
	{
		timer = f;
	}

	float Renderer::getFlickerDuration()
	{
		return flickerDuration;
	}

	void Renderer::setFlickerDuration(float f)
	{
		flickerDuration = f;
		if (flickerDuration < 0.0f)
		{
			flickerDuration = 0.0f;
		}
	}

	bool& Renderer::getWireframeModeRef()
	{
		return wireframeMode;
	}

	bool Renderer::getWireframeMode()
	{
		return wireframeMode;
	}

	void Renderer::setWireframeMode(bool b)
	{
		wireframeMode = b;
	}

	glm::vec4& Renderer::getWireframeColorRef()
	{
		return wireframeColor;
	}

	glm::vec4 Renderer::getWireframeColor()
	{
		return wireframeColor;
	}

	void Renderer::setWireframeColor(glm::vec4& v)
	{
		wireframeColor = v;
	}

	bool Renderer::getAlreadyUsingOwnUbo()
	{
		return alreadyUsingOwnUbo;
	}

	void Renderer::setAlreadyUsingOwnUbo(bool b)
	{
		alreadyUsingOwnUbo = b;
	}

	bool Renderer::getIgnoreLighting()
	{
		return ignoreLighting;
	}

	void Renderer::setIgnoreLighting(bool b)
	{
		ignoreLighting = b;
	}

	bool Renderer::getReflectSpecular()
	{
		return reflectSpecular;
	}

	void Renderer::setReflectSpecular(bool b)
	{
		reflectSpecular = b;
	}

	bool Renderer::getMeshCustomAlbedoStatus(int i)
	{
		return meshInfo[i].hasCustomAlbedoMap;
	}

	bool Renderer::getMeshCustomNormalStatus(int i)
	{
		return meshInfo[i].hasCustomNormalMap;
	}

	bool Renderer::getMeshCustomAmbientOcclusionStatus(int i)
	{
		return meshInfo[i].hasCustomAmbientOcclusionMap;
	}

	bool Renderer::getMeshCustomRoughnessStatus(int i)
	{
		return meshInfo[i].hasCustomRoughnessMap;
	}

	bool Renderer::getMeshCustomEmissiveStatus(int i)
	{
		return meshInfo[i].hasCustomEmissiveMap;
	}

	bool Renderer::getMeshCustomMetallicStatus(int i)
	{
		return meshInfo[i].hasCustomMetallicMap;
	}

	bool Renderer::getMeshCustomHeightStatus(int i)
	{
		return meshInfo[i].hasCustomHeightMap;;
	}

	bool Renderer::getCheckedForCustomMaps()
	{
		return checkedForCustomMaps;
	}

	void Renderer::setCheckedForCustomMaps(bool b)
	{
		checkedForCustomMaps = b;
	}

	void Renderer::checkForCustomMaps()
	{
		for (int i{}; i < meshInfo.size(); ++i)
		{
			meshInfo[i].refreshStatus();
		}
		checkedForCustomMaps = true;
	}

	void Renderer::release(BeDevice* device,BeDescriptorPool& pool)
	{
		for (size_t m{}; m < meshInfo.size(); ++m)
		{
			for (size_t i{}; i < meshInfo[m].currentSet.size(); ++i)
			{
				vkFreeDescriptorSets(device->device(), *pool.getDescriptorPool(), 1, &meshInfo[m].currentSet[i]);
				meshInfo[m].currentSet[i] = VK_NULL_HANDLE;
			}
		}
	}

	void MeshInfo::refreshStatus()
	{
		if (AssetMgr->GetAssetID(defaultTextures[0]) != albedoAlphaMap)
		{
			hasCustomAlbedoMap = true;
		}
		if (AssetMgr->GetAssetID(defaultTextures[1]) != normalMap)
		{
			hasCustomNormalMap = true;
		}
		if (AssetMgr->GetAssetID(defaultTextures[2]) != ambientOcclusionMap)
		{
			hasCustomAmbientOcclusionMap = true;
		}
		if (AssetMgr->GetAssetID(defaultTextures[3]) != roughnessMap)
		{
			hasCustomRoughnessMap = true;
		}
		if (AssetMgr->GetAssetID(defaultTextures[4]) != emissiveMap)
		{
			hasCustomEmissiveMap = true;
		}
		if (AssetMgr->GetAssetID(defaultTextures[5]) != metallicMap)
		{
			hasCustomMetallicMap = true;
		}
		if (AssetMgr->GetAssetID(defaultTextures[6]) != heightMap)
		{
			hasCustomHeightMap = true;
		}
	}

	void Renderer::setOpacity(float f)
	{
		opacity = f;
		if (opacity < 0.0f)
		{
			opacity = 0.0f;
		}
	}
}
#pragma warning ( pop )

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<MeshInfo>(BE::RegistrationNameCreator<MeshInfo>())
		.constructor()()
		.property("meshName", &MeshInfo::meshName)
		.property("hasTextures", &MeshInfo::hasTextures)
		.property("albedoAlphaMap", &MeshInfo::albedoAlphaMap)
		.property("normalMap", &MeshInfo::normalMap)
		.property("ambientOcclusionMap", &MeshInfo::ambientOcclusionMap)
		.property("roughnessMap", &MeshInfo::roughnessMap)
		.property("emissiveMap", &MeshInfo::emissiveMap)
		.property("metallicMap", &MeshInfo::metallicMap)
		.property("heightMap", &MeshInfo::heightMap)
		.property("textureStrength", &MeshInfo::textureStrength)
		.property("brightnessFactor", &MeshInfo::brightnessFactor);
	registration::class_<glm::f32vec4>(BE::RegistrationNameCreator<glm::f32vec4>())
		.constructor()()
		.property("x", &glm::f32vec4::x)
		.property("y", &glm::f32vec4::y)
		.property("z", &glm::f32vec4::z)
		.property("w", &glm::f32vec4::w);
	registration::class_<Renderer>(BE::RegistrationNameCreator<Renderer>())
		.constructor()()
		.property("ModelID", &Renderer::getModelID, &Renderer::setModelID)
		.property("MeshInfoVector", &Renderer::meshInfo)
		.property("opacity", &Renderer::getOpacity, &Renderer::setOpacity)
		.property("flickerDuration", &Renderer::getFlickerDuration, &Renderer::setFlickerDuration)
		.property("flickerColor", &Renderer::getFlickerColor, &Renderer::setFlickerColor)
		.property("wireframeMode", &Renderer::getWireframeMode, &Renderer::setWireframeMode)
		.property("wireframeColor", &Renderer::getWireframeColorRef, &Renderer::setWireframeColor)
		.property("ignoreLighting", &Renderer::getIgnoreLighting, &Renderer::setIgnoreLighting)
		.property("reflectSpecular", &Renderer::getReflectSpecular, &Renderer::setReflectSpecular);


	//.property("hasInitialized", &Renderer::getInitialized,&Renderer::setInitialized)
	//.property("hasTexturesChanged", &Renderer::getHasTexturesChanged,&Renderer::setHasTexturesChanged);
}