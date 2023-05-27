#pragma once

#include "Be_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace BE {

class BeDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(BeDevice & beDevice) : beDevice{ beDevice } {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<BeDescriptorSetLayout> build() const;

   private:
    BeDevice & beDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  BeDescriptorSetLayout(
      BeDevice &beDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~BeDescriptorSetLayout();
  BeDescriptorSetLayout(const BeDescriptorSetLayout &) = delete;
  BeDescriptorSetLayout &operator=(const BeDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& getBindings();
 private:
  BeDevice & beDevice;
  VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class BeDescriptorWriter;
};

class BeDescriptorPool {
 public:
  class Builder {
   public:
    Builder(BeDevice & beDevice) : beDevice{ beDevice } {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<BeDescriptorPool> build() const;

   private:
    BeDevice &beDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  BeDescriptorPool(
      BeDevice & beDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~BeDescriptorPool();
  BeDescriptorPool(const BeDescriptorPool &) = delete;
  BeDescriptorPool &operator=(const BeDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
 // VkDescriptorPool* getDescriptorPool() { return &descriptorPool; }

  void resetPool();
  VkDescriptorPool* getDescriptorPool();
 private:
  BeDevice & beDevice;
  VkDescriptorPool descriptorPool;

  friend class BeDescriptorWriter;
};

class BeDescriptorWriter {
 public:
  BeDescriptorWriter(BeDescriptorSetLayout &setLayout, BeDescriptorPool &pool);

  BeDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  BeDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  BeDescriptorSetLayout &setLayout;
  BeDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace BE
