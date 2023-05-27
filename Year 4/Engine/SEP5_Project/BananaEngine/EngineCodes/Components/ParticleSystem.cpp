#include "pch.h"
#include "ParticleSystem.h"
#include "../Systems/Graphics/Be_swap_chain.hpp"
#include "../Systems/Graphics/Be_descriptors.hpp"
#include <Serialisation/BananaSerialisation.h>

//#include "../Be_descriptors.hpp"
namespace BE 
{

	ParticleSystem::ParticleSystem(const ParticleSystem& rhs) noexcept
	{
		pos = rhs.pos;
		color = rhs.color;
		vel = rhs.vel;
		//particleCount = rhs.particleCount;
		particleSize = rhs.particleSize;
		lifetime = rhs.lifetime;
		particles = rhs.particles;
	}

	ParticleSystem::ParticleSystem(ParticleSystem&& rhs) noexcept
	{
		pos = rhs.pos;
		color = rhs.color;
		vel = rhs.vel;
		//particleCount = rhs.particleCount;
		particleSize = rhs.particleSize;
		lifetime = rhs.lifetime;
		particles = rhs.particles;
	}

	ParticleSystem& ParticleSystem::operator=(const ParticleSystem& rhs) noexcept
	{
		pos = rhs.pos;
		color = rhs.color;
		vel = rhs.vel;
		//particleCount = rhs.particleCount;
		particleSize = rhs.particleSize;
		lifetime = rhs.lifetime;
		particles = rhs.particles;
		return *this;
	}

	ParticleSystem& ParticleSystem::operator=(ParticleSystem&& rhs) noexcept
	{
		pos = rhs.pos;
		color = rhs.color;
		vel = rhs.vel;
		//particleCount = rhs.particleCount;
		particleSize = rhs.particleSize;
		lifetime = rhs.lifetime;
		particles = rhs.particles;
		return *this;
	}

	glm::vec4 const& ParticleSystem::getPos() const
	{
		return pos;
	}

	glm::vec4 const& ParticleSystem::getColor() const
	{
		return color;
	}

	float ParticleSystem::getAlpha() const
	{
		return color.w;
	}

	glm::vec4 const& ParticleSystem::getVel() const
	{
		return vel;
	}

	int ParticleSystem::getParticleCount() const
	{
		return static_cast<int>(particles.size());
	}

	float ParticleSystem::getParticleSize() const
	{
		return particleSize;
	}

	float ParticleSystem::getLifeTime() const
	{
		return lifetime;
	}

	bool ParticleSystem::getHasTexturesChanged()
	{
		return hasTexturesChanged;
	}

	void ParticleSystem::setHasTexturesChanged(bool b)
	{
		hasTexturesChanged = b;
	}

	void ParticleSystem::addTexturesToUpdateList(BeDescriptorSetLayout& setLayout, VkDescriptorImageInfo& blankImage, BEID textureToUse)
	{
		for (int i{}; i < currentSet.size(); ++i)
		{
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorType = setLayout.getBindings()[1].descriptorType;
			write.dstBinding = 1;
			if (textureToUse != 0)
				write.pImageInfo = &AssetMgr->GetAssetData<Texture>(textureToUse).getDescriptorImageInfo(); //this
			else
				write.pImageInfo = &blankImage;
			write.descriptorCount = 1;
			write.dstSet = currentSet[i];
			writes.emplace_back(write); //this
		}
	}

