/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */
#include "pch.h"
#include "Be_buffer.hpp"
#pragma warning (disable:4458)
namespace BE {

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize BeBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0) {
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
  }
  return instanceSize;
}

BeBuffer::BeBuffer(
    BeDevice *device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : beDevice{device},
      instanceSize{instanceSize},
      instanceCount{instanceCount},
      usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} 
{
  alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
  bufferSize = alignmentSize * instanceCount;
  device->createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

void BeBuffer::Create(
    BeDevice* device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
{
    beDevice            = device;
    instanceSize        = instanceSize;
    instanceCount       = instanceCount;
    usageFlags          = usageFlags;
    memoryPropertyFlags = memoryPropertyFlags;
    alignmentSize       = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize          = alignmentSize * instanceCount;

    device->createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}




BeBuffer::~BeBuffer() {
    if (mapped) {
        unmap();
    }
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(beDevice->device(), buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    if (memory != VK_NULL_HANDLE) {
        vkFreeMemory(beDevice->device(), memory, nullptr);
        memory = VK_NULL_HANDLE;
    }
}


void BeBuffer::destroy() {
    if (mapped) {
        unmap();
    }
    if (buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(beDevice->device(), buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    if (memory != VK_NULL_HANDLE) {
        vkFreeMemory(beDevice->device(), memory, nullptr);
        memory = VK_NULL_HANDLE;
    }
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
VkResult BeBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
  //assert(buffer && memory && "Called map on buffer before create");
  AssertCore(buffer && memory, "Called map on buffer before create");
  return vkMapMemory(beDevice->device(), memory, offset, size, 0, &mapped);
}

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void BeBuffer::unmap() {
  if (mapped) {
    vkUnmapMemory(beDevice->device(), memory);
    mapped = nullptr;
  }
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void BeBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
  //assert(mapped && "Cannot copy to unmapped buffer");
  AssertCore(mapped, "Cannot copy to unmapped buffer");
  if (size == VK_WHOLE_SIZE) {
    memcpy(mapped, data, bufferSize);
  } else {
    char *memOffset = (char *)mapped;
    memOffset += offset;
    memcpy(memOffset, data, size);
  }
}

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
VkResult BeBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkFlushMappedMemoryRanges(beDevice->device(), 1, &mappedRange);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
VkResult BeBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkInvalidateMappedMemoryRanges(beDevice->device(), 1, &mappedRange);
}

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
VkDescriptorBufferInfo BeBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
  return VkDescriptorBufferInfo{
      buffer,
      offset,
      size,
  };
}

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void BeBuffer::writeToIndex(void *data, int index) {
  writeToBuffer(data, instanceSize, index * alignmentSize);
}

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
VkResult BeBuffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
VkDescriptorBufferInfo BeBuffer::descriptorInfoForIndex(int index) {
  return descriptorInfo(alignmentSize, index * alignmentSize);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
VkResult BeBuffer::invalidateIndex(int index) {
  return invalidate(alignmentSize, index * alignmentSize);
}

}  // namespace BE
