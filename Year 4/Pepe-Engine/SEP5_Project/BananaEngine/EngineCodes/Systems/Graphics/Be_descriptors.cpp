#include "pch.h"
#include "Be_descriptors.hpp"
#include <Debugger/Debugger.h>
#pragma warning (push)
#pragma warning (disable:26812)

namespace BE {

// *************** Descriptor Set Layout Builder *********************

BeDescriptorSetLayout::Builder &BeDescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count) {
 // assert(bindings.count(binding) == 0 && "Binding already in use");
    AssertCore(bindings.count(binding) == 0, "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  bindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<BeDescriptorSetLayout> BeDescriptorSetLayout::Builder::build() const {
  return std::make_unique<BeDescriptorSetLayout>(beDevice, bindings);
}

// *************** Descriptor Set Layout *********************

BeDescriptorSetLayout::BeDescriptorSetLayout(
    BeDevice & beDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : beDevice{ beDevice }, bindings{bindings} {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  //std::vector< VkDescriptorBindingFlags> flags;

  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
    //flags.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
  }

 
  //VkDescriptorSetLayoutBindingFlagsCreateInfo newInfo;
  //newInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
  //newInfo.bindingCount = flags.size();
  //newInfo.pBindingFlags = &flags[0];

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
  //descriptorSetLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
  //descriptorSetLayoutInfo.pNext = &newInfo;

  if (vkCreateDescriptorSetLayout(
      beDevice.device(),
          &descriptorSetLayoutInfo,
          nullptr,
          &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

BeDescriptorSetLayout::~BeDescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(beDevice.device(), descriptorSetLayout, nullptr);
}

std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& BeDescriptorSetLayout::getBindings()
{
    return bindings;
}

// *************** Descriptor Pool Builder *********************

BeDescriptorPool::Builder &BeDescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32_t count) {
  poolSizes.push_back({descriptorType, count});
  return *this;
}

BeDescriptorPool::Builder &BeDescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags) {
  poolFlags = flags;
  return *this;
}
BeDescriptorPool::Builder &BeDescriptorPool::Builder::setMaxSets(uint32_t count) {
  maxSets = count;
  return *this;
}

std::unique_ptr<BeDescriptorPool> BeDescriptorPool::Builder::build() const {
  return std::make_unique<BeDescriptorPool>(beDevice, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

BeDescriptorPool::BeDescriptorPool(
    BeDevice & beDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize> &poolSizes)
    : beDevice{ beDevice } {
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  descriptorPoolInfo.flags = poolFlags;
  LogCore("Create DescriptorPool");
  if (vkCreateDescriptorPool(beDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

BeDescriptorPool::~BeDescriptorPool() {
    LogCore("Create DestroyDescriptorPool");
  vkDestroyDescriptorPool(beDevice.device(), descriptorPool, nullptr);
}

bool BeDescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.pSetLayouts = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;
  //VKCHECK()
  // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
  // a new pool whenever an old pool fills up. But this is beyond our current scope
  if (vkAllocateDescriptorSets(beDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void BeDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
  vkFreeDescriptorSets(
      beDevice.device(),
      descriptorPool,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
}

void BeDescriptorPool::resetPool() {
  vkResetDescriptorPool(beDevice.device(), descriptorPool, 0);
}

VkDescriptorPool* BeDescriptorPool::getDescriptorPool()
{
    return &descriptorPool;
}

// *************** Descriptor Writer *********************

BeDescriptorWriter::BeDescriptorWriter(BeDescriptorSetLayout &setLayout, BeDescriptorPool &pool)
    : setLayout{setLayout}, pool{pool} {}

BeDescriptorWriter &BeDescriptorWriter::writeBuffer(
    uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
 // assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
    AssertCore(setLayout.bindings.count(binding) == 1, "Layout does not contain specified binding");
  auto &bindingDescription = setLayout.bindings[binding];

  //assert(
  //    bindingDescription.descriptorCount == 1 &&
  //    "Binding single descriptor info, but binding expects multiple");

  AssertCore(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple");
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

BeDescriptorWriter &BeDescriptorWriter::writeImage(
    uint32_t binding, VkDescriptorImageInfo *imageInfo) {
  //assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");
  AssertCore(setLayout.bindings.count(binding) == 1, "Layout does not contain specified binding");
  auto &bindingDescription = setLayout.bindings[binding];

 /* assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");*/
  AssertCore(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

bool BeDescriptorWriter::build(VkDescriptorSet &set) {
  bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void BeDescriptorWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(pool.beDevice.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

}  // namespace BE
#pragma warning (pop)