	void ParticleSystem::UpdateParticleSystem(float deltaTime, int desiredCount)
	{
		// Particles Update
		std::vector<Particle> newParticles;
		for (auto& particle : particles)
		{
			particle.Update(deltaTime);
			if (!particle.IsDead())
			{
				newParticles.emplace_back(particle);
			}
		}
		particles = newParticles;

		// Remove particles that have exceeded their lifetime
		particles.erase(std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.IsDead(); }), particles.end());

		// Add or remove particles based on desired count
		int currentCount = static_cast<int>(particles.size());
		if (currentCount < desiredCount)
		{
			// Add particles until desired count is reached
			for (int i = 0; i < desiredCount - currentCount; i++)
			{
				particles.push_back({ pos, color, vel, particleSize, lifetime });
			}
		}
		else if (currentCount > desiredCount)
		{
			// Remove particles until desired count is reached
			particles.erase(particles.begin() + desiredCount, particles.end());
		}
	}



	std::vector<ParticleSystem::Particle> ParticleSystem::GetParticles() const
	{
		return particles;
	}

	void ParticleSystem::setPos(glm::vec4 const& v)
	{
		pos = v;
	}

	void ParticleSystem::setColor(glm::vec4 const& v)
	{
		color = v;
	}

	void ParticleSystem::setAlpha(float f)
	{
		color.w = f;
	}

	void ParticleSystem::setVel(glm::vec4 const& v)
	{
		vel = v;
	}

	void ParticleSystem::setParticleCount(int count)
	{
		UpdateParticleSystem(0.0f, count);
	}


	//void ParticleSystem::setParticleCount(float f)
	//{
	//	particleCount = f;
	//}

	void ParticleSystem::setParticleSize(float f)
	{
		particleSize = f;
	}

	inline void ParticleSystem::setLifeTime(float f)
	{
		lifetime = f;
	}

	std::vector<VkDescriptorSet>& ParticleSystem::getDescriptorSets()
	{
		return currentSet;
	}
	std::vector<VkWriteDescriptorSet>& ParticleSystem::getWrites()
	{
		return writes;
	}

	void ParticleSystem::updateDescriptorSet(BeDevice& device)
	{
		vkUpdateDescriptorSets(device.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr); //this
		writes.clear();
	}

	void ParticleSystem::InitializeForRendering(BeDescriptorSetLayout& setLayout, BeDescriptorPool& pool, VkDescriptorImageInfo& blankImage, BEID textureToUs)
	{
		for (int i{}; i < BE::BeSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			currentSet.emplace_back(VK_NULL_HANDLE);
			BE::BeDescriptorWriter(setLayout, pool)
				.writeImage(0, textureToUs ? &AssetMgr->GetAssetData<Texture>(textureToUs).getDescriptorImageInfo() : &blankImage)
				.build(currentSet[i]);
		}
		initialized = true;
	}

	//void ParticleSystem::AddFireParticles(std::vector<BE::ParticleSystem>& particleSystems)
	//{
	//	// Add new particle systems
	//	for (int i = 0; i < 3; i++)
	//	{
	//		BE::ParticleSystem particleSystem;
	//		particleSystem.setLifeTime(1.0f);
	//		particleSystem.setParticleSize(0.2f);
	//		particleSystem.setPos(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	//		// Set initial particle colors to yellow/orange
	//		glm::vec4 color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	//		particleSystem.setColor(color);

	//		// Set initial particle velocities to simulate rising flames
	//		glm::vec4 vel = glm::vec4(0.0f, 0.05f, 0.0f, 0.0f);
	//		particleSystem.setVel(vel);

	//		// Add particle system to the list
	//		particleSystems.push_back(particleSystem);
	//	}

	//	// Update particle systems
	//	for (auto& particleSystem : particleSystems)
	//	{
	//		auto count = particleSystem.getParticleCount();
	//		particleSystem.UpdateParticleSystem(0.016f, count);

	//		// Update particle colors to simulate flickering flames
	//		glm::vec4 color = particleSystem.getColor();
	//		color.x = 1.0f;
	//		color.y = (rand() % 10) / 10.0f;
	//		color.z = 0.0f;
	//		particleSystem.setColor(color);
	//	}
	//}
}

RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	registration::class_<ParticleSystem>(RegistrationNameCreator<ParticleSystem>())
		.constructor()
		.constructor<const ParticleSystem&>()
		.constructor<ParticleSystem&&>()
		.property("Color", &ParticleSystem::getColor, &ParticleSystem::setColor)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Position", &ParticleSystem::getPos, &ParticleSystem::setPos)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Vel", &ParticleSystem::getVel, &ParticleSystem::setVel)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("ParticleSize", &ParticleSystem::getParticleSize, &ParticleSystem::setParticleSize)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("Alpha", &ParticleSystem::getAlpha, &ParticleSystem::setAlpha)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		)
		.property("LifeTime", &ParticleSystem::getLifeTime, &ParticleSystem::setLifeTime)
		(
			metadata(MetaDataKey::Serialize, true),
			metadata(MetaDataKey::Copy, true)
		);
}